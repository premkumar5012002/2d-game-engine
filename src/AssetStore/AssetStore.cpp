#include "AssetStore.hpp"

#include "SDL2/SDL_image.h"

#include "../Logger/Logger.hpp"

AssetStore::AssetStore() {
    Logger::Log("AssetStore constructor called!");
}

AssetStore::~AssetStore() {
    ClearAssets();
    Logger::Log("AssetStore destructor called!");
}

void AssetStore::ClearAssets() {
    for (auto texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    for (auto font: fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();

    for (auto sound: sounds) {
        Mix_FreeChunk(sound.second);
    }
    sounds.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Add the texture to the map
    textures.emplace(assetId, texture);

    Logger::Log("New texture added to the Asset Store with id = " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
    return textures[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, int fontSize) {
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::GetFont(const std::string& assetId) {
    return fonts[assetId];
}

void AssetStore::AddSound(const std::string& assetId, const std::string& filePath) {
    sounds.emplace(assetId, Mix_LoadWAV(filePath.c_str()));
}

Mix_Chunk* AssetStore::GetSound(const std::string& assetID) const {
    return sounds.at(assetID);
}