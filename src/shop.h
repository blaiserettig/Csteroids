
#ifndef SHOP_H
#define SHOP_H
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_timer.h"

typedef struct {
    SDL_FRect outer_rect;
    SDL_FRect icon_rect;
    SDL_FRect title_rect;
    SDL_FRect desc_rect;

    char title[32];
    char description[128];
    int price;
    bool is_purchased;
    bool is_affordable;
    SDL_Color bg_color;
    SDL_Color text_color;
} shop_item;

typedef struct {
    SDL_FRect outer_ship_rect;
    SDL_FRect inner_ship_rect;
    shop_item items[3];
    int item_count;

    float item_spacing;
    float padding;

    bool render_ship;
    bool render_items[3];
} shop;

void init_shop(void);
void update_shop(void);
void render_shop(void);
Uint32 enter_shop(void *userdata, SDL_TimerID timerID, Uint32 interval);
void exit_shop(void);

#endif //SHOP_H
