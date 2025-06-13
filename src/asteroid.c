#include "asteroid.h"
#include "util/array_list.h"
#include "main.h"
#include "text.h"
#include "util/math_ext.h"

#define M_TAU (M_PI * 2)

float get_asteroid_scale(const asteroid_size size) {
    switch (size) {
        case SMALL:
            return 20.0f;
        case MEDIUM:
            return 40.0f;
        case LARGE:
            return 60.0f;
        default:
            return 0.0f;
    }
}

float get_asteroid_velocity_scale(const asteroid_size size) {
    switch (size) {
        case SMALL:
            return 2.0f;
        case MEDIUM:
            return 1.1f;
        case LARGE:
            return 0.7f;
        default:
            return 0.0f;
    }
}

float get_asteroid_check_distance(const asteroid_size size) {
    switch (size) {
        case SMALL:
            return 200.0f;
        case MEDIUM:
            return 500.0f;
        case LARGE:
            return 700.0f;
        default:
            return 0.0f;
    }
}

void render_asteroid_explosion_particles(void) {
    SDL_SetRenderDrawColor(state.renderer, 128, 128, 128, 255);
    for (size_t i = 0; i < array_list_size(state.asteroid_particles); i++) {
        const death_line *d = array_list_get(state.asteroid_particles, i);
        if (!isnan(d->r) && !isnan(d->g) && !isnan(d->b)) {
            SDL_SetRenderDrawColor(state.renderer, (Uint8)d->r, (Uint8)d->g, (Uint8)d->b, 255);
        }
        SDL_RenderLine(state.renderer, d->pos.x + d->p1.x, d->pos.y + d->p1.y, d->pos.x + d->p2.x, d->pos.y + d->p2.y);
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}

void update_asteroids(void) {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        asteroid *a = array_list_get(state.asteroids, i);

        if (a->type == PHASER) {
            if (a->is_phased) {
                a->phase_timer -= (float) global_time.dt;
                if (a->phase_timer <= 0) {
                    a->is_phased = false;
                    a->phase_cooldown = 2.0f + randf(0, 2.0f);
                }
            } else {
                a->phase_cooldown -= (float) global_time.dt;
                if (a->phase_cooldown <= 0) {
                    a->is_phased = true;
                    a->phase_timer = 1.0f + randf(0, 1.0f);
                }
            }
        }

        a->position.x = wrap0f(a->position.x += a->velocity.x, SCREEN_WIDTH);
        a->position.y = wrap0f(a->position.y += a->velocity.y, SCREEN_HEIGHT);
    }
}

void draw_2x_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // 2
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y - 4 * scale, center_x - 1 * scale,
                   center_y - 4 * scale);
    SDL_RenderLine(state.renderer, center_x - 1 * scale, center_y - 4 * scale, center_x - 1 * scale,
                   center_y - 2 * scale);
    SDL_RenderLine(state.renderer, center_x - 1 * scale, center_y - 2 * scale, center_x - 3 * scale,
                   center_y - 2 * scale);
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y - 2 * scale, center_x - 3 * scale, center_y);
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y, center_x - 1 * scale, center_y);

    // X
    SDL_RenderLine(state.renderer, center_x + 1 * scale, center_y - 3 * scale + 2, center_x + 3 * scale,
                   center_y - 1 * scale + 2);
    SDL_RenderLine(state.renderer, center_x + 3 * scale, center_y - 3 * scale + 2, center_x + 1 * scale,
                   center_y - 1 * scale + 2);
}

void draw_chain_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // Top
    SDL_RenderLine(state.renderer, center_x - 1 * scale, center_y - 3 * scale - 4, center_x + 1 * scale,
                   center_y - 1 * scale - 4);
    SDL_RenderLine(state.renderer, center_x + 1 * scale, center_y - 3 * scale - 4, center_x - 1 * scale,
                   center_y - 1 * scale - 4);

    // Bottom left
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y + 1 * scale - 4, center_x - 1 * scale,
                   center_y + 3 * scale - 4);
    SDL_RenderLine(state.renderer, center_x - 1 * scale, center_y + 1 * scale - 4, center_x - 3 * scale,
                   center_y + 3 * scale - 4);

    // Bottom right
    SDL_RenderLine(state.renderer, center_x + 1 * scale, center_y + 1 * scale - 4, center_x + 3 * scale,
                   center_y + 3 * scale - 4);
    SDL_RenderLine(state.renderer, center_x + 3 * scale, center_y + 1 * scale - 4, center_x + 1 * scale,
                   center_y + 3 * scale - 4);
}

