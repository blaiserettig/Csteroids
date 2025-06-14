//gcc -I./include src/main.c -o main.exe -L./lib -lSDL3 

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_timer.h"

#include "main.h"
#include "util/math_ext.h"
#include "util/array_list.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "button.h"
#include "text.h"
#include "audio.h"

#define THRUST 0.3
#define M_TAU (M_PI * 2)
#define MAX_SFX_STREAMS 8

game_state state = {0};

g_time global_time = {0};

v2 ship_points[] = {
    {-5, -10},
    {0, 10},
    {5, -10},
    {2, -7},
    {-2, -7},
    {-5, -10}
};

v2 saucer_points[] = {
    {-12, 0},
    {-8, 3},
    {8, 3},
    {12, 0},
    {8, -3},
    {-8, -3},
    {-12, 0},
    {12, 0},
    {6, -3},
    {4, -6},
    {-4, -6},
    {-6, -3}
};

// Robert Jenkins' 96 bit Mix Function
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void main_loop(void) {
    SDL_SetRenderTarget(state.renderer, state.intermediate_texture);
    SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(state.renderer);
    SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    update();
    SDL_SetRenderTarget(state.renderer, NULL);
    apply_screen_effects(state.intermediate_texture, state.renderer);
    SDL_RenderPresent(state.renderer);
    #ifndef __EMSCRIPTEN__
    SDL_Delay(16);
    #endif
}

void init_buttons(void) {
    button_system_init(&state.button_system);
    button_system_add_default(&state.button_system,
                              (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f - 32, 128, 64}, "START",
                              start_game, START_MENU);
    button_system_add_default(&state.button_system,
                              (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f - 32, 128, 64}, "RESTART",
                              reset_game, OVER_MENU);
    button_system_add_default(&state.button_system,
                          (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f - 32, 128, 64}, "RESUME",
                          unpause_game, PAUSE_MENU);
    button_system_add_default(&state.button_system,
                              (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f + 48.0f, 128, 64}, "QUIT",
                              end_game, START_MENU);
    button_system_add_default(&state.button_system,
                          (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f + 48.0f, 128, 64}, "QUIT",
                          end_game, OVER_MENU);
    button_system_add_default(&state.button_system,
                          (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f + 48.0f, 128, 64}, "QUIT",
                          end_game, PAUSE_MENU);
    button_system_add_default(&state.button_system,
                      (SDL_FRect){SCREEN_WIDTH / 2.0f - 64, SCREEN_HEIGHT / 2.0f + 400.0f, 128, 64}, "DONE",
                      exit_shop, SHOP_MENU);

}

int main(int argc, char *argv[]) {

    const unsigned long seed = mix(time(NULL), SDL_GetTicks(), clock());
    srand(seed);

    reset_state();
    state.state = START_MENU;

    state.asteroids = array_list_create(sizeof(asteroid));
    state.projectiles = array_list_create(sizeof(projectile));
    state.asteroid_particles = array_list_create(sizeof(death_line));

    init_hyperspace();
    init_buttons();
    init_shop();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

    state.intermediate_texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,  SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!state.intermediate_texture) {
        SDL_Log("SDL_CreateTexture Error: %s", SDL_GetError());
        cleanup();
        return 1;
    }

    // Start
    if (init_audio() != 0) {
        cleanup();
        return 1;
    }
    if (load_all_audio() != 0) {
        cleanup();
        return 1;
    }

    #ifdef  __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
    #else

    while (!state.quit) {
        main_loop();
    }
    #endif

    cleanup();
    return 0;
}

void apply_screen_effects(SDL_Texture *source_texture, SDL_Renderer *renderer) {
    // render the original at full opacity
    SDL_RenderTexture(renderer, source_texture, NULL, NULL);

    // and create bloom by rendering multiple blurred copies
    SDL_SetTextureBlendMode(source_texture, SDL_BLENDMODE_ADD);

    for (int i = 0; i < 4; i++) {
        // create multiple bloom passes with increasing size and decreasing opacity
        const Uint8 bloom_alphas[] = {48, 32, 16, 8};
        const float bloom_offsets[] = {1.0f, 2.0f, 3.0f, 4.0f};

        /*if (state.state == START_MENU) {
            for (int k =0; k < 4; k++) {
                bloom_alphas[i] /= 2;
            }
        }*/

        SDL_SetTextureAlphaMod(source_texture, bloom_alphas[i]);

        const float offset = bloom_offsets[i];

        //  3x3 pattern around the center
        const SDL_FRect bloom_positions[] = {
            {-offset, -offset, SCREEN_WIDTH, SCREEN_HEIGHT},
            { 0,      -offset, SCREEN_WIDTH, SCREEN_HEIGHT},
            { offset, -offset, SCREEN_WIDTH, SCREEN_HEIGHT},
            {-offset,  0,      SCREEN_WIDTH, SCREEN_HEIGHT},
            { offset,  0,      SCREEN_WIDTH, SCREEN_HEIGHT},
            {-offset,  offset, SCREEN_WIDTH, SCREEN_HEIGHT},
            { 0,       offset, SCREEN_WIDTH, SCREEN_HEIGHT},
            { offset,  offset, SCREEN_WIDTH, SCREEN_HEIGHT}
        };

        for (int j = 0; j < 8; j++) {
            SDL_RenderTexture(renderer, source_texture, NULL, &bloom_positions[j]);
        }
    }

    SDL_SetTextureBlendMode(source_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(source_texture, 255);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // scanlines
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        SDL_RenderLine(renderer, 0, (float) y, SCREEN_WIDTH, (float) y);
    }
}

