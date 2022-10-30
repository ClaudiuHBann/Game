#include "Engine/pch.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/ManagerTexture.h"

#include "Generation/Dungeon.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

#define ZOOM_MIN 5
#define ZOOM_MAX 145

int main(int /*argc*/, char** /*argv*/) {
    Window window(
        "BSP Dungeon Generation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
        WINDOW_FLAGS, RENDERER_FLAGS
    );

    Dungeon dungeon(4, { WINDOW_WIDTH_START, WINDOW_HEIGHT_START });

    bool isRunning = true;
    SDL_Event event {};

	bool isMouseButtonDown = false;
	SDL_Point mousePosOnButtonDown { 0, 0 };
	SDL_Point mousePosDiff { 0, 0 };

	SDL_Point offsetMap { 0, 0 };

	SDL_Point windowSize { WINDOW_WIDTH_START, WINDOW_HEIGHT_START };

	float zoomCurrent = 1.f;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							windowSize = { event.window.data1, event.window.data2 };
							break;
					}

					break;

				case SDL_MOUSEBUTTONDOWN:
					isMouseButtonDown = true;
					SDL_GetMouseState(&mousePosOnButtonDown.x, &mousePosOnButtonDown.y);

					break;

				case SDL_MOUSEMOTION:
					if (isMouseButtonDown) {
						SDL_Point mousePosCurrent { 0, 0 };
						SDL_GetMouseState(&mousePosCurrent.x, &mousePosCurrent.y);

						mousePosDiff.x = -(mousePosOnButtonDown.x - mousePosCurrent.x);
						mousePosDiff.y = -(mousePosOnButtonDown.y - mousePosCurrent.y);
					}

					break;

				case SDL_MOUSEBUTTONUP:
					isMouseButtonDown = false;

					offsetMap.x += mousePosDiff.x;
					offsetMap.y += mousePosDiff.y;
					mousePosOnButtonDown = { 0, 0 };
					mousePosDiff = { 0, 0 };

					break;

				case SDL_MOUSEWHEEL:
					if (event.wheel.y > 0 && (int)(zoomCurrent * 100.f) <= ZOOM_MAX) {
						zoomCurrent += 0.05f;
					} else if (event.wheel.y < 0 && (int)(zoomCurrent * 10.f) >= ZOOM_MIN) {
						zoomCurrent -= 0.05f;
					}

					break;
            }
        }

        SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(window.GetRenderer());
        SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

        dungeon.Render(window.GetRenderer(), { zoomCurrent, zoomCurrent }, 5.f);

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
			- center zoom on player
*/