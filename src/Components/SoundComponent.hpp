#pragma once

#include <string>

struct SoundComponent {
    std::string assetID;
    int loopCount;

    SoundComponent(std::string assetID = "", int loopCount = 0) {
        this->assetID = assetID;
        this->loopCount = loopCount;
    }
};