#ifndef MAIN_H
#define MAIN_H

#include "util/v2.h"
#include "asteroid.h"

void render_ship();

void handle_input();

void update();

void update_time();

Uint32 begin_new_stage(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 reset_level(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 stop_saucer_exp_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

void reset_game(void);

void start_game_over(void);

void start_timer(float seconds);

void update_ship(void);

void update_saucer(void);

void init_saucer(void);

void render_stage_text(void);

Uint32 stop_stage_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

void update_asteroid_explosion_particles();

void update_asteroids(void);

void update_projectiles(void);

int ship_collision_check(void);

void projectile_collision_check(void);

void highlight_collision(v2 v);

void apply_friction(float *v, float amount);

void render_asteroids(void);

void render_asteroids_helper(v2 pos, const v2 *points, int p_count);

void render_booster(void);

void render_projectiles(void);

void on_ship_hit(void);

void render_lives(void);

void render_score(void);

void render_saucer(v2 pos, float scale);

void update_hyperspace(void);

void render_hyperspace(void);

void init_hyperspace(void);

void render_asteroid_explosion_particles();

void render_spacecraft_explosion(bool saucer, bool small);

v2 *render_angle_helper(const v2 *points, int n, float angle);

void generate_small_asteroid(void);

void generate_medium_asteroid(void);

void generate_large_asteroid(void);

void on_asteroid_hit(const asteroid *a, int i);

void add_new_asteroid(AsteroidSize size, v2 pos);

void add_ship_death_lines(float scale);

void add_saucer_death_lines(bool small, float scale);

void add_particles(v2 pos, int n);

void add_projectile(v2 pos, bool from_ship, bool from_small_saucer);

void destroy_all_asteroids(void);

void destroy_all_timers(void);

void cleanup(void);

#endif
