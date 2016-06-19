#include "audio.h"

AUDIO::AUDIO(){
}

AUDIO::~AUDIO(){
	chip_tone = NULL;


	SDL_Quit();
}

bool AUDIO::init(){
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		std::cout << "Unable to initialize audio.\n" << std::endl;
		return false;
	}


	return true;
}


void AUDIO::play_tone(){
}