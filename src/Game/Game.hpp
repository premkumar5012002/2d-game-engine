#pragma once

#include <memory>
#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../EventBus/EventBus.hpp"
#include "../AssetStore/AssetStore.hpp"

const int FPS = 75;
const int MILLISECONDS_PER_FRAME = 1000 / FPS; 

class Game {
    private:
        bool isDebug;
        bool isRunning;
        int millisecondsPreviousFrame;

        SDL_Rect camera;
        SDL_Window* window;
        SDL_Renderer* renderer;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<EventBus> eventBus;
        std::unique_ptr<AssetStore> assetStore;

        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void LoadLevel(int level);

    public:
        static int windowWidth;
        static int windowHeight;
        static int mapWidth;
        static int mapHeight;

        Game();
        ~Game();
        void Initialize();
        void Run();
        void Destroy();
};