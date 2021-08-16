#include "graphics.h"

bool VideoInitChecker::check_sdl_init_code(int init_code) {
    if(init_code < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
};

bool VideoInitChecker::check_sdl_create_window_code(SDL_Window *window_ptr) {
    if(window_ptr == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n",
               SDL_GetError());
        return false;
    }
    return true;
};

/*
 * VIDEO
 * Description: Main constructor for VIDEO object.  Sets the pixel width/height
 *				depending on the size of the window
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

VIDEO::VIDEO() {
    // Set default values
    pixel_width = WINDOW_WIDTH / SCREEN_WIDTH;
    pixel_height = WINDOW_HEIGHT / SCREEN_HEIGHT;
    gWidth = WINDOW_WIDTH;
    gHeight = WINDOW_HEIGHT;
    background_color = BLACK;
    foreground_color = WHITE;
}

/*
 * ~VIDEO
 * Description: Main destructor for VIDEO object.  Terminates SDL program.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

VIDEO::~VIDEO() { close(); }

/*
 * init
 * Description: Initializes SDL library and creates window/surface objects
 *				for drawing graphics.
 * Inputs: None
 * Outputs: None
 * Return Value: success - Boolean value indicating whether all components
 *				 were initialized properly or not.
 */

bool VIDEO::init() {
    // Initialization flag
    bool success = true;

    // Initialize SDL
    int init_code = SDL_Init(SDL_INIT_VIDEO);
    success = success && video_init_checker.check_sdl_init_code(init_code);
    if (success) {
        // Create window
        gWindow = SDL_CreateWindow(
                "CHIP-8 Interpretter", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        success = success && video_init_checker.check_sdl_create_window_code(gWindow);
        if (success) {
            SDL_SetWindowMinimumSize(gWindow, SCREEN_WIDTH * 4,
                                     SCREEN_HEIGHT * 4);
            // Get window surface
            gSurface = SDL_GetWindowSurface(gWindow);
            vid_mem = (uint32_t *) gSurface->pixels;
            clear();
        }
    }

    return success;
}

/*
 * handle_event
 * Description: Function for handling SDL Window events.
 * Inputs: event - The SDL_Event that occurred
 * Outputs: None
 * Return Value: None
 */

void VIDEO::handle_event(SDL_Event event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
            // Check for window resize
            case SDL_WINDOWEVENT_RESIZED:
                // Update window dimensions
                gWidth = event.window.data1;
                gHeight = event.window.data2;

                // Update pixel dimensions to fit new window
                pixel_width = gWidth / SCREEN_WIDTH;
                pixel_height = gHeight / SCREEN_HEIGHT;

                // Update the surface for the modified window
                switch_surface();
                show();
                break;

            case SDL_WINDOWEVENT_FOCUS_LOST:
                while (!wait_for_focus()) {
                }
                break;

            case SDL_WINDOWEVENT_MINIMIZED:
                while (!wait_for_focus()) {
                }
                show();
                break;
        }
    }
}

