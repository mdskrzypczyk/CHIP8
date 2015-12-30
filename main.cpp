#include "chip8.h"
#include <unistd.h>

int main(int argc, char* argv[]){
	CHIP8 myChip8 = CHIP8();
	myChip8.load_program(argv[1]);
	myChip8.mainloop();
	return 0;
}

