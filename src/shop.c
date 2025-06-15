
#include "shop.h"

#include "main.h"
#include "text.h"

#include "SDL3/SDL_render.h"
#include "util/math_ext.h"

void draw_speed_boost_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.8f;

    // arrow pointing right
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);

    // arrow shaft
    const float shaft_y = cy;
    const float shaft_start_x = cx - size * 0.3f;
    const float shaft_end_x = cx + size * 0.2f;
    const float shaft_thickness = size * 0.08f;

    const SDL_FRect shaft = {
        shaft_start_x, shaft_y - shaft_thickness,
        shaft_end_x - shaft_start_x, shaft_thickness * 2
    };
    SDL_RenderFillRect(renderer, &shaft);

    // arrow head
    const float head_size = size * 0.2f;
    for (int i = 0; i < (int)head_size; i++) {
        const float line_x = shaft_end_x + (float)i;
        const float line_top = shaft_y - head_size + (float)i;
        const float line_bottom = shaft_y + head_size - (float)i;
        SDL_RenderLine(renderer, line_x, line_top, line_x, line_bottom);
    }

    // motion lines
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    for (int i = 0; i < 3; i++) {
        float const line_x = cx - size * 0.4f - (float)i * size * 0.05f;
        SDL_RenderLine(renderer, line_x, cy - size * 0.1f, line_x, cy + size * 0.1f);
    }
}

void draw_extra_cannon_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.8f;

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

    // two cannons
    for (int cannon = 0; cannon < 2; cannon++) {
        const float offset_y = cannon == 0 ? -size * 0.15f : size * 0.15f;

        // barrel
        SDL_FRect barrel = {
            cx - size * 0.3f, cy + offset_y - size * 0.05f,
            size * 0.5f, size * 0.1f
        };
        SDL_RenderFillRect(renderer, &barrel);

        // base
        SDL_FRect base = {
            cx - size * 0.35f, cy + offset_y - size * 0.08f,
            size * 0.15f, size * 0.16f
        };
        SDL_RenderFillRect(renderer, &base);
    }

    // Projectiles
    SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
    for (int i = 0; i < 2; i++) {
        const float offset_y = i == 0 ? -size * 0.15f : size * 0.15f;
        SDL_FRect projectile = {
            cx + size * 0.25f, cy + offset_y - size * 0.03f,
            size * 0.06f, size * 0.06f
        };
        SDL_RenderFillRect(renderer, &projectile);
    }
}

void draw_weighted_dice_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.6f;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    const SDL_FRect dice = {
        cx - size * 0.5f, cy - size * 0.5f,
        size, size
    };
    SDL_RenderFillRect(renderer, &dice);

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderRect(renderer, &dice);

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    const float dot_size = size * 0.15f;
    const float spacing = size * 0.22f;

    // L
    const SDL_FRect dot1 = {cx - spacing - dot_size * 0.5f, cy - spacing - dot_size * 0.5f, dot_size, dot_size};
    const SDL_FRect dot2 = {cx - spacing - dot_size * 0.5f, cy - dot_size * 0.5f, dot_size, dot_size};
    const SDL_FRect dot3 = {cx - spacing - dot_size * 0.5f, cy + spacing - dot_size * 0.5f, dot_size, dot_size};

    // R
    const SDL_FRect dot4 = {cx + spacing - dot_size * 0.5f, cy - spacing - dot_size * 0.5f, dot_size, dot_size};
    const SDL_FRect dot5 = {cx + spacing - dot_size * 0.5f, cy - dot_size * 0.5f, dot_size, dot_size};
    const SDL_FRect dot6 = {cx + spacing - dot_size * 0.5f, cy + spacing - dot_size * 0.5f, dot_size, dot_size};

    SDL_RenderFillRect(renderer, &dot1);
    SDL_RenderFillRect(renderer, &dot2);
    SDL_RenderFillRect(renderer, &dot3);
    SDL_RenderFillRect(renderer, &dot4);
    SDL_RenderFillRect(renderer, &dot5);
    SDL_RenderFillRect(renderer, &dot6);

    SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
    const SDL_FRect weight = {
        cx - size * 0.15f, cy + size * 0.25f,
        size * 0.3f, size * 0.15f
    };
    SDL_RenderFillRect(renderer, &weight);
}

void draw_piercing_projectiles_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.8f;

    SDL_SetRenderDrawColor(renderer, 100, 70, 50, 255);

    const SDL_FRect asteroidl = {
        cx - size * 0.35f, cy - size * 0.2f,
        size * 0.25f, size * 0.4f
    };
    SDL_RenderFillRect(renderer, &asteroidl);

    const SDL_FRect asteroidr = {
        cx + size * 0.1f, cy - size * 0.15f,
        size * 0.3f, size * 0.3f
    };
    SDL_RenderFillRect(renderer, &asteroidr);

    // trail
    SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
    const float trail_thickness = size * 0.06f;
    const SDL_FRect trail = {
        cx - size * 0.4f, cy - trail_thickness * 0.5f,
        size * 0.8f, trail_thickness
    };
    SDL_RenderFillRect(renderer, &trail);

    // head
    SDL_SetRenderDrawColor(renderer, 255, 150, 50, 255);
    const SDL_FRect projectile = {
        cx + size * 0.3f, cy - size * 0.05f,
        size * 0.1f, size * 0.1f
    };
    SDL_RenderFillRect(renderer, &projectile);

    // sparkles
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 4; i++) {
        const float spark_x = cx - size * 0.2f + (float)i * size * 0.15f;
        SDL_RenderLine(renderer, spark_x - 3, cy, spark_x + 3, cy);
        SDL_RenderLine(renderer, spark_x, cy - 3, spark_x, cy + 3);
    }
}

