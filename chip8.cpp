#include "chip8.h"
#include <vector>
#include <iostream>


void Chip8::initialize() {
	//Program counter starts at 0x200
	pc = 0x200;
	//clear these
  	opcode = 0;
  	idx = 0;
  	sp = 0;

  	//load fontset
  	for(int i = 0; i < 80; ++i)
    	memory[i] = chip8_fontset[i];
};




void Chip8::emulateCycle() {
	//merge two bytes of the pc
	opcode = memory[pc] << 8 | memory[pc + 1];

	//opcode nibbles
	unsigned char nibble[4];
	//fancy code to get each nibble (4 bits) from the opcode
	for (int i = 3, j = 0xF000; i >= 0; i--, j >>= 4) {
		nibble[i] = (opcode & j) >> i*4;
	}


	switch (nibble[0]) {
		
	}

	pc += 2;
}

/*void Chip8::execOpcode(opcode) {

}*/