//#include "chip8.h"
#include "input.h"
//#include <SDL2/SDL.h>
//#include <unistd.h>
int main(int argc, char* argv[]){
	//CHIP8 myChip8 = CHIP8();
	//myChip8.load_program(argv[1]);
	//myChip8.mainloop();
	//Initialize SDL
	SDL_Window* gWindow;
	//SDL_Surface* gSurface;
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "CHIP8-VIDEO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get window surface
			//gSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	INPUT myInput = INPUT();
	while(true){
		myInput.poll_keyboard();
	}
	return 0;
}

