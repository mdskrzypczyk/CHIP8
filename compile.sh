g++ -Wall -Werror -o graphics.o -c graphics.cpp;
g++ -Wall -Werror -o input.o -c input.cpp;
g++ -Wall -Werror -o chip8.o -c chip8.cpp;
g++ -Wall -Werror `sdl2-config --cflags --libs` -o chip8 chip8.o graphics.o input.o main.cpp; 