void enter_pause_menu(void) {
    SDL_Log("ENTER");
}

void render_pause_menu(void) {
    for (int i = 0; i < SCREEN_HEIGHT; i ++) {
        SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 200);
        SDL_RenderLine(state.renderer, 0, (float)i, (float) SCREEN_WIDTH, (float) i);
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
    render_text_3d_extruded(state.renderer, "PAUSED",
                            (v2){(float) SCREEN_WIDTH / 2.0f, (float) SCREEN_HEIGHT / 2.0f - 100.0f}, 35.0f);
}

void exit_pause_menu(void) {
    SDL_Log("EXIT");
}

void start_game(void) {
    global_time.dt = 0.0f;
    SDL_GetCurrentTime(&global_time.now);
    SDL_GetCurrentTime(&global_time.last);
    state.spawn = true;
    const SDL_TimerID id = SDL_AddTimer(0001, begin_new_stage, NULL);
    if (id == 0) {
        SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
    }
    state.state = GAME_VIEW;
}

void end_game(void) {
    state.quit = true;
}

void update(void) {
    button_system_show_buttons_for_state(&state.button_system, state.state);

    update_time();

    handle_input();

    update_hyperspace();
    render_hyperspace();

    if (state.state == START_MENU) {
        render_text_3d_extruded(state.renderer, "CSTEROIDS", (v2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 100.0f},
                                35.0f);
    }

    if (state.state == GAME_VIEW) {
        music_update();
        if (state.should_spawn_next_stage) {
            state.should_spawn_next_stage = false;

            const int n = state.score < randi(40000, 60000) ? ++state.prev_ast : state.prev_ast;
            for (int i = 0; i < n; i++) {
                add_new_asteroid(LARGE, (v2){NAN, NAN});
            }
        }
    }

    if (state.state == OVER_MENU) {
        render_text_3d_extruded(state.renderer, "GAME OVER",
                                (v2){(float) SCREEN_WIDTH / 2.0f, (float) SCREEN_HEIGHT / 2.0f - 100.0f}, 35.0f);
    }

    if (state.state == SHOP_MENU) {
        update_shop();
        render_shop();
    }

    button_system_render(&state.button_system, state.renderer);

    if (state.state == START_MENU || state.state == SHOP_MENU) return;

    if (!global_time.is_paused) update_saucer_spawn();

    const bool update_player = state.player_static_timer > 0.0f;
    if (update_player) {
        state.player_static_timer -= (float)global_time.dt;
        state.w = 0;
        state.a = 0;
        state.d = 0;
        render_static(state.ship.position.x, state.ship.position.y, 1.0f, 200, 200, 200);
    }

    if (state.player_invincible_timer > 0.0f) {
        state.player_invincible_timer -= (float)global_time.dt;
    }

    if (!state.dead && !update_player && !global_time.is_paused) update_ship();
    update_asteroids();
    update_asteroid_destruction_timers();
    update_projectiles();
    update_asteroid_explosion_particles();
    if (state.s_saucer || state.b_saucer) {
        update_saucer();
    }

    if (ship_collision_check() > 0 && state.player_invincible_timer < 0.05f) {
        on_ship_hit();
    }

    projectile_collision_check();

    if (array_list_size(state.asteroids) < 1 && !state.spawn && !state.s_saucer && !state.b_saucer) {
        state.spawn = true;
        state.stage++;
        const SDL_TimerID id = SDL_AddTimer(1500, begin_new_stage, NULL);
        if (id == 0) {
            SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
        }
    }

    if (state.render_stage_text) render_stage_text();

    if (state.pause_state_change) {
        if (global_time.is_paused) {
            unpause_game();
        } else {
            pause_game();
        }
        state.pause_state_change = false;
    }

    if (global_time.is_paused) {
        render_pause_menu();
    }

    if (!state.dead) render_ship();
    render_asteroids();
    render_projectiles();
    render_asteroid_explosion_particles();
    render_lives();
    render_score();
    if (state.s_saucer) render_saucer(state.small_saucer.pos, 1.25f);
    if (state.b_saucer) render_saucer(state.big_saucer.pos, 1.75f);
    if ((state.s_saucer || state.b_saucer) && state.state != OVER_MENU && !global_time.is_paused) keep_saucer_sound_playing();

    if (state.dead) render_spacecraft_explosion(false, false);
    if (state.render_s_saucer) render_spacecraft_explosion(true, true);
    if (state.render_b_saucer) render_spacecraft_explosion(true, false);

    if (state.draw_lucky) {
        render_text_thick(state.renderer, state.luck_text, (v2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 75.0f}, 25.0f, 2.0f, 25.0f);
        if (state.draw_lucky_timer) return;
        state.draw_lucky_timer = true;
        const SDL_TimerID id = SDL_AddTimer(1000, stop_luck_text_render, NULL);
        if (id == 0) {
            SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
        }
    }
}

