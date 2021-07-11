#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

#include <cmath>
#include <iostream>
#include <queue>

const Sint16 AMPLITUDE = 2000;
const int FREQUENCY = 44100;

struct BeepObject {
    double freq;
    int samplesLeft;
};

class Beeper {
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

void audio_callback(void *_beeper, Uint8 *_stream, int _length);

class AUDIO {
  public:
    AUDIO();

    ~AUDIO();

    bool init();

    void play_tone();

  private:
    Beeper b;
};

#endif