#pragma once

#include <string>
#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include"../EventBus/Event.hpp"

class KeyPressedEvent: public Event {
    public:
        SDL_Keycode symbol;
        KeyPressedEvent(SDL_Keycode symbol): symbol(symbol) {}
};