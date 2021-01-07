#include "chip8.h"


void Chip8::initialize() {
	//Program counter starts at byte 512, or address 0x200
	pc = 512;
	//clear these
  	opcode = 0;
  	idx = 0;
  	sp = 0;

  	/*load fontset, with 16 different
  	characters, and 5 bytes per character*/
  	for(int i = 0; i < 80; i++) {
  		//leave first 96 bytes for the stack!
    	memory[i+96] = fontset[i];

    	if (i%5 == 0) {
    		/*inserts the characters 1 to
    		F and maps them to their respective
    		memory addresses*/
    		char ch = (i/5 <= 10 ? '0' : 'A') + i/5;
			fontsetMap.insert({'0' + i/5, i+96});
    	}
    }

 	loadGame("game.bin");
};

void Chip8::loadGame(std::string const& name) {
	int constexpr maxLen = 3584;

	std::ifstream is(name, std::ios::binary);

	if (!is || is.tellg() > maxLen)
   		//man fuck you
    	throw std::system_error();

    /*first 512 bytes are typically reserved 
    for the interpreter*/
    is.read(memory[512], is.tellg());
}




void Chip8::emulateCycle() {
	//merge two bytes of the pc
	opcode = memory[pc] << 8 | memory[pc + 1];

	//opcode nibbles, MUCH more readable, and efficent
	unsigned char nibble[4];
	//fancy code to get each nibble (4 bits) from the opcode
	for (int i = 3, j = 0xF000; i >= 0; i--, j >>= 4) {
		nibble[i] = (opcode & j) >> i*4;
	}

	/*Warning: slightly cringey code.
	These are notations that make it more
	convenient to read from and write to
	registers that are frequently accessed.
	These registers represent the "X" 
	(second nibble) and "Y" (third nibble)
	in the code.*/
	unsigned char& regX = reg[nibble[1]];
	unsigned char& regY = reg[nibble[2]];
	//the "F" register
	unsigned char& regF = reg[0xF];
	unsigned short nnn 	= opcode & 0x0FFF;
	unsigned short nn 	= opcode & 0x00FF;
	unsigned short n 	= opcode & 0x000F;

	//The juice of the code!
	switch (nibble[0]) {

		case 0x0:
			/*(apparently) depracated, and obscure opcode... 
			maybe don't implement?*/
			if (opcode != 0x00E0 && opcode != 0x00EE)
				opcode = memory[nnn];

			switch (opcode) {
				case 0x00E0:
					//clearDisplay();
					for (int x = 0; x < 64; x++) {
						for (int y = 0; y < 32; y++) {
							gfx[x][y] = 0;
						}
					}
					break;

				case 0x00EE:
					pc = stack[sp] - 2;
					sp--;
					break;
			}

		//1NNN: jump to NNN
		case 0x1:
			//subtract two since it will be added at the end
			pc = nnn - 2;
			break;

		//2NNN: function call at nnn
		case 0x2:
			stack[sp] = pc;
			sp++;
			pc = nnn - 2;
			break;


		/*OUT OF BOUNDS POSSIBLE on 3XNN and 4XNN,
		make sure to change later!*/
		//3XNN: if V[X] == NN, skip instruction
		case 0x3:
			if (regX == nn) {
				pc += 2;
			}
			break;

		//4XNN: if V[X] != NN, skip instruction
		case 0x4:
			if (regX != nn) {
				pc += 2;
			}
			break;

		//5XY0: if V[X] == V[Y], skip instruction
		case 0x5:
			if (regX == regY) {
				pc += 2;
			}
			break;

		//6XNN: set V[X] to NN
		case 0x6:
			regX = nn;
			break;

		/*7XNN: increment V[X] by NN,
		NOT SURE if carry flag is changed or not!*/
		case 0x7:
			regX += nn;
			break;

		case 0x8:
			switch (n) {
				//8XY0: set V[X] to V[Y]
				case 0x0:
					regX = regY;
					break;

				case 0x1:
					regX |= regY;
					break;

				case 0x2:
					regX &= regY;
					break;

				case 0x3:
					regX ^= regY;
					break;

				//INCOMPLETE, change VF
				case 0x4:
					regF = (regX - regY) > 255;
					regX += regY;
					break;

				//INCOMPLETE, change VF
				case 0x5:
					regF = (regX - regY) >= 0;
					regX -= regY;
					break;

				//INCOMPLETE
				case 0x6:
					regF = regX >> 7;
					regX >>= 1;
					break;

				//INCOMPLETE
				case 0x7:
					regF = (regY - regX) < 0;
					regX = regY - regX;
					break;

				//INCOMPLETE
				case 0xE:
					regF = (regX << 7) >> 7;
					regX <<= 1;
					break;
			}
			break;

		case 0x9:
			if (regX != regY) {
				pc += 2;
			}
			break;

		case 0xA:
			idx = nnn;
			break;

		case 0xB:
			pc = reg[0] + nnn - 2;
			break;

		case 0xC:
			regX = rand() & nn;
			break;

		/*Display opcode. This is the most 
		complex opcode, come back to it later.*/
		/*case 0xD: {
			int width = 8;
			int height = n + 1;

			for (int x = regX; x < regX + width; x++) {
				for (int y = regY; y < regY + height; y++) {

				}
			}
		}*/

		case 0xE:
			switch (nn) {
				case 0x9E:
					if (keyDown(regX))
						pc += 2;
					break;

				case 0xA1:
					if (!keyDown(regX))
						pc += 2;
					break;
			}
			break;

		case 0xF:
			switch (nn) {
				case 0x07:
					regX = delayTimer;
					break;

				case 0XA:
					//implement later
					break;

				case 0x15:
					delayTimer = regX;
					break;

				case 0x1E:
					idx += regX;
					break;

				case 0x29:
					idx = fontsetMap[regX];
					break;

				case 0x33: {
					unsigned char val = regX;
					for (int i = 0; i < 3; i--) {
						memory[idx+i] = val%10;
						val /= 10;
					}
					break;
				}

				case 0x55:
					for (int i = 0; i <= nibble[1]; i++) {
						memory[i+idx] = reg[i];
					}
					break;

				case 0x65:
					for (int i = 0; i <= nibble[1]; i++) {
						reg[i] = memory[idx+i];
					}
					break;
			}
			break;

	}

	pc += 2;
}

/*void Chip8::execOpcode(opcode) {

}*/


/*
USE THIS

//opcode nibbles
unsigned char nibble[4];
//fancy code to get each nibble (4 bits) from the opcode
for (int i = 3, j = 0xF000; i >= 0; i--, j >>= 4) {
	nibble[i] = (opcode & j) >> i*4;
}
*/
