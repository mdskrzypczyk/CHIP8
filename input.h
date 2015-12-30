#include <SDL2/SDL.h>
#ifndef INPUT_H
#define INPUT_H
#define NUM_KEYS 16

class INPUT{
	public:
		INPUT();

		~INPUT();

		void poll_keyboard();

		bool get_key_status(uint8_t key);

	private:
		bool key_status[NUM_KEYS];
};

#endif