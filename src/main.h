#ifndef MAIN_H
#define MAIN_H

#include "util/v2.h"
#include "asteroid.h"
#include "audio.h"
#include "util/array_list.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

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
    float r;
    float g;
    float b;
} death_line;

typedef struct {
    v2 pos;
    v2 vel;
    float ttl;
} projectile;

typedef struct {
    float x, y, z;
    float target_x, target_y;
} hyperspace_line;

typedef struct {
    v2 pos;
    v2 vel;
    float shoot_timer;
    float direction_timer;
} big_saucer, small_saucer;

typedef struct {
    SDL_Time now;
    SDL_Time last;
    double dt;
    float scale;
} g_time;

typedef struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID audio_device;
    SDL_AudioStream *fire_stream;
    SDL_AudioStream *asteroid_stream;
    SDL_AudioStream *ship_fire_stream;
    SDL_AudioStream *ship_booster_stream;
    SDL_AudioStream *saucer_stream;
    SDL_AudioStream *music_stream;
    SDL_AudioStream *sfx_stream;
    SDL_Texture* intermediate_texture;
    bool quit;
    bool dead;
    bool spawn;
    bool s_saucer;
    bool render_s_saucer;
    bool b_saucer;
    bool render_b_saucer;
    bool render_stage_text;
    bool should_spawn_next_stage;
    ship ship;
    ArrayList *asteroids;
    ArrayList *projectiles;
    ArrayList *asteroid_particles;
    ArrayList *buttons;
    death_line ship_death_lines[5];
    death_line saucer_death_lines[12];
    hyperspace_line hyperspace_lines[100];
    big_saucer big_saucer;
    small_saucer small_saucer;
    float saucer_spawn_time;
    int lives;
    int score;
    int stage;
    int prev_ast;
    int w;
    int a;
    int d;

    enum {
        START_MENU,
        GAME_VIEW,
        OVER_MENU,
        SHOP_MENU,
    } state;

    float player_static_timer;
    float player_invincible_timer;
    bool draw_lucky;
    bool draw_lucky_timer;
    char luck_text[32];
} game_state;

extern game_state state;

extern g_time global_time;

void apply_screen_effects(SDL_Texture* source_texture, SDL_Renderer* renderer);

void render_ship();

void handle_input();

void update();

void update_time();

void update_saucer_spawn(void);

int init_audio(void);

int load_all_audio(void);

void reset_state(void);

void update_audio_streams(void);

Uint32 begin_new_stage(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 reset_level(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 stop_saucer_exp_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 stop_luck_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

char* get_asset_path(const char* filename);

void reset_game(void);

void update_asteroid_destruction_timers();

void start_game_over(void);

void render_static(float center_x, float center_y, float scale, Uint8 r, Uint8 g, Uint8 b);

void start_timer(float seconds);

void update_ship(void);

void update_saucer(void);

void init_saucer(void);

void render_stage_text(void);

Uint32 stop_stage_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

void update_asteroid_explosion_particles();

void update_projectiles(void);

int ship_collision_check(void);

void projectile_collision_check(void);

void highlight_collision(v2 v);

void apply_friction(float *v, float amount);

void render_booster(void);

void render_projectiles(void);

void on_ship_hit(void);

void render_lives(void);

void render_score(void);

void render_saucer(v2 pos, float scale);

void update_hyperspace(void);

void render_hyperspace(void);

void init_hyperspace(void);

void render_spacecraft_explosion(bool saucer, bool small);

v2 *render_angle_helper(const v2 *points, int n, float angle);

void generate_small_asteroid(void);

void generate_medium_asteroid(void);

void generate_large_asteroid(void);

void add_ship_death_lines(float scale);

void add_saucer_death_lines(bool small, float scale);

void add_particles(const v2 pos, const int n, float r,  float g, float b);

void add_projectile(v2 pos, bool from_ship, bool from_small_saucer);

void destroy_all_asteroids(void);

void destroy_all_timers(void);

void cleanup(void);

#endif