void update_saucer_spawn(void) {
    if (!state.s_saucer && !state.b_saucer) state.saucer_spawn_time -= (float) global_time.dt;
    if (state.saucer_spawn_time < 0.0f && !state.s_saucer && !state.b_saucer && !state.dead) {
        init_saucer();
        play_saucer_sound();
        if (state.score >= 40000) {
            state.s_saucer = true;
        } else {
            const float f = randf(0.0f, 1.0f);
            if (f < 0.5f) state.s_saucer = true;
            else state.b_saucer = true;
        }
        state.saucer_spawn_time = randf(35.0f, 45.0f);
    }
}

void update_time(void) {
    SDL_GetCurrentTime(&global_time.now);
    if (global_time.is_paused) {
        global_time.dt = 0.0;
    } else {
        global_time.dt = ((double) global_time.now - (double) global_time.last) / 1e9;
        global_time.dt * (double)global_time.scale;
    }
    global_time.last = global_time.now;
}

void pause_game(void) {
    if (!global_time.is_paused) {
        global_time.pre_pause_scale = global_time.scale;
        global_time.is_paused = true;
        state.previous_state = state.state;
        state.state = PAUSE_MENU;
        enter_pause_menu();
    }
}

void unpause_game(void) {
    if (global_time.is_paused) {
        global_time.scale = global_time.pre_pause_scale;
        global_time.is_paused = false;
        state.state = state.previous_state;
        exit_pause_menu();
    }
}

void update_asteroid_destruction_timers() {
    const Uint32 current_time = SDL_GetTicks();

    for (int i = (int)array_list_size(state.asteroids) - 1; i >= 0; i--) {
        const asteroid *a = array_list_get(state.asteroids, i);

        if (a->destruction_time > 0 && current_time >= a->destruction_time) {
            on_asteroid_hit(a, i);
        }
    }
}

// ReSharper disable once CppDFAConstantFunctionResult
// Clion warns that this function always returns 0, which is both true and necessary in this case for SDL_Timers
// Weirdly it does not warn about the succeeding function which does the same thing
Uint32 begin_new_stage(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    play_sound_effect(AUDIO_STREAM_ASTEROID, audio_clips.new_stage);
    music_start();

    state.should_spawn_next_stage = true;
    state.spawn = false;
    state.render_stage_text = true;
    const SDL_TimerID id = SDL_AddTimer(3000, stop_stage_text_render, NULL);
    if (id == 0) {
        SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
    }
    return 0;
}

Uint32 reset_level(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    if (--state.lives < 1) {
        start_game_over();
    } else {
        play_sound_effect(AUDIO_STREAM_SHIP_FIRE, audio_clips.respawn);
        music_start();
        state.ship.velocity = (v2){0, 0};
        state.ship.position = (v2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        state.ship.angle = 0;
        state.dead = false;
    }
    return 0;
}

void reset_game(void) {
    state.state = GAME_VIEW;
    reset_state();
    destroy_all_asteroids();
    array_list_free(state.asteroids);
    state.asteroids = array_list_create(sizeof(asteroid));
    global_time.dt = 0.0f;
    SDL_GetCurrentTime(&global_time.now);
    SDL_GetCurrentTime(&global_time.last);

    const SDL_TimerID id = SDL_AddTimer(0001, begin_new_stage, NULL);
    if (id == 0) {
        SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
    }
}

void reset_state(void) {
    state.ship.position.x = SCREEN_WIDTH / 2.0;
    state.ship.position.y = SCREEN_HEIGHT / 2.0;
    state.ship.velocity.x = 0;
    state.ship.velocity.y = 0;
    state.ship.angle = 0;
    state.dead = false;
    state.spawn = false;
    state.s_saucer = false;
    state.render_s_saucer = false;
    state.b_saucer = false;
    state.render_b_saucer = false;
    state.render_stage_text = true;
    state.saucer_spawn_time = randf(35.0f, 55.0f);
    state.w = 0;
    state.a = 0;
    state.d = 0;
    state.stage = 1;
    state.lives = 3;
    state.score = 0;
    state.prev_ast = randi(4, 6);
    state.draw_lucky = false;
    state.draw_lucky_timer = false;
    state.pause_state_change = false;
    state.coins = 0;
}

void start_game_over(void) {
    play_sound_effect(AUDIO_STREAM_SHIP_FIRE, audio_clips.game_over);
    state.state = OVER_MENU;
}

void render_stage_text(void) {
    char stage_text[32];
    snprintf(stage_text, sizeof(stage_text), "STAGE %d", state.stage);
    render_text_3d(state.renderer, stage_text, (v2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 100.0f}, 35.0f, (SDL_Color){255, 255, 255, 255});
}

Uint32 stop_stage_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    state.render_stage_text = false;
    return 0;
}

