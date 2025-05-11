//gcc -I./include src/main.c -o main.exe -L./lib -lSDL3 

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <stdlib.h>

#include "main.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t u32;

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400

#define THRUST 0.5

typedef struct {
    float x;
    float y;
} v2;

typedef struct {
    v2 position;
    v2 velocity;
    float angle;
    float speedScale;
} ship;

struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
    ship ship;
    int w;
    int a;
    int d;
} state;


v2 ship_points[] = {
    {-5, -10},
    {0, 10},
    {5, -10},
    {2, -7},
    {-2, -7},
    {-5, -10}
};

int main(int argc, char* argv[]) {

    state.ship.position.x = SCREEN_WIDTH / 2;
    state.ship.position.y = SCREEN_HEIGHT / 2;
    state.ship.velocity.x = 0;
    state.ship.velocity.y = 0;
    state.ship.angle = 0;
    state.w = 0;
    state.a = 0;
    state.d = 0;


    if (!SDL_Init(SDL_INIT_VIDEO)) {
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
        SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(state.renderer);
        SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        update();
        SDL_RenderPresent(state.renderer);
        SDL_Delay(16);
    }

    SDL_DestroyWindow(state.window);
    SDL_Quit();

    return 0;
}

void update() {
    handle_input();

    state.ship.position.x = wrap(state.ship.position.x + state.ship.velocity.x, SCREEN_WIDTH);
    state.ship.position.y = wrap(state.ship.position.y + state.ship.velocity.y, SCREEN_HEIGHT);

    float radians = state.ship.angle * (M_PI / 180.0f);
    if (state.w) {
        state.ship.velocity.x -= sinf(radians) * THRUST;
        state.ship.velocity.y += cosf(radians) * THRUST;
    }
    if (state.a) {
        state.ship.angle -= 5;
    }
    if (state.d) {
        state.ship.angle += 5;
    }
    
    apply_friction(&state.ship.velocity.x, 0.04f);
    apply_friction(&state.ship.velocity.y, 0.04f);

    state.ship.velocity.x = clamp(state.ship.velocity.x, -10, 10);
    state.ship.velocity.y = clamp(state.ship.velocity.y, -10, 10);

    render_ship();
}

void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                state.quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.scancode) {
                    case SDL_SCANCODE_W:
                        state.w = 1;
                        break;
                    case SDL_SCANCODE_D:
                        state.d = 1;
                        break;
                    case SDL_SCANCODE_A:
                        state.a = 1;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_KEY_UP:
                switch (event.key.scancode) {
                    case SDL_SCANCODE_W:
                        state.w = 0;
                        break;
                    case SDL_SCANCODE_D:
                        state.d = 0;
                        break;
                    case SDL_SCANCODE_A:
                        state.a = 0;
                        break;
                    default:
                        break;
            }
            break;
        }
    }
}

void render_ship() {
    v2 pos;
    pos.x = state.ship.position.x;
    pos.y = state.ship.position.y;

    v2 new_points[6];

    for (int i = 0; i < 6; i++) {
        new_points[i] = ship_points[i];
        new_points[i].x = ship_points[i].x * cos(state.ship.angle * (M_PI / 180.f)) - ship_points[i].y * sin(state.ship.angle * (M_PI / 180.f));
        new_points[i].y = ship_points[i].x * sin(state.ship.angle * (M_PI / 180.f)) + ship_points[i].y * cos(state.ship.angle * (M_PI / 180.0f));
    }

    for (int i = 0; i < 5; i++) {
        SDL_RenderLine(state.renderer, pos.x + new_points[i].x, pos.y + new_points[i].y, pos.x + new_points[i + 1].x, pos.y + new_points[i + 1].y);
    }
}

float wrap(float given, float max) {
    if (given < 0) return max;
    if (given > max) return 0;
    return given;
}

void apply_friction(float *v, float amount) {
    if (*v > 0) *v = fmaxf(0, *v - amount);
    else if (*v < 0) *v = fminf(0, *v + amount);
}

float clamp(float val, float min, float max) {
    return fmaxf(min, fminf(max, val));
}
