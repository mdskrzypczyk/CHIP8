#include "chip8.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

bool theflag = false;

uint8_t SPRITE_MAP[80] = {
    0xF0,       //Hex digit 0
    0x90,
    0x90,
    0x90,
    0xF0,

    0x20,       //Hex digit 1   
    0x60,
    0x20,
    0x20,
    0x70,

    0xF0,       //Hex digit 2
    0x10,
    0xF0,
    0x80,
    0xF0,

    0xF0,       //Hex digit 3
    0x10,
    0xF0,
    0x10,
    0xF0,

    0x90,       //Hex digit 4
    0x90,
    0xF0,
    0x10,
    0x10,

    0xF0,       //Hex digit 5
    0x80,
    0xF0,
    0x10,
    0xF0,

    0xF0,       //Hex digit 6
    0x80,
    0xF0,
    0x90,
    0xF0,

    0xF0,       //Hex digit 7
    0x10,
    0x20,
    0x40,
    0x40,

    0xF0,       //Hex digit 8
    0x90,
    0xF0,
    0x90,
    0xF0,

    0xF0,       //Hex digit 9
    0x90,
    0xF0,
    0x10,
    0xF0,

    0xF0,       //Hex digit A
    0x90,
    0xF0,
    0x90,
    0x90,

    0xE0,       //Hex digit B
    0x90,
    0xE0,
    0x90,
    0xE0,

    0xF0,       //Hex digit C
    0x80,
    0x80,
    0x80,
    0xF0,

    0xE0,       //Hex digit D
    0x90,
    0x90,
    0x90,
    0xE0,

    0xF0,       //Hex digit E
    0x80,
    0xF0,
    0x80,
    0xF0,

    0xF0,       //Hex digit F
    0x80,
    0xF0,
    0x80,
    0x80
};

CHIP8::CHIP8()
{
	CHIPVIDEO = VIDEO();
	CHIPVIDEO.init();
	CHIPVIDEO.show();
	PC = PC_START;
	SP = 0xFF;
	I = 0x00;
	DT = 0x00;
	ST = 0x00;
	for(int i = 0; i < MEM_SIZE; i++){
		if(i < STACK_SIZE)
			STACK[i] = 0;
		if(i < REG_SIZE)
			V[i] = 0;
		MEM[i] = 0;
	}
	load_hex_sprites();
}

CHIP8::~CHIP8(){}

void CHIP8::mod_I(uint16_t val){
	I = val;
}

void CHIP8::load_hex_sprites(){
	for(int i = 0; i < MAP_LENGTH; i++){
		MEM[i] = SPRITE_MAP[i];
	}
}

void CHIP8::load_program(const char* program_name){
	uint8_t program_data[0xA00];
	for(int i = 0; i<0xA00; i++) program_data[i] = 0x00;
	FILE* program_file = fopen(program_name, "r+");
	fread(program_data, sizeof(uint8_t), 2560, program_file);
	fclose(program_file);
	for(int i = 0; i<0xA00; i++) MEM[PC_START + i] = program_data[i];
}

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

void CHIP8::mainloop(){
	unsigned short opcode;
	time_t now, prev;
	time(&prev);
	while(true){
		print_sys_contents();
		if(PC > MEM_SIZE) break;
		opcode = MEM[PC];
		opcode <<= 8;
		opcode |= MEM[PC+1];
		PC += 2;
		exec_op(opcode);
		show_video();
		time(&now);
		if(difftime(now, prev) > 0.016){
			prev = now;
			show_video();
			if(ST != 0) ST--;
			if(DT != 0) DT--;
		}
	}
}

void CHIP8::draw_sprite(uint8_t x, uint8_t y, uint8_t nibble){
	V[0xF] = 0;
	for(int line = 0; line < nibble; line++){
		uint8_t byte = MEM[I + line];
		uint32_t pix_y = y + line;
		for(int bit = 0; bit < 8; bit++){
			uint32_t pix_x = x + bit;
			if((byte << bit) & 0x80){
				if(CHIPVIDEO.xor_color(pix_x, pix_y)) V[0xF] = 1;
			}
		}
	}
}

