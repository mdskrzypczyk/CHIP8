#include "audio.h"

Beeper::Beeper(){
}

Beeper::~Beeper(){
}

void Beeper::generateSamples(Sint16 *stream, int length){
    int i = 0;
    while (i < length) {

        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        BeepObject& bo = beeps.front();

        int samplesToDo = std::min(i + bo.samplesLeft, length);
        bo.samplesLeft -= samplesToDo - i;

        while (i < samplesToDo) {
            stream[i] = AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);
            i++;
            v += bo.freq;
        }

        if (bo.samplesLeft == 0) {
            beeps.pop();
        }
    }
}

void Beeper::beep(double freq, int duration){
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

void Beeper::wait(){
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);

}

void audio_callback(void *_beeper, Uint8 *_stream, int _length){
    Sint16 *stream = (Sint16*) _stream;
    int length = _length / 2;
    Beeper* beeper = (Beeper*) _beeper;

    beeper->generateSamples(stream, length);
}

AUDIO::AUDIO(){
    b = Beeper();
}

AUDIO::~AUDIO(){
    SDL_CloseAudio();
	SDL_Quit();
}

bool AUDIO::init(){
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		std::cout << "Unable to initialize audio.\n" << std::endl;
		return false;
	}

    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    want.freq = 48000;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 4096;
    want.callback = audio_callback;
    want.userdata = this;

    if (SDL_OpenAudio(&want, &have) < 0) {
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
    if (have.format != want.format) {
        std::cout << "We didn't get Sint16 audio format." << std::endl;
    }

    // start play audio
    SDL_PauseAudio(0);

    return true;
}


void AUDIO::play_tone(){
    b.beep(440, 100);
    b.wait();
}