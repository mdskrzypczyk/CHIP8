#include "audio.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(BeeperTests, TestBeep) {
    Beeper beeper = Beeper();
    std::queue<BeepObject> *beeps = beeper.get_beeps();

    EXPECT_EQ(beeps->empty(), true);

    double freq = 3.14;
    int duration = 42;
    beeper.beep(freq, duration);

    EXPECT_EQ(beeps->empty(), false);

    BeepObject bo = beeps->front();

    EXPECT_EQ(bo.freq, freq);
    EXPECT_EQ(bo.samplesLeft, duration * FREQUENCY / 1000);
}

TEST(BeeperTests, TestGenerateSamples) {
    Beeper beeper = Beeper();
    std::queue<BeepObject> *beeps = beeper.get_beeps();

    EXPECT_EQ(beeps->empty(), true);

    int stream_length = 30;
    Sint16 stream[stream_length];

    beeper.generateSamples(stream, stream_length);

    for (int i = 0; i < stream_length; i++) {
        EXPECT_EQ(stream[i], 0);
    }

    double freq = 3.14;
    int duration = 42;
    beeper.beep(freq, duration);

    EXPECT_EQ(beeps->empty(), false);

    beeper.generateSamples(stream, stream_length);

    EXPECT_EQ(beeps->empty(), false);

    BeepObject &bo = beeps->front();
    bo.samplesLeft = 0;

    beeper.generateSamples(stream, stream_length);
    EXPECT_EQ(beeps->empty(), true);
}

TEST(BeeperTests, DISABLED_TestWait) {
    AUDIO audio = AUDIO();

    bool init_success = audio.init();
    EXPECT_EQ(init_success, true);

    Beeper *beeper = audio.get_beeper();
    std::queue<BeepObject> *beeps = beeper->get_beeps();
    EXPECT_EQ(beeps->empty(), true);

    beeper->wait();
    EXPECT_EQ(beeps->empty(), true);

    double freq = 3.14;
    int duration = 42;
    beeper->beep(freq, duration);
    EXPECT_EQ(beeps->empty(), false);
}

TEST(AudioInitChecker, TestChecks) {
    AudioInitChecker checker;

    int code = 0;
    bool status = checker.check_sdl_init_code(code);
    EXPECT_EQ(status, true);

    code = -1;
    status = checker.check_sdl_init_code(code);
    EXPECT_EQ(status, false);

    code = 0;
    status = checker.check_open_audio_code(code);
    EXPECT_EQ(status, true);

    code = -1;
    status = checker.check_open_audio_code(code);
    EXPECT_EQ(status, false);

    SDL_AudioSpec want, have;

    SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    want.freq = 48000;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 4096;
    want.callback = audio_callback;
    want.userdata = this;

    SDL_memset(&have, 0, sizeof(have)); /* or SDL_zero(want) */
    have.freq = 48000;
    have.format = AUDIO_S16SYS;
    have.channels = 2;
    have.samples = 4096;
    have.callback = audio_callback;
    have.userdata = this;

    status = checker.check_audio_format(&want, &have);
    EXPECT_EQ(status, true);

    want.format = AUDIO_S32SYS;
    status = checker.check_audio_format(&want, &have);
    EXPECT_EQ(status, false);
}

TEST(AUDIOTests, TestConstructor) {
    AUDIO audio = AUDIO();

    Beeper *beeper = audio.get_beeper();

    EXPECT_NE(beeper, nullptr);
}

TEST(AUDIOTests, DISABLED_TestInit) {
    AUDIO audio = AUDIO();

    bool init_success = audio.init();
    EXPECT_EQ(init_success, true);
}

TEST(AUDIOTests, DISABLED_TestPlayTone) {
    AUDIO audio = AUDIO();

    bool init_success = audio.init();
    EXPECT_EQ(init_success, true);

    Beeper *beeper = audio.get_beeper();
    std::queue<BeepObject> *beeps = beeper->get_beeps();
    EXPECT_EQ(beeps->empty(), true);

    audio.play_tone();
    EXPECT_EQ(beeps->empty(), true);
}