/*
 * switch_surface
 * Description: Function for acquiring window surface and redrawing
 *				the pixel map for resizing.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::switch_surface() {
    gSurface = SDL_GetWindowSurface(gWindow);  // Grab new window surface
    if (gSurface != nullptr) {
        vid_mem =
                (uint32_t *) gSurface->pixels;  // Get new video memory pointer
        draw_pix_map();                         // Redraw the surface
    }
}

/*
 * wait_for_focus
 * Description: Function that while-loops while waiting for window to regain
 * focus, keeps interpretter from proceeding while user not watching
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

bool VIDEO::wait_for_focus() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        return (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED);
    }
    return false;
}

/*
 * draw_pix_map
 * Description: Function for redrawing the surface using pixel map
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::draw_pix_map() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            draw_pixel(x, y, pix_map[y][x]);
        }
    }
}

uint32_t VIDEO::get_pixel_width() { return pixel_width; }
uint32_t VIDEO::get_pixel_height() { return pixel_height; }
int VIDEO::get_window_width() { return gWidth; }
int VIDEO::get_window_height() { return gHeight; }
uint32_t VIDEO::get_background_color() { return background_color; }
uint32_t VIDEO::get_foreground_color() { return foreground_color; }

uint32_t *VIDEO::get_pix_map() { return pix_map[0]; }

/*
 * rand_color_scheme
 * Description: Function for randomizing the color scheme of the Chip-8.
 * Repaints the screen.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::rand_color_scheme() {
    mtx.lock();
    srand(time(nullptr));

    // Randomly select 32-bit values for color
    uint32_t newforeground_color = rand() % INTMAX;
    uint32_t newbackground_color = rand() % INTMAX;

    // Iterate through each pixel and update with new color
    uint32_t *pixel;
    for (int i = 0; i < gWidth * gHeight; i++) {
        pixel = (vid_mem + i);
        if (*pixel == foreground_color) {
            *pixel = newforeground_color;
        } else {
            *pixel = newbackground_color;
        }
    }

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (pix_map[y][x] == foreground_color) {
                pix_map[y][x] = newforeground_color;
            } else if (pix_map[y][x] == background_color) {
                pix_map[y][x] = newbackground_color;
            }
        }
    }

    // Update the colors
    foreground_color = newforeground_color;
    background_color = newbackground_color;

    mtx.unlock();
}

/*
 * draw_pixel
 * Description: Function for drawing a pixel at a given (x, y) coordinate pair
 * for the CHIP8's display.  Because the SDL window is not a 1:1 pixel mapping,
 * we need to scale the amount of pixels colored in the window based on the
 * window size.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::draw_pixel(uint8_t x, uint8_t y, uint32_t rgb) {
    // Do not draw pixels out of screen
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }

    // Get pointer to first pixel of surface
    uint32_t *pixmem = vid_mem + x * pixel_width + y * pixel_height * gWidth;

    // Iterate through pixels that make up larger Chip-8 pixel and color
    for (int screen_y = 0; screen_y < (int) pixel_height; screen_y++) {
        for (int screen_x = 0; screen_x < (int) pixel_width; screen_x++) {
            *(pixmem + screen_x + screen_y * gWidth) = rgb;
        }
    }
}

/*
 * xor_color
 * Description: Helper function for the CHIP8's Dxyn instruction, checks the
 * pixel color at a given CHIP8 (x,y) and XOR's the color.
 * Inputs: None
 * Outputs: None
 * Return Value: true/false - Indicates whether information on the screen was
 * deleted
 */

bool VIDEO::xor_color(uint8_t x, uint8_t y) {
    // Do not draw pixels out of screen
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return false;
    }
    mtx.lock();
    // Grab the appropriate pixel color from the pixel map
    uint32_t pix_color = pix_map[y][x];

    // Flip foreground/background color
    bool ret = true;
    if (pix_color == background_color) {
        draw_pixel(x, y, foreground_color);
        pix_map[y][x] = foreground_color;
        ret = false;  // Information not deleted
    } else if (pix_color == foreground_color) {
        draw_pixel(x, y, background_color);
        pix_map[y][x] = background_color;
        ret = true;  // Information deleted
    }

    mtx.unlock();
    return ret;
}

/*
 * show
 * Description: Function for updating the display window.  Must be called at
 * 60Hz to emulate CHIP8 display.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::show() {
    // Update the surface
    SDL_UpdateWindowSurface(gWindow);
}

/*
 * clear
 * Description: Helper function for clearing the game display.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::clear() {
    // Clear the display
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            *(vid_mem + y * WINDOW_WIDTH + x) = background_color;
        }
    }

    // Clear the pixel map
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            pix_map[y][x] = background_color;
        }
    }
}

/*
 * close
 * Description: Helper function to terminate SDL window and free resources.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

void VIDEO::close() {
    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

uint32_t VIDEO::get_color(uint8_t r, uint8_t g, uint8_t b) {
    return SDL_MapRGB(gSurface->format, r, g, b);
}