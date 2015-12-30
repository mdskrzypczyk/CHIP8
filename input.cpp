#include "input.h"

INPUT::INPUT(){
	for(int i = 0; i < NUM_KEYS; i++){
		key_status[i] = false;
	}
}

INPUT::~INPUT(){}

uint8_t INPUT::poll_keyboard(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
			switch(event.key.keysym.sym){
				case SDLK_1: key_status[0] = true;
						   return 0;
				case SDLK_2: key_status[1] = true;
						   return 1;
				case SDLK_3: key_status[2] = true;
						   return 2;
				case SDLK_4: key_status[3] = true;
						   return 3;
				case SDLK_q: key_status[4] = true;
						   return 4;
				case SDLK_w: key_status[5] = true;
						   return 5;
				case SDLK_e: key_status[6] = true;
						   return 6;
				case SDLK_r: key_status[7] = true;
						   return 7;
				case SDLK_a: key_status[8] = true;
						   return 8;
				case SDLK_s: key_status[9] = true;
						   return 9;
				case SDLK_d: key_status[10] = true;
						   return 10;
				case SDLK_f: key_status[11] = true;
						   return 11;
				case SDLK_z: key_status[12] = true;
						   return 12;
				case SDLK_x: key_status[13] = true;
						   return 13;
				case SDLK_c: key_status[14] = true;
						   return 14;
				case SDLK_v: key_status[15] = true;
						   return 15;
				default: break;
			}
		}
		else if(event.type == SDL_KEYUP && event.key.repeat == 0){
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
		}
	}

	return 0xFF;
}

bool INPUT::get_key_status(uint8_t key){
	return key_status[key];
}

void INPUT::print_keyboard_status(){
	for(int i = 0; i < NUM_KEYS; i++){
		if(key_status[i]) printf("%d: %d\n", i, key_status[i]);
	}
}