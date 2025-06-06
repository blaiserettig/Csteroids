#ifndef MAIN_H
#define MAIN_H

#include "util/v2.h"
#include "asteroid.h"

void render_ship();
void handle_input();
void update();
void update_time();
void reset_level();
void start_timer(float seconds);
void update_ship();
void update_asteroid_explosion_particles();
void update_asteroids();
void update_projectiles();
int ship_collision_check();
void projectile_collision_check();
void highlight_collision(v2 v);
void apply_friction(float *v, float amount);
void render_asteroids();
void render_asteroids_helper(v2 pos, const v2* points, int p_count);
void render_booster();
void render_projectiles();
void render_asteroid_explosion_particles();
void draw_ship_explosion();
v2* render_angle_helper(const v2 *points, int n);
void generate_small_asteroid();
void generate_medium_asteroid();
void generate_large_asteroid();
void on_asteroid_hit(const asteroid *a, int i);
void add_new_asteroid(AsteroidSize size, v2 pos);
void add_death_lines(float scale);
void add_particles(v2 pos, int n);
void add_projectile();
void destroy_all_asteroids();
void cleanup();

#endif
