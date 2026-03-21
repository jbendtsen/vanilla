#include "vanilla.h"
#include <SDL2/SDL.h>

#define INITIAL_WIDTH  640
#define INITIAL_HEIGHT 480

#define SHOULD_QUIT    1
#define SHOULD_RENDER  2

int main(int argc, char **argv) {
    int res = SDL_Init(SDL_INIT_VIDEO);
    if (res != 0) {
        SDL_Log("SDL_Init() failed (%d)", res);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Vanilla",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        INITIAL_WIDTH,
        INITIAL_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("SDL_CreateWindow() failed");
        return 1;
    }

    float dpiX = 0;
    float dpiY = 0;
    SDL_GetDisplayDPI(0, NULL, &dpiX, &dpiY);

    Freetype ft = {0};
    if (!initFreetype(&ft, (int)dpiX, (int)dpiY)) {
        SDL_Log("initFreetype() failed");
        return 1;
    }

    AllFontCaches fonts = {0};
    if (!initAllFonts(&fonts, &ft)) {
        SDL_Log("initAllFonts() failed");
        return 1;
    }

    Input input = {0};
    FileView file = {0};
    Syntax syntax = {0};
    Layout layout = {0};
    Theme theme = {0};

    int flags = SHOULD_RENDER;
    while ((flags & SHOULD_QUIT) == 0) {
        // TODO: some logic here to set SHOULD_RENDER if something else changed
        //       e.g. new file or menu opened

        SDL_Event event = {0};
        while ((flags & SHOULD_QUIT) == 0) {
            if (!SDL_PollEvent(&event)) {
                if (flags)
                    break;
                if (!SDL_WaitEvent(&event)) {
                    flags |= SHOULD_QUIT;
                    break;
                }
            }
            switch (event.type) {
            case SDL_QUIT:
                flags |= SHOULD_QUIT;
                break;
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_KEYDOWN:
                flags |= SHOULD_RENDER;
                break;
            }
        }
        if ((flags & SHOULD_RENDER) == 0 || (flags & SHOULD_QUIT))
            continue;

        int sW, sH;
        SDL_GetWindowSize(window, &sW, &sH);
        SDL_Surface *surface = SDL_GetWindowSurface(window);
        SDL_LockSurface(surface);

        layout.windowWidth = sW;
        layout.windowHeight = sH;
        Rect area = {0, 0, sW, sH};

        draw(
            &input,
            &file,
            &syntax,
            &layout,
            &theme,
            &fonts,
            &area,
            surface->pixels
        );

        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);

        flags = 0;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    closeAllFonts(&fonts);
    return 0;
}
