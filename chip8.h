
class Chip8 {
	//general registers
	unsigned char reg[16];
	//program counter
	unsigned short pc;
	/*index register
	changed it to "idx" instead of 
	"I" since "I" is very ugly and
	"i" is an awful idea.*/
	unsigned short idx = 0;

	//currently loaded opcode
	unsigned short opcode = 0;
	//vm memory
	unsigned char memory[4096];
	//stack memory
	unsigned short stack[16];
	//stack pointer
	unsigned short sp = 0;

	//graphics buffer
	unsigned char gfx[64 * 32];

	//self explanatory lol
	unsigned char delayTimer = 0;
	unsigned char soundTimer = 0;

	//keyboard input
	unsigned char key[16];

public:
	void Chip8::initialize();

	void Chip8::emulateCycle();
}
