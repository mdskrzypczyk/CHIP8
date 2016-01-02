#include "chip8.h"

/* Hexadecimal Sprite Bit Map loaded into Interpreter Area of CHIP 8 Memory (0x000 - 0x1FF) */
uint8_t SPRITE_MAP[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  //Hex digit 0
    0x20, 0x60, 0x20, 0x20, 0x70,  //Hex digit 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  //Hex digit 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  //Hex digit 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  //Hex digit 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  //Hex digit 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  //Hex digit 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  //Hex digit 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  //Hex digit 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  //Hex digit 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  //Hex digit A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  //Hex digit B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  //Hex digit C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  //Hex digit D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  //Hex digit E
    0xF0, 0x80, 0xF0, 0x80, 0x80   //Hex digit F
};

/*
 * CHIP8
 * Description: Main constructor for CHIP8 object, initializes graphic components
 *				and clears all internal registers.  Sets PC to program start.  Loads
 *				CHIP8 memory 0x000-0x1FF with Hex Sprite data.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

CHIP8::CHIP8()
{
	//Assign graphics class
	CHIPVIDEO = VIDEO();

	//Assign input class
	CHIPINPUT = INPUT();

	//Initialize internals
	PC = PC_START;
	SP = 0xFF;
	I = 0x00;
	DT = 0x00;
	ST = 0x00;

	//Clear stack, V registers, and memory
	for(int i = 0; i < MEM_SIZE; i++){
		//Stack
		if(i < STACK_SIZE){
			STACK[i] = 0;
		}

		//Registers
		if(i < REG_SIZE){
			V[i] = 0;
		}

		//Load memory with sprite map if in area
		if(i < MAP_LENGTH){
			MEM[i] = SPRITE_MAP[i];
		}
		else{
			MEM[i] = 0;
		}
	}
}

bool CHIP8::init_video(){
	bool success = CHIPVIDEO.init();
	CHIPVIDEO.show();
	return success;
}

/*
 * ~CHIP8
 * Description: Main destructor for CHIP8 object, CHIP8 does not allocate space for
 *				its own variables so we need to make sure SDL window closes properly.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

CHIP8::~CHIP8(){
	CHIPVIDEO.close();
}

/*
 * load_program
 * Description: Reads binary file CHIP8 program and stores data into memory starting 
 *				at address 0x200.
 * Inputs: program_name - A string containing the file name of the program to load.
 * Outputs: None
 * Return Value: None
*/

bool CHIP8::load_program(const char* program_name){
	//Temporary data array, clear contents so we don't load garbage
	uint8_t program_data[MAX_PROG_SIZE];
	for(int i = 0; i<MAX_PROG_SIZE; i++) program_data[i] = 0x00;

	//Open chip 8 game
	FILE* program_file = fopen(program_name, "r+");
	if(program_file == NULL){
		std::cout << "Unable to open file.\n" << std::endl;
		return false;
	}

	// Obtain the file size
	fseek(program_file, 0, SEEK_END);
	size_t f_size = ftell(program_file);
	rewind(program_file);


	size_t result = fread(program_data, sizeof(uint8_t), MAX_PROG_SIZE, program_file);	
	if(result != f_size){
		std::cout << "Error reading file.\n" << std::endl;
		return false;
	}

	fclose(program_file);

	//Copy from the data array into memory, this can be skipped by having fread read directly into the correct starting address of memory
	//**********************************************************************************************************************************
	for(int i = 0; i<MAX_PROG_SIZE; i++) MEM[PC_START + i] = program_data[i];
	return true;
}

/*
 * mainloop
 * Description: The main emulation loop of CHIP8.  Grabs current opcode from memory location
 *				indicated by PC, increments the PC by 2, executes the opcode, checks to 
 *				display new video frame at 60Hz.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

void CHIP8::mainloop(){
	//Opcode and time variables
	unsigned short opcode;
	uint32_t start;
	uint32_t FPS = 30;
	bool draw;

	while(true){
		start = SDL_GetTicks();

		//Break out if PC escapes memory
		if(PC > MEM_SIZE) break;

		//Grab Opcode
		opcode = MEM[PC];
		opcode <<= 8;
		opcode |= MEM[PC+1];

		//Increment Program Counter
		PC += 2;

		//Execute opcode
		draw = exec_op(opcode);

		//Update the keyboard state
		CHIPINPUT.poll_keyboard();

		//Check if we should update Sound Timer, Delay Timer, and video frame
		if(1000/FPS > SDL_GetTicks() - start && draw){
			SDL_Delay(1000/FPS - (SDL_GetTicks() - start));
			show_video();
		}
		
		if(ST != 0) ST--;
		if(DT != 0) DT--;
	}
}

/*
 * draw_sprite
 * Description: Helper function for handling DXYN instruction for CHIP8.
 * Inputs: x - CHIP8 x coordinate to start drawing sprite at
 *		   y - CHIP8 y coordinate to start drawing sprite at
 *		   nibble - Number of bytes that make up the sprite
 * Outputs: None
 * Return Value: None
*/

