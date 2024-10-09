#pragma once

#include <SDL2/SDL.h>

class Game {
    private:
        bool isRunning;
        SDL_Window* window;
        SDL_Renderer* renderer;

        void ProcessInput();
        void Update();
        void Render();

    public:
        int windowWidth;
        int windowHeight;

        Game();
        ~Game();
        void Initialize();
        void Run();
        void Destroy();
};