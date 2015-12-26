#include "graphics.h"

VIDEO::VIDEO(){
	pixel_width = WINDOW_WIDTH / 64;
	pixel_height = WINDOW_HEIGHT / 32;
}

VIDEO::~VIDEO(){}

bool VIDEO::init(){
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "CHIP8-VIDEO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	vid_mem = (uint32_t *)gSurface->pixels;
	clear();
	return success;
}

uint32_t VIDEO::get_color(uint8_t r, uint8_t g, uint8_t b){
	return SDL_MapRGB(gSurface->format, r, g, b);
}

void VIDEO::draw_pixel(uint8_t x, uint8_t y, uint32_t rgb){
	uint32_t* pixmem = vid_mem + x*pixel_width + y*pixel_height*WINDOW_WIDTH; //May be an issue with flexible window dimensions

	for(int screen_y = 0; screen_y < (uint8_t)pixel_height; screen_y++){
		for(int screen_x = 0; screen_x < (uint8_t)pixel_width; screen_x++){
			//May be an issue with flexible window dimensions
			*(pixmem + screen_x + screen_y*WINDOW_WIDTH) = rgb;
		}
	}
}

bool VIDEO::xor_color(uint8_t x, uint8_t y){
	uint32_t pix_color = *(vid_mem + y*pixel_height*WINDOW_WIDTH + x*pixel_width);

	if(pix_color == BLACK){
		draw_pixel(x, y, WHITE);
		return false;
	}
	else{
		draw_pixel(x, y, BLACK);
		return true;
	}
}

void VIDEO::show(){
	//Update the surface
	SDL_UpdateWindowSurface( gWindow );
}

void VIDEO::clear(){
	for(int y = 0; y < WINDOW_HEIGHT; y++){
		for(int x = 0; x < WINDOW_WIDTH; x++){
			*(vid_mem + y*WINDOW_WIDTH + x) = BLACK;
		}
	}
}

void VIDEO::close(){
	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

