all: chip8

chip8: main.cpp graphics.o input.o chip8.o
	g++ -Wall -Werror `sdl2-config --cflags --libs` -o chip8 chip8.o graphics.o input.o main.cpp; 

graphics.o:
	g++ -Wall -Werror -o graphics.o -c graphics.cpp

input.o:
	g++ -Wall -Werror -o input.o -c input.cpp

chip8.o:
	g++ -Wall -Werror -o chip8.o -c chip8.cpp

clean:
	rm *o chip8