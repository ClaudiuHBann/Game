#include "pch.h"
#include "Utility/Miscellaneous.h"
#include "Utility/ManagerFile.h"
#include "ManagerTexture.h"

ManagerTexture::ManagerTexture() {
    mObjectCounter++;

    if (!mIsInitialized) {
        if (!IMG_Init(IMG_INIT_EVERYTHING)) {
            LOG(mSubsystems + " could not be initialized! Error: " + IMG_GetError() + '.', LOG_TYPE_ERROR);
        } else {
            mIsInitialized = true;
        }
    }
}

ManagerTexture::~ManagerTexture() {
    for (const auto& item : mTextures) {
        if (item.second) {
            SDL_DestroyTexture(item.second);
        }
    }

    if (!--mObjectCounter && mIsInitialized) {
        IMG_Quit();
    }
}

///
void ManagerTexture::Render(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Point& position, const SDL_FPoint& size, const SDL_Rect& textureRect) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, );
    LOG_AND_RETURN_IF_PARAM_IS_NULL(renderer, );
    LOG_AND_RETURN_IF_PARAM_IS_NULL(texture, );

    SDL_Rect textureRectFinal = { position.x, position.y, 0, 0 };
    SDL_QueryTexture(texture, nullptr, nullptr, &textureRectFinal.w, &textureRectFinal.h);

    textureRectFinal.w = (int)(textureRectFinal.w * size.x);
    textureRectFinal.h = (int)(textureRectFinal.h * size.y);

    if (textureRect.x == -1 && textureRect.y == -1 &&
        textureRect.w == -1 && textureRect.h == -1) {
        SDL_RenderCopy(renderer, texture, nullptr, &textureRectFinal);
    } else {
        SDL_RenderCopy(renderer, texture, &textureRect, &textureRectFinal);
    }
}

SDL_Texture* ManagerTexture::Load(SDL_Renderer* renderer, const string& path) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, nullptr);
    LOG_AND_RETURN_IF_PARAM_IS_NULL(renderer, nullptr);
    if (!ManagerFile::Exists(path)) {
        LOG("The texture's path \"" + path + "\" is invalid!", LOG_TYPE_WARNING);
        return nullptr;
    }

    if (mTextures.find(path) != mTextures.end()) {
        return mTextures[path];
    }

    SDL_Texture* newTexture = nullptr;

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        LOG("The surface for the texture \"" + path + "\" could not be loaded! Error: " + IMG_GetError() + '.', LOG_TYPE_WARNING);
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!newTexture) {
            LOG("The texture \"" + path + "\" could not be created! Error: " + IMG_GetError() + '.', LOG_TYPE_WARNING);
        } else {
            mTextures[path] = newTexture;
        }

        SDL_FreeSurface(surface);
    }

    return newTexture;
}

void ManagerTexture::Remove(SDL_Texture* texture) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, );
    LOG_AND_RETURN_IF_PARAM_IS_NULL(texture, );

    for (const auto& item : mTextures) {
        if (texture == item.second) {
            mTextures.erase(item.first);
            break;
        }
    }

    SDL_DestroyTexture(texture);
}

SDL_Texture* ManagerTexture::GetTexture(const string& path) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, nullptr);

    if (!ManagerFile::Exists(path)) {
        LOG("The texture's path \"" + path + "\" is invalid!", LOG_TYPE_WARNING);
        return nullptr;
    }

    return (mTextures.find(path) != mTextures.end()) ? mTextures[path] : nullptr;
}
