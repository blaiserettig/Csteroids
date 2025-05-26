//gcc -I./include src/main.c -o main.exe -L./lib -lSDL3 

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#include "main.h"
#include "util/math_ext.h"
#include "util/array_list.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util/time_ext.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define THRUST 0.3
#define M_TAU (M_PI * 2)

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
    global_time *global_time;
    bool quit;
    bool dead;
    ship ship;
    ArrayList *asteroids;
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

    srand((unsigned int)time(NULL));

    state.ship.position.x = SCREEN_WIDTH / 2.0;
    state.ship.position.y = SCREEN_HEIGHT / 2.0;
    state.ship.velocity.x = 0;
    state.ship.velocity.y = 0;
    state.ship.angle = 0;
    state.dead = false;
    state.w = 0;
    state.a = 0;
    state.d = 0;

    state.asteroids = array_list_create(sizeof(asteroid));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        cleanup();
    }

    state.window = SDL_CreateWindow("Csteroids", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!state.window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        cleanup();
    }

    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        cleanup();
    }

    // Start
    state.global_time = malloc(sizeof(global_time));
    init_time(state.global_time);

    while (!state.quit) {
        SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(state.renderer);
        SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        update();
        SDL_RenderPresent(state.renderer);
        SDL_Delay(16);
    }

    destroy_all_asteroids();
    array_list_free(state.asteroids);

    SDL_DestroyWindow(state.window);
    SDL_Quit();

    return 0;
}

void update() {
    update_time(state.global_time);

    handle_input();

    if (!state.dead) update_ship();
    update_asteroids();

    if (ship_collision_check() > 0) {

    }

    if (!state.dead) render_ship();
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
    for (size_t i = 0; i < array_list_size(state.asteroids); i++ ) {
        asteroid *a = array_list_get(state.asteroids, i);
        a->position.x = wrapf(a->position.x += a->velocity.x, SCREEN_WIDTH);
        a->position.y = wrapf(a->position.y += a->velocity.y, SCREEN_HEIGHT);
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
                        add_new_asteroid(LARGE);
                        add_new_asteroid(MEDIUM);
                        add_new_asteroid(SMALL);
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
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        const asteroid *a  = array_list_get(state.asteroids, i);
        for (int j = 0; j < a->point_count; j++) {
            const int next = (j + 1) % a->point_count;
            SDL_RenderLine(state.renderer,
                a->position.x + a->points[j].x,  a->position.y + a->points[j].y,
                a->position.x + a->points[next].x, a->position.y + a->points[next].y);
        }
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

int ship_collision_check() {

    v2 *points = render_angle_helper(ship_points, 6);

    for (int i = 0; i < 6; i++) {
        const int next_ship = (i + 1) % 6;
        for (int j = 0; j < array_list_size(state.asteroids); j++) {
            const asteroid *a  = array_list_get(state.asteroids, j);

            if (v2_dist_sqr(state.ship.position, a->position) > 20000.0f) continue;

            for (int k = 0; k < a->point_count; k++) {
                const int next_point = (k + 1) % a->point_count;
                const v2 s_p = state.ship.position;
                const v2 a_p = a->position;

                v2 res = v2_intersection(
                    v2_sum(points[i], s_p),
                    v2_sum(points[next_ship], s_p),
                    v2_sum(a->points[k], a_p),
                    v2_sum(a->points[next_point], a_p));

                if (isnan(res.x) || isnan(res.y)) continue;

                highlight_collision(res);
                free(points);
                return 1;
            }
        }
    }
    free(points);
    return -1;
}

void highlight_collision(const v2 v) {
    SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255);

    const int radius = 4;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderPoint(state.renderer, v.x + (float)x, v.y + (float)y);
            }
        }
    }

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void add_new_asteroid(const AsteroidSize size) {
    const int n = randi(12, 18);
    v2 *points = malloc((size_t)n * sizeof(v2));

    for (int i = 0; i < n; i++) {
        float radius = 0.3f + 0.2f * randf(0.0f, 1.0f);
        if (randf(0.0f, 1.0f) < 0.2f) radius -= 0.2f;
        const float angle =  (float)i * ((float)M_TAU / (float)n) + (float)M_PI * 0.125f * randf(0.0f, 1.0f);

        points[i] = v2_scale(v2_scale((v2) {cosf(angle), sinf(angle)}, radius), get_asteroid_scale(size));
    }

    const float angle = M_TAU * randf(0.0f, 1.0f);

    array_list_add(state.asteroids, &(asteroid) {
        .position = {randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
        .velocity = v2_scale((v2) {cosf(angle), sinf(angle)}, get_asteroid_velocity_scale(size)),
        .angle = 0,
        .scale = get_asteroid_scale(size),
        .size = size,
        .points = points,
        .point_count = n,});
}

void destroy_all_asteroids() {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        const asteroid *a = array_list_get(state.asteroids, i);
        free(a->points);
    }
}

void apply_friction(float *v, const float amount) {
    if (*v > 0) *v = fmaxf(0, *v - amount);
    else if (*v < 0) *v = fminf(0, *v + amount);
}

int cleanup() {
    destroy_all_asteroids();
    array_list_free(state.asteroids);
    free(state.global_time);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    return 1;
}