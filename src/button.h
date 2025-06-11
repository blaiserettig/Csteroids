#ifndef BUTTON_H
#define BUTTON_H
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

typedef struct {
    SDL_FRect draw_rect;

    struct {
        Uint8 r, g, b, a;
    } btn_color;

    struct {
        Uint8 r, g, b, a;
    } label_color;

    bool pressed;
    bool play_hover;
    int tag;
    char label[15];
} button;

void button_process_event(button *btn, const SDL_Event *ev);

bool button_press(SDL_Renderer *r, button *btn);

#endif //BUTTON_H
