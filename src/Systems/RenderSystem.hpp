#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/TransformComponent.hpp"

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();  
        }

        void Update(SDL_Renderer* renderer) {
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()) {
                // Update entity position based on its velocity
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                SDL_Rect objRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    sprite.width,
                    sprite.height
                };

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                SDL_RenderFillRect(renderer, &objRect);
            }
        }
};