#include "chip8.h"

/* Hexadecimal Sprite Bit Map loaded into Interpreter Area of CHIP 8 Memory
 * (0x000 - 0x1FF)
 */
uint8_t SPRITE_MAP[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // Hex digit 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // Hex digit 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // Hex digit 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // Hex digit 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // Hex digit 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // Hex digit 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // Hex digit 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // Hex digit 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // Hex digit 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // Hex digit 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // Hex digit A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // Hex digit B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // Hex digit C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // Hex digit D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // Hex digit E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // Hex digit F
};

/*
 * CHIP8
 * Description: Main constructor for CHIP8 object, initializes graphic
 * components and clears all internal registers.  Sets PC to program start.
 * Loads CHIP8 memory 0x000-0x1FF with Hex Sprite data.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

CHIP8::CHIP8() {
    quit = false;
    draw = true;

    // Assign graphics class
    CHIPVIDEO = VIDEO();

    // Assign input class
    CHIPINPUT = INPUT();

    // Assign audio class
    CHIPAUDIO = AUDIO();

    // Initialize internals
    PC = PC_START;
    SP = 0xFF;
    I = 0x00;
    DT = 0x00;
    ST = 0x00;

    // Clear stack, V registers, and memory
    for (int i = 0; i < MEM_SIZE; i++) {
        // Stack
        if (i < STACK_SIZE) {
            STACK[i] = 0;
        }

        // Registers
        if (i < REG_SIZE) {
            V[i] = 0;
        }

        // Load memory with sprite map if in area
        if (i < MAP_LENGTH) {
            MEM[i] = SPRITE_MAP[i];
        } else {
            MEM[i] = 0;
        }
    }
}

bool CHIP8::init_video() {
    bool success = CHIPVIDEO.init();
    CHIPVIDEO.show();
    return success;
}

bool CHIP8::init_audio() { return CHIPAUDIO.init(); }

void CHIP8::play_audio() { CHIPAUDIO.play_tone(); }

/*
 * ~CHIP8
 * Description: Main destructor for CHIP8 object, CHIP8 does not allocate space
 * for its own variables so we need to make sure SDL window closes properly.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

CHIP8::~CHIP8() { CHIPVIDEO.close(); }

/*
 * load_program
 * Description: Reads binary file CHIP8 program and stores data into memory
 * starting at address 0x200.
 * Inputs: program_name - A string containing the file name of the program to
 * load.
 * Outputs: None
 * Return Value: true/false - Indicates of load was successful
 */

bool CHIP8::load_program(const char *program_name) {
    // Temporary data array, clear contents so we don't load garbage
    uint8_t program_data[MAX_PROG_SIZE];
    for (int i = 0; i < MAX_PROG_SIZE; i++) {
        program_data[i] = 0x00;
    }

    // Open chip 8 game
    FILE *program_file = fopen(program_name, "r+");
    if (program_file == nullptr) {
        std::cout << "Unable to open file.\n" << std::endl;
        return false;
    }

    // Obtain the file size
    fseek(program_file, 0, SEEK_END);
    size_t f_size = ftell(program_file);
    rewind(program_file);

    size_t result =
            fread(program_data, sizeof(uint8_t), MAX_PROG_SIZE, program_file);
    if (result != f_size) {
        std::cout << "Error reading file.\n" << std::endl;
        return false;
    }

    fclose(program_file);

    // Copy from the data array into memory, this can be skipped by having fread
    // read directly into the correct starting address of memory
    for (int i = 0; i < MAX_PROG_SIZE; i++) {
        MEM[PC_START + i] = program_data[i];
    }
    return true;
}

/*
 * load_state
 * Description: Reads binary file Chip-8 state and restores the state of the
 * Chip-8.
 * Inputs: state_name - A string containing the name of the file to load the
 * state from
 * Outputs: None
 * Return Value: true/false - Indicates if restore was successful
 */

