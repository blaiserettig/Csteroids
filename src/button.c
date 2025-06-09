#include "button.h"

#include "text.h"

bool is_mouse_over(const float x, const float y, const button *btn) {
    return (float) x >= btn->draw_rect.x && (float) x <= btn->draw_rect.x + btn->draw_rect.w &&
           (float) y >= btn->draw_rect.y && (float) y <= btn->draw_rect.y + btn->draw_rect.h;
}

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
    if (ev->type == SDL_EVENT_MOUSE_MOTION) {
        const float mx = ev->motion.x;
        const float my = ev->motion.y;
        if (is_mouse_over(mx, my, btn)) {
            btn->btn_color.r = 255;
            btn->btn_color.g = 255;
            btn->btn_color.b = 255;
        } else {
            btn->btn_color.r = 200;
            btn->btn_color.g = 200;
            btn->btn_color.b = 200;
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
