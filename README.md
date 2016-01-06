#CHIP-8 Interpretter

This is a github repository containing code written for a Chip-8 Interpretter.  The code was written in C++ and uses SDL for handling graphics and user input.  The documentation used for my implemenation can be found here: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

##Dependencies
This program requires SDL2 for compilation.

##Usage
The program can be compiled using the provided Makefile.  Once the program has been built it can be run with: ./chip8 <program>

##Controls
Currently the Chip-8's hexadecimal keyboard has been mapped in the following manner:
0: 1

1: 2

2: 3

3: 4

4: Q

5: W

6: E

7: R

8: A

9: S

A: D

B: F

C: Z

D: X

E: C

F: V

##Future Changes
-Allow modification of keyboard mapping

-Adjustable window dimensions

-Add changing of game without restarting program

-Add sound support

##Update/Changelog
V1.2: Added random color scheming via pressint "T"

V1.1: Fixed screen flickering issues, improved FPS, fixed exiting by x-ing out of window.  Added Makefile for compilation.

rV1.0: Functioning Interpretter