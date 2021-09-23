#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>

#include <cmath>
#include <iostream>
#include <queue>

const Sint16 AMPLITUDE = 2000;
const int FREQUENCY = 44100;

/**
 * Simple struct for tracking tone information for use with SDL.
 */
struct BeepObject {
    /**
     * Frequency of tone.
     */
    double freq;

    /**
     * Number of samples left to produce.
     */
    int samplesLeft;
};

/**
 * Class used for interfacing audio module with SDL.  Provides functions
 * for generating audio samples to be used in SDL.
 */
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

/**
 * Module used for verifying initialization of audio-related SDL components.
 */
class AudioInitChecker {
  public:
    bool check_sdl_init_code(int init_code);
    bool check_open_audio_code(int open_audio_code);
    bool check_audio_format(SDL_AudioSpec *want, SDL_AudioSpec *have);
};

/**
 * Module for handling audio-related activities of CHIP 8.  Handles playing
 * tones when CHIP 8 roms instruct so.
 */
class AUDIO {
  public:
    AUDIO();

    ~AUDIO();

    bool init();

    void play_tone();

    Beeper *get_beeper();

  private:
    Beeper b;

    AudioInitChecker init_checker;
};

#endif