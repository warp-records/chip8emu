#include "chip8.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"


void copyGraphicsOutput(SDL_Renderer* renderer, SDL_Texture* texture, unsigned int (&renderBuff)[2048], unsigned char const* gfxBuff) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			/*Wait, WHAT? WHY? How? You should
			really figure out the answers to 
			these questions later*/
			renderBuff[j + i*64] = ((gfxBuff[j*32 + i] * 0xFFFFFF00) | 0x000000FF);
		}
	}

	SDL_UpdateTexture(texture, NULL, renderBuff, 64 * sizeof(uint32_t));
}


int main(int argc, char* argv[]) {
	
	//SDL Initialization stuff
	//window to render to
	SDL_Window* window = nullptr;

	//the surface contained by the window
	SDL_Surface* surface = nullptr;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::exception();

	SDL_Window* win = SDL_CreateWindow("Dael Chip8 Interpreter", 200, 200, 1280, 660, SDL_WINDOW_BORDERLESS);
	if (win == nullptr)
		throw std::exception();

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//SDL_RenderSetLogicalSize(ren, 1280, 640);
	if (ren == nullptr)
		throw std::exception();

	SDL_Texture* texture = SDL_CreateTexture(ren, 
											SDL_PIXELFORMAT_RGBA8888, 
											SDL_TEXTUREACCESS_STATIC,
											64, 32);

	unsigned int renderBuff[2048];


	//Load the game and initailize the emu
	if (argc < 2)
		throw std::exception();

	Chip8 emulator;
	emulator.initialize(argv[1]);

	if (argv[2])
		emulator.setClockSpeed(std::stoi(argv[2]));


	SDL_Event event;

				//ImGui stuff
	ImGui::CreateContext();
	ImGuiSDL::Initialize(ren, 1280, 660);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;
	style.WindowPadding.x = 0;
	style.WindowPadding.y = 0;
			
	ImGuiIO& io = ImGui::GetIO();

	int clockSlider = 100;

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

				case SDL_QUIT:
					SDL_Quit();
					return 0;
				}
			}

			if (emulator.drawFlag) {
				//copy the emulator graphcis output to our SDL texture
				copyGraphicsOutput(ren, texture, renderBuff, emulator.gfxBuffer());

				int mouseX, mouseY;
				const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
				
				io.DeltaTime = 1.0f / 60.0f;
				io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
				io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

				ImGui::NewFrame();

				ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y+20));
				ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

				ImGui::Begin("Dael Chip8 Interpreter", nullptr, ImGuiWindowFlags_NoTitleBar);

				if (ImGui::BeginMainMenuBar()) {
					if (ImGui::BeginMenu("Settings")) {
						ImGui::SliderInt("ClockSpeed", &clockSlider, 1, 500, "%d");
						ImGui::EndMenu();
					}
					ImGui::EndMainMenuBar();
				}

				emulator.setClockSpeed(clockSlider);

				ImGui::Image(texture, ImVec2(1280, 640));

				ImGui::End();

				//SDL_RenderClear(renderer);
				//SDL_RenderCopy(renderer, texture, NULL, NULL);

				ImGui::Render();
				ImGuiSDL::Render(ImGui::GetDrawData());

				SDL_RenderPresent(ren);
			}
		}

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
