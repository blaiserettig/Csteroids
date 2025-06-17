#ifndef BUTTON_H
#define BUTTON_H
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "game_types.h"

typedef struct {
    SDL_FRect draw_rect;

    SDL_Color btn_color;
    SDL_Color label_color;
    SDL_Color hover_color;

    bool is_hovered;
    bool was_clicked;
    void (*on_click)(void);
    bool visible;

    char label[32];

    enum state display_state;
} button;

typedef struct {
    button *buttons;
    size_t count;
    size_t capacity;
} button_system;

void button_process_event(button *btn, const SDL_Event *ev);

bool button_press(SDL_Renderer *r, button *btn);

void button_system_init(button_system *bs);
void button_system_cleanup(button_system *bs);
button button_create_default(SDL_FRect rect, const char *label, void (*callback)(void), enum state state);
button* button_system_add_default(button_system *bs, SDL_FRect rect, const char *label, void (*callback)(void), enum state state);
button* button_system_add_custom(button_system *bs, button btn);
void button_system_process_event(const button_system *bs, const SDL_Event *event);
void button_system_render(const button_system *bs, SDL_Renderer *renderer);
void button_system_show_buttons_for_state(const button_system *bs, enum state game_state);

#endif //BUTTON_H
