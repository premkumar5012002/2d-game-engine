#include "Game.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <SDL2/SDL_image.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl2.h>

#include "../ECS/ECS.hpp"
#include "./LevelLoader.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"

#include "../Systems/DamgeSystem.hpp"
#include "../Systems/SoundSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/ScriptSystem.hpp"
#include "../Systems/MovementSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/RenderGUISystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/RenderTextSystem.hpp"
#include "../Systems/CameraMovementSystem.hpp"
#include "../Systems/ProjectileEmitSystem.hpp"
#include "../Systems/RenderColliderSystem.hpp"
#include "../Systems/RenderHealthBarSystem.hpp"
#include "../Systems/KeyboardControlSystem.hpp"
#include "../Systems/ProjectileLifeCycleSystem.hpp"

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
    isDebug = false;
    isRunning = false;
    millisecondsPreviousFrame = 0;
    registry = std::make_unique<Registry>();
    eventBus = std::make_unique<EventBus>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game constructor called!");
}

Game::~Game() {
    Logger::Log("Game destructor called!");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL.");
        return;
    }

    if (TTF_Init() != 0) {
        Logger::Err("Error initializing SDL TTF.");
        return;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
        Logger::Err("Error initializing SDL Mixer.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    window = SDL_CreateWindow(
        "2D Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        Logger::Err("Error creating SDL window.");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        return;
    }

    // Initialize the ImGui context
    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

    // Initialize the camera view with the entire screen area
    camera.x = 0;
    camera.y = 0;
    camera.w = windowWidth;
    camera.h = windowHeight;

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run() {
    Setup();
    while(isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Setup() {
    // Add the systems that need to be processed in our game
    registry->AddSystem<SoundSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<ScriptSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<RenderGUISystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<RenderHealthBarSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<ProjectileLifeCycleSystem>();

    // Create the bindings between C++ and Lua
    registry->GetSystem<ScriptSystem>().CreateLuaBinding(lua);

    LevelLoader levelLoader;
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
    levelLoader.LoadLevel(lua, registry, assetStore, renderer, 1);
}

void Game::ProcessInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // ImGui SDL input
        ImGui_ImplSDL2_ProcessEvent(&event);
        ImGuiIO& io = ImGui::GetIO();

        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        // Handle core SDL events (close window, key pressed, etc.)
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN: {

                eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                } else if (event.key.keysym.sym == SDLK_F1) {
                    isDebug = !isDebug;
                }

                break;
            }
        }
    }
}

void Game::Update() {
    int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks64() - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    // Difference in ticks from last frame, coverted to seconds
    double deltaTime = (SDL_GetTicks64() - millisecondsPreviousFrame) / 1000.0;

    // Store the current frame time in previous frame for next frame update
    millisecondsPreviousFrame = SDL_GetTicks64();

    // Reset all event handlers for the current frame
    eventBus->Reset();

    // Perform the subscription of the events for all systems
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Ask all the systems to update
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<SoundSystem>().Update(assetStore);
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<ProjectileLifeCycleSystem>().Update();
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry);
    registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks64());
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to render
    registry->GetSystem<RenderSystem>().Update(camera, renderer, assetStore);
    registry->GetSystem<RenderTextSystem>().Update(camera, renderer, assetStore);
    registry->GetSystem<RenderHealthBarSystem>().Update(camera, renderer, assetStore);

    if (isDebug) {
        registry->GetSystem<RenderGUISystem>().Update(camera, registry);
        registry->GetSystem<RenderColliderSystem>().Update(camera, renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
