#include <SDL2/SDL.h>
#ifndef INPUT_H
#define INPUT_H
#define NUM_KEYS 16

class INPUT{
	public:
		INPUT();

		~INPUT();

		uint8_t poll_keyboard();

		bool get_key_status(uint8_t key);

		void print_keyboard_status();

	private:
		bool key_status[NUM_KEYS];
};

#endif