bool CHIP8::load_state(const char *state_name) {
    // Temporary array to hold state info
    uint8_t state_data[STATE_SIZE];

    // Open the state file
    FILE *state_file = fopen(state_name, "r");
    if (state_file == nullptr) {
        std::cout << "Unable to load state.\n" << std::endl;
        return false;
    }

    // Read state data
    size_t bytes_read =
            fread(state_data, sizeof(uint8_t), STATE_SIZE, state_file);
    if (bytes_read != STATE_SIZE) {
        std::cout << "Error loading state file.\n" << std::endl;
        return false;
    }

    // Close state file
    fclose(state_file);

    // Restore Chip-8 state.
    // 1.  Restore PC, SP, I, ST, and DT
    PC = ((uint16_t)(state_data[0]) << 8) | state_data[1];
    SP = state_data[2];
    I = ((uint16_t)(state_data[3]) << 8) | state_data[4];
    ST = state_data[5];
    DT = state_data[6];

    // 2.  Restore V registers
    for (int i = V_OFFSET; i < STACK_OFFSET; i++) {
        V[i - V_OFFSET] = state_data[i];
    }

    // 3.  Restore Stack
    for (int i = STACK_OFFSET; i < MEM_OFFSET; i += 2) {
        STACK[i - STACK_OFFSET] =
                ((uint16_t)(state_data[i]) << 8) | state_data[i + 1];
    }

    // 4.  Restore Memory
    for (int i = MEM_OFFSET; i < PIX_OFFSET; i++) {
        MEM[i - MEM_OFFSET] = state_data[i];
    }

    CHIPVIDEO.clear();  // Clear the display

    // 5.  Restore the pixel map
    uint32_t *pixel_map = CHIPVIDEO.get_pix_map();
    for (int i = 0; i < 2048; i++) {
        pixel_map[i] = (uint32_t) state_data[4 * i + PIX_OFFSET] << 24 |
                       (uint32_t) state_data[4 * i + 1 + PIX_OFFSET] << 16 |
                       (uint32_t) state_data[4 * i + 2 + PIX_OFFSET] << 8 |
                       (uint32_t) state_data[4 * i + 3 + PIX_OFFSET];
    }

    // Redraw the pixel map
    CHIPVIDEO.draw_pix_map();
    show_video();

    return true;
}

/*
 * save_state
 * Description: Turns internal Chip-8 state into array and writes data to file.
 * Inputs: state_name - A string containing the name of the file to save state
 * to.
 * Outputs: None
 * Return Value: true/false - Indicates if save was successful
 */

bool CHIP8::save_state(const char *state_name) {
    // Temporary array to hold state info
    uint8_t state_data[STATE_SIZE];

    // Save PC
    state_data[0] = (uint8_t)(PC >> 8), state_data[1] = (uint8_t)(PC);
    // Save SP
    state_data[2] = SP;
    // Save I
    state_data[3] = (uint8_t)(I >> 8), state_data[4] = (uint8_t)(I);
    // Save ST and DT
    state_data[5] = ST, state_data[6] = DT;

    // Save V Registers
    for (int i = V_OFFSET; i < STACK_OFFSET; i++) {
        state_data[i] = V[i - 7];
    }

    // Save Stack
    for (int i = 0; i < STACK_SIZE; i++) {
        state_data[2 * i + STACK_OFFSET] = (uint8_t)(STACK[i] >> 8);
        state_data[2 * i + 1 + STACK_OFFSET] = (uint8_t)(STACK[i]);
    }

    // Save Memory
    for (int i = MEM_OFFSET; i < PIX_OFFSET; i++) {
        state_data[i] = MEM[i - MEM_OFFSET];
    }

    // Save screen info
    uint32_t *pixel_map = CHIPVIDEO.get_pix_map();
    for (int i = 0; i < 2048; i++) {
        state_data[4 * i + PIX_OFFSET] = (uint8_t)(pixel_map[i] >> 24);
        state_data[4 * i + 1 + PIX_OFFSET] = (uint8_t)(pixel_map[i] >> 16);
        state_data[4 * i + 2 + PIX_OFFSET] = (uint8_t)(pixel_map[i] >> 8);
        state_data[4 * i + 3 + PIX_OFFSET] = (uint8_t)(pixel_map[i]);
    }

    // Write state information to file
    FILE *state_file = fopen(state_name, "wb");
    if (state_file == nullptr) {
        std::cout << "Unable to open file for writing.\n" << std::endl;
        return false;
    }

    // Write state data, check if all data written
    size_t bytes_written =
            fwrite(state_data, sizeof(uint8_t), STATE_SIZE, state_file);
    if (bytes_written != STATE_SIZE) {
        std::cout << "Error writing state to file.\n" << std::endl;
        return false;
    }

    // Close the file
    fclose(state_file);

    return true;
}

