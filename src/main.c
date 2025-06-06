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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

typedef struct {
    v2 pos;
    v2 vel;
    v2 p1;
    v2 p2;
    float ttl;
} death_line;

typedef struct {
    v2 pos;
    v2 vel;
} projectile;

struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    bool quit;
    bool dead;
    ship ship;
    ArrayList *asteroids;
    ArrayList *projectiles;
    ArrayList *asteroid_particles;
    death_line death_lines[5];
    bool timer_active;
    bool lock;
    SDL_Time timer_end_time;
    int lives;
    int score;
    int prev_ast;
    int w;
    int a;
    int d;
} state;

struct {
    SDL_Time now;
    SDL_Time last;
    double dt;
    float scale;
} global_time;

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
    state.lock = false;
    state.w = 0;
    state.a = 0;
    state.d = 0;
    state.lives = 3;
    state.score = 0;

    state.asteroids = array_list_create(sizeof(asteroid));
    state.projectiles = array_list_create(sizeof(projectile));
    state.asteroid_particles = array_list_create(sizeof(death_line));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        cleanup();
        return 1;
    }

    state.window = SDL_CreateWindow("Csteroids", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!state.window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        cleanup();
        return 1;
    }

    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        cleanup();
        return 1;
    }

    // Start
    global_time.dt = 0.0f;
    SDL_GetCurrentTime(&global_time.now);
    SDL_GetCurrentTime(&global_time.last);
    state.prev_ast = 2;
    for (int i = 0; i < state.prev_ast; i++) {
        add_new_asteroid(LARGE, (v2) {NAN, NAN});
    }

    while (!state.quit) {
        SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(state.renderer);
        SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        update();
        SDL_RenderPresent(state.renderer);
        SDL_Delay(16);
    }

    cleanup();
    return 0;
}

void update() {
    update_time();

    handle_input();

    if (!state.dead) update_ship();
    update_asteroids();
    update_projectiles();
    update_asteroid_explosion_particles();

    if (ship_collision_check() > 0) {
        state.dead = true;
        add_death_lines(25.0f);
        add_particles(state.ship.position, randi(30, 40));
        start_timer(3.0f);
    }

    projectile_collision_check();

    if (array_list_size(state.asteroids) < 1) on_level_complete();

    if (!state.dead) render_ship();
    render_asteroids();
    render_projectiles();
    render_asteroid_explosion_particles();
    render_lives();

    if (state.dead) draw_ship_explosion();
}

void update_time() {
    SDL_GetCurrentTime(&global_time.now);
    global_time.dt = ((double)global_time.now - (double)global_time.last) / 1e9;
    global_time.last = global_time.now;

    if (state.timer_active) {
        SDL_Time current_time;
        SDL_GetCurrentTime(&current_time);

        if (current_time >= state.timer_end_time) {
            state.timer_active = false;
            reset_level();
        } else {
            const float remaining = (float)(state.timer_end_time - current_time) / 1e9f;
            printf("TIMER: %f\n", remaining);
        }
    }
}

void on_level_complete() {
    const int n = state.score < randi(40000, 60000) ? ++state.prev_ast : state.prev_ast;
    for (int i = 0; i < n; i++) {
        add_new_asteroid(LARGE, (v2) {NAN, NAN});
    }
}