Uint32 stop_luck_text_render(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    state.draw_lucky = false;
    state.draw_lucky_timer = false;
    return 0;
}

void on_ship_hit(void) {
    play_sound_effect(AUDIO_STREAM_SHIP_FIRE, audio_clips.explode);
    music_stop();
    state.dead = true;
    add_ship_death_lines(25.0f);
    add_particles(state.ship.position, randi(30, 40), NAN, NAN, NAN);
    const SDL_TimerID id = SDL_AddTimer(3000, reset_level, NULL);
    if (id == 0) {
        SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
    }
}

void update_saucer(void) {
    state.small_saucer.pos.x = wrap0f(state.small_saucer.pos.x + state.small_saucer.vel.x * SPEED * (float)global_time.dt, SCREEN_WIDTH);
    state.small_saucer.pos.y = wrap0f(state.small_saucer.pos.y + state.small_saucer.vel.y * SPEED * (float)global_time.dt, SCREEN_HEIGHT);
    if (state.s_saucer) {
        static float shoot_time = 0.0f;
        shoot_time += (float) global_time.dt;
        if (shoot_time >= state.small_saucer.shoot_timer) {
            add_projectile(state.small_saucer.pos, false, true);
            shoot_time = 0.0f;
        }
        static float direction_time = 0.0f;
        direction_time += (float) global_time.dt;
        if (direction_time >= state.small_saucer.direction_timer) {
            const float angle = randf(0.0f, 1.0f) * (float) M_TAU;
            state.small_saucer.vel = (v2){-sinf(angle), cosf(angle)};
            direction_time = 0.0f;
        }
    }

    state.big_saucer.pos.x = wrap0f(state.big_saucer.pos.x + state.big_saucer.vel.x * SPEED * (float)global_time.dt, SCREEN_WIDTH);
    state.big_saucer.pos.y = wrap0f(state.big_saucer.pos.y + state.big_saucer.vel.y * SPEED * (float)global_time.dt, SCREEN_HEIGHT);
    if (state.b_saucer) {
        static float time = 0.0f;
        time += (float) global_time.dt;
        if (time >= state.big_saucer.shoot_timer) {
            add_projectile(state.big_saucer.pos, false, false);
            time = 0.0f;
        }
        static float direction_time = 0.0f;
        direction_time += (float) global_time.dt;
        if (direction_time >= state.big_saucer.direction_timer) {
            const float angle = randf(0.0f, 1.0f) * (float) M_TAU;
            state.big_saucer.vel = (v2){-sinf(angle), cosf(angle)};
            direction_time = 0.0f;
        }
    }
}

void update_ship(void) {
    state.ship.position.x = wrap0f(state.ship.position.x + state.ship.velocity.x * SPEED * (float)global_time.dt, SCREEN_WIDTH);
    state.ship.position.y = wrap0f(state.ship.position.y + state.ship.velocity.y * SPEED * (float)global_time.dt, SCREEN_HEIGHT);

    const float radians = state.ship.angle * ((float) M_PI / 180.0f);
    if (state.w) {
        state.ship.velocity.x -= sinf(radians) * (float) THRUST;
        state.ship.velocity.y += cosf(radians) * (float) THRUST;
        render_booster();
        if (!SDL_GetAudioStreamAvailable(state.ship_booster_stream) > 0) {
            play_sound_effect(AUDIO_STREAM_SHIP_BOOSTER, audio_clips.booster);
        }
    }
    if (state.a) {
        state.ship.angle -= 7;
    }
    if (state.d) {
        state.ship.angle += 7;
    }

    apply_friction(&state.ship.velocity.x, 0.06f);
    apply_friction(&state.ship.velocity.y, 0.06f);

    state.ship.velocity.x = clampf(state.ship.velocity.x, -10, 10);
    state.ship.velocity.y = clampf(state.ship.velocity.y, -10, 10);
}

void update_projectiles(void) {
    for (size_t i = 0; i < array_list_size(state.projectiles); i++) {
        projectile *p = array_list_get(state.projectiles, i);
        p->pos.x = wrap0f(p->pos.x + p->vel.x * SPEED * (float)global_time.dt, SCREEN_WIDTH);
        p->pos.y = wrap0f(p->pos.y + p->vel.y * SPEED * (float)global_time.dt, SCREEN_HEIGHT);
        p->ttl -= (float) global_time.dt;
        if (p->ttl < 0) array_list_remove(state.projectiles, i);
    }
}

