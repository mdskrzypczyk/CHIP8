#include "input.h"

/*
 * INPUT
 * Description: Constructor for CHIPINPUT object, initializes all keys to
 *				unpressed state.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

INPUT::INPUT() {
    for (int i = 0; i < NUM_KEYS; i++) {
        key_status[i] = false;
    }
}

/*
 * ~INPUT
 * Description: Empty destructor for CHIPINPUT
 * Inputs: None
 * Outputs: None
 * Return Value: None
 */

INPUT::~INPUT() {}

/*
 * poll_keyboard
 * Description: Updates the key status array with up/down presses on a key
 * Inputs: None
 * Outputs: None
 * Return Value: The hexadecimal value of the key that was pressed, otherwise -1
 *				 if no keys were pressed
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

/*
 * get_key_status
 * Description: Pulls key status information from status array
 * Inputs: None
 * Outputs: None
 * Return Value: Boolean representings if key is pressed (true) or not (false)
 */

bool INPUT::get_key_status(uint8_t key) { return key_status[key]; }

void INPUT::flip_key_status(uint8_t key) { key_status[key] = !key_status[key]; }

// Debugging
void INPUT::print_keyboard_status() {
    for (int i = 0; i < NUM_KEYS; i++) {
        if (key_status[i]) {
            printf("%d: %d\n", i, static_cast<int>(key_status[i]));
        }
    }
}
