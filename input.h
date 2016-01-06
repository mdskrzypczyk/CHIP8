#include <SDL2/SDL.h>
#ifndef INPUT_H
#define INPUT_H
#define NUM_KEYS 16		//Chip 8 has 16 hexadecimal keys on its keyboard

class INPUT{
	public:
		//Constructor function
		INPUT();

		//Destructor function
		~INPUT();

		//Function for updating key_status boolean array, checks the keyboard for pressed/unpressed keys
		uint8_t poll_keyboard(SDL_Event event);

		//Function that returns pressed state of a given key
		bool get_key_status(uint8_t key);

		//Debugging
		void print_keyboard_status();

	private:
		bool key_status[NUM_KEYS];	//Holds pressed status of each key, true = pressed false = unpressed
};

#endif