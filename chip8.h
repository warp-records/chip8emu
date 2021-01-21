#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <cmath>
#include <sstream>
/*sleep libraries, but they're
different for Windows and Linux
*/
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


class Chip8 {

	static constexpr int screenWidth = 64;
	static constexpr int screenHeight = 32;

	//in Hz, NOT MHz
	//appears to be broken lol
	static constexpr int clockSpeed = 50;

	//general registers
	unsigned char reg[16] { 0 };
	//program counter
	unsigned int pc;
	/*index register
	changed it to "idx" instead of 
	"I" since "I" is very ugly and
	"i" is an awful idea.*/
	unsigned short idx;

	//currently loaded opcode
	unsigned short opcode;
	//vm memory
	unsigned char memory[4096] {0};
	//stack pointer
	unsigned short* sp;

	//graphics buffer
	unsigned char gfx[screenWidth][screenHeight] { 0 };

	//self explanatory lol
	unsigned char delayTimer;
	unsigned char soundTimer;

	//keep track of keyboard input presses
	bool keyPresses[16];
	//map keyboard input characters to keypress index
	std::map<char, int> keyInputMap {
		{'1', 0 },  {'2', 1 },  {'3', 2 },  {'4', 3}, 
		{'Q', 4 },  {'W', 5 },  {'E', 6 },  {'R', 7}, 
		{'A', 8 },  {'S', 9 },  {'D', 10},  {'F', 11},
		{'Z', 12},  {'X', 13},  {'C', 14},  {'V', 15}
	};

	std::map<char, int> keyOutputMap {
		{'1', 0 },  {'2', 1 },  {'3', 2 },  {'C', 3}, 
		{'4', 4 },  {'5', 5 },  {'6', 6 },  {'D', 7}, 
		{'7', 8 },  {'8', 9 },  {'9', 10},  {'E', 11},
		{'A', 12},  {'0', 13},  {'B', 14},  {'F', 15}
	};


	//apparently a chip8 fontset?
	unsigned char fontset[80] =
	{ 
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	/*number of cycles since the last time
	the timers were decremented*/
	unsigned int cycleNumber = 0;

	/*map each character to its memory address
	 in the font set, for instruction FX29*/
	std::map<char, unsigned short> fontsetMap {};


	//handle keyboard events
	bool keyDown(char key) {
		return keyPresses[keyInputMap[key]];
	}

	void pressKey(char key) {
		keyPresses[keyInputMap[key]] = true;
	}

	void unpressKey(char key) {
		keyPresses[keyInputMap[key]] = false;
	}

	void loadGame(std::string const& fileName);

public:
	bool debugFlag = false;

	void initialize(std::string const& game);

	void executeCycle();

	auto getGraphicsBuffer() const {
		return gfx;
	}

	std::pair<int, int> screenDimensions() {
		return std::make_pair(screenWidth, screenHeight);
	}

	std::ostream& memdump(std::ostream& os);

	std::ostream& gfxDraw(std::ostream& os);
};
