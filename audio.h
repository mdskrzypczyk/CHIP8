#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

#ifndef AUDIO_H
#define AUDIO_H

void audio_callback(void* userdata, uint8_t* stream, int len);

class AUDIO{
	public:
		AUDIO();
		
		~AUDIO();

		bool init();

		void play_tone();

	private:
		Mix_Chunk* chip_tone;
};

#endif