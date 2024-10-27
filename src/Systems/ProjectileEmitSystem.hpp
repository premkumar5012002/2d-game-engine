#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../ECS/ECS.hpp"
#include "../EventBus/EventBus.hpp"

#include "../Events/KeyPressedEvent.hpp"

#include "../Components/SpriteComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileEmitterComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"

class ProjectileEmitSystem : public System {
    public:
        ProjectileEmitSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<ProjectileEmitterComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::onKeyPressed);
        }

        void onKeyPressed(KeyPressedEvent& event) {
            for (auto entity: GetSystemEntities()) {

                if (event.symbol == SDLK_SPACE) {
                    if (entity.HasComponent<CameraFollowComponent>()) {
                        auto transform = entity.GetComponent<TransformComponent>();
                        const auto rigidBody = entity.GetComponent<RigidBodyComponent>();
                        const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

                        auto& projectilePosition = transform.position;
                        
                        if (entity.HasComponent<SpriteComponent>()) {
                            auto sprite = entity.GetComponent<SpriteComponent>();
                            projectilePosition.x += (transform.scale.x * sprite.width / 2);
                            projectilePosition.y += (transform.scale.y * sprite.height / 2);
                        }

                        glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;

                        int directionX = 0;
                        int directionY = 0;

                        if (rigidBody.velocity.x > 0) {
                            directionX = +1;
                        } 
                        
                        if (rigidBody.velocity.x < 0){
                            directionX = -1;
                        }

                        if (rigidBody.velocity.y > 0) {
                            directionY = +1;
                        }
                        
                        if (rigidBody.velocity.y < 0) {
                            directionY = -1;
                        }

                        projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
                        projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;

                        Entity projectile = entity.registry->CreateEntity();
                        projectile.Group("projectiles");
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                        projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                        projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
                        projectile.AddComponent<BoxColliderComponent>(4, 4);
                        projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
                    }

                }
            }
        }

        void Update(std::unique_ptr<Registry>& registry) {
            for (auto entity: GetSystemEntities()) {
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();

                // If emission frequency is zero, bypass re-emission logic 
                if (projectileEmitter.repeatFrequency == 0) {
                    continue;
                }

                // Check if its time to re-emit a new projectile
                if (static_cast<int>(SDL_GetTicks64()) - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
                    glm::vec2 projectilePosition = transform.position;

                    if (entity.HasComponent<SpriteComponent>()) {
                        auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);
                    }

                    // Add a new projectile entity to the registry
                    Entity projectile = registry->CreateEntity();
                    projectile.Group("projectiles");
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);
                    projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);

                    // Update the projectile emitter component last emission to the current milliseconds
                    projectileEmitter.lastEmissionTime = SDL_GetTicks64();
                }
            }
        }    
};