#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Csteroids", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Delay(10000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
