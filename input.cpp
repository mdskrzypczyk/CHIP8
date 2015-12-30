#include "input.h"

INPUT::INPUT(){
	for(int i = 0; i < NUM_KEYS; i++){
		key_status[i] = false;
	}
}

INPUT::~INPUT(){}

void INPUT::poll_keyboard(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		printf("polling...\n");
		switch(event.type){
			case SDL_KEYDOWN:
				printf("Key pressed: %d", event.key.keysym.sym);
				switch(event.key.keysym.sym){
					case SDLK_1: key_status[0] = true;
							   break;
					case SDLK_2: key_status[1] = true;
							   break;
					case SDLK_3: key_status[2] = true;
							   break;
					case SDLK_4: key_status[3] = true;
							   break;
					case SDLK_q: key_status[4] = true;
							   break;
					case SDLK_w: key_status[5] = true;
							   break;
					case SDLK_e: key_status[6] = true;
							   break;
					case SDLK_r: key_status[7] = true;
							   break;
					case SDLK_a: key_status[8] = true;
							   break;
					case SDLK_s: key_status[9] = true;
							   break;
					case SDLK_d: key_status[10] = true;
							   break;
					case SDLK_f: key_status[11] = true;
							   break;
					case SDLK_z: key_status[12] = true;
							   break;
					case SDLK_x: key_status[13] = true;
							   break;
					case SDLK_c: key_status[14] = true;
							   break;
					case SDLK_v: key_status[15] = true;
							   break;
					default: break;
				}
				break;
			case SDL_KEYUP:
				printf("Key let go!: %d", event.key.keysym.sym);
				switch(event.key.keysym.sym){
					case SDLK_1: key_status[0] = false;
							   break;
					case SDLK_2: key_status[1] = false;
							   break;
					case SDLK_3: key_status[2] = false;
							   break;
					case SDLK_4: key_status[3] = false;
							   break;
					case SDLK_q: key_status[4] = false;
							   break;
					case SDLK_w: key_status[5] = false;
							   break;
					case SDLK_e: key_status[6] = false;
							   break;
					case SDLK_r: key_status[7] = false;
							   break;
					case SDLK_a: key_status[8] = false;
							   break;
					case SDLK_s: key_status[9] = false;
							   break;
					case SDLK_d: key_status[10] = false;
							   break;
					case SDLK_f: key_status[11] = false;
							   break;
					case SDLK_z: key_status[12] = false;
							   break;
					case SDLK_x: key_status[13] = false;
							   break;
					case SDLK_c: key_status[14] = false;
							   break;
					case SDLK_v: key_status[15] = false;
							   break;
					default: break;
				}
				break;
			default: break;
		}
	}
}

bool INPUT::get_key_status(uint8_t key){
	return key_status[key];
}