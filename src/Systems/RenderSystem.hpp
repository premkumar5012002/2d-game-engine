#pragma once

#include <algorithm>
#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/TransformComponent.hpp"

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();  
        }

        void Update(
            SDL_Rect& camera,
            SDL_Renderer* renderer,
            std::unique_ptr<AssetStore>& assetStore
        ) { 

            auto entities = GetSystemEntities();

            // Sort the vector by the z-index value
            std::sort(entities.begin(), entities.end(), [](const Entity& e1, const Entity& e2) {
                return e1.GetComponent<SpriteComponent>().zIndex < e2.GetComponent<SpriteComponent>().zIndex;
            });

            // Loop all entities that the system is interested in
            for (auto entity: entities) {

                // Update entity position based on its velocity
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                bool isEntityOutside = (
                    transform.position.x + (sprite.width * transform.scale.x) < camera.x || 
                    transform.position.y + (sprite.height * transform.scale.y) < camera.y ||
                    transform.position.x > camera.x + camera.w ||
                    transform.position.y > camera.y + camera.h
                );

                if (isEntityOutside && !sprite.isFixed) {
                    continue;
                }

                // Set the source rectange of our original sprite texture
                SDL_Rect srcRect = sprite.srcRect;

                // Set the destination rectange with the x, y position to be rendered
                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                    static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer, 
                    assetStore->GetTexture(sprite.assetId),
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    NULL,
                    sprite.flip
                );
            }
        }
};