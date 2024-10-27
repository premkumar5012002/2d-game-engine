#pragma once

#include <SDL2/SDL.h>
#include <sol/sol.hpp>

#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"

class LevelLoader {
    public:
        LevelLoader();
        ~LevelLoader();
        void LoadLevel(
            sol::state& lua,
            const std::unique_ptr<Registry>& registry, 
            const std::unique_ptr<AssetStore>& assetStore,
            SDL_Renderer* renderer, 
            int levelNumber
        );
};