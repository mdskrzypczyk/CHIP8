#include "input.h"

#include "gtest/gtest.h"

TEST(InputTests, TestConstructor) {
    INPUT input = INPUT();

    // Check that key status is initialized to false
    for (int i = 0; i < NUM_KEYS; i++) {
        EXPECT_EQ(input.get_key_status(i), false);
    }
}

TEST(InputTests, TestKeymapAllChip8KeysPresent) {
    std::map<uint8_t, bool> c8_keys;
    uint8_t num_keys = 16;

    // Track which keys we have found mapped
    for (uint8_t i = 0; i < num_keys; i++) {
        c8_keys[i] = false;
    }

    // Iterate over keys in keymap
    std::map<SDL_Keycode, uint8_t>::iterator it;

    for (it = keymap.begin(); it != keymap.end(); it++) {
        uint8_t value = it->second;

        // Make sure no SDL_Keycode or CHIP-8 key is
        // only present once
        EXPECT_EQ(c8_keys[value], false);
        c8_keys[value] = true;
    }

    // Check that all keys were mapped to
    for (uint8_t i = 0; i < num_keys; i++) {
        EXPECT_EQ(c8_keys[i], true);
    }
}

TEST(InputTests, TestKeymapDuplicates) {
    std::map<SDL_Keycode, int> extracted_sdl_keycodes;
    std::map<uint8_t, int> extracted_c8_keys;
    std::map<SDL_Keycode, uint8_t>::iterator it;

    // Make sure no SDL_Keycode or CHIP-8 key is
    // only present once
    for (it = keymap.begin(); it != keymap.end(); it++) {
        SDL_Keycode key = it->first;
        uint8_t value = it->second;

        EXPECT_EQ(extracted_sdl_keycodes.count(key), 0);
        extracted_sdl_keycodes[key] = 1;
        EXPECT_EQ(extracted_c8_keys.count(value), 0);
        extracted_c8_keys[value] = 1;
    }
}

TEST(InputTests, TestHandleKeyDownKeyUp) {
    INPUT input = INPUT();
    std::map<SDL_Keycode, uint8_t>::iterator it;

    // Make sure no SDL_Keycode or CHIP-8 key is
    // only present once
    for (it = keymap.begin(); it != keymap.end(); it++) {
        SDL_Keycode key = it->first;
        uint8_t value = it->second;

        // Check that initially all keys are set to false
        for (uint8_t i = 0; i <= 0xF; i++) {
            EXPECT_EQ(input.get_key_status(i), false);
        }

        // Keydown
        uint8_t ret = input.handle_keydown(key);
        EXPECT_EQ(ret, value);

        // Check only this key changed
        for (uint8_t i = 0; i <= 0xF; i++) {
            if (i == value) {
                EXPECT_EQ(input.get_key_status(i), true);
            } else {
                EXPECT_EQ(input.get_key_status(i), false);
            }
        }

        // Keydown
        ret = input.handle_keyup(key);
        EXPECT_EQ(ret, value);

        // Check all keys are still not set
        for (uint8_t i = 0; i <= 0xF; i++) {
            EXPECT_EQ(input.get_key_status(i), false);
        }
    }
}

TEST(InputTests, TestFlipKeyStatus) {
    INPUT input = INPUT();

    for (uint8_t i = 0; i <= 0xF; i++) {
        EXPECT_EQ(input.get_key_status(i), false);
        input.flip_key_status(i);
        EXPECT_EQ(input.get_key_status(i), true);
    }
}

TEST(InputTests, TestPollKeyboard) {
    // Create SDL Event
    SDL_Keysym keysym;
    keysym.sym = SDLK_1;
    SDL_KeyboardEvent key;
    key.keysym = keysym;
    key.repeat = 0;
    SDL_Event event;
    event.key = key;
    event.type = SDL_KEYDOWN;

    // Set up the input and feed the SDL event in
    INPUT input = INPUT();
    uint8_t ret = input.poll_keyboard(event);

    // Check key was pressed
    EXPECT_EQ(ret, keymap[keysym.sym]);
    EXPECT_EQ(input.get_key_status(ret), true);

    // Change to a keyup event
    event.type = SDL_KEYUP;
    ret = input.poll_keyboard(event);

    // Check that key was unpressed
    EXPECT_EQ(ret, keymap[keysym.sym]);
    EXPECT_EQ(input.get_key_status(ret), false);

    // Check the quit button
    event.type = SDL_QUIT;
    ret = input.poll_keyboard(event);
    EXPECT_EQ(ret, keymap[SDL_QUIT]);
}