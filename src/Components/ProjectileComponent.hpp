#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentage;
    int duration;
    int startTime;

    ProjectileComponent(bool isFriendly = false, int hitPercentage = 0, int duration = 0) {
        this->isFriendly = isFriendly;
        this->duration = duration;
        this->hitPercentage = hitPercentage;
        this->startTime = SDL_GetTicks64();
    }
};
