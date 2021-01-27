#include "chip8.h"
#include <iostream>
#include <SDL2/SDL.h>

/* These dependend on the fact that 
a short is 2 bytes, so if this is
different on your machine, this 
MAY BREAK the emulator! */

void drawGraphics(SDL_Renderer* renderer, SDL_Texture* texture, unsigned int (&renderBuff)[2048], unsigned char const* gfxBuff) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			/*Wait, WHAT? WHY? How? You should
			really figure out the answers to 
			these questions later*/
			renderBuff[j + i*64] = ((gfxBuff[j*32 + i] * 0xFFFFFF00) | 0x000000FF);
		}
	}

	SDL_UpdateTexture(texture, NULL, renderBuff, 64 * sizeof(uint32_t));

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}



int main(int argc, char* argv[]) {
	
	//SDL Initialization stuff
	//window to render to
	SDL_Window* window = nullptr;

	//the surface contained by the window
	SDL_Surface* surface = nullptr;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::exception();

	SDL_Window* win = SDL_CreateWindow("Dael Chip8 Interpreter", 200, 200, 1280, 640, SDL_WINDOW_SHOWN);
	if (win == nullptr)
		throw std::exception();

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);//SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//SDL_RenderSetLogicalSize(ren, 1280, 640);
	if (ren == nullptr)
		throw std::exception();

	SDL_Texture* texture = SDL_CreateTexture(ren, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STATIC,
											64, 32);
	
	unsigned int renderBuff[2048];


	//Load the game and initailize the emu
	if (argc != 2)
		throw std::exception();

	Chip8 emulator;
	emulator.initialize(argv[1]);


	SDL_Event event;
	//game loop
	for (int i = 0; true; i++) {
		//std::cout << std::dec << "Cycle Count: " << i << '\n';
		emulator.executeCycle();

		if (SDL_PollEvent(&event)) {
			switch (event.type)  {

				case SDL_KEYDOWN:
					emulator.pressKey(event.key.keysym.sym);
					break;

				case SDL_KEYUP:
					emulator.unpressKey(event.key.keysym.sym);
					break;
			}
		}

		if (emulator.drawFlag) {
			drawGraphics(ren, texture, renderBuff, emulator.gfxBuffer());
			//emulator.gfxDraw(std::cout);
		}
		//std::cin.get();
	}

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
