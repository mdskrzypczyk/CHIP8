#ifndef CHIP8_H
#define CHIP8_H

#include "audio.h"
#include "graphics.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#define MEM_SIZE 4096        // 4kB memory
#define REG_SIZE 16          // 16 Registers
#define STACK_SIZE 16        // 16 Stack entries
#define PC_START 0x200       // Programs start at 0x200
#define MAX_PROG_SIZE 0xD00  // Maximum allowable size of program
#define ETI_START 0x600      // Start of ETI area in memory
#define MAP_LENGTH (5 * 16)  // Size of Sprite Map
#define STATE_SIZE 12343     // Size of entire Chip-8 state
#define FPS 60

#define V_OFFSET 7
#define STACK_OFFSET (V_OFFSET + REG_SIZE)
#define MEM_OFFSET (STACK_OFFSET + 2 * STACK_SIZE)
#define PIX_OFFSET (MEM_OFFSET + MEM_SIZE)

/* Hexadecimal Sprite Bit Map loaded into Interpreter Area of CHIP 8 Memory
 * (0x000 - 0x1FF)
 */
extern uint8_t SPRITE_MAP[MAP_LENGTH];

class CHIP8 {
  public:
    // Main constructor for CHIP8
    CHIP8();

    // Main destructor for CHIP8
    ~CHIP8();

    // Video Initialization
    bool init_video();

    // Audio Initialization
    bool init_audio();
    void play_audio();

    // Function for loading a program file into the interpretter's memory
    bool load_program(const char *program_name);

    // Function for loading save state information
    bool load_state(const char *state_name);

    // Function for saving Chip-8 state information
    bool save_state(const char *state_name);

    bool load_config();

    // Main emulating loop for CHIP8
    void mainloop();

    // Function for checking for graphics and keyboard updates
    void check_peripherals();

    // Function for decoding and executing opcodes
    bool exec_op(unsigned short opcode);

    // Helper function for Dxyn opcode, draws a sprite to the screen
    bool draw_sprite(uint8_t x, uint8_t y, uint8_t nibble);

    // Function for showing
    void show_video();

    // Debugging Functions
    void print_mem_contents();

    void print_sys_contents();

    uint16_t get_pc();
    uint8_t get_sp();
    uint16_t *get_stack();
    uint8_t *get_mem();
    uint8_t *get_reg_file();
    uint16_t get_index_reg();
    uint8_t get_delay_timer();
    uint8_t get_sound_timer();
    bool get_quit();
    bool get_draw();
    INPUT* get_input_device();

  private:
    VIDEO CHIPVIDEO;  // Graphics/Video object for handling sprites and display
    INPUT CHIPINPUT;  // Input object for handling hex keyboard info
    AUDIO CHIPAUDIO;  // Audio object for handling sound
    uint16_t PC;      // 16-bit Program Counter
    uint8_t SP;       // 8-bit Stack pointer
    uint16_t STACK[STACK_SIZE];  // Stack
    uint8_t MEM[MEM_SIZE];       // Memory
    uint8_t V[REG_SIZE];         // Register file
    uint16_t I;                  // Index register
    uint8_t DT, ST;              // Delay timer and sound timer
    bool quit;
    bool draw;
};

#endif