void update_asteroid_explosion_particles(void) {
    for (size_t i = 0; i < array_list_size(state.asteroid_particles); i++) {
        death_line *d = array_list_get(state.asteroid_particles, i);
        d->p1 = v2_sum(d->p1, v2_scale(v2_scale(d->vel, SPEED), (float)global_time.dt));
        d->p2 = v2_sum(d->p2, v2_scale(v2_scale(d->vel, SPEED), (float)global_time.dt));
        d->ttl -= (float) global_time.dt;
        if (d->ttl < 0) array_list_remove(state.asteroid_particles, i);
    }
}

void render_static(const float center_x, const float center_y, const float scale, const Uint8 r, const Uint8 g, const Uint8 b) {
    static float accumulated_time = 0.0f;
    accumulated_time += (float)global_time.dt;
    const float rotation = accumulated_time * 10.0f;
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);

    const int num_points = 12;
    const float base_radius = 25.0f * scale;

    float points_x[num_points];
    float points_y[num_points];

    for (int i = 0; i < num_points; i++) {
        const float angle = (float)i * ((float)M_TAU / (float)num_points) + rotation;

        const float jag = 8.0f * scale * sinf(rotation * 3.0f + (float)i * 1.2f);
        const float radius = base_radius + jag;

        points_x[i] = center_x + cosf(angle) * radius;
        points_y[i] = center_y + sinf(angle) * radius;
    }

    for (int i = 0; i < num_points; i++) {
        const int next = (i + 1) % num_points;
        SDL_RenderLine(state.renderer, points_x[i], points_y[i], points_x[next], points_y[next]);
    }
}

void handle_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        button_system_process_event(&state.button_system, &event);
        switch (event.type) {
            case SDL_EVENT_QUIT:
                state.quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (state.state == START_MENU) return;
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
                        const SDL_TimerID id = SDL_AddTimer(1000, enter_shop, NULL);
                        if (id == 0) {
                            SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
                        }
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        state.pause_state_change = state.pause_state_change ?  0 : 1;
                        break;
                    case SDL_SCANCODE_SPACE:
                        const SDL_KeyboardEvent e = event.key;
                        if (!e.repeat && !state.dead && !global_time.is_paused) add_projectile(state.ship.position, true, false);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (state.state == START_MENU) return;
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

void render_spacecraft_explosion(const bool saucer, const bool small) {
    const v2 pos = saucer ? small ? state.small_saucer.pos : state.big_saucer.pos : state.ship.position;

    const int n = saucer ? 12 : 5;

    for (int i = 0; i < n; i++) {
        // ReSharper disable once CppDFAArrayIndexOutOfBounds
        // Clion thinks its possible for death_lines to be selected with n = 12, even though its not
        death_line d = saucer ? state.saucer_death_lines[i] : state.ship_death_lines[i];
        if (d.ttl > 0.0f) SDL_RenderLine(state.renderer, pos.x + d.p1.x, pos.y + d.p1.y, pos.x + d.p2.x,
                                         pos.y + d.p2.y);
        d.p1 = v2_sum(d.p1, v2_scale(v2_scale(d.vel, SPEED), (float)global_time.dt));
        d.p2 = v2_sum(d.p2, v2_scale(v2_scale(d.vel, SPEED), (float)global_time.dt));
        d.ttl -= (float) global_time.dt;
        if (saucer) {
            state.saucer_death_lines[i] = d;
        } else {
            // ReSharper disable once CppDFAArrayIndexOutOfBounds
            // See above
            state.ship_death_lines[i] = d;
        }
    }
}

void render_ship(void) {
    v2 *new_points = render_angle_helper(ship_points, 6, state.ship.angle);

    const v2 pos = state.ship.position;
    for (int i = 0; i < 5; i++) {
        SDL_RenderLine(state.renderer, pos.x + new_points[i].x, pos.y + new_points[i].y,
                       pos.x + new_points[i + 1].x, pos.y + new_points[i + 1].y);
    }

    free(new_points);
}

void render_lives(void) {
    v2 offset = (v2){30.0f, 50.0f};
    v2 *new_points = render_angle_helper(ship_points, 6, 180.0f);
    for (int j = 0; j < state.lives; j++) {
        for (int i = 0; i < 5; i++) {
            SDL_RenderLine(state.renderer, offset.x + new_points[i].x, offset.y + new_points[i].y,
                           offset.x + new_points[i + 1].x, offset.y + new_points[i + 1].y);
        }
        offset.x += 12.0f;
    }
    free(new_points);
}

void render_score(void) {
    const int length = snprintf(NULL, 0, "%d", state.score);
    char buffer[length + 1];
    snprintf(buffer, length + 1, "%d", state.score);
    render_text(state.renderer, buffer, (v2){62.0f, 20.0f}, 20.0f);
}

void render_booster(void) {
    const v2 booster_points[] = {
        {-2, -7},
        {0, -13},
        {2, -7},
    };

    v2 *new_points = render_angle_helper(booster_points, 3, state.ship.angle);

    const v2 pos = state.ship.position;

    //SDL_SetRenderDrawColor(state.renderer, 112, 73, 0, 255);

    SDL_RenderLine(state.renderer, pos.x + new_points[0].x, pos.y + new_points[0].y,
                   pos.x + new_points[1].x, pos.y + new_points[1].y);

    SDL_RenderLine(state.renderer, pos.x + new_points[1].x, pos.y + new_points[1].y,
                   pos.x + new_points[2].x, pos.y + new_points[2].y);

    //SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);

    free(new_points);
}

void render_projectiles(void) {
    for (int i = 0; i < array_list_size(state.projectiles); i++) {
        const projectile *p = array_list_get(state.projectiles, i);
        const int radius = 1;
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    SDL_RenderPoint(state.renderer, p->pos.x + (float) x, p->pos.y + (float) y);
                }
            }
        }
    }
}

