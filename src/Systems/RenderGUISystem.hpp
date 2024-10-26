#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

#include "../ECS/ECS.hpp"

#include "../Components/SpriteComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileEmitterComponent.hpp"

class RenderGUISystem: public System {
    public:
        RenderGUISystem() = default;

        void Update(const SDL_Rect& camera, const std::unique_ptr<Registry>& registry) {
            ImGui::NewFrame();

            if (ImGui::Begin("Spawn enemies")) {
                static int posX = 0;
                static int posY = 0; 

                static int scaleX = 1;
                static int scaleY = 1;

                static float rotation = 0;

                static int health = 100;

                static int velocityX = 0;
                static int velocityY = 0;

                static float projectileAngle = 0.0;
                static float projectileSpeed = 100.0;
                static int projectileRepeat = 10;
                static int projectileDuration = 10;

                static int selectedSpriteIndex = 0;
                std::string sprites[] = { "tank-image", "truck-image" };

                if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if(ImGui::BeginCombo("Select sprite image", sprites[selectedSpriteIndex].c_str())) {
                        for (int n = 0; n < IM_ARRAYSIZE(sprites); n++) {
                            const bool isSelected = (selectedSpriteIndex == n);
                            if (ImGui::Selectable(sprites[n].c_str(), isSelected)) {
                                selectedSpriteIndex = n;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus(); 
                            }   
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::Spacing();

                if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen)) {                
                    ImGui::InputInt("position x", &posX);
                    ImGui::InputInt("position y", &posY);

                    ImGui::SliderInt("scale x", &scaleX, 1, 10);
                    ImGui::SliderInt("scale y", &scaleY, 1, 10);
 
                    ImGui::SliderFloat("rotation (deg)", &rotation, 0, 360);
                }
                ImGui::Spacing();

                if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::InputInt("velocity x", &velocityX);
                    ImGui::InputInt("velocity y", &velocityY);
                }
                ImGui::Spacing();

                if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::SliderFloat("angle (deg)", &projectileAngle, 0, 360);
                    ImGui::SliderFloat("speed (px/sec)", &projectileSpeed, 10.0, 500.0);
                    ImGui::InputInt("repeat (sec)", &projectileRepeat);
                    ImGui::InputInt("duration (sec)", &projectileDuration);
                }
                ImGui::Spacing();

                if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::SliderInt("%", &health, 1, 100);
                }
                ImGui::Spacing();

                if (ImGui::Button("Create new enemy")) {
                    Entity enemy = registry->CreateEntity();
                    enemy.Group("enemies");
                    enemy.AddComponent<HealthComponent>(health);
                    enemy.AddComponent<BoxColliderComponent>(32, 32);
                    enemy.AddComponent<RigidBodyComponent>(glm::vec2(velocityX, velocityY));
                    enemy.AddComponent<TransformComponent>(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY), glm::degrees(rotation));
                    enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 1);
                    enemy.AddComponent<ProjectileEmitterComponent>(
                        glm::vec2(
                            cos(projectileAngle) * projectileSpeed, 
                            sin(projectileAngle) * projectileSpeed
                        ), 
                        projectileRepeat * 1000, 
                        projectileDuration * 1000, 
                        10, 
                        false
                    );

                    // Reset all input values after we create a new enemy
                    scaleX = scaleY = 1;
                    projectileSpeed = health = 100;
                    projectileRepeat = projectileDuration = 10;
                    posX = posX = rotation = projectileAngle = 0;
                }                
            }
            ImGui::End();

            // Display a small overlay window to display the map position using the mouse
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
            ImGui::SetNextWindowBgAlpha(0.9f);
            if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
                ImGui::Text(
                    "Map coordinates (x=%.1f, y=%.1f)",
                    ImGui::GetIO().MousePos.x + camera.x,
                    ImGui::GetIO().MousePos.y + camera.y
                );
            }
            ImGui::End();

            ImGui::Render();
            ImGuiSDL::Render(ImGui::GetDrawData());
        }
};