#include "button.h"

#include "audio.h"
#include "text.h"
#include "main.h"
#include "util/math_ext.h"

void button_system_init(button_system *bs) {
    bs->buttons = NULL;
    bs->count = 0;
    bs->capacity = 0;
}

void button_system_cleanup(button_system *bs) {
    free(bs->buttons);
    bs->buttons = NULL;
    bs->count = 0;
    bs->capacity = 0;
}

button button_create_default(const SDL_FRect rect, const char *label, void (*callback)(void), const enum state state) {
    button btn = {
        .draw_rect = rect,
        .btn_color = {200, 200, 200, 255},
        .hover_color = {255, 255, 255, 255},
        .label_color = {0, 0, 0, 255},
        .is_hovered = false,
        .was_clicked = false,
        .on_click = callback,
        .visible = false,
        .display_state = state,
    };
    strncpy(btn.label, label, sizeof(btn.label) - 1);
    btn.label[sizeof(btn.label) - 1] = '\0';
    return btn;
}

button* button_system_add_default(button_system *bs, const SDL_FRect rect, const char *label, void (*callback)(void), const enum state state) {
    const button btn = button_create_default(rect, label, callback, state);
    return button_system_add_custom(bs, btn);
}

button* button_system_add_custom(button_system *bs, const button btn) {
    if (bs->count >= bs->capacity) {
        bs->capacity = bs->capacity == 0 ? 8 : bs->capacity * 2;
        bs->buttons = realloc(bs->buttons, bs->capacity * sizeof(button));
    }

    bs->buttons[bs->count] = btn;
    return &bs->buttons[bs->count++];
}

void button_system_process_event(const button_system *bs, const SDL_Event *event) {
    for (size_t i = 0; i < bs->count; i++) {
        button *btn = &bs->buttons[i];
        if (!btn->visible) continue;

        switch (event->type) {
            case SDL_EVENT_MOUSE_MOTION: {
                const bool was_hovered = btn->is_hovered;
                btn->is_hovered = point_in_rect(event->motion.x, event->motion.y, &btn->draw_rect);

                if (btn->is_hovered && !was_hovered) {
                    SDL_ClearAudioStream(state.saucer_stream);
                    play_sound_effect(AUDIO_STREAM_SAUCER, audio_clips.button_hover);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                if (event->button.button == SDL_BUTTON_LEFT &&
                    point_in_rect(event->button.x, event->button.y, &btn->draw_rect)) {
                    btn->was_clicked = true;
                    SDL_ClearAudioStream(state.saucer_stream);
                    play_sound_effect(AUDIO_STREAM_SAUCER, audio_clips.button_select);
                    }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                if (event->button.button == SDL_BUTTON_LEFT && btn->was_clicked &&
                    point_in_rect(event->button.x, event->button.y, &btn->draw_rect)) {
                    if (btn->on_click) {
                        btn->on_click();
                    }
                    }
                btn->was_clicked = false;
                break;
            }
            default:
                break;
        }
    }
}

void button_system_render(const button_system *bs, SDL_Renderer *renderer) {
    for (size_t i = 0; i < bs->count; i++) {
        button *btn = &bs->buttons[i];
        if (!btn->visible) continue;

        const SDL_Color color = btn->is_hovered ? btn->hover_color : btn->btn_color;

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &btn->draw_rect);

        SDL_SetRenderDrawColor(renderer, btn->label_color.r, btn->label_color.g,
                              btn->label_color.b, btn->label_color.a);
        render_text_thick(renderer, btn->label,
                         (v2){btn->draw_rect.x + btn->draw_rect.w / 2.0f,
                              btn->draw_rect.y + btn->draw_rect.h / 2.0f},
                         btn->draw_rect.w * 0.125f, 4.0f, btn->draw_rect.w * 0.125f * 1.15f);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void button_system_show_buttons_for_state(const button_system *bs, const enum state game_state) {
    for (size_t i = 0; i < bs->count; i++) {
        if (strcmp(bs->buttons[i].label, "") == 0) continue;
        bs->buttons[i].visible = false;
    }

    for (size_t i = 0; i < bs->count; i++) {
        button *btn = &bs->buttons[i];
        if (strcmp(btn->label, "") == 0) continue;
        if (game_state == btn->display_state) {
            btn->visible = true;
        }
    }
}