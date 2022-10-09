#include "pch.h"
#include "Utility/Miscellaneous.h"
#include "ManagerTexture.h"

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
