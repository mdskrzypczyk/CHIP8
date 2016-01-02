#include "graphics.h"

/*
 * VIDEO
 * Description: Main constructor for VIDEO object.  Sets the pixel width/height
 *				depending on the size of the window
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

VIDEO::VIDEO(){
	pixel_width = WINDOW_WIDTH / SCREEN_WIDTH;
	pixel_height = WINDOW_HEIGHT / SCREEN_HEIGHT;
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

VIDEO::~VIDEO(){
	close();
}

/*
 * init
 * Description: Initializes SDL library and creates window/surface objects
 *				for drawing graphics.
 * Inputs: None
 * Outputs: None
 * Return Value: success - Boolean value indicating whether all components
 *						   were initialized properly or not.
*/

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

/*
 * draw_pixel
 * Description: Function for drawing a pixel at a given (x, y) coordinate pair for
 *				the CHIP8's display.  Because the SDL window is not a 1:1 pixel mapping,
 *				we need to scale the amount of pixels colored in the window based on the
 *				window size.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

void VIDEO::draw_pixel(uint8_t x, uint8_t y, uint32_t rgb){
	uint32_t* pixmem = vid_mem + x*pixel_width + y*pixel_height*WINDOW_WIDTH; //May be an issue with flexible window dimensions

	for(int screen_y = 0; screen_y < (uint8_t)pixel_height; screen_y++){
		for(int screen_x = 0; screen_x < (uint8_t)pixel_width; screen_x++){
			//May be an issue with flexible window dimensions
			*(pixmem + screen_x + screen_y*WINDOW_WIDTH) = rgb;
		}
	}
}

/*
 * xor_color
 * Description: Helper function for the CHIP8's Dxyn instruction, checks the pixel color
 *				at a given CHIP8 (x,y) and XOR's the color.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

bool VIDEO::xor_color(uint8_t x, uint8_t y){
	uint32_t pix_color = *(vid_mem + y*pixel_height*WINDOW_WIDTH + x*pixel_width);

	if(pix_color == background_color){
		draw_pixel(x, y, foreground_color);
		return false;
	}
	else{
		draw_pixel(x, y, background_color);
		return true;
	}
}

/*
 * show
 * Description: Function for updating the display window.  Must be called at 60Hz to emulate
 *				CHIP8 display.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

void VIDEO::show(){
	//Update the surface
	SDL_UpdateWindowSurface( gWindow );
}

/*
 * clear
 * Description: Helper function for clearing the game display.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

void VIDEO::clear(){
	for(int y = 0; y < WINDOW_HEIGHT; y++){
		for(int x = 0; x < WINDOW_WIDTH; x++){
			*(vid_mem + y*WINDOW_WIDTH + x) = background_color;
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

void VIDEO::close(){
	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

uint32_t VIDEO::get_color(uint8_t r, uint8_t g, uint8_t b){
	return SDL_MapRGB(gSurface->format, r, g, b);
}