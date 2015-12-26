#include <SDL2/SDL.h>
#include <stdio.h>
#ifndef GRAPHICS_H
#define GRAPHICS_H
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define BLACK 0
#define WHITE 16777215
 	
class VIDEO{
	public:
		VIDEO();

		~VIDEO();

		bool init();

		uint32_t get_color(uint8_t r, uint8_t g, uint8_t b);

		bool xor_color(uint8_t x, uint8_t y);

		void show();

		void clear();

		void close();

		void draw_pixel(uint8_t x, uint8_t y, uint32_t rgb);

		void update_pixel_dimensions();

		uint32_t pixel_width;
		uint32_t pixel_height;

	private:
		SDL_Window* gWindow;
		SDL_Surface* gSurface;
		SDL_Surface* gBackground;
		uint32_t* vid_mem;
};

#endif