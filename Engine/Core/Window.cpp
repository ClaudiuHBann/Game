#include "pch.h"
#include "Utility/Miscellaneous.h"
#include "Window.h"

Window::Window() {
    mObjectCounter++;

    if (!mIsInitialized) {
        mIsInitialized = InitializeVideoAndEvents();
    }
}

Window::Window(const string& title, const uint32_t x, const uint32_t y, const uint16_t width, const uint16_t height, const uint32_t flagWindow, const uint32_t flagRenderer) : Window() {
    SetWindowAndRenderer(title, x, y, width, height, flagWindow, flagRenderer);
}

Window::~Window() {
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
    }

    if (mWindow) {
        SDL_DestroyWindow(mWindow);
    }

    if (!--mObjectCounter && mIsInitialized) {
        SDL_Quit();
    }
}

void Window::SetWindowAndRenderer(const string& title, const uint32_t x, const uint32_t y, const uint16_t width, const uint16_t height, const uint32_t flagWindow, const uint32_t flagRenderer) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, );

    if (mIsInitialized && _CreateWindow(title, x, y, width, height, flagWindow)) {
        CreateRenderer(-1, flagRenderer);
    }
}

SDL_Window* Window::GetWindow() {
    return mWindow;
}

SDL_Renderer* Window::GetRenderer() {
    return mRenderer;
}

bool Window::_CreateWindow(const string& title, const uint32_t x, const uint32_t y, const uint16_t width, const uint16_t height, const uint32_t flags) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, false);

    mWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);
    if (!mWindow) {
        LOG("Window \"" + title + "\" could not be created! Error: " + string(SDL_GetError()) + '.', LOG_TYPE_ERROR);
        return false;
    }

    return true;
}

void Window::CreateRenderer(int32_t index, const uint32_t flags) {
    LOG_AND_RETURN_IF_NOT_INIT(mIsInitialized, mSubsystems, );

    mRenderer = SDL_CreateRenderer(mWindow, index, flags);
    if (!mRenderer) {
        LOG("Renderer for window \"" + string(SDL_GetWindowTitle(mWindow)) + "\" could not be created! Error: " + string(SDL_GetError()) + '.', LOG_TYPE_WARNING);
    }
}

bool Window::InitializeVideoAndEvents() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        LOG(mSubsystems + " could not be initialized! Error: " + string(SDL_GetError()) + '.', LOG_TYPE_ERROR);
        return false;
    }

    return true;
}
