#include "graphics.h"
#include "input.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#ifndef CHIP_H
#define CHIP_H
#define MEM_SIZE 4096       //4kB memory
#define REG_SIZE 16         //16 Registers
#define STACK_SIZE 16       //16 Stack entries
#define PC_START 0x200      //Programs start at 0x200
#define MAX_PROG_SIZE 0xA00 //Maximum allowable size of program
#define ETI_START 0x600     //Start of ETI area in memory
#define MAP_LENGTH 5*16     //Size of Sprite Map

class CHIP8
{
    public:
        //Main constructor for CHIP8
        CHIP8();

        //Main destructor for CHIP8
        ~CHIP8();

        //Video Initialization
        bool init_video();

        //Function for loading a program file into the interpretter's memory
        bool load_program(const char* program_name);

        //Main emulating loop for CHIP8
        void mainloop();

        //Function for decoding and executing opcodes
        bool exec_op(unsigned short opcode);

        //Helper function for Dxyn opcode, draws a sprite to the screen
        void draw_sprite(uint8_t x, uint8_t y, uint8_t nibble);

        //Function for showing 
        void show_video();

        //Debugging Functions
        void print_mem_contents();

        void print_sys_contents();

    private:
        VIDEO CHIPVIDEO;        //Graphics/Video object for handling sprites and display
        INPUT CHIPINPUT;        //Input object for handling hex keyboard info
        uint16_t PC;            //16-bit Program Counter
        uint8_t SP;             //8-bit Stack pointer
        uint16_t STACK[STACK_SIZE]; //Stack
        uint8_t MEM[MEM_SIZE];      //Memory
        uint8_t V[REG_SIZE];        //Register file
        uint16_t I;                 //Index register
        uint8_t DT, ST;             //Delay timer and sound timer
};

#endif