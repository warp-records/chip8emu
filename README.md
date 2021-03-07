# Chip8emu

A Chip8 interpreter featuring keyboard IO and graphics output. 

There are many more features which could easily be implemented (save / load states, a game open menu, debug), however I grew very tired toward the end of this project, and was starting to get more and more feature creep. This was my first time working on such a large project, and my first time with a graphics / GUI library, so the code quality involving those parts might not be the best. It doesn't have fancy optimization features, but it runs fine. Tested on my computer with an i7-7700k running Ubuntu, (just eyeballing it) the Space Invaders intro ran at it's maximum up to around 500hz. It is certainly fast enough that it can run games without clocking out, at far beyond what is needed for any game.


# Dependencies
I'm not sure how I would include this properly into my github, but my game uses SDL2 as a renderer and ImGui as a GUI library with the custom ImGuiSDL backend. These can be found here: 

SDL: https://www.libsdl.org/
ImGui: https://github.com/ocornut/imgui
ImGuiSDL: https://github.com/Tyyppi77/imgui_sdl
