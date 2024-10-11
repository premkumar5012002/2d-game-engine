#pragma once

#include "../ECS/ECS.hpp"

class MovementSystem: public System {
    public:
        MovementSystem() {

        }

        void Update() {
            for (auto entity: GetEntities()) {
                
            }
        }
};