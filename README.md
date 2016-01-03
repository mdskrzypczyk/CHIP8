#CHIP-8 Interpretter

This is a github repository containing code written for a Chip-8 Interpretter.  The code was written in C++ and uses SDL for handling graphics and user input.  The documentation used for my implemenation can be found here: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

##Dependencies
This program requires SDL2 for compilation.

##Usage
The program can be compiled using the compile.sh shell script.  Once the program has been built it can be run with: ./chip8 <program>

##Controls
The hexadecimal keyboard has been mapped as follows:

+---+---+---+---+		    +---+---+---+---+
| 0 | 1 | 2 | 3 |           | 1 | 2 | 3 | 4 |
+---+---+---+---+           +---+---+---+---+
| 4 | 5 | 6 | 7 |           | Q | W | E | R |
+---+---+---+---+    =>     +---+---+---+---+
| 8 | 9 | A | B |           | A | S | D | F |
+---+---+---+---+           +---+---+---+---+
| C | D | E | F |           | Z | X | C | V |
+---+---+---+---+           +---+---+---+---+

##Update/Changelog
V1.0: Function Interpretter