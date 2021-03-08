# Chip8emu

A Chip8 interpreter featuring keyboard IO and graphics output. 

There are many more features which could easily be implemented (save / load states, a game open menu, debug), however I grew very tired toward the end of this project, and was starting to get more and more feature creep. This was my first time working on such a large project, and my first time with a graphics / GUI library, so the code quality involving those parts might not be the best. It doesn't have fancy optimization features, but it runs fine. Tested on my computer with an i7-7700k running Ubuntu, (just eyeballing it) the Space Invaders intro screen ran at it's maximum up to around 500hz. It is certainly fast enough that it can run games without clocking out, at far beyond what is needed for any game.

# Usage
To use the emulator, open a game by specifying a command line argument in shell with the game ROM you want to open. For example: `./chip8emu games/glitchGhost.ch8`. If the emulator is just quickly opening and closing, make sure you specified the correct game path! You can control the clock speed by clicking the "Settings" menu bar in the top right corner, and dragging the ClockSpeed slider.

# Dependencies
I'm not sure how I would include this properly into my github, but my game uses SDL2 as a renderer and imgui as a GUI library with the custom imgui_sdl backend. These can be found here: 

SDL2: https://www.libsdl.org/
imgui: https://github.com/ocornut/imgui
imgui_sdl: https://github.com/Tyyppi77/imgui_sdl

# Building
I didn't implement a proper makefile, and I just relied on a build script to build my code. You'll have to install `libsdl2-dev`, and clone into imgui and imgui_sdl. Then, copy the contents of imgui_sdl into imgui. Since there's no makefile, imgui_sdl can't include SDL, because no include path is specified. To get around this, change the SDL include from "SDL.h" to <SDL2/SDL.h>. Finally, run the build shell script. 
