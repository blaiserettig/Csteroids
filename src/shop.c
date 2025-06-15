
#include "shop.h"

#include <stdio.h>

#include "main.h"
#include "text.h"

#include "SDL3/SDL_render.h"
#include "util/math_ext.h"

void shop_item_init(void) {
    state.shop.items_list[0] = (shop_item){
        .title = "SPEED BOOST",
        .description = "INCREASE SHIP SPEED BY 5 PERCENT",
        .price = 3,
        .included = false
    };
    state.shop.items_list[1] = (shop_item){
        .title = "EXTRA CANNON",
        .description = "FIRE AN ADDITIONAL PROJECTILE",
        .price = 10,
        .included = false
    };
    state.shop.items_list[2] = (shop_item){
        .title = "WEIGHTED DICE",
        .description = "LUCKY ASTEROIDS SPAWN\n2X MORE FREQUENTLY",
        .price = 5,
        .included = false
    };
    state.shop.items_list[3] = (shop_item){
        .title = "PIERCING PROJECTILES",
        .description = "PROJECTILES PASS THROUGH\nASTEROIDS AND CAN HIT MULTIPLE TIMES",
        .price = 10,
        .included = false
    };
    state.shop.items_list[4] = (shop_item){
        .title = "SPARE PARTS",
        .description = "GAIN AN EXTRA LIFE",
        .price = 5,
        .included = false
    };
    state.shop.items_list[5] = (shop_item){
        .title = "DYNAMITE",
        .description = "CHAIN ASTEROIDS SPAWN\n2X MORE FREQUENTLY",
        .price = 5,
        .included = false
    };
    state.shop.items_list[6] = (shop_item){
        .title = "SAFE WARP",
        .description = "HYPERSPACE NEVER SPAWNS\nYOU INSIDE ASTEROIDS",
        .price = 10,
        .included = false
    };
}

shop_item get_random_shop_item(void) {
    while (true) {
        const int n = randi(0, 7);
        if (state.shop.items_list[n].included == false) {
            state.shop.items_list[n].included = true;
            return state.shop.items_list[n];
        }
    }
}

void shop_item_container_init(shop_item_container *container, const SDL_FRect root_container, float icon_size_ratio,
                    const char *title, const char *desc, const int price) {
    container->outer_rect = root_container;

    const float padding = 8.0f;
    const float icon_width = root_container.h - (padding * 2);

    container->icon_rect = (SDL_FRect){
        .x = root_container.x + padding,
        .y = root_container.y + padding,
        .w = icon_width,
        .h = icon_width
    };

    const float text_area_x = container->icon_rect.x + container->icon_rect.w + padding;
    const float text_area_width = root_container.x + root_container.w - text_area_x - padding;

    container->title_rect = (SDL_FRect){
        .x = text_area_x,
        .y = root_container.y + padding,
        .w = text_area_width,
        .h = (root_container.h - padding * 3) * 0.33f
    };

    container->desc_rect = (SDL_FRect){
        .x = text_area_x,
        .y = container->title_rect.y + container->title_rect.h + padding,
        .w = text_area_width,
        .h = (root_container.h - padding * 3) * 0.66f
    };

    strncpy(container->item.title, title, sizeof(container->item.title) - 1);
    container->item.title[sizeof(container->item.title) - 1] = '\0';

    strncpy(container->item.description, desc, sizeof(container->item.description) - 1);
    container->item.description[sizeof(container->item.description) - 1] = '\0';

    container->item.price = price;
    container->item.is_purchased = false;
    container->item.is_affordable = true;

    container->item.bg_color = (SDL_Color){60, 60, 60, 255};
    container->item.text_color = (SDL_Color){255, 255, 255, 255};
}

void shop_init(shop *s, const float screen_width, const float screen_height) {
    shop_item_init();

    s->render_ship = false;
    s->render_items[0] = false;
    s->render_items[1] = false;
    s->render_items[2] = false;
    s->init = false;

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
}

void shop_update_affordability(shop *s, const int coins) {
    for (int i = 0; i < s->item_count; i++) {
        s->containers[i].item.is_affordable = coins >= s->containers[i].item.price && !s->containers[i].item.is_purchased;

        if (s->containers[i].item.is_purchased) {
            s->containers[i].item.bg_color = (SDL_Color){40, 80, 40, 255};
            s->containers[i].item.text_color = (SDL_Color){200, 255, 200, 255};
        } else if (s->containers[i].item.is_affordable) {
            s->containers[i].item.bg_color = (SDL_Color){60, 60, 60, 255};
            s->containers[i].item.text_color = (SDL_Color){255, 255, 255, 255};
        } else {
            s->containers[i].item.bg_color = (SDL_Color){80, 40, 40, 255};
            s->containers[i].item.text_color = (SDL_Color){150, 150, 150, 255};
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
            const shop_item_container *container = &s->containers[i];
            // item outer border
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &container->outer_rect);

            // item background
            //SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            //SDL_RenderFillRect(renderer, &item->outer_rect);

            // icon area
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &container->icon_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &container->icon_rect);

            // title area
            //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            //SDL_RenderFillRect(renderer, &item->title_rect);

            // description area
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &container->desc_rect);

            // borders around title and desc areas
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &container->title_rect);
            SDL_RenderRect(renderer, &container->desc_rect);

            const v2 title_pos = (v2) {.x = container->title_rect.x + container->title_rect.w * 0.5f, .y = container->title_rect.y + container->title_rect.h * 0.5f};
            render_text_3d(state.renderer, container->item.title, title_pos, 25.0f, (SDL_Color) {.r = 255, .g = 255, .b = 255, .a = 255});

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            const v2 desc_pos = (v2) {.x = container->desc_rect.x + container->desc_rect.w * 0.5f, .y = container->desc_rect.y + container->desc_rect.h * 0.5f};
            render_text(state.renderer, container->item.description,  desc_pos, 15.0f);
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

Uint32 enter_shop(void *userdata, SDL_TimerID timerID, const Uint32 interval) {
    if (!state.shop.init) {
        const float items_area_x = state.shop.outer_ship_rect.x + state.shop.outer_ship_rect.w + 40; // SCALE from init_shop = 0.75f below
        const float items_area_width = (SCREEN_WIDTH - (SCREEN_WIDTH - state.shop.padding * 2) * 0.75f) / 2.0f + (SCREEN_WIDTH - state.shop.padding * 2) * 0.75f - items_area_x - 20;
        const float items_area_height = (SCREEN_HEIGHT - state.shop.padding * 2) * 0.75f - 40;

        const float item_height = (items_area_height - state.shop.item_spacing * 2) / 3.0f;

        state.shop.item_count = 3;

        for (int i = 0; i < state.shop.item_count; i++) {
            const SDL_FRect item_container = {
                .x = items_area_x,                                          // here too
                .y = (SCREEN_HEIGHT - (SCREEN_HEIGHT - state.shop.padding * 2) * 0.75f) / 2.0f + 20 + (float)i * (item_height + state.shop.item_spacing),
                .w = items_area_width,
                .h = item_height
            };

            const shop_item h = get_random_shop_item();
            shop_item_container_init(&state.shop.containers[i], item_container, 0.8f,  h.title, h.description, h.price);
        }
        state.shop.init = true;
        state.state = SHOP_MENU;
    }
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
    for (int i = 0; i < 7; i++) {
        state.shop.items_list[i].included = false;
    }
    state.shop.render_ship = false;
    state.shop.render_items[0] = false;
    state.shop.render_items[1] = false;
    state.shop.render_items[2] = false;
    state.shop.init = false;
    state.state = GAME_VIEW;
}