#include "vanilla.h"
#include <SDL3/SDL.h>

#define INITIAL_WIDTH  640
#define INITIAL_HEIGHT 480

#define SHOULD_QUIT    1
#define SHOULD_RENDER  2

int main(int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init() failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Vanilla",
        INITIAL_WIDTH,
        INITIAL_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("SDL_CreateWindow() failed");
        return 1;
    }

    float dsScale = SDL_GetWindowDisplayScale(window);
    float dpiX = 96.0 * dsScale;
    float dpiY = 96.0 * dsScale;

    int sW, sH;
    SDL_GetWindowSize(window, &sW, &sH);

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

    theme.backEditor = 0xffffff;
    theme.foreEditor = 0x000000;

    int counter = 0;

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
            case SDL_EVENT_QUIT:
                flags |= SHOULD_QUIT;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                sW = event.window.data1;
                sH = event.window.data2;
                SDL_UpdateWindowSurface(window);
                // don't break here
            //case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                flags |= SHOULD_RENDER;
                break;
            }
        }
        if ((flags & SHOULD_RENDER) == 0 || (flags & SHOULD_QUIT))
            continue;

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
        counter++;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    closeAllFonts(&fonts);
    return 0;
}

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args);
    SDL_LogMessageV(0, SDL_LOG_PRIORITY_INFO, fmt, args);
    va_end(args);
}