v2 *render_angle_helper(const v2 *points, const int n, const float angle) {
    v2 *new_points = malloc(n * sizeof(v2));
    for (int i = 0; i < n; i++) {
        new_points[i] = points[i];

        new_points[i].x = points[i].x * cosf(angle *
                                             ((float) M_PI / 180.f)) - points[i].y * sinf(
                              angle * ((float) M_PI / 180.f));

        new_points[i].y = points[i].x * sinf(angle *
                                             ((float) M_PI / 180.f)) + points[i].y * cosf(
                              angle * ((float) M_PI / 180.0f));
    }
    return new_points;
}

void render_saucer(const v2 pos, const float scale) {
    for (int i = 0; i < 12; i++) {
        const int next = (i + 1) % 12;
        SDL_RenderLine(state.renderer, pos.x + saucer_points[i].x * scale, pos.y + saucer_points[i].y * scale,
                       pos.x + saucer_points[next].x * scale, pos.y + saucer_points[next].y * scale);
    }
}

void init_saucer(void) {
    // random pos but always starts on an edge of the screen
    v2 pos = {NAN, NAN};
    switch (randi(0, RAND_MAX) % 4) {
        case 0: pos = (v2){0, randf(0, SCREEN_HEIGHT)};
            break;
        case 1: pos = (v2){SCREEN_WIDTH, randf(0, SCREEN_HEIGHT)};
            break;
        case 2: pos = (v2){randf(0, SCREEN_WIDTH), 0};
            break;
        case 3: pos = (v2){randf(0, SCREEN_WIDTH), SCREEN_HEIGHT};
            break;
        default: break;
    }

    const float s_angle = randf(0.0f, 1.0f) * (float) M_TAU;
    state.small_saucer.pos = pos;
    state.small_saucer.vel = (v2){-sinf(s_angle), cosf(s_angle)};
    state.small_saucer.shoot_timer = 1.0f;
    state.small_saucer.direction_timer = 2.0f;

    const float b_angle = randf(0.0f, 1.0f) * (float) M_TAU;
    state.big_saucer.pos = pos;
    state.big_saucer.vel = (v2){-sinf(b_angle), cosf(b_angle)};
    state.big_saucer.shoot_timer = 1.5f;
    state.big_saucer.direction_timer = 4.0f;
}

void on_saucer_hit(const bool small) {
    SDL_ClearAudioStream(state.ship_fire_stream);
    play_sound_effect(AUDIO_STREAM_SHIP_FIRE, audio_clips.explode);
    stop_saucer_sound();
    if (small) {
        state.score += 1000;
        add_particles(state.small_saucer.pos, 25, NAN, NAN, NAN);
        add_saucer_death_lines(true, 25.0f);
        state.s_saucer = false;
        state.render_s_saucer = true;
        const SDL_TimerID id = SDL_AddTimer(3000, stop_saucer_exp_render, NULL);
        if (id == 0) {
            SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
        }
    } else {
        state.score += 200;
        add_particles(state.big_saucer.pos, 25, NAN, NAN, NAN);
        add_saucer_death_lines(true, 25.0f);
        state.b_saucer = false;
        state.render_b_saucer = true;
        const SDL_TimerID id = SDL_AddTimer(3000, stop_saucer_exp_render, NULL);
        if (id == 0) {
            SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
        }
    }
}

Uint32 stop_saucer_exp_render(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    state.render_s_saucer = false;
    state.render_b_saucer = false;
    return 0;
}