void reset_level() {
    if (--state.lives < 1) {
        start_game_over();
    } else {
        state.ship.velocity = (v2) {0, 0};
        state.ship.position = (v2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        state.ship.angle = 0;
        state.dead = false;
    }
}

void start_game_over() {
    state.lock = true;
}

void start_timer(const float seconds) {
    SDL_GetCurrentTime(&state.timer_end_time);
    state.timer_end_time += (SDL_Time)(seconds * 1e9); // Convert seconds to nanoseconds
    state.timer_active = true;
}

void update_ship() {
    state.ship.position.x = wrap0f(state.ship.position.x + state.ship.velocity.x, SCREEN_WIDTH);
    state.ship.position.y = wrap0f(state.ship.position.y + state.ship.velocity.y, SCREEN_HEIGHT);

    const float radians = state.ship.angle * ((float)M_PI / 180.0f);
    if (state.w && !state.lock) {
        state.ship.velocity.x -= sinf(radians) * (float)THRUST;
        state.ship.velocity.y += cosf(radians) * (float)THRUST;
        render_booster();
    }
    if (state.a && !state.lock) {
        state.ship.angle -= 5;
    }
    if (state.d && !state.lock) {
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
        a->position.x = wrap0f(a->position.x += a->velocity.x, SCREEN_WIDTH);
        a->position.y = wrap0f(a->position.y += a->velocity.y, SCREEN_HEIGHT);
    }
}

void update_projectiles() {
    for (size_t i = 0; i < array_list_size(state.projectiles); i++ ) {
        projectile *p = array_list_get(state.projectiles, i);
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;
        if (p->pos.x < 0 || p->pos.x > SCREEN_WIDTH ||  p->pos.y < 0 || p->pos.y > SCREEN_HEIGHT) array_list_remove(state.projectiles, i);
    }
}

void update_asteroid_explosion_particles() {
    for (size_t i = 0; i < array_list_size(state.asteroid_particles); i++ ) {
        death_line *d = array_list_get(state.asteroid_particles, i);
        d->p1 = v2_sum(d->p1, d->vel);
        d->p2 = v2_sum(d->p2, d->vel);
        d->ttl -= (float)global_time.dt;
        if (d->ttl < 0) array_list_remove(state.asteroid_particles, i);
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
                    case SDL_SCANCODE_SPACE:
                        const SDL_KeyboardEvent e = event.key;
                        if (!e.repeat) add_projectile();
                        break;
                    case SDL_SCANCODE_F:
                        add_new_asteroid(LARGE, (v2) {NAN, NAN});
                        add_new_asteroid(MEDIUM, (v2) {NAN, NAN});
                        add_new_asteroid(SMALL, (v2) {NAN, NAN});
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
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        // do nothing (for now)
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void draw_ship_explosion() {
    const v2 pos = state.ship.position;
    for (int i = 0; i < 5; i++) {
        death_line d = state.death_lines[i];
        if (d.ttl > 0.0f) SDL_RenderLine(state.renderer, pos.x + d.p1.x, pos.y + d.p1.y, pos.x + d.p2.x, pos.y + d.p2.y);
        d.p1 = v2_sum(d.p1, d.vel);
        d.p2 = v2_sum(d.p2, d.vel);
        d.ttl -= (float)global_time.dt;
        state.death_lines[i] = d;
    }
}

void render_asteroid_explosion_particles() {
    SDL_SetRenderDrawColor(state.renderer, 128, 128, 128, 255);
    for (size_t i = 0; i < array_list_size(state.asteroid_particles); i++ ) {
        const death_line *d = array_list_get(state.asteroid_particles, i);
        SDL_RenderLine(state.renderer, d->pos.x + d->p1.x, d->pos.y + d->p1.y, d->pos.x + d->p2.x,  d->pos.y + d->p2.y);
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void render_ship() {
    v2* new_points = render_angle_helper(ship_points, 6, state.ship.angle);

    const v2 pos = state.ship.position;
    for (int i = 0; i < 5; i++) {
        SDL_RenderLine(state.renderer, pos.x + new_points[i].x, pos.y + new_points[i].y,
            pos.x + new_points[i + 1].x, pos.y + new_points[i + 1].y);
    }

    free(new_points);
}

void render_lives() {
    v2 offset = (v2) {20.0f, 50.0f};
    v2 *new_points = render_angle_helper(ship_points, 6, 180.0f);
    for (int j = 0; j < state.lives; j++) {
        for (int i = 0; i <  5; i++) {
            SDL_RenderLine(state.renderer, offset.x + new_points[i].x, offset.y + new_points[i].y, offset.x + new_points[i + 1].x, offset.y + new_points[i + 1].y);
        }
        offset.x += 12.0f;
    }
    free(new_points);
}

void render_asteroids() {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        const asteroid *a  = array_list_get(state.asteroids, i);
        SDL_SetRenderDrawColor(state.renderer, a->color, a->color, a->color, 255);
        for (int j = 0; j < a->point_count; j++) {
            const int next = (j + 1) % a->point_count;
            SDL_RenderLine(state.renderer,
                a->position.x + a->points[j].x,  a->position.y + a->points[j].y,
                a->position.x + a->points[next].x, a->position.y + a->points[next].y);
        }
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void render_booster() {
    const v2 booster_points[] = {
        {-2, -7},
        {0, -13},
        {2, -7},
    };

    v2* new_points = render_angle_helper(booster_points, 3, state.ship.angle);

    const v2 pos = state.ship.position;

    SDL_RenderLine(state.renderer, pos.x + new_points[0].x, pos.y + new_points[0].y,
        pos.x + new_points[1].x, pos.y + new_points[1].y);

    SDL_RenderLine(state.renderer, pos.x + new_points[1].x, pos.y + new_points[1].y,
        pos.x + new_points[2].x, pos.y + new_points[2].y);

    free(new_points);
}

void render_projectiles() {

    for (int i = 0; i < array_list_size(state.projectiles); i++) {
        const projectile *p  = array_list_get(state.projectiles, i);
        const int radius = 1;
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    SDL_RenderPoint(state.renderer, p->pos.x + (float)x, p->pos.y + (float)y);
                }
            }
        }
    }
}

v2* render_angle_helper(const v2 *points, const int n, const float angle) {
    v2* new_points = malloc(n * sizeof(v2));
    for (int i = 0; i < n; i++) {
        new_points[i] = points[i];

        new_points[i].x = points[i].x * cosf(angle *
            ((float)M_PI / 180.f)) - points[i].y * sinf(angle * ((float)M_PI / 180.f));

        new_points[i].y = points[i].x * sinf(angle *
            ((float)M_PI / 180.f)) + points[i].y * cosf(angle * ((float)M_PI / 180.0f));
    }
    return new_points;
}

void projectile_collision_check() {
    for (int i = 0; i < array_list_size(state.projectiles); i++) {
        const projectile *p = array_list_get(state.projectiles, i);
        for (int j = 0; j < array_list_size(state.asteroids); j++) {
            const asteroid *a  = array_list_get(state.asteroids, j);

            const float check_distance = get_asteroid_check_distance(a->size);

            if (v2_dist_sqr(p->pos, a->position) < check_distance) {
                highlight_collision(p->pos);
                array_list_remove(state.projectiles, i);
                on_asteroid_hit(a, j);
            }
        }
    }
}

int ship_collision_check() {

    if (state.dead) return 0;

    v2 *points = render_angle_helper(ship_points, 6, state.ship.angle);

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
                on_asteroid_hit(a, j);
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

void on_asteroid_hit(const asteroid *a, const int i) {
    add_particles(a->position, randi(15, 20));
    switch (a->size) {
        case SMALL:
            array_list_remove(state.asteroids, i);
            break;
        case MEDIUM:
            add_new_asteroid(SMALL,  a->position);
            add_new_asteroid(SMALL,  a->position);
            array_list_remove(state.asteroids, i);
            break;
        case LARGE:
            add_new_asteroid(MEDIUM,  a->position);
            add_new_asteroid(MEDIUM,  a->position);
            array_list_remove(state.asteroids, i);
            break;
    }
}

void add_new_asteroid(const AsteroidSize size, v2 pos) {
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
        .position = !isnan(pos.x) ? pos : (v2){randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
        .velocity = v2_scale((v2) {cosf(angle), sinf(angle)}, get_asteroid_velocity_scale(size)),
        .angle = 0,
        .scale = get_asteroid_scale(size),
        .size = size,
        .points = points,
        .point_count = n,
        .color = randi(128, 255)});
}

void add_death_lines(const float scale) {
    for (int i = 0; i < 5; i++) {

        const float angle = randf(0.01f, 1.0f) * (float)M_TAU;

        const death_line d = {
        .pos = state.ship.position,
        .vel = (v2) {-sinf(angle) * ((0.2f + state.ship.velocity.y) / 3.0f), cosf(angle) * ((0.2f + state.ship.velocity.x) / 3.0f)},
        .p1 = (v2) {randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
        .p2 = (v2) {randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
        .ttl = randf(1.8f, 3.2f)};

        state.death_lines[i] = d;
    }
}

void add_particles(const v2 pos, const int n) {
    for (int i = 0; i < n; i++) {
        const float scale = 25.0f;
        const float angle = randf(0.01f, 1.0f) * (float)M_TAU;
        array_list_add(state.asteroid_particles, &(death_line) {
        .pos = {pos.x + randf(-20.0f, 20.0f), pos.y + randf(-20.0f, 20.0f)},
        .vel = (v2) {-sinf(angle), cosf(angle)},
        .p1 = (v2) {randf(0.0f, 0.05f) * scale, randf(0.0f, 0.05f) * scale},
        .p2 = (v2) {randf(0.0f, 0.05f) * scale, randf(0.0f, 0.05f) * scale},
        .ttl = randf(0.6f, 1.0f)});
    }
}

void add_projectile() {
    if (state.lock) return;
    array_list_add(state.projectiles, &(projectile) {
    .pos = state.ship.position,
    .vel = v2_scale((v2) {-sinf((state.ship.angle * (float)M_PI) / 180.0f), cosf((state.ship.angle * (float)M_PI) / 180.0f)}, 4.0f)});
    state.ship.velocity.x += sinf((state.ship.angle * (float)M_PI) / 180.0f);     // apply kickback in the opposite direction
    state.ship.velocity.y -= cosf((state.ship.angle * (float)M_PI) / 180.0f);
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

void cleanup() {
    destroy_all_asteroids();
    array_list_free(state.asteroids);
    array_list_free(state.projectiles);
    array_list_free(state.asteroid_particles);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}