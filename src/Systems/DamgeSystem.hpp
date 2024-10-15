#pragma once

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../Components/BoxColliderComponent.hpp"

class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Logger::Log("The Damage system received an event collision between entities " + std::to_string(event.a.GetId()) + " " + std::to_string(event.b.GetId()));
        }

        void Update() {}
};