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

#define TILE_SIZE (10.f)

Point<float> translation {};

void ScreenToObject(Point<float>& mousePos, const float zoom) {
    mousePos.SetX((mousePos.GetX() - translation.GetX()) / zoom);
    mousePos.SetY((mousePos.GetY() - translation.GetY()) / zoom);
}

void ObjectToScreen(Point<float>& mousePos, const float zoom) {
    mousePos.SetX(translation.GetX() + (mousePos.GetX() * zoom));
    mousePos.SetY(translation.GetY() + (mousePos.GetY() * zoom));
}

int main(int /*argc*/, char** /*argv*/) {
    Window window(
        "Dangian",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
        WINDOW_FLAGS, RENDERER_FLAGS
    );

    Dungeon dungeon(4, { WINDOW_WIDTH_START, WINDOW_HEIGHT_START }, TILE_SIZE);

    vector<vector<Dungeon::Tile>> tileMatrix;
    dungeon.GenerateTileMatrix(tileMatrix);

    for (size_t i = 0; i < tileMatrix.size(); i++) {
        for (size_t j = 0; j < tileMatrix[0].size(); j++) {
            logger << (int)tileMatrix[i][j] << " ";
        }
        logger << "\n";
    }

    bool isRunning = true;
    SDL_Event event {};

    bool isMouseButtonDown {};
    SDL_Point mousePosOnButtonDown {};
    Point<float> mousePosDiff {};

    Point<float> offsetMapLast {};
    Point<float> offsetMapCurrent {};

    float zoomCurrent = 1.f;
    float zoomScale = 0.05f;
    bool zoomed = false;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    {
                        if (zoomed) {
                            offsetMapLast = offsetMapCurrent;
                            translation = {};

                            zoomed = false;
                        }

                        isMouseButtonDown = true;
                        SDL_GetMouseState(&mousePosOnButtonDown.x, &mousePosOnButtonDown.y);
                    }

                    break;

                case SDL_MOUSEMOTION:
                    if (isMouseButtonDown) {
                        SDL_Point mousePosCurrent {};
                        SDL_GetMouseState(&mousePosCurrent.x, &mousePosCurrent.y);

                        mousePosDiff = { -(float)(mousePosOnButtonDown.x - mousePosCurrent.x),
                                         -(float)(mousePosOnButtonDown.y - mousePosCurrent.y) };

                        const float zoomCurrentInverted = 1.f / zoomCurrent;
                        offsetMapCurrent = offsetMapLast + mousePosDiff * zoomCurrentInverted;
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    {
                        isMouseButtonDown = false;
                        offsetMapLast = offsetMapCurrent;
                    }

                    break;

                case SDL_MOUSEWHEEL:
                    {
                        int zoomType = 0;
                        if (event.wheel.y > 0 && zoomCurrent <= ZOOM_MAX) {
                            zoomType = 1;
                        } else if (event.wheel.y < 0 && zoomCurrent >= ZOOM_MIN) {
                            zoomType = -1;
                        }

                        if (zoomType) {
                            zoomed = zoomType;

                            SDL_Point mousePosCurrent {};
                            SDL_GetMouseState(&mousePosCurrent.x, &mousePosCurrent.y);
                            Point<float> mousePos { (float)mousePosCurrent.x, (float)mousePosCurrent.y };

                            ScreenToObject(mousePos, zoomCurrent);
                            zoomCurrent += zoomScale * zoomType;
                            ObjectToScreen(mousePos, zoomCurrent);

                            translation.SetX(translation.GetX() + ((float)mousePosCurrent.x - mousePos.GetX()));
                            translation.SetY(translation.GetY() + ((float)mousePosCurrent.y - mousePos.GetY()));

                            offsetMapCurrent = offsetMapLast + translation;
                        }
                    }

                    break;
            }
        }

        SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(window.GetRenderer());
        SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

        dungeon.Render(
            window.GetRenderer(),
            { zoomCurrent, zoomCurrent },
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
            - zoom + move map = shit
            - add some comments because people tend to forget :)
*/