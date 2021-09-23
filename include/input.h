#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include <iostream>
#include <map>

#define NUM_KEYS 16  // Chip 8 has 16 hexadecimal keys on its keyboard

// Define keycodes
#define KEY_0 0x0
#define KEY_1 0x1
#define KEY_2 0x2
#define KEY_3 0x3
#define KEY_4 0x4
#define KEY_5 0x5
#define KEY_6 0x6
#define KEY_7 0x7
#define KEY_8 0x8
#define KEY_9 0x9
#define KEY_A 0xA
#define KEY_B 0xB
#define KEY_C 0xC
#define KEY_D 0xD
#define KEY_E 0xE
#define KEY_F 0xF
#define KEY_QUIT 0x10
#define KEY_COLOR_CHANGE 0x11
#define KEY_SAVE 0x12
#define KEY_LOAD 0x13
#define KEY_ERR 0xFF

// Default keymap
static std::map<SDL_Keycode, uint8_t> keymap{
        {SDLK_1, KEY_0},    {SDLK_2, KEY_1},      {SDLK_3, KEY_2},
        {SDLK_4, KEY_3},    {SDLK_q, KEY_4},      {SDLK_w, KEY_5},
        {SDLK_e, KEY_6},    {SDLK_r, KEY_7},      {SDLK_a, KEY_8},
        {SDLK_s, KEY_9},    {SDLK_d, KEY_A},      {SDLK_f, KEY_B},
        {SDLK_z, KEY_C},    {SDLK_x, KEY_D},      {SDLK_c, KEY_E},
        {SDLK_v, KEY_F},    {SDL_QUIT, KEY_QUIT}, {SDLK_t, KEY_COLOR_CHANGE},
        {SDLK_p, KEY_SAVE}, {SDLK_l, KEY_LOAD}};

/**
 * Module for handling input-related activities of CHIP 8.  Handles translation
 * of keyboard presses to emulated hexadecimal keyboard and tracks key state.
 */
class INPUT {
  public:
    // Constructor function
    INPUT();

    // Destructor function
    ~INPUT();

    // Function for updating key_status boolean array, checks the keyboard for
    // pressed/unpressed keys
    uint8_t poll_keyboard(SDL_Event event);
    uint8_t handle_keydown(SDL_Keycode sym);
    uint8_t handle_keyup(SDL_Keycode sym);

    // Function that returns pressed state of a given key
    bool get_key_status(uint8_t key);

    void flip_key_status(uint8_t key);

    // Debugging
    void print_keyboard_status();

  private:
    bool key_status[NUM_KEYS];  // Holds pressed status of each key,
                                // true = pressed false = unpressed
};

#endif