bool CHIP8::load_config() {
    std::ifstream config_file;
    config_file.open("config.txt");
    if (!config_file.good()) {
        std::cout << "Unable to open config.txt.\n" << std::endl;
        return false;
    }

    uint8_t key = 0;

    while (!config_file.eof()) {
        sleep(1);
        char line[10];

        config_file.getline(line, 3);

        printf("%s\n", line);
        printf("%d mapped to %c\n", key, line[3]);
        key++;
    }

    return true;
}

/*
 * mainloop
 * Description: The main emulation loop of CHIP8.  Grabs current opcode from
 * memory location indicated by PC, increments the PC by 2, executes the opcode,
 * checks to display new video frame at 60Hz.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void CHIP8::mainloop() {
    // Opcode and time variables
    unsigned short opcode;
    uint32_t v_timer_start = SDL_GetTicks();
    uint32_t sd_timer_start = SDL_GetTicks();

    // Seed random generator
    srand(time(nullptr));

    while (!quit) {
        // Break out if PC escapes memory
        if (PC > MEM_SIZE) {
            break;
        }
        // Grab Opcode (Fetch)
        opcode = (uint16_t) MEM[PC] << 8 | MEM[PC + 1];

        // Execute opcode (Decode and Execute)
        draw = exec_op(opcode);

        // Check for keyboard and window updates
        check_peripherals();

        // Update Sound Timer and Delay Timer at 60Hz
        if (1000 / FPS <= SDL_GetTicks() - sd_timer_start) {
            if (DT != 0) {
                DT--;
            }
            if (ST != 0) {
                ST--;
                // Render audio
                CHIPAUDIO.play_tone();
            }
        }

        // Check if we should update video frame
        if (1000 / FPS <= SDL_GetTicks() - v_timer_start) {
            v_timer_start = SDL_GetTicks();
            show_video();
        } else if ((1000 / FPS > SDL_GetTicks() - v_timer_start) && draw) {
            SDL_Delay(1000 / FPS - (SDL_GetTicks() - v_timer_start));
            v_timer_start = SDL_GetTicks();
            show_video();
        }
    }
}

/*
 * check_peripherals
 * Description: Function for handling keyboard and window updates
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void CHIP8::check_peripherals() {
    SDL_Event event;  // For processing keyboard/window updates

    // Check for keyboard and window updates
    while (SDL_PollEvent(&event) != 0) {
        uint8_t key_return =
                CHIPINPUT.poll_keyboard(event);  // Update key status
        CHIPVIDEO.handle_event(event);           // Update window

        quit = (key_return == 16);  // Quit if 'x' clicked

        // Change the color scheme if user wishes
        if (key_return == 17) {
            CHIPVIDEO.rand_color_scheme();
        } else if (key_return == 18) {
            save_state("chip8.sv");  // Save state
        } else if (key_return == 19) {
            load_state("chip8.sv");  // Load state
        }
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

bool CHIP8::draw_sprite(uint8_t x, uint8_t y, uint8_t nibble) {
    // Set VF to 0
    V[0xF] = 0;

    // Iterate through each line of sprite
    for (int line = 0; line < nibble; line++) {
        // Grab the byte for the line
        uint8_t byte = MEM[I + line];

        // Update the CHIP8 y coordinate
        uint32_t pix_y = y + line;

        // Iterate through each bit in the byte
        for (int bit = 0; bit < 8; bit++) {
            // Update the CHIP8 x coordinate
            uint32_t pix_x = x + bit;

            // Only perform XOR on screen pixel if we have a '1'
            if (((byte << bit) & static_cast<int>(0x80)) != 0) {
                // Set the VF flag if we have a collision
                if (CHIPVIDEO.xor_color(pix_x, pix_y)) {
                    V[0xF] = 1;
                }
            }
        }
    }

    return V[0xF] == 0;
}

/*
 * show_video
 * Description: Makes a call to the VIDEO's show function, updates the frame
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void CHIP8::show_video() { CHIPVIDEO.show(); }

/*
 * exec_op
 * Description: Executes the current opcode and updates internal registers
 * Inputs: opcode - The 16-bit opcode to execute
 * Outputs: None
 * Return Value: None
 */

