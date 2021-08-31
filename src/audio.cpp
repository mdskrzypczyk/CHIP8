#include "audio.h"

Beeper::Beeper() {}

Beeper::~Beeper() {}

std::queue<BeepObject> *Beeper::get_beeps() { return &beeps; }

void Beeper::beep(double freq, int duration) {
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

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

bool AudioInitChecker::check_sdl_init_code(int init_code) {
    if (init_code < 0) {
        std::cout << "Unable to initialize audio.\n" << std::endl;
        return false;
    }
    return true;
}

bool AudioInitChecker::check_open_audio_code(int open_audio_code) {
    if (open_audio_code < 0) {
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool AudioInitChecker::check_audio_format(SDL_AudioSpec *want,
                                          SDL_AudioSpec *have) {
    if (have->format != want->format) {
        std::cout << "We didn't get Sint16 audio format." << std::endl;
        return false;
    }
    return true;
}

// LCOV_EXCL_START
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

    // start play audio
    SDL_PauseAudio(0);

    return success;
}

void AUDIO::play_tone() {
    // Excluded from coverage due to soundcard requirements
    b.beep(440, 100);
    b.wait();
}
// LCOV_EXCL_STOP

Beeper *AUDIO::get_beeper() { return &b; }
