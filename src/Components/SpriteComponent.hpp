#pragma once

#include <string>
#include <SDL2/SDL.h>

struct  SpriteComponent {
    int width;
    int height;
    int zIndex;
    SDL_Rect srcRect;
    std::string assetId;

    SpriteComponent(
        const std::string assetId = "", 
        int width = 0, 
        int height = 0,
        int zIndex = 0, 
        int srcRectX = 0,
        int srcRectY = 0
    ) {
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->assetId = assetId;
        this->srcRect = { srcRectX, srcRectY, width, height };
    }
};