void CHIP8::show_video(){
	CHIPVIDEO.show();
}

void CHIP8::exec_op(uint16_t opcode){
	uint8_t x = (uint8_t)((opcode >> 8) & 0x0F);
	uint8_t y = (uint8_t)((opcode >> 4) & 0x0F);
	uint8_t kk = (uint8_t)(opcode & 0xFF);
	uint8_t nibble = (uint8_t)(opcode & 0x0F);
	srand(time(NULL));

	if((opcode >> 12) == 0x0){
		switch(opcode){
			case 0xE0 : CHIPVIDEO.clear();
						break;
			case 0xEE : PC = STACK[SP], SP -= 1;
						break;
		}
	}
	if((opcode >> 12) == 0x1) PC = (0x0FFF & opcode);
	if((opcode >> 12) == 0x2) SP += 1, STACK[SP] = PC, PC = (0x0FFF & opcode);
	if((opcode >> 12) == 0x3) PC = ((V[x] == kk) ? PC+2 : PC);
	if((opcode >> 12) == 0x4) PC = ((V[x] != kk) ? PC+2 : PC);
	if((opcode >> 12) == 0x5) PC = ((V[x] == V[y]) ? PC+2 : PC);
	if((opcode >> 12) == 0x6) V[x] = kk;
	if((opcode >> 12) == 0x7) V[x] += kk;
	if((opcode >> 12) == 0x8){
		switch(opcode & 0xF){
			case 0x0: V[x] = V[y];
					  break;
			case 0x1: V[x] |= V[y];
					  break;
			case 0x2: V[x] &= V[y];
					  break;
			case 0x3: V[x] ^= V[y];
					  break;
			case 0x4: V[0xF] = (((V[x] + V[y]) > 255) ? 1 : 0), V[x] += V[y];
					  break;
			case 0x5: V[0xF] = ((V[x] > V[y]) ? 1 : 0), V[x] -= V[y];
					  break;
			case 0x6: V[0xF] = ((V[x] & 0x1) ? 1 : 0), V[x] = (V[x] >> 1);
					  break;
			case 0x7: V[0xF] = ((V[y] > V[x]) ? 1 : 0), V[x] = V[y] - V[x];
					  break;
			case 0xE: V[0xF] = ((V[x] & 0x8000) ? 1 : 0), V[x] = (V[x] << 1);
					  break;
			default: ;
		}
 	}
 	if((opcode >> 12) == 0x9) PC = ((V[x] != V[y]) ? PC + 2 : PC);
 	if((opcode >> 12) == 0xA) I = (opcode & 0xFFF);
 	if((opcode >> 12) == 0xB) PC = V[0] + (opcode & 0xFFF);
 	if((opcode >> 12) == 0xC) V[x] = (rand() % 256) & kk;
 	if((opcode >> 12) == 0xD) draw_sprite(V[x], V[y], nibble); //Display Related
 	if((opcode >> 12) == 0xE){
 		if((opcode & 0xFF) == 0x9E) ; //Input related 
 		if((opcode & 0xFF) == 0xA1) ; //Input related
 	}
 	if((opcode >> 12) == 0xF){
 		switch(opcode & 0xFF){
 			case 0x7: V[x] = DT;
 					  break;
 			case 0xA: ;//Input related
 					  break;
 			case 0x15: DT = V[x];
 					   break;
 			case 0x18: ST = V[x];
 					   break;
 			case 0x1E: I += V[x];
 					   break;
 			case 0x29: I = 5*V[x];
 					   break;
 			case 0x33: MEM[I] = (V[x] % 1000) / 100, MEM[I+1] = (V[x] % 100) / 10, MEM[I+2] = (V[x] % 10);
 					   break;
 			case 0x55: for(int i = 0; i <= x; i++) MEM[I+i] = V[i];
 					   break;
 			case 0x65: for(int i = 0; i<= x; i++) V[i] = MEM[I + i];
 					   break;
 		}
 	}
}

