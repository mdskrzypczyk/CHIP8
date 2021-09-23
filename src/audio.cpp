#include "audio.h"

Beeper::Beeper() {}

Beeper::~Beeper() {}

/**
 * Returns the internal buffer of BeepObjects that are used for generating
 * samples to pass to the audio backend.
 *
 * @return A queue of BeepObjects.
 */
std::queue<BeepObject> *Beeper::get_beeps() { return &beeps; }

/**
 * Updates the internal BeepObject used for generating samples.
 *
 * @param freq The frequency of the tone to produce.
 * @param duration The duration (in ms) to play the tone for.
 */
void Beeper::beep(double freq, int duration) {
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

/**
 * Generates samples to pass to the audio backend for producing sound.
 *
 * @param stream Pointer to a signed 16-bit array that stores the samples
 * @param length Length of the stream to produce.
 */
void Beeper::generateSamples(Sint16 *stream, int length) {
    int i = 0;
    while (i < length) {
        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        BeepObject &bo = beeps.front();

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

// LCOV_EXCL_START
/**
 * Waits until internal queue of BeepObjects has been emptied.
 */
void Beeper::wait() {
    // Excluded from coverage due to soundcard requirements
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);
}

/**
 * Audio callback used by SDL to generate samples and pass data to soundcard.
 * @param _beeper Pointer to Beeper object in audio module.
 * @param _stream Pointer to audio stream that should be populated by Beeper.
 * @param _length Number of samples to generate in stream.
 */
void audio_callback(void *_beeper, Uint8 *_stream, int _length) {
    // Excluded from coverage due to soundcard requirements
    Sint16 *stream = (Sint16 *) _stream;
    int length = _length / 2;
    Beeper *beeper = (Beeper *) _beeper;

    beeper->generateSamples(stream, length);
}
// LCOV_EXCL_STOP

AUDIO::AUDIO() {
    b = Beeper();
    init_checker = AudioInitChecker();
}

AUDIO::~AUDIO() {
    SDL_CloseAudio();
    SDL_Quit();
}

/**
 * Checks the status code returned from initializing audio in SDL.
 * @param init_code Status code returned from SDL
 * @return Boolean specifying if initialization was successful or not.
 */
bool AudioInitChecker::check_sdl_init_code(int init_code) {
    if (init_code < 0) {
        std::cout << "Unable to initialize audio.\n" << std::endl;
        return false;
    }
    return true;
}

/**
 * Checks the status code returned from opening the audio device using SDL.
 * @param open_audio_code Status code returned from Open Audio.
 * @return Boolean specifying if opening audio device was successful or not.
 */
bool AudioInitChecker::check_open_audio_code(int open_audio_code) {
    if (open_audio_code < 0) {
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

/**
 * Verifies that the SDL AudioSpec obtained matches our desired configuration.
 * @param want SDL_AudioSpec of our desired audio settings.
 * @param have SDL_AudioSpec of obtained audio settings.
 * @return Boolean specifying whether obtained audio settings match desired.
 */
bool AudioInitChecker::check_audio_format(SDL_AudioSpec *want,
                                          SDL_AudioSpec *have) {
    if (have->format != want->format) {
        std::cout << "We didn't get Sint16 audio format." << std::endl;
        return false;
    }
    return true;
}

// LCOV_EXCL_START

/**
 * Initializer for the audio component of CHIP 8.
 *
 * Runs the AudioInitChecker to make sure audio playback is supported by the
 * backend.
 */
bool AUDIO::init() {
    // Excluded from coverage due to soundcard requirements
    bool success = true;
    int init_code = SDL_Init(SDL_INIT_AUDIO);
    success = success && init_checker.check_sdl_init_code(init_code);

    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    want.freq = 48000;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 4096;
    want.callback = audio_callback;
    want.userdata = &b;

    int open_audio_code = SDL_OpenAudio(&want, &have);
    success = success && init_checker.check_open_audio_code(open_audio_code);
    success = success && init_checker.check_audio_format(&want, &have);

    // Start play audio
    SDL_PauseAudio(0);

    return success;
}

/**
 * Causes the internal Beeper object to beep.
 */
void AUDIO::play_tone() {
    // Excluded from coverage due to soundcard requirements
    b.beep(440, 100);
    b.wait();
}
// LCOV_EXCL_STOP

/**
 * Returns the beeper object inside the AUDIO module.
 *
 * @return The beeper object used for sounds.
 */
Beeper *AUDIO::get_beeper() { return &b; }
