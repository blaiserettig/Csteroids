//gcc -I./include src/main.c -o main.exe -L./lib -lSDL3 

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t u32;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define THRUST 0.3

#define MAX_ASTEROIDS 32

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
    small_asteroid small_asteroids[MAX_ASTEROIDS];
    int small_asteroid_count;
    medium_asteroid medium_asteroids[MAX_ASTEROIDS];
    int medium_asteroid_count;
    large_asteroid large_asteroids[MAX_ASTEROIDS];
    int large_asteroid_count;
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

    state.ship.position.x = SCREEN_WIDTH / 2.0;
    state.ship.position.y = SCREEN_HEIGHT / 2.0;
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

    update_ship();
    update_asteroids();

    ship_collision_check();

    render_ship();
    render_asteroids();
}

void update_ship() {
    state.ship.position.x = wrapf(state.ship.position.x + state.ship.velocity.x, SCREEN_WIDTH);
    state.ship.position.y = wrapf(state.ship.position.y + state.ship.velocity.y, SCREEN_HEIGHT);

    const float radians = state.ship.angle * ((float)M_PI / 180.0f);
    if (state.w) {
        state.ship.velocity.x -= sinf(radians) * (float)THRUST;
        state.ship.velocity.y += cosf(radians) * (float)THRUST;
        render_booster();
    }
    if (state.a) {
        state.ship.angle -= 5;
    }
    if (state.d) {
        state.ship.angle += 5;
    }
    
    apply_friction(&state.ship.velocity.x, 0.06f);
    apply_friction(&state.ship.velocity.y, 0.06f);

    state.ship.velocity.x = clampf(state.ship.velocity.x, -10, 10);
    state.ship.velocity.y = clampf(state.ship.velocity.y, -10, 10);
}

void update_asteroids() {
    for (int i = 0; i < state.small_asteroid_count; i++) {

        state.small_asteroids[i].position.x =
            wrapf(state.small_asteroids[i].position.x +=
                state.small_asteroids[i].velocity.x, SCREEN_WIDTH);

        state.small_asteroids[i].position.y =
            wrapf(state.small_asteroids[i].position.y +=
                state.small_asteroids[i].velocity.y, SCREEN_HEIGHT);
    }
    for (int i = 0; i < state.medium_asteroid_count; i++) {

        state.medium_asteroids[i].position.x =
            wrapf(state.medium_asteroids[i].position.x +=
                state.medium_asteroids[i].velocity.x, SCREEN_WIDTH);

        state.medium_asteroids[i].position.y =
            wrapf(state.medium_asteroids[i].position.y +=
                state.medium_asteroids[i].velocity.y, SCREEN_HEIGHT);
    }
    for (int i = 0; i < state.large_asteroid_count; i++) {

        state.large_asteroids[i].position.x =
            wrapf(state.large_asteroids[i].position.x +=
                state.large_asteroids[i].velocity.x, SCREEN_WIDTH);

        state.large_asteroids[i].position.y =
            wrapf(state.large_asteroids[i].position.y +=
                state.large_asteroids[i].velocity.y, SCREEN_HEIGHT);
    }
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
                    case SDL_SCANCODE_F:
                        generate_small_asteroid();
                        generate_large_asteroid();
                        generate_medium_asteroid();
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
            default:
                break;
        }
    }
}

void render_ship() {
    v2* new_points = render_angle_helper(ship_points, 6);

    const v2 pos = state.ship.position;
    for (int i = 0; i < 5; i++) {
        SDL_RenderLine(state.renderer, pos.x + new_points[i].x, pos.y + new_points[i].y,
            pos.x + new_points[i + 1].x, pos.y + new_points[i + 1].y);
    }

    free(new_points);
}

void render_asteroids() {
    for (int i = 0; i < state.small_asteroid_count; i++) {
        render_asteroids_helper(state.small_asteroids[i].position, state.small_asteroids[i].points, 9);
    }
    for (int i = 0; i < state.medium_asteroid_count; i++) {
        render_asteroids_helper(state.medium_asteroids[i].position, state.medium_asteroids[i].points, 11);
    }
    for (int i = 0; i < state.large_asteroid_count; i++) {
        render_asteroids_helper(state.large_asteroids[i].position, state.large_asteroids[i].points, 12);
    }
}

void render_asteroids_helper(const v2 pos, const v2* points, const int p_count) {
    for (int i = 0; i < p_count; i++) {
        const int next = (i + 1) % p_count;

        SDL_RenderLine(state.renderer, pos.x + points[i].x, pos.y + points[i].y,
            pos.x + points[next].x, pos.y + points[next].y);
    }
}

void render_booster() {
    const v2 booster_points[] = {
        {-2, -7},
        {0, -13},
        {2, -7},
    };

    v2* new_points = render_angle_helper(booster_points, 3);

    const v2 pos = state.ship.position;

    SDL_RenderLine(state.renderer, pos.x + new_points[0].x, pos.y + new_points[0].y,
        pos.x + new_points[1].x, pos.y + new_points[1].y);

    SDL_RenderLine(state.renderer, pos.x + new_points[1].x, pos.y + new_points[1].y,
        pos.x + new_points[2].x, pos.y + new_points[2].y);

    free(new_points);
}

