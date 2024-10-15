#pragma once

#include <string>

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp" 
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"

class CollisionSystem: public System {
    public:
        CollisionSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(std::unique_ptr<EventBus>& eventBus) {
            auto entities = GetSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                auto aTransform = a.GetComponent<TransformComponent>();
                auto aCollider = a.GetComponent<BoxColliderComponent>();

                for (auto j = i + 1; j != entities.end(); j++) {
                    Entity b = *j;
                    auto bTransform = b.GetComponent<TransformComponent>();
                    auto bCollider = b.GetComponent<BoxColliderComponent>();

                    bool collisionHappened = CheckAABBCollision(
                        aTransform.position.x + aCollider.offset.x,
                        aTransform.position.y + aCollider.offset.y,
                        aCollider.width,
                        aCollider.height,
                        bTransform.position.x + bCollider.offset.x,
                        bTransform.position.y + bCollider.offset.y,
                        bCollider.width,
                        bCollider.height                        
                    );

                    if (collisionHappened) {
                        Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding width entity " + std::to_string(b.GetId()));
                        eventBus->EmitEvent<CollisionEvent>(a, b);
                    }
                }         
            }
        }

        bool CheckAABBCollision(double aX, double aY, double aW,  double aH, double bX, double bY, double bW,  double bH) {
            return (
                aX < bX + bW &&
                aX + aW > bX &&
                aY < bY + bY &&
                aY + aH > bY
            );
        }
};