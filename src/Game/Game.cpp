#include "Game.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>

#include "../ECS/ECS.hpp"
#include "../Logger/Logger.hpp"
#include "../EventBus/EventBus.hpp"

#include "../Events/KeyPressedEvent.hpp"

#include "../Systems/DamgeSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/MovementSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/CameraMovementSystem.hpp"
#include "../Systems/ProjectileEmitSystem.hpp"
#include "../Systems/RenderColliderSystem.hpp"
#include "../Systems/KeyboardControlSystem.hpp"
#include "../Systems/ProjectileLifeCycleSystem.hpp"

#include "../Components/SpriteComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include "../Components/ProjectileEmitterComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"

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

void Game::LoadLevel(int level) {
    // Add the systems that need to be processed in our game
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<ProjectileLifeCycleSystem>();

    // Adding assets to the asset store    
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");

    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.Group("tile");
            tile.AddComponent<TransformComponent>(
                glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                glm::vec2(tileScale, tileScale),
                0.0  
            );
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    
    mapFile.close();
    mapWidth = mapNumCols * tileSize * tileScale;
    mapHeight = mapNumRows * tileSize * tileScale;

    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 100.0), 0, 10000, 10, true);
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0, -50), glm::vec2(50, 0), glm::vec2(0, 60), glm::vec2(-60, 0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);      

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 3, true);
    radar.AddComponent<AnimationComponent>(8, 5, true); 

    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100, 0.0), 5000, 3000, 10, false);
    tank.AddComponent<HealthComponent>(100); 
 
    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 5000, 10, false);
    truck.AddComponent<HealthComponent>(100); 
}

void Game::Setup() {
    LoadLevel(1);
}

void Game::ProcessInput() { 
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN: {

                eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                } else if (event.key.keysym.sym == SDLK_d) {
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
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Ask all the systems to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry);
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<ProjectileLifeCycleSystem>().Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);  

    // Invoke all the systems that need to render
    registry->GetSystem<RenderSystem>().Update(camera, renderer, assetStore);

    if (isDebug) {
        registry->GetSystem<RenderColliderSystem>().Update(renderer);
    } 

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}