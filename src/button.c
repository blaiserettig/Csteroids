#include "button.h"

#include "text.h"

void button_process_event(button *btn, const SDL_Event *ev) {
    if (ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (ev->button.button == SDL_BUTTON_LEFT &&
            ev->button.x >= btn->draw_rect.x &&
            ev->button.x <= btn->draw_rect.x + btn->draw_rect.w &&
            ev->button.y >= btn->draw_rect.y &&
            ev->button.y <= btn->draw_rect.y + btn->draw_rect.h) {
            btn->pressed = true;
        }
    }
}

bool button_press(SDL_Renderer *r, button *btn) {
    SDL_SetRenderDrawColor(r, btn->btn_color.r, btn->btn_color.g, btn->btn_color.b, btn->btn_color.a);
    SDL_RenderFillRect(r, &btn->draw_rect);
    SDL_SetRenderDrawColor(r, btn->label_color.r, btn->label_color.g, btn->label_color.b, btn->label_color.a);
    render_text(r, btn->label,
                (v2){btn->draw_rect.x + btn->draw_rect.w / 2.0f, btn->draw_rect.y + btn->draw_rect.h / 2.0f},
                btn->draw_rect.w * 0.125f);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    if (btn->pressed) {
        btn->pressed = false;
        return true;
    }
    return false;
}