void draw_shield_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    SDL_RenderLine(state.renderer, center_x, center_y - 4 * scale, center_x - 3 * scale, center_y - 1 * scale);
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y - 1 * scale, center_x - 2 * scale,
                   center_y + 3 * scale);
    SDL_RenderLine(state.renderer, center_x - 2 * scale, center_y + 3 * scale, center_x, center_y + 2 * scale);
    SDL_RenderLine(state.renderer, center_x, center_y + 2 * scale, center_x + 2 * scale, center_y + 3 * scale);
    SDL_RenderLine(state.renderer, center_x + 2 * scale, center_y + 3 * scale, center_x + 3 * scale,
                   center_y - 1 * scale);
    SDL_RenderLine(state.renderer, center_x + 3 * scale, center_y - 1 * scale, center_x, center_y - 4 * scale);
}

void draw_phaser_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // Inner
    SDL_RenderLine(state.renderer, center_x - 2 * scale, center_y - 1 * scale, center_x - 1 * scale, center_y);
    SDL_RenderLine(state.renderer, center_x - 1 * scale, center_y, center_x - 2 * scale, center_y + 1 * scale);

    // Middle
    //SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y - 2 * scale, center_x - 2 * scale, center_y);
    //SDL_RenderLine(state.renderer, center_x - 2 * scale, center_y, center_x - 3 * scale, center_y + 2 * scale);

    // Outer
    SDL_RenderLine(state.renderer, center_x - 4 * scale, center_y - 3 * scale, center_x - 3 * scale, center_y);
    SDL_RenderLine(state.renderer, center_x - 3 * scale, center_y, center_x - 4 * scale, center_y + 3 * scale);

    // point
    SDL_RenderPoint(state.renderer, center_x + 1 * scale, center_y);
    SDL_RenderPoint(state.renderer, center_x + 1 * scale - 1, center_y);
    SDL_RenderPoint(state.renderer, center_x + 1 * scale, center_y - 1);
    SDL_RenderPoint(state.renderer, center_x + 1 * scale, center_y + 1);
    SDL_RenderPoint(state.renderer, center_x + 1 * scale + 1, center_y);
}

void draw_split_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // Six dots in two groups
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            SDL_RenderPoint(state.renderer, (center_x - 2 * scale) + (float) i, center_y - 1 * scale + (float) j);
            SDL_RenderPoint(state.renderer, center_x - 3 * scale + (float) i, center_y + 1 * scale + (float) j);
            SDL_RenderPoint(state.renderer, center_x - 1 * scale + (float) i, center_y + 1 * scale + (float) j);

            SDL_RenderPoint(state.renderer, center_x + 2 * scale + (float) i, center_y - 1 * scale + (float) j);
            SDL_RenderPoint(state.renderer, center_x + 1 * scale + (float) i, center_y + 1 * scale + (float) j);
            SDL_RenderPoint(state.renderer, center_x + 3 * scale + (float) i, center_y + 1 * scale + (float) j);
        }
    }
}

void draw_vampire_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // Two fangs
    SDL_RenderLine(state.renderer, center_x - 2 * scale, center_y - 2 * scale, center_x - 2 * scale,
                   center_y + 1 * scale);
    SDL_RenderLine(state.renderer, center_x - 2 * scale, center_y + 1 * scale, center_x - 1 * scale,
                   center_y + 2 * scale);

    SDL_RenderLine(state.renderer, center_x + 2 * scale, center_y - 2 * scale, center_x + 2 * scale,
                   center_y + 1 * scale);
    SDL_RenderLine(state.renderer, center_x + 2 * scale, center_y + 1 * scale, center_x + 1 * scale,
                   center_y + 2 * scale);
}

void draw_static_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    SDL_RenderLine(state.renderer,
                   center_x - 1 * scale, center_y - 3 * scale,
                   center_x + 1 * scale, center_y - 1 * scale);

    SDL_RenderLine(state.renderer,
                   center_x + 1 * scale, center_y - 1 * scale,
                   center_x - 1 * scale, center_y + 1 * scale);

    SDL_RenderLine(state.renderer,
                   center_x - 1 * scale, center_y + 1 * scale,
                   center_x + 1 * scale, center_y + 3 * scale);
}

