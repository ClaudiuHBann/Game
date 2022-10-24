#include "Engine/pch.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/ManagerTexture.h"

#include "Generation/Dungeon.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

int main(int /*argc*/, char** /*argv*/) {
    Window window(
        "BSP Dungeon Generation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
        WINDOW_FLAGS, RENDERER_FLAGS
    );

    Dungeon dungeon(4, { WINDOW_WIDTH_START, WINDOW_HEIGHT_START }, { 1.25f, 1.25f }, 5.f);

    bool isRunning = true;
    SDL_Event event {};

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(window.GetRenderer());
        SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

        dungeon.Render(window.GetRenderer());

        SDL_RenderPresent(window.GetRenderer());
    }

    return EXIT_SUCCESS;
}

/*
    TO DO:
            - make the alg iterative
            - check every shit because OCD...
            - solve the dependency problem
            - add namespaces to keep the engine directory included
            - what my inline is happening
*/