#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

#include <cstdio>
#include <ctime>
#include <iostream>
#include <mutex>

#define WINDOW_WIDTH 640  // Window Dimensions
#define WINDOW_HEIGHT 480
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define BLACK 0         // Constants for Black and White to be used
#define WHITE 16777215  // for 32-bit pixel color info in SDL window.
#define INTMAX 4294967296

static std::mutex mtx;

class VideoInitChecker {
  public:
    bool check_sdl_init_code(int init_code);
    bool check_sdl_create_window_code(SDL_Window *window_ptr);
};

class VIDEO {
  public:
    // Constructor for VIDEO object
    VIDEO();

    // Destructor to terminate SDL components
    ~VIDEO();

    // Function to initialize SDL components
    bool init();

    // Function for handling SDL window events
    void handle_event(SDL_Event event);

    // Function for updating surface when window resized
    void switch_surface();

    // Function that stalls Chip-8 when window focus lost
    bool wait_for_focus();

    // Function for drawing the screen per the pixel map
    void draw_pix_map();

    // Function for chaning the Chip-8 color scheme
    void rand_color_scheme();

    // Debugging function
    uint32_t get_color(uint8_t r, uint8_t g, uint8_t b);

    // Helper function for Dxyn CHIP8 instruction
    bool xor_color(uint8_t x, uint8_t y);

    // Function to update SDL window
    void show();

    // Clear SDL display
    void clear();

    // Terminate SDL components
    void close();

    // Draws pixels to the SDL window
    void draw_pixel(uint8_t x, uint8_t y, uint32_t rgb);

    uint32_t get_pixel_width();
    uint32_t get_pixel_height();
    int get_window_width();
    int get_window_height();
    uint32_t get_background_color();
    uint32_t get_foreground_color();

    uint32_t *get_pix_map();

  private:
    VideoInitChecker video_init_checker;
    SDL_Window *gWindow;  // Pointer to SDL window object
    uint32_t pixel_width;    // Pixel dimensions in terms of larger scale window dimensions
    uint32_t pixel_height;
    int gWidth;
    int gHeight;
    SDL_Surface *gSurface;     // Main surface we draw to
    SDL_Surface *gBackground;  // Background
    uint32_t *vid_mem;         // Pointer to beginning of video memory for SDL
    uint32_t pix_map[SCREEN_HEIGHT]
                    [SCREEN_WIDTH];  // Pixel map containing pixel
                                     // representation of Chip-8 screen
    uint32_t background_color;       // Background color for surface
    uint32_t foreground_color;       // Foreground color for surface
};

#endif