bool CHIP8::exec_op(uint16_t opcode) {
    // Increment PC
    PC += 2;

    // Extract all argument information from the opcode
    uint8_t x = (uint8_t)((opcode >> 8) & 0x0F);
    uint8_t y = (uint8_t)((opcode >> 4) & 0x0F);
    uint8_t kk = (uint8_t)(opcode & 0xFF);
    uint8_t nibble = (uint8_t)(opcode & 0x0F);
    SDL_Event event;

    // Decode and execute opcode
    switch (opcode >> 12) {
        case 0x0: {
            // Two opcodes have first hex zero
            switch (opcode) {
                case 0xE0:
                    CHIPVIDEO.clear();  // CLS - Clear Display
                    break;
                case 0xEE:
                    PC = STACK[SP];
                    SP -= 1;  // RET - Restore PC from stack, decrement Stack
                              // Pointer
                    break;
            }
            break;
        }
        case 0x1: {
            PC = (0x0FFF & opcode);  // JMP - PC gets value of lower 12 bits
            break;
        }
        case 0x2: {
            SP += 1;
            STACK[SP] = PC;
            PC = (0x0FFF & opcode);  // CALL - Increment Stack Pointer, store
                                     // PC, PC gets lower 12 bits
            break;
        }
        case 0x3: {
            PC = ((V[x] == kk) ? PC + 2 : PC);  // Skip Equal, skip next
                                                // instruction if Vx == kk
            break;
        }
        case 0x4: {
            PC = ((V[x] != kk) ? PC + 2 : PC);  // Skip Not Equal, skip next
                                                // instruction if Vx != kk
            break;
        }
        case 0x5: {
            PC = ((V[x] == V[y]) ? PC + 2 : PC);  // Skip Equal, skip next
                                                  // instruction if Vx == Vy
            break;
        }
        case 0x6: {
            V[x] = kk;  // LoaD Vx with kk
            break;
        }
        case 0x7: {
            V[x] += kk;  // ADD kk to Vx
            break;
        }
        case 0x8: {  // Nine opcodes begin with Hex 8
            switch (opcode & 0xF) {
                case 0x0: {
                    V[x] = V[y];  // LoaD Vx with Vy
                    break;
                }
                case 0x1: {
                    V[x] |= V[y];  // OR Vx with Vy
                    break;
                }
                case 0x2: {
                    V[x] &= V[y];  // AND Vx with Vy
                    break;
                }
                case 0x3: {
                    V[x] ^= V[y];  // XOR Vx with Vy
                    break;
                }
                case 0x4: {
                    V[0xF] = (((V[x] + V[y]) > 255) ? 1 : 0);
                    V[x] += V[y];  // Add Vx with Vy, store carry bit into VF
                    break;
                }
                case 0x5: {
                    V[0xF] = ((V[x] > V[y]) ? 1 : 0);
                    V[x] -= V[y];  // Subtract Vy from Vx, store borrow bit into
                                   // VF
                    break;
                }
                case 0x6: {
                    V[0xF] = ((V[x] & 0x1) != 0 ? 1 : 0);
                    V[x] = (V[x] >> 1);  // VF gets LSB of Vx, Vx gets
                                         // bitshifted to the right by 1
                    break;
                }
                case 0x7: {
                    V[0xF] = ((V[y] > V[x]) ? 1 : 0);
                    V[x] = V[y] -
                           V[x];  // Vx gets Vy - Vx, store borrow bit into VF
                    break;
                }
                case 0xE: {
                    V[0xF] = ((V[x] & 0x80) != 0 ? 1 : 0);
                    V[x] = (V[x] << 1);  // VF gets MSB of Vx, Vx gets
                                         // bitshifted to the left by 1
                    break;
                }
            }
            break;
        }
        case 0x9: {
            PC = ((V[x] != V[y]) ? PC + 2 : PC);  // Skip Not Equal, skip next
                                                  // instruction if Vx != Vy
            break;
        }
        case 0xA: {
            I = (opcode & 0xFFF);  // LoaD I, I gets lower 12 bits of opcode
            break;
        }
        case 0xB: {
            PC = V[0] +
                 (opcode & 0xFFF);  // JMP, PC gets V0 + lower 12 bits of opcode
            break;
        }
        case 0xC: {
            V[x] = (uint8_t)(rand() % 256) &
                   kk;  // Vx gets a random number ANDed with lower byte of
                        // opcode
            break;
        }
        case 0xD: {
            return draw_sprite(V[x], V[y],
                               nibble);  // Draw sprite at coordinate x, y that
                                         // is nibble-lines long
        }
        case 0xE: {  // 2 Opcodes begin with Hex E
            if ((opcode & 0xFF) == 0x9E) {
                if (CHIPINPUT.get_key_status(V[x])) {
                    PC += 2;
                }
            } else if ((opcode & 0xFF) == 0xA1) {
                if (!CHIPINPUT.get_key_status(V[x])) {
                    PC += 2;
                }
            }
            break;
        }
        case 0xF: {
            // Nine opcodes begin with Hex F
            switch (opcode & 0xFF) {
                case 0x7: {
                    V[x] = DT;  // Vx gets Delay Timer value
                    break;
                }
                case 0xA: {
                    if (SDL_WaitEvent(&event) != 0) {
                        V[x] = CHIPINPUT.poll_keyboard(event);
                    }
                    break;
                }
                case 0x15: {
                    DT = V[x];  // Delay Timer gets Vx
                    break;
                }
                case 0x18: {
                    ST = V[x];  // Sound Timer gets Vx
                    break;
                }
                case 0x1E: {
                    I += V[x];  // I gets incremented by Vx
                    break;
                }
                case 0x29: {
                    I = 5 * V[x];  // I gets address of sprite corresponding to
                    break;         // value in Vx
                }
                case 0x33: {
                    MEM[I] = V[x] / 100;
                    MEM[I + 1] = (V[x] % 100) / 10;
                    MEM[I + 2] = (V[x] % 10);  // Store BCD representation of Vx
                    break;                     // in I, I+1, I+2
                }
                case 0x55: {
                    for (int i = 0; i <= x; i++) {
                        MEM[I + i] = V[i];  // Store V0 through Vx starting at
                                            // memory I
                    }
                    break;
                }
                case 0x65: {
                    for (int i = 0; i <= x; i++) {
                        V[i] = MEM[I + i];  // Load V0 through Vx with values
                                            // starting at memory I
                    }
                    break;
                }
            }
            break;
        }
    }
    return false;
}

/* Debugging functions */
void CHIP8::print_mem_contents() {
    for (int i = PC_START; i < MEM_SIZE; i += 2) {
        printf("%x: %x%x\n", i, MEM[i], MEM[i + 1]);
    }
}

void CHIP8::print_sys_contents() {
    printf("PC: %02x  INSTR: %02x%02x  SP: %02x  I: %x  I POINTS AT: %x\n", PC,
           MEM[PC], MEM[PC + 1], SP, I, MEM[I]);
    for (int i = 0; i < REG_SIZE; i++) {
        printf("V%d: %d,%x  ", i, V[i], V[i]);
    }
    printf("\n");
}

uint16_t CHIP8::get_pc() { return PC; }