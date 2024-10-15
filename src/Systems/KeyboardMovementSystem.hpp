#pragma once

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"

class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {}

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onKeyPressed);
        }

        void onKeyPressed(KeyPressedEvent& event) {
            std::string keyCode = std::to_string(event.symbol);
            std::string keySymbol(1, event.symbol);
            Logger::Log("Key pressed event emitted: [" + keyCode + "] " + keySymbol);
        }

        void Update() {}
};