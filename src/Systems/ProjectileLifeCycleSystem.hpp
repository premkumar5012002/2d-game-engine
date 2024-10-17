#pragma once

#include "../ECS/ECS.hpp"

#include "../Components/ProjectileComponent.hpp"

class ProjectileLifeCycleSystem: public System {
    public:
        ProjectileLifeCycleSystem() {
            RequireComponent<ProjectileComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                auto projectile = entity.GetComponent<ProjectileComponent>();

                // Kill projectile after they reach their duration limit
                if (SDL_GetTicks64() - projectile.startTime > projectile.duration) {
                    entity.Kill();
                }
            }
        }
};