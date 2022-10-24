#pragma once

#define IMG_INIT_EVERYTHING (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP)

class ManagerTexture {
public:
    ManagerTexture();

    ~ManagerTexture();

    static void Render(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Point& position, const SDL_FPoint& size = { 1.f, 1.f }, const SDL_Rect& textureRect = { -1, -1, -1, -1 });

    SDL_Texture* Load(SDL_Renderer* renderer, const std::string& path);

    void Remove(SDL_Texture* texture);

    SDL_Texture* GetTexture(const string& path);

private:
    static inline bool mIsInitialized = false;
    static inline uint32_t mObjectCounter = 0;

    map<string, SDL_Texture*> mTextures;

    static inline string mSubsystems = "SDL_image";
};