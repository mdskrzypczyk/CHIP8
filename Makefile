all: chip8

chip8: main.cpp graphics.o input.o chip8.o audio.o
	g++ -Wall -Werror -o chip8 chip8.o graphics.o input.o audio.o main.cpp `sdl2-config --cflags --libs`;

graphics.o:
	g++ -Wall -Werror -o graphics.o -c graphics.cpp

audio.o:
	g++ -Wall -o audio.o -c audio.cpp -lSDL -lSDL_mixer

input.o:
	g++ -Wall -Werror -o input.o -c input.cpp

chip8.o:
	g++ -Wall -Werror -o chip8.o -c chip8.cpp

clean:
	rm *o chip8