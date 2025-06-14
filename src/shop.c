
#include "shop.h"

#include <stdio.h>

#include "main.h"
#include "text.h"

#include "SDL3/SDL_render.h"

void shop_item_init(shop_item *item, const SDL_FRect container, float icon_size_ratio,
                    const char *title, const char *desc, const int price) {
    item->outer_rect = container;

    const float padding = 8.0f;
    const float icon_width = container.h - (padding * 2);

    item->icon_rect = (SDL_FRect){
        .x = container.x + padding,
        .y = container.y + padding,
        .w = icon_width,
        .h = icon_width
    };

    const float text_area_x = item->icon_rect.x + item->icon_rect.w + padding;
    const float text_area_width = container.x + container.w - text_area_x - padding;

    item->title_rect = (SDL_FRect){
        .x = text_area_x,
        .y = container.y + padding,
        .w = text_area_width,
        .h = (container.h - padding * 3) * 0.33f
    };

    item->desc_rect = (SDL_FRect){
        .x = text_area_x,
        .y = item->title_rect.y + item->title_rect.h + padding,
        .w = text_area_width,
        .h = (container.h - padding * 3) * 0.66f
    };

    strncpy(item->title, title, sizeof(item->title) - 1);
    item->title[sizeof(item->title) - 1] = '\0';

    strncpy(item->description, desc, sizeof(item->description) - 1);
    item->description[sizeof(item->description) - 1] = '\0';

    item->price = price;
    item->is_purchased = false;
    item->is_affordable = true;

    item->bg_color = (SDL_Color){60, 60, 60, 255};
    item->text_color = (SDL_Color){255, 255, 255, 255};
}

void shop_init(shop *s, const float screen_width, const float screen_height) {
    s->render_ship = false;
    s->render_items[0] = false;
    s->render_items[1] = false;
    s->render_items[2] = false;

    s->padding = 50.0f;
    s->item_spacing = 15.0f;

    const float scale = 0.75f;
    const float scaled_width = (screen_width - s->padding * 2) * scale;
    const float scaled_height = (screen_height - s->padding * 2) * scale;

    const float center_x = (screen_width - scaled_width) / 2.0f;
    const float center_y = (screen_height - scaled_height) / 2.0f;

    const float ship_area_width = scaled_width * 0.35f;

    s->outer_ship_rect = (SDL_FRect){
        .x = center_x,
        .y = center_y,
        .w = ship_area_width,
        .h = scaled_height
    };

    s->inner_ship_rect = (SDL_FRect){
        .x = s->outer_ship_rect.x + 1,
        .y = s->outer_ship_rect.y + 1,
        .w = ship_area_width - 2,
        .h = s->outer_ship_rect.h - 2
    };

    const float items_area_x = s->outer_ship_rect.x + s->outer_ship_rect.w + 40;
    const float items_area_width = center_x + scaled_width - items_area_x - 20;
    const float items_area_height = scaled_height - 40;

    const float item_height = (items_area_height - s->item_spacing * 2) / 3.0f;

    s->item_count = 3;

    for (int i = 0; i < s->item_count; i++) {
        const SDL_FRect item_container = {
            .x = items_area_x,
            .y = center_y + 20 + (float)i * (item_height + s->item_spacing),
            .w = items_area_width,
            .h = item_height
        };

        // placeholder
        switch (i) {
            case 0:
                shop_item_init(&s->items[i], item_container, 0.8f,
                              "Speed Boost", "Increases ship speed by 25%", 100);
                break;
            case 1:
                shop_item_init(&s->items[i], item_container, 0.8f,
                              "Extra Life", "Gain an additional life", 200);
                break;
            case 2:
                shop_item_init(&s->items[i], item_container, 0.8f,
                              "Shield", "Temporary invincibility", 150);
                break;
            default:
                break;
        }
    }
}

void shop_update_affordability(shop *s, const int coins) {
    for (int i = 0; i < s->item_count; i++) {
        s->items[i].is_affordable = coins >= s->items[i].price && !s->items[i].is_purchased;

        if (s->items[i].is_purchased) {
            s->items[i].bg_color = (SDL_Color){40, 80, 40, 255};
            s->items[i].text_color = (SDL_Color){200, 255, 200, 255};
        } else if (s->items[i].is_affordable) {
            s->items[i].bg_color = (SDL_Color){60, 60, 60, 255};
            s->items[i].text_color = (SDL_Color){255, 255, 255, 255};
        } else {
            s->items[i].bg_color = (SDL_Color){80, 40, 40, 255};
            s->items[i].text_color = (SDL_Color){150, 150, 150, 255};
        }
    }
}

void shop_render(const shop *s, SDL_Renderer *renderer) {

    if (s->render_ship) {
        // outer shop background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &s->outer_ship_rect);

        // outer area
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &s->outer_ship_rect);

        // ship display
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &s->inner_ship_rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &s->inner_ship_rect);
    }

    for (int i = 0; i < s->item_count; i++) {
        if (s->render_items[i]) {
            const shop_item *item = &s->items[i];
            // item outer border
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &item->outer_rect);

            // item background
            //SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            //SDL_RenderFillRect(renderer, &item->outer_rect);

            // icon area
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &item->icon_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &item->icon_rect);

            // title area
            //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            //SDL_RenderFillRect(renderer, &item->title_rect);

            // description area
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &item->desc_rect);

            // borders around title and desc areas
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &item->title_rect);
            SDL_RenderRect(renderer, &item->desc_rect);

            const v2 title_pos = (v2) {.x = item->title_rect.x + item->title_rect.w * 0.5f, .y = item->title_rect.y + item->title_rect.h * 0.5f};
            render_text_3d(state.renderer, item->title, title_pos, 25.0f, (SDL_Color) {.r = 255, .g = 255, .b = 255, .a = 255});

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            const v2 desc_pos = (v2) {.x = item->desc_rect.x + item->desc_rect.w * 0.5f, .y = item->desc_rect.y + item->desc_rect.h * 0.5f};
            render_text(state.renderer, item->description,  desc_pos, 15.0f);
        }
    }
}

void init_shop(void) {
    shop_init(&state.shop, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void update_shop(void) {
    shop_update_affordability(&state.shop, state.coins);
}

void render_shop(void) {
    shop_render(&state.shop, state.renderer);
}

Uint32 enter_shop(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    state.state = SHOP_MENU;
    if (!state.shop.render_ship) {
        state.shop.render_ship = true;
        return interval / 2;
    }
    if (!state.shop.render_items[0]) {
        state.shop.render_items[0] = true;
        return interval / 2;
    }
    if (!state.shop.render_items[1]) {
        state.shop.render_items[1] = true;
        return interval / 2;
    }
    if (!state.shop.render_items[2]) {
        state.shop.render_items[2] = true;
        return 0;
    }
    return 0;
}

void exit_shop(void) {
    state.shop.render_ship = false;
    state.shop.render_items[0] = false;
    state.shop.render_items[1] = false;
    state.shop.render_items[2] = false;
    state.state = GAME_VIEW;
}