void draw_spare_parts_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.7f;

    SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);

    const float heart_size = size * 0.8f;
    const float half_heart = heart_size * 0.5f;

    const SDL_FRect left_lobe = {
        cx - heart_size * 0.5f, cy - heart_size * 0.3f,
        half_heart, half_heart
    };
    SDL_RenderFillRect(renderer, &left_lobe);

    const SDL_FRect right_lobe = {
        cx, cy - heart_size * 0.3f,
        half_heart, half_heart
    };
    SDL_RenderFillRect(renderer, &right_lobe);

    // bottom triangle
    const float triangle_height = heart_size * 0.6f;
    for (int i = 0; i < (int)triangle_height; i++) {
        const float y = cy + heart_size * 0.2f + (float)i;
        const float width = triangle_height - (float)i;
        const float x_start = cx - width * 0.5f;
        SDL_RenderLine(renderer, x_start, y, x_start + width, y);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float plus_size = size * 0.2f;
    const float plus_thickness = size * 0.04f;

    const SDL_FRect h_line = {
        cx + size * 0.2f, cy + size * 0.2f - plus_thickness * 0.5f,
        plus_size, plus_thickness
    };
    SDL_RenderFillRect(renderer, &h_line);

    const SDL_FRect v_line = {
        cx + size * 0.3f - plus_thickness * 0.5f, cy + size * 0.1f,
        plus_thickness, plus_size
    };
    SDL_RenderFillRect(renderer, &v_line);
}

void draw_dynamite_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.7f;

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    const SDL_FRect stick = {
        cx - size * 0.15f, cy - size * 0.4f,
        size * 0.3f, size * 0.6f
    };
    SDL_RenderFillRect(renderer, &stick);

    SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);
    const SDL_FRect fuse = {
        cx - size * 0.02f, cy - size * 0.5f,
        size * 0.04f, size * 0.2f
    };
    SDL_RenderFillRect(renderer, &fuse);

    SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
    const SDL_FRect spark = {
        cx - size * 0.03f, cy - size * 0.55f,
        size * 0.06f, size * 0.06f
    };
    SDL_RenderFillRect(renderer, &spark);

    SDL_SetRenderDrawColor(renderer, 255, 150, 50, 255);
    for (int i = 0; i < 8; i++) {
        const float angle = (float)i * 45.0f * ((float)M_PI / 180.0f);
        const float explosion_radius = size * 0.4f;
        const float end_x = cx + cosf(angle) * explosion_radius;
        const float end_y = cy + sinf(angle) * explosion_radius;

        // rays
        for (int j = 0; j < 3; j++) {
            const float offset = ((float)j - 1) * 2;
            SDL_RenderLine(renderer,
                cx + offset, cy + offset,
                end_x + offset, end_y + offset);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const SDL_FRect label = {
        cx - size * 0.1f, cy - size * 0.1f,
        size * 0.2f, size * 0.1f
    };
    SDL_RenderRect(renderer, &label);
}

void draw_safe_warp_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.8f;

    SDL_SetRenderDrawColor(renderer, 100, 50, 200, 255);

    // concentric circles
    const int num_rings = 5;
    for (int ring = 0; ring < num_rings; ring++) {
        const float ring_size = size * (0.8f - (float)ring * 0.15f);

        // Approximate circle with lines
        for (int angle = 0; angle < 360; angle += 10) {
            const float rad = (float)angle * ((float)M_PI / 180.0f);
            const float x1 = cx + cosf(rad) * (ring_size * 0.5f);
            const float y1 = cy + sinf(rad) * (ring_size * 0.5f);
            const float x2 = cx + cosf(rad + 0.1f) * (ring_size * 0.5f);
            const float y2 = cy + sinf(rad + 0.1f) * (ring_size * 0.5f);

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }

    SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);

    const float shield_size = size * 0.3f;
    const SDL_FRect shield_body = {
        cx - shield_size * 0.5f, cy - shield_size * 0.4f,
        shield_size, shield_size * 0.7f
    };
    SDL_RenderFillRect(renderer, &shield_body);

    const float point_height = shield_size * 0.3f;
    for (int i = 0; i < (int)point_height; i++) {
        const float y = cy + shield_size * 0.3f + (float)i;
        const float width = point_height - (float)i;
        const float x_start = cx - width * 0.5f;
        SDL_RenderLine(renderer, x_start, y, x_start + width, y);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const float check_size = shield_size * 0.4f;
    SDL_RenderLine(renderer,
        cx - check_size * 0.2f, cy,
        cx, cy + check_size * 0.2f);
    SDL_RenderLine(renderer,
        cx, cy + check_size * 0.2f,
        cx + check_size * 0.3f, cy - check_size * 0.2f);
}

void render_shop_item_icon(SDL_Renderer *renderer, const shop_item_container *container) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &container->icon_rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &container->icon_rect);

    if (strcmp(container->item.title, "SPEED BOOST") == 0) {
        draw_speed_boost_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "EXTRA CANNON") == 0) {
        draw_extra_cannon_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "WEIGHTED DICE") == 0) {
        draw_weighted_dice_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "PIERCING PROJECTILES") == 0) {
        draw_piercing_projectiles_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "SPARE PARTS") == 0) {
        draw_spare_parts_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "DYNAMITE") == 0) {
        draw_dynamite_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "SAFE WARP") == 0) {
        draw_safe_warp_icon(renderer, &container->icon_rect);
    }
}

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

            render_shop_item_icon(renderer, container);

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