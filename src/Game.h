#pragma once

#include <SDL2/SDL.h>

const int FPS = 75;
const int MILLISECONDS_PER_FRAME = 1000 / FPS; 

class Game {
    private:
        bool isRunning;
        int millisecondsPreviousFrame;
        SDL_Window* window;
        SDL_Renderer* renderer;

        void Setup();
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