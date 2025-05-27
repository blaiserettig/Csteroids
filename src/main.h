#ifndef MAIN_H
#define MAIN_H

#include "util/v2.h"
#include "asteroid.h"

typedef struct {
    v2 position;
    v2 velocity;
    float angle;
    v2 points[9];
} small_asteroid;

typedef struct {
    v2 position;
    v2 velocity;
    float angle;
    v2 points[11];
} medium_asteroid;

typedef struct {
    v2 position;
    v2 velocity;
    float angle;
    v2 points[12];
} large_asteroid;

void render_ship();
void handle_input();
void update();
void update_time();
void reset_level();
void start_timer(float seconds);
void update_ship();
void update_asteroids();
int ship_collision_check();
void highlight_collision(v2 v);
void apply_friction(float *v, float amount);
void render_asteroids();
void render_asteroids_helper(v2 pos, const v2* points, int p_count);
void render_booster();
v2* render_angle_helper(const v2 *points, int n);
void generate_small_asteroid();
void generate_medium_asteroid();
void generate_large_asteroid();
void add_new_asteroid(AsteroidSize size);
void destroy_all_asteroids();
int cleanup();

#endif