void draw_lucky_icon(const float center_x, const float center_y, const float scale, const Uint8 r,  const Uint8 g, const Uint8 b) {
    SDL_SetRenderDrawColor(state.renderer, r, g, b, 255);
    // 4-leaf clover
    SDL_RenderLine(state.renderer, center_x, center_y, center_x - 1 * scale, center_y - 2 * scale);
    SDL_RenderLine(state.renderer, center_x, center_y, center_x + 1 * scale, center_y - 2 * scale);

    SDL_RenderLine(state.renderer, center_x, center_y, center_x + 2 * scale, center_y - 1 * scale);
    SDL_RenderLine(state.renderer, center_x, center_y, center_x + 2 * scale, center_y + 1 * scale);

    SDL_RenderLine(state.renderer, center_x, center_y, center_x - 1 * scale, center_y + 2 * scale);
    SDL_RenderLine(state.renderer, center_x, center_y, center_x + 1 * scale, center_y + 2 * scale);

    SDL_RenderLine(state.renderer, center_x, center_y, center_x - 2 * scale, center_y - 1 * scale);
    SDL_RenderLine(state.renderer, center_x, center_y, center_x - 2 * scale, center_y + 1 * scale);
}

void render_asteroids(void) {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        asteroid *a = array_list_get(state.asteroids, i);

        const float icon_scale = a->size == LARGE ? 2.25f : a->size == MEDIUM ? 2.0f : 1.0f;
        const float center_x = a->position.x;
        const float center_y = a->position.y;

        int r, g, b;
        switch (a->type) {
            case STD:
                r = g = b = a->color;
                break;
            case DBLXP:
                r = a->color - 60;
                g = (a->color + 80 > 255) ? 255 : a->color + 80;
                b = a->color - 60;
                draw_2x_icon(center_x, center_y, icon_scale, r, g, b);
                break; //  green tint
            case CHAIN:
                r = (a->color + 80 > 255) ? 255 : a->color + 80;
                g = a->color - 60;
                b = a->color - 60;
                draw_chain_icon(center_x, center_y, icon_scale, r, g, b);
                break; //  red tint
            case ARMOR:
                r = g = b = a->color - 80;
                //draw_shield_icon(center_x, center_y, icon_scale);
                break; // darker metallic
            case PHASER:
                if (a->is_phased) {
                    r = g = b = a->color / 3; //  faint when phased
                } else {
                    r = a->color - 60;
                    g = a->color - 60;
                    b = (a->color + 80 > 255) ? 255 : a->color + 80; // blue tint
                }
                draw_phaser_icon(center_x, center_y, icon_scale, r, g, b);
                break;
            case SPLIT:
                r = (a->color + 80 > 255) ? 255 : a->color + 80;
                g = (a->color - 80 < 128) ? 128 : a->color - 80;
                b = (a->color + 80 > 255) ? 255 : a->color + 80;
                draw_split_icon(center_x, center_y, icon_scale, r, g, b);
                break; // pink tint
            case VAMPIRE:
                r = (a->color + 50 > 255) ? 255 : a->color + 50;
                g = (a->color - 50 < 128) ? 128 : a->color - 50;
                b = (a->color - 50 < 128) ? 128 : a->color - 50;
                draw_vampire_icon(center_x, center_y, icon_scale, r, g, b);
                break; // red tint
            case LUCKY:
                r = (a->color + 80 > 255) ? 255 : a->color + 80;
                g = (a->color + 80 > 255) ? 255 : a->color + 80;
                b = a->color - 60;
                draw_lucky_icon(center_x, center_y, icon_scale, r, g, b);
                break; // gold tint
            case STATIC:
                r = a->color - 60;
                g = a->color - 60;
                b = (a->color + 60 > 255) ? 255 : a->color + 60;
                draw_static_icon(center_x, center_y, icon_scale, r, g, b);
                break; // blue tint
            default:
                r = g = b = a->color;
                break;
        }

        a->r = r;
        a->g = g;
        a->b = b;

        SDL_SetRenderDrawColor(state.renderer, r, g, b, a->is_phased ? 100 : 255);

        for (int j = 0; j < a->point_count; j++) {
            const int next = (j + 1) % a->point_count;
            SDL_RenderLine(state.renderer,
                           a->position.x + a->points[j].x, a->position.y + a->points[j].y,
                           a->position.x + a->points[next].x, a->position.y + a->points[next].y);
        }

        //  armor indicator
        if (a->type == ARMOR && a->armor_hits > 1) {
            SDL_SetRenderDrawColor(state.renderer, 200, 200, 200, 255);
            for (int h = 0; h < a->armor_hits - 1; h++) {
                const float angle = ((float) h * 2.0f * (float) M_PI) / ((float) a->armor_hits - 1);
                const float dot_x = a->position.x + cosf(angle) * ((float) a->size * 15.0f + 10.0f);
                const float dot_y = a->position.y + sinf(angle) * ((float) a->size * 15.0f + 10.0f);
                SDL_RenderPoint(state.renderer, dot_x, dot_y);
                SDL_RenderPoint(state.renderer, dot_x + 1, dot_y);
                SDL_RenderPoint(state.renderer, dot_x, dot_y + 1);
                SDL_RenderPoint(state.renderer, dot_x + 1, dot_y + 1);
            }
        }
    }
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
}


