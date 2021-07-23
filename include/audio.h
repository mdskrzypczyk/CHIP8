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
    std::queue<BeepObject> *get_beeps();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

void audio_callback(void *_beeper, Uint8 *_stream, int _length);

class AudioInitChecker {
  public:
    bool check_sdl_init_code(int init_code);
    bool check_open_audio_code(int open_audio_code);
    bool check_audio_format(SDL_AudioSpec *want, SDL_AudioSpec *have);
};

class AUDIO {
  public:
    AUDIO();

    ~AUDIO();

    bool init();

    void play_tone();

    Beeper *get_beeper();
    void set_beeper(Beeper *beeper);

  private:
    Beeper b;

    AudioInitChecker init_checker;
};

#endif