v2* render_angle_helper(const v2 *points, const int n) {
    v2* new_points = malloc(n * sizeof(v2));
    for (int i = 0; i < n; i++) {
        new_points[i] = points[i];

        new_points[i].x = points[i].x * cosf(state.ship.angle *
            ((float)M_PI / 180.f)) - points[i].y * sinf(state.ship.angle * ((float)M_PI / 180.f));

        new_points[i].y = points[i].x * sinf(state.ship.angle *
            ((float)M_PI / 180.f)) + points[i].y * cosf(state.ship.angle * ((float)M_PI / 180.0f));
    }
    return new_points;
}

void ship_collision_check() {

    // TODO: refactor literally all of this because it only sort of works. Probably need to just steal the points
    // from render_ship which makes a lot more sense now that I think about it

    for (int i = 0; i < 6; i++) {
        const int next_ship = (i + 1) % 6;

        for (int j = 0; j < state.small_asteroid_count; j++) {
            for (int k = 0; k < 9; k++) {
                const int next_small = (k + 1) % 9;
                const v2 a_p = state.small_asteroids[j].position;
                const v2 s_p = state.ship.position;

                v2 res = v2_intersection(
                    v2_sum(ship_points[i], s_p),
                    v2_sum(ship_points[next_ship], s_p),
                    v2_sum(state.small_asteroids[j].points[k], a_p),
                    v2_sum(state.small_asteroids[j].points[next_small], a_p));

                if (isnan(res.x) || isnan(res.y)) {
                    continue;
                }
                printf("COLLISION AT: %f, %f \n",  res.x, res.y);
                highlight_collision(res);
            }
        }
        for (int j = 0; j < state.medium_asteroid_count; j++) {
            for (int  k = 0; k < 11; k++) {
                const int next_medium = (k + 1) % 11;
                const v2 a_p = state.medium_asteroids[j].position;
                const v2 s_p = state.ship.position;

                v2 res = v2_intersection(
                    v2_sum(ship_points[i], s_p),
                    v2_sum(ship_points[next_ship], s_p),
                    v2_sum(state.medium_asteroids[j].points[k], a_p),
                    v2_sum(state.medium_asteroids[j].points[next_medium], a_p));

                if (isnan(res.x) || isnan(res.y)) {
                    continue;
                }
                printf("COLLISION AT: %f, %f \n",  res.x, res.y);
                highlight_collision(res);
            }
        }
        for (int j = 0; j < state.large_asteroid_count; j++) {
            for (int k = 0; k < 12; k++) {
                const int next_large = (k + 1) % 12;
                const v2 a_p = state.large_asteroids[j].position;
                const v2 s_p = state.ship.position;

                v2 res = v2_intersection(
                    v2_sum(ship_points[i], s_p),
                    v2_sum(ship_points[next_ship], s_p),
                    v2_sum(state.large_asteroids[j].points[k], a_p),
                    v2_sum(state.large_asteroids[j].points[next_large], a_p));

                if (isnan(res.x) || isnan(res.y)) {
                    continue;
                }
                printf("COLLISION AT: %f, %f \n",  res.x, res.y);
                highlight_collision(res);
            }
        }
    }
}

void highlight_collision(const v2 v) {
    SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255);

    const int radius = 4;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderPoint(state.renderer, v.x + x, v.y + y);
            }
        }
    }

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void generate_small_asteroid() {

    if (state.small_asteroid_count >= MAX_ASTEROIDS)
        return;

    const small_asteroid small = {
        .position = {randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
        .velocity = {randf(-2.0f, 2.0f), randf(-2.0f, 2.0f)},
        .angle = 0,
        .points = {
            {-4, 0},
            {-5, 2},
            {-3, 4},
            {-1, 3},
            {2, 5},
            {5, 1},
            {4, -4},
            {-1, -5},
            {-5, -3}
        }
    };
    state.small_asteroids[state.small_asteroid_count++] = small;
}

void generate_medium_asteroid() {

    if (state.medium_asteroid_count >= MAX_ASTEROIDS)
        return;

    const medium_asteroid medium = {
        .position = {randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
        .velocity = {randf(-1.5f, 1.5f), randf(-1.5f, 1.5f)},
        .angle = 0,
        .points = {
            {-2, -1},
            {-7, -7},
            {-8, -2},
            {-5, -1},
            {-8, 3},
            {-3, 8},
            {3, 8},
            {8, 5},
            {8, -3},
            {1, -9},
            {-2, -9}
        }
    };
    state.medium_asteroids[state.medium_asteroid_count++] = medium;
}

void generate_large_asteroid() {

    if (state.large_asteroid_count >= MAX_ASTEROIDS)
        return;

    const large_asteroid large = {
        .position = {randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
        .velocity = {randf(-1.0f, 1.0f), randf(-1.0f, 1.0f)},
        .angle = 0,
        .points = {
            {-11, -1},
            {-13, 7},
            {-5, 14},
            {1, 12},
            {6, 14},
            {13, 8},
            {8, 5},
            {15, -1},
            {9, -11},
            {-3, -13},
            {-5, -17},
            {-14, -8}
        }
    };
    state.large_asteroids[state.large_asteroid_count++] = large;
}

void apply_friction(float *v, const float amount) {
    if (*v > 0) *v = fmaxf(0, *v - amount);
    else if (*v < 0) *v = fminf(0, *v + amount);
}

float wrapf(const float given, const float max) {
    if (given < 0) return max;
    if (given > max) return 0;
    return given;
}

float clampf(const float val, const float min, const float max) {
    return fmaxf(min, fminf(max, val));
}

float randf(const float min, const float max) {
    const float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}
