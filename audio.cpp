#include "audio.h"

static uint8_t* audio_pos;
static int audio_len;

AUDIO::AUDIO(){
	chip_tone.freq = 44100;
	chip_tone.format = AUDIO_S16;
	chip_tone.channels = 2;
	chip_tone.samples = 1024;
	chip_tone.callback = audio_callback;
	chip_tone.userdata = NULL;
}

AUDIO::~AUDIO(){
	SDL_CloseAudio();
}

bool AUDIO::init(){
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		std::cout << "Unable to initialize audio.\n" << std::endl;
		return false;
	}

	if(SDL_OpenAudio(&chip_tone, NULL) < 0){
		std::cout << "Error opening audio device.\n" << std::endl;
		return false;
	}

	return true;
}

void audio_callback(void* userdata, uint8_t* stream, int len){
	if(audio_len == 0){
		return;
	}

	len = (len > audio_len) ? audio_len : len;

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

	audio_pos += len;
	audio_len -= len;
}

void AUDIO::play_tone(){
	SDL_PauseAudio(0);
}