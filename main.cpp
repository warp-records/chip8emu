#include "chip8.h"
#include <iostream>
#include <SDL2/SDL.h>

/* These dependend on the fact that 
a short is 2 bytes, so if this is
different on your machine, this 
MAY BREAK the emulator! */

void render() {

}


int main(int argc, char* argv[]) {
	
	//SDL shit going on here
	//window to render to
	SDL_Window* window = nullptr;

	//the surface contained by the window
	SDL_Surface* surface = nullptr;


	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::exception();

	SDL_Window* win = SDL_CreateWindow("Dael Chip8 Interpreter", 200, 200, 1280, 640, SDL_WINDOW_SHOWN);
	if (win == nullptr)
		throw std::exception();

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
		throw std::exception();

	std::cin.get();

	SDL_Quit();


	return 0;
}
/*
void oldGameLoop(int argc, char* argv[]) {
	if (argc != 2)
		throw std::exception();


	Chip8 emulator;
	emulator.initialize(argv[1]);

	for (int i = 0; true; i++) {
		//std::cout << std::dec << "Cycle Count: " << i << '\n';
		emulator.executeCycle();

		if (!emulator.debugFlag && emulator.drawFlag)
			emulator.gfxDraw(std::cout);
		//std::cin.get();
	}
}*/
