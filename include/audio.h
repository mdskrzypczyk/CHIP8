#include <SDL2/SDL.h>
#include <iostream>
#include <queue>
#include <cmath>

#ifndef AUDIO_H
#define AUDIO_H

const Sint16 AMPLITUDE = 2000;
const int FREQUENCY = 44100;

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
    private:
        double v;
        std::queue<BeepObject> beeps;
    public:
        Beeper();
        ~Beeper();
        void beep(double freq, int duration);
        void generateSamples(Sint16 *stream, int length);
        void wait();
};

void audio_callback(void*, Uint8*, int);

class AUDIO{
	public:
		AUDIO();
		
		~AUDIO();

		bool init();

		void play_tone();

	private:
        Beeper b;
};

#endif