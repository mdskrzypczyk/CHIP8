#include "input.h"

/*
 * INPUT
 * Description: Constructor for CHIPINPUT object, initializes all keys to 
 *				unpressed state.
 * Inputs: None
 * Outputs: None
 * Return Value: None
*/

INPUT::INPUT(){
	for(int i = 0; i < NUM_KEYS; i++){
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

INPUT::~INPUT(){}

/*
 * poll_keyboard
 * Description: Updates the key status array with up/down presses on a key
 * Inputs: None
 * Outputs: None
 * Return Value: The hexadecimal value of the key that was pressed, otherwise -1
 *				 if no keys were pressed
*/

uint8_t INPUT::poll_keyboard(SDL_Event event){
	uint8_t return_val = 0xFF;

	//For handling key presses
	if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
		switch(event.key.keysym.sym){
			case SDLK_1: key_status[0] = true;
					   return_val = 0;
					   break;
			case SDLK_2: key_status[1] = true;
					   return_val = 1;
					   break;
			case SDLK_3: key_status[2] = true;
					   return_val = 2;
					   break;
			case SDLK_4: key_status[3] = true;
					   return_val = 3;
					   break;
			case SDLK_q: key_status[4] = true;
					   return_val = 4;
					   break;
			case SDLK_w: key_status[5] = true;
					   return_val = 5;
					   break;
			case SDLK_e: key_status[6] = true;
					   return_val = 6;
					   break;
			case SDLK_r: key_status[7] = true;
					   return_val = 7;
					   break;
			case SDLK_a: key_status[8] = true;
					   return_val = 8;
					   break;
			case SDLK_s: key_status[9] = true;
					   return_val = 9;
					   break;
			case SDLK_d: key_status[10] = true;
					   return_val = 10;
					   break;
			case SDLK_f: key_status[11] = true;
					   return_val = 11;
					   break;
			case SDLK_z: key_status[12] = true;
					   return_val = 12;
					   break;
			case SDLK_x: key_status[13] = true;
					   return_val = 13;
					   break;
			case SDLK_c: key_status[14] = true;
					   return_val = 14;
					   break;
			case SDLK_v: key_status[15] = true;
					   return_val = 15;
					   break;

			//For color cycling
			case SDLK_t: return_val = 17;
					   break;

			//For Save State loading/saving
			case SDLK_p: return_val = 18;
						break;

			case SDLK_l: return_val = 19;
						break;
			default: break;
		}
	}

	//Sets the key status to unpressed
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

	//Handles clicking 'x' for window
	else if(event.type == SDL_QUIT){
		return_val = 16;
	}

	return return_val;
}

/*
 * get_key_status
 * Description: Pulls key status information from status array
 * Inputs: None
 * Outputs: None
 * Return Value: Boolean representings if key is pressed (true) or not (false)
*/

bool INPUT::get_key_status(uint8_t key){
	return key_status[key];
}

void INPUT::flip_key_status(uint8_t key){
	key_status[key] = key_status[key] ? true : false;
}

//Debugging
void INPUT::print_keyboard_status(){
	for(int i = 0; i < NUM_KEYS; i++){
		if(key_status[i]) printf("%d: %d\n", i, key_status[i]);
	}
}