void add_new_asteroid(const asteroid_size size, const v2 pos) {
    add_new_asteroid_typed(size, pos, get_random_asteroid_type());
}

void add_new_asteroid_typed(const asteroid_size size, v2 pos, const asteroid_type type) {
    const int n = randi(12, 18);
    // ReSharper disable once CppDFAMemoryLeak
    // Clion says there's a memory leak here, but all points arrays are freed in every code path
    v2 *points = malloc((size_t) n * sizeof(v2));

    for (int i = 0; i < n; i++) {
        float radius = 0.3f + 0.2f * randf(0.0f, 1.0f);
        if (randf(0.0f, 1.0f) < 0.2f) radius -= 0.2f;
        const float angle = (float) i * ((float) M_TAU / (float) n) + (float) M_PI * 0.125f * randf(0.0f, 1.0f);

        points[i] = v2_scale(v2_scale((v2){cosf(angle), sinf(angle)}, radius), get_asteroid_scale(size));
    }

    const float angle = M_TAU * randf(0.0f, 1.0f);

    int armor_hits = 1;
    const bool is_phased = false;
    const float phase_timer = 0;
    float phase_cooldown = 0;

    switch (type) {
        case ARMOR:
            armor_hits = (size == LARGE) ? 3 : 2;
            break;
        case PHASER:
            phase_cooldown = randf(1.0f, 3.0f);
            break;
        default:
            break;
    }
    array_list_add(state.asteroids, &(asteroid){
                       .position = !isnan(pos.x) ? pos : (v2){randf(0.0f, SCREEN_WIDTH), randf(0.0f, SCREEN_HEIGHT)},
                       .velocity = v2_scale((v2){cosf(angle), sinf(angle)}, get_asteroid_velocity_scale(size)),
                       .angle = 0,
                       .scale = get_asteroid_scale(size),
                       .size = size,
                       .type = type,
                       .points = points,
                       .point_count = n,
                       .color = randi(128, 255),
                       .armor_hits = armor_hits,
                       .is_phased = is_phased,
                       .phase_timer = phase_timer,
                       .phase_cooldown = phase_cooldown,
                       .destruction_time = 0,
                       .marked_for_chain_destruction = false
                   });

}

bool can_hit_asteroid(const asteroid *a) {
    return a->type != PHASER || !a->is_phased;
}

