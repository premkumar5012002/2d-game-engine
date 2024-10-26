#pragma once

#include "../ECS/ECS.hpp"
#include "../EventBus/EventBus.hpp"

#include "../Events/CollisionEvent.hpp"

#include "../Components/SpriteComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"

class MovementSystem: public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();  
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;
            Logger::Log(
                "The movement system received an event collision between entities " + 
                std::to_string(a.GetId()) + " " + std::to_string(b.GetId())
            );

            if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(a, b);
            }

            if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles")) {
                OnEnemyHitsObstacle(b, a);
            }
        }

        void OnEnemyHitsObstacle(Entity enemy, Entity obstacle) {
            if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
                auto& sprite = enemy.GetComponent<SpriteComponent>();
                auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();

                if (rigidBody.velocity.x != 0) {
                    rigidBody.velocity.x *= -1;
                    sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }

                if (rigidBody.velocity.y != 0) {
                    rigidBody.velocity.y *= -1;
                    sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            }
        }

        void Update(double deltaTime) {
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()) {
                // Update entity position based on its velocity
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

                // Prevent the main player from moving outside the map boundaries
                if (entity.HasTag("player")) {
                    int paddingLeft = 10;
                    int paddingTop = 10;
                    int paddingRight = 50;
                    int paddingBottom = 50;
                    transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
                    transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
                    transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
                    transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transform.position.y;
                }

                // Kill the entity if moves outside of the map
                if (
                    transform.position.x < 0 ||
                    transform.position.y < 0 ||
                    transform.position.x > Game::mapWidth ||
                    transform.position.y > Game::mapHeight
                ) {
                    entity.Kill();
                }
            }
        }
};