#pragma once

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"

#include "../Components/HealthComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
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
            Entity a = event.a;
            Entity b = event.b;
            Logger::Log("The Damage system received an event collision between entities " + std::to_string(a.GetId()) + " " + std::to_string(b.GetId()));

            if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
                OnProjectileHitsPlayer(a, b);
            }

            if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
                OnProjectileHitsPlayer(b, a);
            }

            if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(a, b);
            }

            if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
                OnProjectileHitsEnemy(b, a);
            }
        }

        void OnProjectileHitsPlayer(Entity projectile, Entity player) {
            auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

            if (!projectileComponent.isFriendly) {
                // Reduce the health of the player by the projectile hitPercentDamage
                auto& health = player.GetComponent<HealthComponent>();

                // Subtract the health of the player 
                health.healthPercentage -= projectileComponent.hitPercentage;

                // Kills the player when health reaches zero
                if (health.healthPercentage <= 0) {
                    player.Kill();
                }

                // Kill the projectile
                projectile.Kill();
            }  
        }

        void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
            auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

            // Only damage the enemy if projectile is friendly
            if (projectileComponent.isFriendly) {
                // Reduce the health of the player by the projectile hitPercentDamage
                auto& health = enemy.GetComponent<HealthComponent>();

                // Subtract the health of the player 
                health.healthPercentage -= projectileComponent.hitPercentage;

                // Kills the enemy when health reaches zero
                if (health.healthPercentage <= 0) {
                    enemy.Kill();
                }

                // Kill the projectile
                projectile.Kill();
            }  
        }        

        void Update() {}
};