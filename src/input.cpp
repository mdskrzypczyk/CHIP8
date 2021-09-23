#include "input.h"

/**
 * Constructor for CHIPINPUT object, initializes all keys to unpressed state.
 */
INPUT::INPUT() {
    for (int i = 0; i < NUM_KEYS; i++) {
        key_status[i] = false;
    }
}

/**
 * Empty destructor for CHIPINPUT
 */
INPUT::~INPUT() {}

/**
 * Updates the key status array with up/down presses on a key
 *
 * @return The hexadecimal value of the key that was pressed, otherwise -1 if no
 * keys were pressed.
 */
uint8_t INPUT::poll_keyboard(SDL_Event event) {
    uint8_t return_val = 0xFF;

    // For handling key presses
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        return_val = handle_keydown(event.key.keysym.sym);
    }

    // Sets the key status to unpressed
    else if (event.type == SDL_KEYUP && event.key.repeat == 0) {
        return_val = handle_keyup(event.key.keysym.sym);
    }

    // Handles clicking 'x' for window
    else if (event.type == SDL_QUIT) {
        return_val = 16;
    }

    return return_val;
}

/**
 * Handles pressing of keyboard keys and updates key status internally.
 * @param sym SDL_Keycode of key pressed.
 * @return The CHIP 8 hex keyboard value pressed.
 */
uint8_t INPUT::handle_keydown(SDL_Keycode sym) {
    if (keymap.count(sym) == 0) {
        return KEY_ERR;
    } else {
        uint8_t key = keymap[sym];

        // Only track key status for Chip-8 input
        if (key <= KEY_F) {
            key_status[key] = true;
        }

        return key;
    }
}

/**
 * Handles unpressing of keyboard keys and updates key status internally.
 * @param sym SDL_Keycode of key unpressed.
 * @return The CHIP 8 hex keyboard value unpressed.
 */
uint8_t INPUT::handle_keyup(SDL_Keycode sym) {
    if (keymap.count(sym) == 0) {
        return KEY_ERR;
    } else {
        uint8_t key = keymap[sym];

        // Only track key status for Chip-8 input
        if (key <= KEY_F) {
            key_status[key] = false;
        }

        return key;
    }
}

/**
 * Pulls key status information from status array
 * @return Boolean representings if key is pressed (true) or not (false)
 */
bool INPUT::get_key_status(uint8_t key) { return key_status[key]; }

/**
 * Flips key status information in status array.
 */
void INPUT::flip_key_status(uint8_t key) { key_status[key] = !key_status[key]; }

/**
 * Debugging function for printing the keyboard status array.
 */
void INPUT::print_keyboard_status() {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (key_status[i]) {
            printf("%d: %d\n", i, static_cast<int>(key_status[i]));
        }
    }
}
