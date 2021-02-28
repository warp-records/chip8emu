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
	int clockSpeed = 100;

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
	bool keyPresses[16] { 0 };
	//map keyboard input characters to keypress index
	std::map<char, int> keyInputMap {
		{'1', 0x1 },  {'2', 0x2 },  {'3', 0x3 },  {'4', 0xC}, 
		{'q', 0x4 },  {'w',0x5 },  {'e', 0x6 },  {'r', 0xD}, 
		{'a', 0x7},  {'s', 0x8 },  {'d', 0x9},  {'f', 0xE},
		{'z', 0xA},  {'x', 0x0},  {'c', 0xB},  {'v', 0xF}
	};


	//apparently a chip8 fontset?
	unsigned char fontset[80]
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
	bool keyDown(int key) {
		/*std::cout << "Testing for: " << std::hex <<
		key << std::dec << '\n';*/
		return keyPresses[key];
	}

	void loadGame(std::string const& fileName);

public:
	bool drawFlag = false;

	bool debugFlag = false;

	void initialize(std::string const& game);

	void executeCycle();

	const unsigned char* gfxBuffer() const {
		return reinterpret_cast<const unsigned char*> (gfx);
	}

	std::pair<int, int> screenDimensions() {
		return std::make_pair(screenWidth, screenHeight);
	}

	void pressKey(char key) {
		if (keyInputMap.find(key) != keyInputMap.end()) {
			keyPresses[keyInputMap[key]] = true;

			/*for (int i = 0; i < 16; i++) {
				std::cout << "  " << keyPresses[i];

				if ((i%4 == 0))
					std::cout << '\n';
			}
			std::cout << "\n\n";*/

		}
	}

	void unpressKey(char key) {
		if (keyInputMap.find(key) != keyInputMap.end()) {
			keyPresses[keyInputMap[key]] = false;
		}
	}

	void setClockSpeed(int speed) {
		clockSpeed = speed;
	}

	std::ostream& memdump(std::ostream& os);

	std::ostream& gfxDraw(std::ostream& os);
};
