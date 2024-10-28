#pragma once

#include <SDL2/SDL_mixer.h>

#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"

#include "../Components/SoundComponent.hpp"

class SoundSystem: public System {
    public:
        SoundSystem() {
            RequireComponent<SoundComponent>();
        }

        void Update(std::unique_ptr<AssetStore>& assetStore) {
            for (auto entity: GetSystemEntities()) {
                auto& sound = entity.GetComponent<SoundComponent>();
                Mix_PlayChannel(-1, assetStore->GetSound(sound.assetID), sound.loopCount);
            }
        }
};