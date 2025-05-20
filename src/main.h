#ifndef MAIN_H
#define MAIN_H

typedef struct {
    float x;
    float y;
} v2;

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

void update_ship();

void update_asteroids();

float wrapf(float given, float max);

void apply_friction(float *v, float amount);

float clampf(float val, float min, float max);

void render_asteroids();

void render_asteroids_helper(v2 pos, const v2* points, int p_count);

void render_booster();

v2* render_angle_helper(const v2 *points, int n);

void generate_small_asteroid();

void generate_medium_asteroid();

void generate_large_asteroid();

float randf(float min, float max);

#endif
