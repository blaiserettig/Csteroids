#ifndef ASTEROID_H
#define ASTEROID_H

#include <stdbool.h>
#include <stdint.h>

#include "SDL3/SDL_stdinc.h"
#include "util/v2.h"

typedef enum {
    SMALL,
    MEDIUM,
    LARGE
} asteroid_size;

typedef enum {
    STD,
    DBLXP,
    CHAIN,
    ARMOR,
    PHASER,
    SPLIT,
    VAMPIRE,
    LUCKY,
    STATIC
} asteroid_type;

typedef struct {
    v2 position;
    v2 velocity;
    v2 *points;
    int point_count;
    float angle;
    float scale;
    uint8_t color;
    Uint8 r, g, b; // ignore this right after the line above lmao
    asteroid_size size;
    asteroid_type type;
    int armor_hits;
    bool is_phased;
    float phase_timer;
    float phase_cooldown;
    Uint32 destruction_time;  // 0 = not scheduled for destruction, >0 = SDL_GetTicks() when to destroy
    bool marked_for_chain_destruction;
} asteroid;

float get_asteroid_scale(asteroid_size size);

float get_asteroid_velocity_scale(asteroid_size size);

float get_asteroid_check_distance(asteroid_size size);

void render_asteroids(void);

void update_asteroids(void);

void render_asteroid_explosion_particles(void);

void on_asteroid_hit(const asteroid *a, int i);

void add_new_asteroid(asteroid_size size, v2 pos);

void handle_lucky_bonus(void);

void trigger_chain_reaction(v2 explosion_pos, float radius);

asteroid_type get_random_asteroid_type(void);

void add_new_asteroid_typed(asteroid_size size, v2 pos, asteroid_type type);

void add_new_asteroid(asteroid_size size, v2 pos);

#endif //ASTEROID_H
