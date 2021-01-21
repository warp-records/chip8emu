#include <iostream>
#include "chip8.h"

//Laurence Miller's Chip8 Emuo tutorial SUCKS!

int main() {
	Chip8 emulator;
	emulator.initialize("test_opcode.ch8");

	for (int i = 0; true; i++) {
		//std::cout << std::dec << "Cycle Count: " << i << '\n';
		emulator.executeCycle();

		if (!emulator.debugFlag)
			emulator.gfxDraw(std::cout);
		//std::cin.get();
	}

	return 0;
}
