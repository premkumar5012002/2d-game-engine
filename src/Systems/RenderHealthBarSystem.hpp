#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"

#include "../Components/HealthComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/TransformComponent.hpp"

class RenderHealthBarSystem: public System {
    public:
        RenderHealthBarSystem() {
            RequireComponent<HealthComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(
            const SDL_Rect& camera,
            SDL_Renderer* renderer,
            const std::unique_ptr<AssetStore>& assetStore
        ) {
            for (auto entity: GetSystemEntities()) {
                const auto health = entity.GetComponent<HealthComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();

                SDL_Color healthBarColor = {255, 255, 255};

                if (health.healthPercentage >= 0 && health.healthPercentage <= 40) {
                    healthBarColor = {255, 0, 0};
                } 
                
                if (health.healthPercentage >= 40 && health.healthPercentage <= 80) {
                    healthBarColor = { 255, 255, 0 };
                } 
                
                if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                    healthBarColor = { 0, 255, 0 };
                }
                
                // Render Health Bar

                SDL_Rect healthBarDstRect = {
                    static_cast<int>(transform.position.x + (sprite.width * transform.scale.x) - camera.x),
                    static_cast<int>(transform.position.y - camera.y),
                    static_cast<int>(20 * (health.healthPercentage / 100.0)),
                    3
                };

                SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
                SDL_RenderFillRect(renderer, &healthBarDstRect);

                // Render health font

                SDL_Surface* surface = TTF_RenderText_Blended(
                    assetStore->GetFont("charriot-font"),
                    std::to_string(health.healthPercentage).c_str(),
                    healthBarColor
                );

                SDL_Texture* texture = SDL_CreateTextureFromSurface(
                    renderer,
                    surface
                );

                SDL_FreeSurface(surface);

                int labelWidth = 0;
                int labelHeight = 0;

                SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

                SDL_Rect dstRect = {
                    static_cast<int>(healthBarDstRect.x),
                    static_cast<int>(healthBarDstRect.y + 5),
                    labelWidth,
                    labelHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                
                SDL_DestroyTexture(texture);
            }
        }
};