void on_asteroid_hit(const asteroid *a, const int i) {
    if (!can_hit_asteroid(a)) {
        return;
    }

    if (a->type == ARMOR && a->armor_hits > 1) {
        asteroid *modifiable_a = array_list_get(state.asteroids, i);
        modifiable_a->armor_hits--;
        play_sound_effect(AUDIO_STREAM_SFX, audio_clips.armor_hit);
        return;
    }

    SDL_ClearAudioStream(state.asteroid_stream);
    const int last_div = state.score / 10000;

    if (a->type == VAMPIRE) {
        const int stolen_points = (a->size == LARGE) ? 100 : (a->size == MEDIUM) ? 50 : 25;
        state.score = (state.score > stolen_points) ? state.score - stolen_points : 0;
    }

    if (a->type == STATIC) {
        state.player_static_timer = 2.0f;
    }

    add_particles(a->position, randi(15, 20), a->r, a->g, a->b);

    int base_score = 0;
    const int xp_multiplier = (a->type == DBLXP) ? 2 : 1;

    switch (a->size) {
        case SMALL:
            base_score = 100;
            state.score += base_score * xp_multiplier;

            if (a->type == LUCKY) {
                handle_lucky_bonus();
            }

            free(a->points);
            array_list_remove(state.asteroids, i);
            play_sound_effect(AUDIO_STREAM_ASTEROID, audio_clips.small_asteroid_hit);
            break;

        case MEDIUM:
            base_score = 50;
            state.score += base_score * xp_multiplier;

            const int split_count = (a->type == SPLIT) ? 3 : 2;
            for (int j = 0; j < split_count; j++) {
                const asteroid_type child_type = (a->type == CHAIN) ? CHAIN : STD;
                add_new_asteroid_typed(SMALL, a->position, child_type);
            }

            if (a->type == LUCKY) {
                handle_lucky_bonus();
            }

            free(a->points);
            array_list_remove(state.asteroids, i);
            play_sound_effect(AUDIO_STREAM_ASTEROID, audio_clips.medium_asteroid_hit);
            break;

        case LARGE:
            base_score = 20;
            state.score += base_score * xp_multiplier;

            const int split_count_2 = (a->type == SPLIT) ? 4 : 2;
            for (int j = 0; j < split_count_2; j++) {
                const asteroid_type child_type = (a->type == CHAIN) ? CHAIN : STD;
                add_new_asteroid_typed(MEDIUM, a->position, child_type);
            }

            if (a->type == LUCKY) {
                handle_lucky_bonus();
            }

            free(a->points);
            array_list_remove(state.asteroids, i);
            play_sound_effect(AUDIO_STREAM_ASTEROID, audio_clips.big_asteroid_hit);
            break;
    }

    if (a->type == CHAIN) {
        trigger_chain_reaction(a->position, 100.0f); // 100 pixel radius
    }

    if (state.score / 10000 != last_div) {
        state.lives++;
    }
}

void handle_lucky_bonus(void) {
    play_sound_effect(AUDIO_STREAM_SFX, audio_clips.lucky);
    const int luck_roll = randi(1, 3);
    switch (luck_roll) {
        case 1:
            state.lives++;
            state.draw_lucky = true;
            strncpy(state.luck_text, "EXTRA LIFE", sizeof(state.luck_text));
            break;
        case 2:
            state.score += 10000;
            state.draw_lucky = true;
            strncpy(state.luck_text, "PLUS 10000", sizeof(state.luck_text));
            break;
        case 3:
            state.player_invincible_timer = 5.0f;
            state.draw_lucky = true;
            strncpy(state.luck_text, "5S INVINCIBLE", sizeof(state.luck_text));
            break;
        default:
            break;
    }
}

void trigger_chain_reaction(const v2 explosion_pos, const float radius) {
    for (size_t i = 0; i < array_list_size(state.asteroids); i++) {
        asteroid *a = array_list_get(state.asteroids, i);

        if (a->destruction_time > 0 || a->marked_for_chain_destruction) {
            continue;
        }

        const float distance = sqrtf(powf(a->position.x - explosion_pos.x, 2.0f) +
                                     powf(a->position.y - explosion_pos.y, 2.0f));

        if (distance <= radius) {
            a->destruction_time = SDL_GetTicks() + randi(250, 500);
            a->marked_for_chain_destruction = true;
        }
    }
}

asteroid_type get_random_asteroid_type(void) {
    const int roll = randi(1, 100);
    if (roll <= 50) return STD; // 50% standard
    if (roll <= 65) return DBLXP; // 15% double XP
    if (roll <= 75) return CHAIN; // 10% chain reaction
    if (roll <= 85) return ARMOR; // 10% armored
    if (roll <= 90) return PHASER; // 5% phaser
    if (roll <= 95) return SPLIT; // 5% splitter
    if (roll <= 98) return VAMPIRE; // 3% vampire
    if (roll <= 99) return STATIC; // 1% static
    return LUCKY; // 1% lucky
}