void CHIP8::draw_sprite(uint8_t x, uint8_t y, uint8_t nibble){
	//Set VF to 0
	V[0xF] = 0;
	
	//Iterate through each line of sprite
	for(int line = 0; line < nibble; line++){
		//Grab the byte for the line
		uint8_t byte = MEM[I + line];

		//Update the CHIP8 y coordinate 
		uint32_t pix_y = y + line;

		//Iterate through each bit in the byte
		for(int bit = 0; bit < 8; bit++){
			//Update the CHIP8 x coordinate
			uint32_t pix_x = x + bit;

			//Only perform XOR on screen pixel if we have a '1'
			if((byte << bit) & 0x80){
				//Set the VF flag if we have a collision
				if(CHIPVIDEO.xor_color(pix_x, pix_y)) V[0xF] = 1;
			}
		}
	}
}

/*
 * show_video
 * Description: Makes a call to the VIDEO's show function, updates the frame
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

void CHIP8::show_video(){
	CHIPVIDEO.show();
}

/*
 * exec_op
 * Description: Executes the current opcode and updates internal registers
 * Inputs: opcode - The 16-bit opcode to execute
 * Outputs: None
 * Return Value: None
*/

bool CHIP8::exec_op(uint16_t opcode){
	//Extract all argument information from the opcode
	uint8_t x = (uint8_t)((opcode >> 8) & 0x0F);
	uint8_t y = (uint8_t)((opcode >> 4) & 0x0F);
	uint8_t kk = (uint8_t)(opcode & 0xFF);
	uint8_t nibble = (uint8_t)(opcode & 0x0F);

	//Seed random generator
	srand(time(NULL));

	//Decode and execute opcode
	if((opcode >> 12) == 0x0){
		//Two opcodes have first hex zero
		switch(opcode){
			case 0xE0 : CHIPVIDEO.clear();			//CLS - Clear Display
						break;
			case 0xEE : PC = STACK[SP], SP -= 1;	//RET - Restore PC from stack, decrement Stack Pointer
						break;
		}
	}
	if((opcode >> 12) == 0x1) PC = (0x0FFF & opcode);	//JMP - PC gets value of lower 12 bits
	if((opcode >> 12) == 0x2) SP += 1, STACK[SP] = PC, PC = (0x0FFF & opcode);	//CALL - Increment Stack Pointer, store PC, PC gets lower 12 bits
	if((opcode >> 12) == 0x3) PC = ((V[x] == kk) ? PC+2 : PC);		//Skip Equal, skip next instruction if Vx == kk
	if((opcode >> 12) == 0x4) PC = ((V[x] != kk) ? PC+2 : PC);		//Skip Not Equal, skip next instruction if Vx != kk
	if((opcode >> 12) == 0x5) PC = ((V[x] == V[y]) ? PC+2 : PC);	//Skip Equal, skip next instruction if Vx == Vy
	if((opcode >> 12) == 0x6) V[x] = kk;	//LoaD Vx with kk
	if((opcode >> 12) == 0x7) V[x] += kk;	//ADD kk to Vx
	if((opcode >> 12) == 0x8){				//Nine opcodes begin with Hex 8
		switch(opcode & 0xF){
			case 0x0: V[x] = V[y];			//LoaD Vx with Vy
					  break;
			case 0x1: V[x] |= V[y];			//OR Vx with Vy
					  break;
			case 0x2: V[x] &= V[y];			//AND Vx with Vy
					  break;
			case 0x3: V[x] ^= V[y];			//XOR Vx with Vy
					  break;
			case 0x4: V[0xF] = (((V[x] + V[y]) > 255) ? 1 : 0), V[x] += V[y];	//Add Vx with Vy, store carry bit into VF
					  break;
			case 0x5: V[0xF] = ((V[x] > V[y]) ? 1 : 0), V[x] -= V[y];			//Subtract Vy from Vx, store borrow bit into VF
					  break;
			case 0x6: V[0xF] = ((V[x] & 0x1) ? 1 : 0), V[x] = (V[x] >> 1);		//VF gets LSB of Vx, Vx gets bitshifted to the right by 1
					  break;
			case 0x7: V[0xF] = ((V[y] > V[x]) ? 1 : 0), V[x] = V[y] - V[x];		//Vx gets Vy - Vx, store borrow bit into VF
					  break;
			case 0xE: V[0xF] = ((V[x] & 0x8000) ? 1 : 0), V[x] = (V[x] << 1);	//VF gets MSB of Vx, Vx gets bitshifted to the left by 1
					  break;
			default: ;
		}
 	}
 	if((opcode >> 12) == 0x9) PC = ((V[x] != V[y]) ? PC + 2 : PC);		//Skip Not Equal, skip next instruction if Vx != Vy
 	if((opcode >> 12) == 0xA) I = (opcode & 0xFFF);						//LoaD I, I gets lower 12 bits of opcode
 	if((opcode >> 12) == 0xB) PC = V[0] + (opcode & 0xFFF);				//JMP, PC gets V0 + lower 12 bits of opcode
 	if((opcode >> 12) == 0xC) V[x] = (rand() % 256) & kk;				//Vx gets a random number ANDed with lower byte of opcode
 	if((opcode >> 12) == 0xD){
 		draw_sprite(V[x], V[y], nibble); 			//Draw sprite at coordinate x, y that is nibble-lines long
 		return true;
 	}
 	if((opcode >> 12) == 0xE){				//2 Opcodes begin with Hex E
 		if((opcode & 0xFF) == 0x9E){
 			if(CHIPINPUT.get_key_status(V[x]) == true){
 				PC+=2; //Input related
 			}
 		} 
 		if((opcode & 0xFF) == 0xA1){
 			if(CHIPINPUT.get_key_status(V[x]) == false){
 				PC+=2; //Input related
 			}
 		}
 	}
 	if((opcode >> 12) == 0xF){				//Nine opcodes begin with Hex F
 		switch(opcode & 0xFF){
 			case 0x7: V[x] = DT;			//Vx gets Delay Timer value
 					  break;
 			case 0xA: V[x] = CHIPINPUT.poll_keyboard();
 					  while(V[x] == 0xFF){
 					  	V[x] = CHIPINPUT.poll_keyboard();
 					  };
 					  break;
 			case 0x15: DT = V[x];			//Delay Timer gets Vx
 					   break;
 			case 0x18: ST = V[x];			//Sound Timer gets Vx
 					   break;
 			case 0x1E: I += V[x];			//I gets incremented by Vx
 					   break;
 			case 0x29: I = 5*V[x];			//I gets address of sprite corresponding to value in Vx
 					   break;
 			case 0x33: MEM[I] = (V[x] % 1000) / 100, MEM[I+1] = (V[x] % 100) / 10, MEM[I+2] = (V[x] % 10);		//Store BCD representation of Vx in I, I+1, I+2
 					   break;
 			case 0x55: for(int i = 0; i <= x; i++) MEM[I+i] = V[i];		//Store V0 through Vx starting at memory I
 					   break;
 			case 0x65: for(int i = 0; i<= x; i++) V[i] = MEM[I + i];	//Load V0 through Vx with values starting at memory I
 					   break;
 		}
 	}
 	return false;
}

/* Debugging functions */
void CHIP8::print_mem_contents(){
	for(int i = PC_START; i < MEM_SIZE; i+=2) printf("%x: %x%x\n", i, MEM[i], MEM[i+1]);
}

void CHIP8::print_sys_contents(){
	printf("PC: %x  INSTR: %x%x  SP: %x  I: %x  I POINTS AT: %x\n", PC, MEM[PC], MEM[PC+1], SP, I, MEM[I]);
	for(int i = 0; i < REG_SIZE; i++){
		printf("V%d: %d,%x  ", i, V[i], V[i]);
	}
	printf("\n");
}
