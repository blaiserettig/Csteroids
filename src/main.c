//gcc -I./include src/main.c -o main.exe -L./lib -lSDL3 

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t u32;

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400

struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
} state;

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    state.window = SDL_CreateWindow("Csteroids", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!state.window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    while (!state.quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    state.quit = true;
            }
        }

        SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(state.renderer);
        SDL_SetRenderDrawColor(state.renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderLine(state.renderer, 0, 0, 100, 100);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyWindow(state.window);
    SDL_Quit();

    return 0;
}
