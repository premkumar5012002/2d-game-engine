#pragma once

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"

#include "../Events/KeyPressedEvent.hpp"

#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"

class KeyboardControlSystem: public System {
    public:
        KeyboardControlSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<KeyboardControlledComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::onKeyPressed);
        }

        void onKeyPressed(KeyPressedEvent& event) {
            for (auto entity: GetSystemEntities()) {
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                const auto keyboardControl = entity.GetComponent<KeyboardControlledComponent>();

                switch (event.symbol) {
                    case SDLK_UP:
                        rigidBody.velocity = keyboardControl.upVelocity;
                        sprite.srcRect.y = sprite.height * 0;
                        break;

                    case SDLK_RIGHT:
                        rigidBody.velocity = keyboardControl.rightVelocity;
                        sprite.srcRect.y = sprite.height * 1;
                        break;

                    case SDLK_DOWN:
                        rigidBody.velocity = keyboardControl.downVelocity;
                        sprite.srcRect.y = sprite.height * 2;
                        break;

                    case SDLK_LEFT:
                        rigidBody.velocity = keyboardControl.leftVelocity;
                        sprite.srcRect.y = sprite.height * 3;
                        break;
                }
            }
        }

        void Update() {}
};