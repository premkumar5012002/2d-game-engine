#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"

class RenderColliderSystem: public System {
    public:
        RenderColliderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();  
        }

        void Update(SDL_Renderer* renderer) {
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()) {
                auto transform = entity.GetComponent<TransformComponent>();
                auto collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect colliderRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(collider.width),
                    static_cast<int>(collider.height),
                };

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }
};