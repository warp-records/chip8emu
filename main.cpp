#include <iostream>
//#include "chip8.h"

/*
Huge thanks to Laurence Miller's
tutorial on multigesture.net for 
making this possible!

http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

Or, alternatively
https://web.archive.org/web/20201111234103/http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
*/

/* These dependend on the fact that 
a short is 2 bytes, so if this is
different on your machine, this 
MAY BREAK the emulator! */


int main(int argc, char** argv) {
	unsigned short opcode = 0xABCD;
	
	unsigned char nibble[4];

	for (int i = 3, j = 0xF000; i >= 0; i--, j >>= 4) {
		nibble[i] = (opcode & j) >> i*4;
	}
}