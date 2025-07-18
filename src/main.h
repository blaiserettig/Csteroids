#ifndef MAIN_H
#define MAIN_H

#include "util/v2.h"
#include "asteroid.h"
#include "audio.h"
#include "button.h"
#include "game_types.h"
#include "shop.h"
#include "util/array_list.h"

#define SCREEN_WIDTH 1536
#define SCREEN_HEIGHT 864

#define DEFAULT_SPEED 60
#define SCALE 1.5

extern int FIRE_STREAMS;
extern float ADDED_SPEED;
extern float LUCKY_CHANCE;
extern float CHAIN_CHANCE;
extern bool HAS_SAFE_WARP;
extern bool HAS_PIERCING;
extern bool HAS_MAGNET;
extern int RADAR_STACKS;
extern float HYPERSPACE_COOLDOWN;
extern float FIRE_COOLDOWN;
extern bool HAS_SALVAGE_RIGHTS;
extern float PROJ_SPEED;
extern int PROX_STACK;

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
    v2 dir;
    float uptime;
    float ttl;
    struct {
        v2 dir;
        float speed;
    } particles[20];
} prox_explosion;

typedef struct {
    v2 pos;
    v2 vel;
    float ttl;
    float cooldown;
    bool has_homing;
    float homing_accuracy;
    v2 target_pos;
    bool has_target;
    bool from_ship;
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
    v2 pos;
    float ttl;
} s_coin;

typedef struct {
    SDL_Time now;
    SDL_Time last;
    double dt;
    float scale;
    bool is_paused;
    float pre_pause_scale;
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
    SDL_Texture *intermediate_texture;
    bool quit;
    bool dead;
    bool spawn;
    bool s_saucer;
    bool render_s_saucer;
    bool b_saucer;
    bool render_b_saucer;
    bool render_stage_text;
    bool should_spawn_next_stage;
    bool pause_state_change;
    bool shop_state_change;
    bool enter_shop;
    bool reset;
    ship ship;
    ArrayList *asteroids;
    ArrayList *projectiles;
    ArrayList *asteroid_particles;
    ArrayList *a_coins;
    ArrayList *prox_explosions;
    button_system button_system;
    death_line ship_death_lines[5];
    death_line saucer_death_lines[12];
    hyperspace_line hyperspace_lines[100];
    big_saucer big_saucer;
    small_saucer small_saucer;
    shop shop;
    int coins;
    float saucer_spawn_time;
    int lives;
    int score;
    int stage;
    int prev_ast;
    int w;
    int a;
    int d;

    enum state state;
    enum state previous_state;

    float player_static_timer;
    float player_invincible_timer;
    bool draw_lucky;
    bool draw_lucky_timer;
    char luck_text[32];

    float hyperspace_cooldown;
    float fire_cooldown;

    int tutorial_page_idx;
    int max_tutorial_pages;
} game_state;

extern game_state state;

extern g_time global_time;

void apply_screen_effects(SDL_Texture *source_texture, SDL_Renderer *renderer);

void render_ship();

void handle_input();

void update();

void update_time();

void pause_game(void);

void unpause_game(void);

void start_game(void);

void end_game(void);

void update_saucer_spawn(void);

int init_audio(void);

int load_all_audio(void);

void reset_state(void);

void update_audio_streams(void);

void update_prox_explosions(void);

void render_prox_explosions(void);

void set_instructions_inactive(void);

void set_instructions_active(void);

void render_how_to_play(void);

void decrement_instructions(void);

void increment_instructions(void);

Uint32 begin_new_stage(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 reset_level(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 stop_saucer_exp_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

Uint32 stop_luck_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval);

char *get_asset_path(const char *filename);

void reset_game(void);

void update_asteroid_destruction_timers();

void start_game_over(void);

void render_static(float center_x, float center_y, float scale, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float speed);

void start_timer(float seconds);

void update_ship(void);

void update_saucer(void);

void handle_coins_world(void);


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

void render_lives(v2 offset);

void render_score(v2 pos, float scale);

void render_coins_ui(v2 pos);

void render_saucer(v2 pos, float scale);

void update_background_lines(void);

void render_background_lines(void);

void init_hyperspace(void);

void render_spacecraft_explosion(bool saucer, bool small);

v2 *render_angle_helper(const v2 *points, int n, float angle);

void generate_small_asteroid(void);

void generate_medium_asteroid(void);

void generate_large_asteroid(void);

void add_ship_death_lines(float scale);

void add_saucer_death_lines(bool small, float scale);

void add_particles(const v2 pos, const int n, float r, float g, float b);

void add_projectile(v2 pos, bool from_ship, bool from_small_saucer);

void destroy_all_asteroids(void);

void destroy_all_timers(void);

void cleanup(void);

#endif
