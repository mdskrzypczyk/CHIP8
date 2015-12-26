#include "graphics.h"

#ifndef CHIP_H
#define CHIP_H
#define MEM_SIZE 4096
#define REG_SIZE 16
#define STACK_SIZE 16
#define PC_START 0x200
#define ETI_START 0x600
#define MAP_LENGTH 5*16

class CHIP8
{
    public:
        /* Initialization routine for CHIP8 registers */
        CHIP8();

        /* Destructor routine for CHIP8 Object */
        ~CHIP8();

        /* Routine to load hex sprite data into memory */
        void load_hex_sprites();

        void draw_sprite(uint8_t x, uint8_t y, uint8_t nibble);

        void load_program(const char* program_name);

        void print_mem_contents();

        void print_sys_contents();

        void mainloop();

        void show_video();

        void mod_I(uint16_t val);

        /* Routine to execute opcode */
        void exec_op(unsigned short opcode);


    private:
        VIDEO CHIPVIDEO;
        uint16_t PC;
        uint8_t SP;
        uint16_t STACK[STACK_SIZE];
        uint8_t MEM[MEM_SIZE];
        uint8_t V[REG_SIZE];
        uint16_t I;
        uint8_t DT, ST;
};

#endif