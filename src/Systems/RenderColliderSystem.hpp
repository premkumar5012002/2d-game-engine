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

        void Update(const SDL_Rect& camera, SDL_Renderer* renderer) {
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()) {
                auto transform = entity.GetComponent<TransformComponent>();
                auto collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect colliderRect = {
                    static_cast<int>(transform.position.x - camera.x),
                    static_cast<int>(transform.position.y - camera.y),
                    static_cast<int>(collider.width * transform.scale.x),
                    static_cast<int>(collider.height * transform.scale.y),
                };

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }
};