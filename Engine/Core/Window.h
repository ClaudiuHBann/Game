#pragma once

class Window {
public:
    Window();

    Window(
        const string& title,
        const uint32_t x, const uint32_t y,
        const uint16_t width, const uint16_t height,
        const uint32_t flagWindow, const uint32_t flagRenderer
    );

    ~Window();

    void SetWindowAndRenderer(
        const string& title,
        const uint32_t x, const uint32_t y,
        const uint16_t width, const uint16_t height,
        const uint32_t flagWindow, const uint32_t flagRenderer
    );

    SDL_Window* GetWindow();

    SDL_Renderer* GetRenderer();

private:
    bool _CreateWindow(
        const string& title,
        const uint32_t x, const uint32_t y,
        const uint16_t width, const uint16_t height,
        const uint32_t flags
    );

    void CreateRenderer(int32_t index, const uint32_t flags);

    bool InitializeVideoAndEvents();

    static inline bool mIsInitialized = false;
    static inline uint8_t mObjectCounter = 0;

    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;

    static inline string mSubsystems = "SDL's VIDEO and EVENTS";
};