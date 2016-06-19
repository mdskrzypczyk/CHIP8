#include "chip8.h"
#include <unistd.h>

int main(int argc, char* argv[]){
	CHIP8 myChip8 = CHIP8();
	
	if(!myChip8.load_program(argv[1])){
		std::cout << "Unable to load program.\n" << std::endl;
		return -1;
	}

	// if(!myChip8.load_config()){
	// 	std::cout << "Unable to load config.txt.\n" << std::endl;
	// }

	if(!myChip8.init_video()){
		std::cout << "Unable to initialize chip video.\n" << std::endl;
		return -1;
	}

	if(!myChip8.init_audio()){
		std::cout << "Unable to initialize chip audio.\n" << std::endl;
		return -1;
	}
	
	myChip8.mainloop();
	return 0;
}

