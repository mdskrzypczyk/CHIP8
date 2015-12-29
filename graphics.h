#include <SDL2/SDL.h>
#include <stdio.h>
#ifndef GRAPHICS_H
#define GRAPHICS_H
#define WINDOW_WIDTH 640	//Window Dimensions
#define WINDOW_HEIGHT 480
#define BLACK 0				//Constants for Black and White to be used
#define WHITE 16777215		//for 32-bit pixel color info in SDL window.
 	
class VIDEO{
	public:
		//Constructor for VIDEO object
		VIDEO();

		//Destructor to terminate SDL components
		~VIDEO();

		//Function to initialize SDL components
		bool init();

		//Debugging function
		uint32_t get_color(uint8_t r, uint8_t g, uint8_t b);

		//Helper function for Dxyn CHIP8 instruction
		bool xor_color(uint8_t x, uint8_t y);

		//Function to update SDL window
		void show();

		//Clear SDL display
		void clear();

		//Terminate SDL components
		void close();

		//Draws pixels to the SDL window
		void draw_pixel(uint8_t x, uint8_t y, uint32_t rgb);

		uint32_t pixel_width;
		uint32_t pixel_height;

	private:
		SDL_Window* gWindow;		//Pointer to SDL window object
		SDL_Surface* gSurface;		//Main surface we draw to
		SDL_Surface* gBackground;	//Background
		uint32_t* vid_mem;			//Pointer to beginning of video memory for SDL
};

#endif