#include "Engine/pch.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/ManagerTexture.h"

#include "Engine/Utility/Miscellaneous.h"

#include "Generation/Dungeon.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

#define ZOOM_MIN (0.5f)
#define ZOOM_MAX (1.5f)
#define ZOOM_DIFF ((ZOOM_MAX) - (ZOOM_MIN))

int main(int /*argc*/, char** /*argv*/) {
    Window window(
        "Dangian",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
        WINDOW_FLAGS, RENDERER_FLAGS
    );

    Dungeon dungeon(4, { WINDOW_WIDTH_START, WINDOW_HEIGHT_START });

    bool isRunning = true;
    SDL_Event event {};

    bool isMouseButtonDown {};
    SDL_Point mousePosOnButtonDown {};
    SDL_Point mousePosDiff {};

    Point<float> offsetMapLast {};
    Point<float> offsetMapCurrent {};

    float zoomCurrent = ZOOM_DIFF;
    float zoomScale = 0.05f;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    {
                        isMouseButtonDown = true;
                        SDL_GetMouseState(&mousePosOnButtonDown.x, &mousePosOnButtonDown.y);
                    }

                    break;

                case SDL_MOUSEMOTION:
                    if (isMouseButtonDown) {
                        SDL_Point mousePosCurrent {};
                        SDL_GetMouseState(&mousePosCurrent.x, &mousePosCurrent.y);

                        mousePosDiff = { -(mousePosOnButtonDown.x - mousePosCurrent.x),
                                         -(mousePosOnButtonDown.y - mousePosCurrent.y) };

                        float zoomCurrentInverted = 1.f / zoomCurrent;
                        offsetMapCurrent = { offsetMapLast.GetX() + mousePosDiff.x * zoomCurrentInverted,
                                             offsetMapLast.GetY() + mousePosDiff.y * zoomCurrentInverted };
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    {
                        isMouseButtonDown = false;
                        offsetMapLast = offsetMapCurrent;
                    }

                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0 && zoomCurrent <= ZOOM_MAX) {
                        zoomCurrent += zoomScale;
                    } else if (event.wheel.y < 0 && zoomCurrent >= ZOOM_MIN) {
                        zoomCurrent -= zoomScale;
                    }
            }
        }

        SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(window.GetRenderer());
        SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

        dungeon.Render(
            window.GetRenderer(),
            { zoomCurrent, zoomCurrent }, 5.f,
            offsetMapCurrent
        );

        SDL_RenderPresent(window.GetRenderer());
    }

    return EXIT_SUCCESS;
}

/*
    TO DO:
            - make the alg iterative
            - check every shit because OCD...
            - add namespaces to keep the popular naming everywhere
            - The Engine needs more abstractization to stop using the sdl2 dependencies in the actual game
            - center zoom on mouse cursor
*/