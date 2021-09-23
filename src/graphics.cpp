#include "graphics.h"

/**
 * Checks the status code returned from initializing graphics in SDL.
 * @param init_code Status code returned from SDL
 * @return Boolean specifying if initialization was successful or not.
 */
bool VideoInitChecker::check_sdl_init_code(int init_code) {
    if (init_code < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
};

/**
 * Checks the SDL_Window pointer returned from creating a window in SDL.
 * @param window_ptr Pointer to SDL_Window object used for graphics displayed.
 * @return Boolean specifying if creation was successful or not.
 */
bool VideoInitChecker::check_sdl_create_window_code(SDL_Window *window_ptr) {
    if (window_ptr == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
};

/**
 * Main constructor for VIDEO object.  Sets the pixel width/height depending on
 * the size of the window
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

/**
 * Main destructor for VIDEO object.  Terminates SDL program.
 */
VIDEO::~VIDEO() { close(); }

// LCOV_EXCL_START
/**
 * Initializes SDL library and creates window/surface objects for drawing
 * graphics.
 * @return Boolean value indicating whether all components were initialized
 * properly or not.
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
        success = success &&
                  video_init_checker.check_sdl_create_window_code(gWindow);
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
// LCOV_EXCL_STOP

/**
 * Function for handling SDL Window events.
 */

// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

// LCOV_EXCL_START
/**
 * Function for acquiring window surface and redrawing the pixel map for
 * resizing.
 */
void VIDEO::switch_surface() {
    gSurface = SDL_GetWindowSurface(gWindow);  // Grab new window surface
    if (gSurface != nullptr) {
        vid_mem =
                (uint32_t *) gSurface->pixels;  // Get new video memory pointer
        draw_pix_map();                         // Redraw the surface
    }
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
/**
 * Function that while-loops while waiting for window to regain focus, keeps
 * interpretter from proceeding while user not watching
 */
bool VIDEO::wait_for_focus() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        return (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED);
    }
    return false;
}
// LCOV_EXCL_STOP

/**
 * Function for redrawing the surface using pixel map
 */
void VIDEO::draw_pix_map() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            draw_pixel(x, y, pix_map[y][x]);
        }
    }
}

/**
 * Getter function for width of pixels in SDL window.
 * @return Number of window pixels corresponding to CHIP 8 pixel width.
 */
uint32_t VIDEO::get_pixel_width() { return pixel_width; }

/**
 * Getter function for height of pixels in SDL window.
 * @return Number of window pixels corresponding to CHIP 8 pixel height.
 */
uint32_t VIDEO::get_pixel_height() { return pixel_height; }

/**
 * Getter function for width of SDL window in pixels.
 * @return Number of pixels in width of window.
 */
int VIDEO::get_window_width() { return gWidth; }

/**
 * Getter function for height of SDL window in pixels.
 * @return Number of pixels in height of window.
 */
int VIDEO::get_window_height() { return gHeight; }

/**
 * Getter function for background color code.
 * @return Unsigned 32-bit integer representing color code.
 */
uint32_t VIDEO::get_background_color() { return background_color; }

/**
 * Getter function for foreground color code.
 * @return Unsigned 32-bit integer representing color code.
 */
uint32_t VIDEO::get_foreground_color() { return foreground_color; }

/**
 * Getter function for pixel map used to represent CHIP 8  screen internally.
 * @return Pointer to array of pixels.
 */
uint32_t (*VIDEO::get_pix_map())[SCREEN_WIDTH] { return pix_map; }

/**
 * Getter function for video memory used by SDL window to display graphics.
 * @return Pointer to array of video memory.
 */
uint32_t *VIDEO::get_vid_mem() { return vid_mem; }

/**
 * Function for randomizing the color scheme of the Chip-8.
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

/**
 * Function for drawing a pixel at a given (x, y) coordinate pair for the
 * CHIP8's display.  Because the SDL window is not a 1:1 pixel mapping, we need
 * to scale the amount of pixels colored in the window based on the window size.
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

/**
 * Helper function for the CHIP8's Dxyn instruction, checks the pixel color at
 * a given CHIP8 (x,y) and XOR's the color.
 * @return Boolean indicating whether information on the screen was deleted
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

// LCOV_EXCL_START
/**
 * Function for updating the display window.  Must be called at 60Hz to emulate
 * CHIP8 display.
 */
void VIDEO::show() {
    // Update the surface
    SDL_UpdateWindowSurface(gWindow);
}
// LCOV_EXCL_STOP

/**
 * Helper function for clearing the game display.
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

/**
 * Helper function to terminate SDL window and free resources.
 */
void VIDEO::close() {
    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

/**
 * Converts an RGB expression of a color to one SDL understands.
 * @param r Red level in color, 0-255
 * @param g Green level in color, 0-255
 * @param b Blue level in color, 0-255
 * @return Unsigned 32-bit integer color code for SDL
 */
uint32_t VIDEO::get_color(uint8_t r, uint8_t g, uint8_t b) {
    return SDL_MapRGB(gSurface->format, r, g, b);
}