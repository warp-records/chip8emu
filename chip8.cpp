#include "chip8.h"
#include <bitset>



void Chip8::initialize(std::string const& game) {

	//Program counter starts at byte 512, or address 0x200
	pc = 512;
	//clear these
  	opcode = 0;
  	idx = 0;
  	//set the stack pointer to the beginning of memory
  	sp = (unsigned short*) memory;

  	soundTimer = 0;
  	delayTimer = 0;

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

 	loadGame(game);
};

void Chip8::loadGame(std::string const& name) {
	int constexpr maxLen = 3584;

	std::ifstream is(name, std::ios::binary | std::ios::in);
	
	is.seekg(0, is.end);
	int fileSize = is.tellg();
	is.seekg(0, is.beg);

	if (!is || fileSize > maxLen) {
		std::cout << "Error: file size is " << fileSize << '\n';
   		/*man fuck you whoever you
   		are lmao*/
    	throw std::system_error();
    }

    /*first 512 bytes are typically reserved 
    for the interpreter*/
    is.read(reinterpret_cast<char*> (memory+512), fileSize);
}



void Chip8::executeCycle() {

	if (pc > 4095)
		throw std::exception();
	//merge two bytes of the pc
	opcode = (memory[pc] << 8) | (memory[pc + 1]);

	//opcode nibbles, MUCH more readable, and efficent
	unsigned char nibble[4];
	//fancy code to get each nibble (4 bits) from the opcode
	for (int i = 3, j = 0xF000; i >= 0; i--, j >>= 4) {
		nibble[3-i] = (opcode & j) >> i*4;
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
			//break;
			/*(apparently) depracated, and obscure opcode... 
			maybe don't implement?
			if (opcode != 0x00E0 && opcode != 0x00EE)
				opcode = memory[nnn];*/

			switch (opcode) {
				case 0x00E0:
					//clearDisplay();
					for (int x = 0; x < screenWidth; x++) {
						for (int y = 0; y < screenHeight; y++) {
							gfx[x][y] = 0;
						}
					}
					break;

				case 0x00EE:
					pc = *sp;
					sp -= 2;
					break;
			}
			break;

		//1NNN: jump to NNN
		case 0x1:
			//subtract two since it will be added at the end
			pc = nnn - 2;
			break;

		//2NNN: function call at nnn
		case 0x2:
			sp += 2;
			*sp = pc;
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

				case 0x4:
					regF = (regX - regY) > 0xFF;
					regX += regY;
					break;

				case 0x5:
					regF = (regX - regY) >= 0;
					regX -= regY;
					break;

				case 0x6:
					regF = regX >> 7;
					regX >>= 1;
					break;

				case 0x7:
					regF = (regY - regX) < 0;
					regX = regY - regX;
					break;

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
		case 0xD: {
			/*consider making these local to
			the class itself*/

			int constexpr width = 8;
			//not n+1 for some reason...
			int height = n;

			/*wrap around starting positions
			but DONT wrap around the actual drawings*/
			int startX = regX % screenWidth;
			int startY = regY % screenHeight;

			unsigned char spriteRow;

			//false by default
			regF = false;

			//clipping.
			for (int y = 0; y < height && y + startY < screenHeight; y++) {
				spriteRow = memory[idx + y];

				for (int x = 0; x < width && x + startX < screenWidth; x++) {
					unsigned char& pixel = gfx[startX + x][startY + y];

					/*if the current pixel is XOR'd to zero,
					set the V[F] flag to true*/
					if (spriteRow&0x80 && pixel)
						regF = true;
					/*xor the current pixel with the current
					bit of the sprite row byte*/
					pixel ^= (spriteRow >> 7);

					spriteRow <<= 1;

					if (x < 0 || x > 64 || y < 0 || y > 32)
						throw std::exception();
				}
			}

			break;

		}

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
					unsigned int val = regX;
					//std::cout << "BCD Instruction\nVal: " << val;

					for (int i = 0; i < 3; i++) {
						memory[idx+i] = val%10;
						val /= 10;
					}
					break;
				}
				break;

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

		default: {
			std::cout << "Encountered unknown opcode! Terminating!\n";
			throw std::exception();
		}
	}

	pc += 2;

	//count down timers at 60hz
	if (cycleNumber/clockSpeed*60 >= 1) {
		delayTimer--;
		soundTimer--;
		//clear console, apparently

		//sleep for a certain amount of time
		usleep(16667);

		cycleNumber = 1;
	} else {
		cycleNumber++;
	}

	//----------------DEBUG-------------
	if (debugFlag) {
		std::cout << "Executing opcode 0x" << std::hex << 
		std::setfill('0') << std::setw(4) << opcode << '\n';

		std::cout <<  "PC: " << pc << '\n';
		for (int i = 0; i < 16; i++) {
			std::cout << "Register " << i << ": " << (int) reg[i] << '\n';
		}
		std::cout << "Reg Idx: " << (int) idx << '\n';
		std::cout << "nnn: " << nnn << '\n';
		std::cout << "nn: " << nn << '\n';
		std::cout << "n: " << n << '\n';
		std::cout << "Stack Pointer offset: " << ((long) sp - (long) memory) << '\n';
	}

}


std::ostream& Chip8::memdump(std::ostream& os) {
	os << "Memory dump:\n";
	for (int i = 0; i < 4096; i++) {
	    if (memory[i] != 0) {
		   	os << "[" << i << 
		    "]: " << std::hex << (int) memory[i] <<
		    std::dec << '\n';
	    }
	}

	return os;
}

std::ostream& Chip8::gfxDraw(std::ostream& os) {
	std::stringstream buffer;
	/*for (int i = 0; i < 64; i++)
		os << std::setw(2) << i;*/

	buffer << "\033[H\033[J";

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			buffer << ((int) gfx[j][i] ? "\u2588\u2588" : "  ");
		}
		buffer << '\n';
	}

	return os << buffer.str();
}

//there was an attempt at least...
/*std::ostream& Chip8::debugInfo(std::ostream& os, DebugFlags flags) {
	os << "Debug Info:\n";
	if (flags & 0x1) {
		os << ""
	}
}*/

/*Chip8::Chip8() {
	initialize();
}*/