void update_hyperspace(void) {
    for (int i = 0; i < 100; i++) {
        if (state.state == GAME_VIEW) {
            state.hyperspace_lines[i].z -= 0.5f;
        } else {
            state.hyperspace_lines[i].z -= 1.0f;
        }

        const float scale = 50.0f / state.hyperspace_lines[i].z;
        state.hyperspace_lines[i].x = (SCREEN_WIDTH / 2.0f) + (state.hyperspace_lines[i].target_x * scale);
        state.hyperspace_lines[i].y = (SCREEN_HEIGHT / 2.0f) + (state.hyperspace_lines[i].target_y * scale);

        if (state.hyperspace_lines[i].z < 5.0f && !global_time.is_paused) {
            state.hyperspace_lines[i].z = randf(50.0f, 100.0f);
            state.hyperspace_lines[i].target_x = randf(-200.0f, 200.0f);
            state.hyperspace_lines[i].target_y = randf(-200.0f, 200.0f);
        }
    }
}

void render_hyperspace(void) {
    if (state.state != START_MENU) {
        SDL_SetRenderDrawColor(state.renderer, 16, 16, 16, 255);
    } else {
        SDL_SetRenderDrawColor(state.renderer, 64, 64, 64, 255);
    }
    for (int i = 0; i < 100; i++) {
        SDL_RenderLine(state.renderer,
                       SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f,
                       state.hyperspace_lines[i].x, state.hyperspace_lines[i].y);
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void init_hyperspace(void) {
    for (int i = 0; i < 100; i++) {
        state.hyperspace_lines[i] = (hyperspace_line){
            .x = SCREEN_WIDTH / 2.0f, .y = SCREEN_HEIGHT / 2.0f, .z = randf(50.0f, 100.0f),
            .target_x = randf(-200.0f, 200.0f), .target_y = randf(-200.0f, 200.0f)
        };
    }
}

void projectile_collision_check(void) {
    for (int i = 0; i < array_list_size(state.projectiles); i++) {
        const projectile *p = array_list_get(state.projectiles, i);

        //Projectile vs. ship collision
        if (v2_dist_sqr(p->pos, state.ship.position) < 150 && p->ttl < 1.25f && !state.dead && state.player_invincible_timer <= 0.05f) {
            on_ship_hit();
        }

        // Projectile vs. saucer collision
        if (state.s_saucer) {
            if (v2_dist_sqr(p->pos, state.small_saucer.pos) < 235 && p->ttl < 1.35f) {
                on_saucer_hit(true);
            }
        }
        if (state.b_saucer) {
            if (v2_dist_sqr(p->pos, state.big_saucer.pos) < 260 && p->ttl < 1.35f) {
                on_saucer_hit(false);
            }
        }

        // Projectile vs. asteroid collision
        for (int j = 0; j < array_list_size(state.asteroids); j++) {
            const asteroid *a = array_list_get(state.asteroids, j);

            const float check_distance = get_asteroid_check_distance(a->size);

            if (v2_dist_sqr(p->pos, a->position) < check_distance) {
                //highlight_collision(p->pos);
                if (a->is_phased) continue;
                array_list_remove(state.projectiles, i);
                on_asteroid_hit(a, j);
            }
        }
    }
}

int ship_collision_check(void) {
    if (state.dead) return 0;

    v2 *points = render_angle_helper(ship_points, 6, state.ship.angle);

    for (int i = 0; i < 6; i++) {
        const int next_ship = (i + 1) % 6;

        // check vs asteroids
        for (int j = 0; j < array_list_size(state.asteroids); j++) {
            const asteroid *a = array_list_get(state.asteroids, j);

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

                if (a->is_phased) continue;

                //highlight_collision(res);
                free(points);
                on_asteroid_hit(a, j);
                return 1;
            }
        }

        // check vs saucers
        if (state.s_saucer) {
            for (int j = 0; j < 12; j++) {
                const int next_point = (j + 1) % 12;
                const v2 sh_p = state.ship.position;
                const v2 sa_p = state.small_saucer.pos;

                v2 res = v2_intersection(
                    v2_sum(points[i], sh_p),
                    v2_sum(points[next_ship], sh_p),
                    v2_sum(v2_scale(saucer_points[j], 1.25f), sa_p),
                    v2_sum(v2_scale(saucer_points[next_point], 1.25f), sa_p));

                if (isnan(res.x) || isnan(res.y)) continue;

                //highlight_collision(res);
                free(points);
                on_saucer_hit(true);
                return 1;
            }
        }

        if (state.b_saucer) {
            for (int j = 0; j < 12; j++) {
                const int next_point = (j + 1) % 12;
                const v2 sh_p = state.ship.position;
                const v2 sa_p = state.big_saucer.pos;

                v2 res = v2_intersection(
                    v2_sum(points[i], sh_p),
                    v2_sum(points[next_ship], sh_p),
                    v2_sum(v2_scale(saucer_points[j], 1.75f), sa_p),
                    v2_sum(v2_scale(saucer_points[next_point], 1.75f), sa_p));

                if (isnan(res.x) || isnan(res.y)) continue;

                //highlight_collision(res);
                free(points);
                on_saucer_hit(false);
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
            if (x * x + y * y <= radius * radius) {
                SDL_RenderPoint(state.renderer, v.x + (float) x, v.y + (float) y);
            }
        }
    }

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void add_ship_death_lines(const float scale) {
    for (int i = 0; i < 5; i++) {
        const float angle = randf(0.01f, 1.0f) * (float) M_TAU;

        const death_line d = {
            .pos = state.ship.position,
            .vel = (v2){
                -sinf(angle) * ((0.2f + state.ship.velocity.y) / 3.0f),
                cosf(angle) * ((0.2f + state.ship.velocity.x) / 3.0f)
            },
            .p1 = (v2){randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
            .p2 = (v2){randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
            .ttl = randf(1.8f, 3.2f)
        };

        state.ship_death_lines[i] = d;
    }
}

void add_saucer_death_lines(const bool small, const float scale) {
    for (int i = 0; i < 12; i++) {
        const float angle = randf(0.00f, 1.0f) * (float) M_TAU;

        const death_line d = {
            .pos = small ? state.small_saucer.pos : state.big_saucer.pos,
            .vel = (v2){-sinf(angle), cosf(angle)},
            .p1 = (v2){randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
            .p2 = (v2){randf(0.0f, 1.0f) * scale, randf(0.0f, 1.0f) * scale},
            .ttl = randf(1.8f, 3.2f)
        };

        state.saucer_death_lines[i] = d;
    }
}

void add_particles(const v2 pos, const int n, const float r, const float g, const float b) {

    for (int i = 0; i < n; i++) {
        const float scale = 25.0f;
        const float angle = randf(0.01f, 1.0f) * (float) M_TAU;
        array_list_add(state.asteroid_particles, &(death_line){
                           .pos = {pos.x + randf(-20.0f, 20.0f), pos.y + randf(-20.0f, 20.0f)},
                           .vel = (v2){-sinf(angle), cosf(angle)},
                           .p1 = (v2){randf(0.0f, 0.05f) * scale, randf(0.0f, 0.05f) * scale},
                           .p2 = (v2){randf(0.0f, 0.05f) * scale, randf(0.0f, 0.05f) * scale},
                           .ttl = randf(0.6f, 1.0f),
                           .r = r,
                           .g = g,
                           .b = b
                       });
    }

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void add_projectile(const v2 pos, const bool from_ship, const bool from_small_saucer) {
    if (state.state == OVER_MENU) return;

    const v2 ship_vel = (v2){
        -sinf((state.ship.angle * (float) M_PI) / 180.0f),
        cosf((state.ship.angle * (float) M_PI) / 180.0f)
    };
    const float rand_angle = randf(0.00f, 1.0f) * (float) M_TAU;

    v2 projectile_vel;
    if (from_ship) {
        // we are firing it
        projectile_vel = v2_scale(ship_vel, 6.0f);
        SDL_ClearAudioStream(state.fire_stream);
        play_sound_effect(AUDIO_STREAM_FIRE, audio_clips.fire);
    } else if (from_small_saucer) {
        // the small saucer is firing it, so aim at the player
        v2 direction = {
            state.ship.position.x - pos.x,
            state.ship.position.y - pos.y
        };

        const float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }

        const float accuracy_factor = fminf((float) state.score / 40000.0f, 1.0f);
        const float max_angle_deviation = (1.0f - accuracy_factor) * (float) M_PI_4;

        const float angle_deviation = randf(-max_angle_deviation, max_angle_deviation);

        const float cos_dev = cosf(angle_deviation);
        const float sin_dev = sinf(angle_deviation);
        const v2 rotated_direction = {
            direction.x * cos_dev - direction.y * sin_dev,
            direction.x * sin_dev + direction.y * cos_dev
        };

        projectile_vel = v2_scale(rotated_direction, 6.0f);
    } else {
        // the big saucer is firing it, so aim randomly
        projectile_vel = v2_scale((v2){-sinf(rand_angle), cosf(rand_angle)}, 6.0f);
    }

    array_list_add(state.projectiles, &(projectile){
                       .pos = pos,
                       .ttl = 1.5f,
                       .vel = projectile_vel
                   });
    if (from_ship) {
        // apply kickback in the opposite direction
        state.ship.velocity.x += sinf((state.ship.angle * (float) M_PI) / 180.0f);
        state.ship.velocity.y -= cosf((state.ship.angle * (float) M_PI) / 180.0f);
    }
}

void destroy_all_asteroids(void) {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        const asteroid *a = array_list_get(state.asteroids, i);
        free(a->points);
    }
}

void apply_friction(float *v, const float amount) {
    if (*v > 0) *v = fmaxf(0, *v - amount);
    else if (*v < 0) *v = fminf(0, *v + amount);
}

void cleanup(void) {
    destroy_all_asteroids();
    music_cleanup();
    cleanup_saucer_sound();
    button_system_cleanup(&state.button_system);
    array_list_free(state.asteroids);
    array_list_free(state.projectiles);
    array_list_free(state.asteroid_particles);
    SDL_DestroyTexture(state.intermediate_texture);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}