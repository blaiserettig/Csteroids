
#include "shop.h"

#include <stdio.h>
#include <time.h>

#include "main.h"
#include "text.h"
#include "util/v3.h"

#include "SDL3/SDL_render.h"
#include "util/math_ext.h"


void draw_circle_outline(SDL_Renderer *renderer, const float center_x, const float center_y, const float radius) {
    for (int angle = 0; angle < 360; angle++) {
        const float rad = (float)angle * (float)M_PI / 180.0f;
        const float x = center_x + (float)radius * cosf(rad);
        const float y = center_y + (float)radius * sinf(rad);
        SDL_RenderPoint(renderer, x, y);
    }
}

void render_coin(SDL_Renderer *renderer, const v2 position, const float radius) {
    const float coin_radius = radius;
    const float center_x = position.x;
    const float center_y = position.y;

    draw_circle_outline(renderer, center_x, center_y, coin_radius);

    const float c_size = coin_radius / 2;

    for (int i = (int) -c_size / 2; i <= (int) c_size / 2; i++) {
        SDL_RenderPoint(renderer, center_x - c_size / 2, center_y + (float) i);
    }

    for (int i = (int) -c_size / 2; i <= (int) c_size / 4; i++) {
        SDL_RenderPoint(renderer, center_x + (float) i, center_y - c_size / 2);
    }

    for (int i = (int) -c_size / 2; i <= (int) c_size / 4; i++) {
        SDL_RenderPoint(renderer, center_x + (float) i, center_y + c_size / 2);
    }
}

void shop_update_ship_preview(shop *s, const float delta_time) {
    static float rotation = 0.0f;
    rotation += delta_time * 2.0f;

    if (rotation > 2.0f * M_PI) {
        rotation -= 2.0f * (float)M_PI;
    }
    s->ship_rotation = rotation;
}

v3 ship_3d_vertices[] = {
    {0, 15, 5},    // nose tip front
    {-8, -15, 5},  //left wing tip front
    {-2, -12, 5},  // left engine mount front
    {0, -8, 5},    // center back front
    {2, -12, 5},   // right engine mount front
    {8, -15, 5},   // right wing tip front

    {0, 15, -5},   // nose tip back
    {-8, -15, -5}, // left wing tip back
    {-2, -12, -5}, //left engine mount back
    {0, -8, -5},   //center back back
    {2, -12, -5},  // right engine mount back
    {8, -15, -5},  // right wing tip back
};

typedef struct {
    int start, end;
} edge;

edge ship_3d_edges[] = {
    // front
    {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 0},

    // back
    {6, 7}, {7, 8}, {8, 9}, {9, 10}, {10, 11}, {11, 6},

    // connect front-back
    {0, 6}, {1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}
};

void render_3d_wireframe_ship(SDL_Renderer *renderer, const SDL_FRect *ship_rect, const float rotation_angle) {
    const int num_vertices = sizeof(ship_3d_vertices) / sizeof(ship_3d_vertices[0]);
    const int num_edges = sizeof(ship_3d_edges) / sizeof(ship_3d_edges[0]);

    const float center_x = ship_rect->x + ship_rect->w * 0.5f;
    const float center_y = ship_rect->y + ship_rect->h * 0.5f;

    const float scale = fminf(ship_rect->w, ship_rect->h) * 0.02f;

    v2 screen_points[num_vertices];

    for (int i = 0; i < num_vertices; i++) {
        const v3 rotated = rotate_3d_y(ship_3d_vertices[i], rotation_angle);

        const v2 projected = project_3d_to_2d(rotated, 80.0f);

        screen_points[i].x = center_x + projected.x * scale;
        screen_points[i].y = center_y + projected.y * scale;
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

    for (int i = 0; i < num_edges; i++) {
        const int start_idx = ship_3d_edges[i].start;
        const int end_idx = ship_3d_edges[i].end;

        SDL_RenderLine(renderer,
                      screen_points[start_idx].x, screen_points[start_idx].y,
                      screen_points[end_idx].x, screen_points[end_idx].y);
    }

    //depth cueing
    SDL_SetRenderDrawColor(renderer, 0, 180, 180, 255);

    for (int i = 0; i < num_edges; i++) {
        const int start_idx = ship_3d_edges[i].start;
        const int end_idx = ship_3d_edges[i].end;

        const v3 start_rotated = rotate_3d_y(ship_3d_vertices[start_idx], rotation_angle);
        const v3 end_rotated = rotate_3d_y(ship_3d_vertices[end_idx], rotation_angle);
        const float avg_z = (start_rotated.z + end_rotated.z) * 0.5f;

        if (avg_z < -2.0f) {
            SDL_RenderLine(renderer,
                          screen_points[start_idx].x, screen_points[start_idx].y,
                          screen_points[end_idx].x, screen_points[end_idx].y);
        }
    }
}

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
    const float cy = rect->y + rect->h * 0.5f - 25;
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

    /*const float shield_size = size * 0.3f;
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
        cx + check_size * 0.3f, cy - check_size * 0.2f);*/
}

void draw_magnet_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.7f;

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);

    const SDL_FRect left_arm = {
        cx - size * 0.4f, cy - size * 0.4f,
        size * 0.2f, size * 0.6f
    };
    SDL_RenderFillRect(renderer, &left_arm);

    const SDL_FRect right_arm = {
        cx + size * 0.2f, cy - size * 0.4f,
        size * 0.2f, size * 0.6f
    };
    SDL_RenderFillRect(renderer, &right_arm);

    const SDL_FRect bottom = {
        cx - size * 0.4f, cy + size * 0.1f,
        size * 0.8f, size * 0.1f
    };
    SDL_RenderFillRect(renderer, &bottom);

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    const SDL_FRect north_pole = {
        cx - size * 0.4f, cy - size * 0.4f,
        size * 0.2f, size * 0.2f
    };
    SDL_RenderFillRect(renderer, &north_pole);

    SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
    const SDL_FRect south_pole = {
        cx + size * 0.2f, cy - size * 0.4f,
        size * 0.2f, size * 0.2f
    };
    SDL_RenderFillRect(renderer, &south_pole);

    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);

    for (int i = 0; i < 6; i++) {
        const float curve_offset = ((float)i - 2.5f) * size * 0.08f;
        const float start_x = cx - size * 0.3f;
        const float start_y = cy - size * 0.3f + curve_offset + 30;
        const float end_x = cx + size * 0.3f;

        for (int j = 0; j < 10; j++) {
            const float t = (float)j / 9.0f;
            const float next_t = (float)(j + 1) / 9.0f;

            const float curve_height = size * 0.15f * sinf(t * (float)M_PI);
            const float next_curve_height = size * 0.15f * sinf(next_t * (float)M_PI);

            const float x1 = start_x + t * (end_x - start_x);
            const float y1 = start_y - curve_height;
            const float x2 = start_x + next_t * (end_x - start_x);
            const float y2 = start_y - next_curve_height;

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    const SDL_FRect n_label = {
        cx - size * 0.35f, cy - size * 0.35f,
        size * 0.1f, size * 0.1f
    };
    SDL_RenderRect(renderer, &n_label);

    const SDL_FRect s_label = {
        cx + size * 0.25f, cy - size * 0.35f,
        size * 0.1f, size * 0.1f
    };
    SDL_RenderRect(renderer, &s_label);
}

// BEGIN CLAUDE CODE
// This function generated by Claude Sonnet 4 because I cannot make another one of these icons myself man they are so painful
void draw_radar_icon(SDL_Renderer *renderer, const SDL_FRect *rect) {
    const float cx = rect->x + rect->w * 0.5f;
    const float cy = rect->y + rect->h * 0.5f;
    const float size = rect->w * 0.7f;
    const float radius = size * 0.4f;

    // Draw radar screen background (dark circle)
    SDL_SetRenderDrawColor(renderer, 20, 40, 20, 255);
    for (int y = (int)-radius; y <= (int)radius; y++) {
        for (int x = (int)-radius; x <= (int)radius; x++) {
            if ((float)x * (float)x + (float)y * (float)y <= radius * radius) {
                SDL_RenderPoint(renderer, cx + (float)x, cy + (float)y);
            }
        }
    }

    // Draw concentric circles (range rings)
    SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255); // Green
    for (int ring = 1; ring <= 3; ring++) {
        const float ring_radius = radius * (float)ring / 3.0f;
        // Draw circle as points (simple circle drawing)
        for (int angle = 0; angle < 360; angle += 2) {
            const float rad = (float)angle * (float)M_PI / 180.0f;
            const float x = cx + cosf(rad) * ring_radius;
            const float y = cy + sinf(rad) * ring_radius;
            SDL_RenderPoint(renderer, x, y);
        }
    }

    // Draw crosshairs
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Brighter green
    // Horizontal line
    SDL_RenderLine(renderer, cx - radius, cy, cx + radius, cy);
    // Vertical line
    SDL_RenderLine(renderer, cx, cy - radius, cx, cy + radius);

    // Draw radar sweep (bright green arc)
    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
    const float sweep_angle = 45.0f; // 45 degree sweep

    for (int angle = 0; angle <= (int)sweep_angle; angle += 2) {
        const float sweep_start = 0.0f;
        const float rad = (sweep_start + (float)angle) * (float)M_PI / 180.0f;
        const int end_x = (int)(cx + cosf(rad) * radius);
        const int end_y = (int)(cy + sinf(rad) * radius);

        // Draw sweep line with fading intensity
        const int intensity = 255 - angle * 155 / (int)sweep_angle;
        SDL_SetRenderDrawColor(renderer, intensity / 3, intensity, intensity / 3, 255);
        SDL_RenderLine(renderer, cx, cy, (float)end_x, (float)end_y);
    }

    // Draw radar blips (detected objects)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow

    // Blip 1
    const SDL_FRect blip1 = {
        cx + radius * 0.3f - 1, cy - radius * 0.2f - 1,
        2, 2
    };
    SDL_RenderFillRect(renderer, &blip1);

    // Blip 2
    const SDL_FRect blip2 = {
        cx - radius * 0.5f - 1, cy + radius * 0.4f - 1,
        2, 2
    };
    SDL_RenderFillRect(renderer, &blip2);

    // Blip 3 (smaller, more distant)
    SDL_RenderPoint(renderer, cx + radius * 0.6f, cy - radius * 0.5f);

    // Draw outer frame/bezel
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray
    const SDL_FRect outer_frame = {
        cx - radius - 2, cy - radius - 2,
        (radius + 2) * 2, (radius + 2) * 2
    };
    SDL_RenderRect(renderer, &outer_frame);

    // Draw center dot
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    const SDL_FRect center_dot = {
        cx - 1, cy - 1,
        2, 2
    };
    SDL_RenderFillRect(renderer, &center_dot);
}
// END CLAUDE CODE

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
    } else if (strcmp(container->item.title, "PIERCING") == 0) {
        draw_piercing_projectiles_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "SPARE PARTS") == 0) {
        draw_spare_parts_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "DYNAMITE") == 0) {
        draw_dynamite_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "SAFE WARP") == 0) {
        draw_safe_warp_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "MAGNET") == 0) {
        draw_magnet_icon(renderer, &container->icon_rect);
    } else if (strcmp(container->item.title, "TARGET RADAR") == 0) {
        draw_radar_icon(renderer, &container->icon_rect);
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
        .title = "PIERCING",
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
    state.shop.items_list[7] = (shop_item){
        .title = "MAGNET",
        .description = "PULL COINS TOWARDS YOU",
        .price = 7,
        .included = false
    };
    state.shop.items_list[8] = (shop_item){
        .title = "TARGET RADAR",
        .description = "PROJECTILES SEEK OUT TARGETS\nPLUS 5 PERCENT ACCURACY PER STACK",
        .price = 5,
        .included = false
    };
}

void shop_item_click_callback(void) {
    float x, y;
    SDL_GetMouseState(&x, &y);
    for (int i = 0; i < state.shop.item_count; i++) {
        if (point_in_rect(x, y, &state.shop.containers[i].outer_rect)) {
            if (state.shop.containers[i].item.price > state.coins) {
                //play_sound_effect(AUDIO_STREAM_SFX, audio_clips.no);
                return;
            }
            state.shop.containers[i].item.is_purchased = true;
            state.coins -= state.shop.containers[i].item.price;
            if (strcmp(state.shop.containers[i].item.title, "SPEED BOOST") == 0) {
                SDL_Log("SPEED BOOST CLICKED");
                ADDED_SPEED += 0.05f;
            }
            if (strcmp(state.shop.containers[i].item.title, "EXTRA CANNON") == 0) {
                SDL_Log("EXTRA CANNON CLICKED");
                FIRE_STREAMS++;
            }
            if (strcmp(state.shop.containers[i].item.title, "WEIGHTED DICE") == 0) {
                SDL_Log("WEIGHTED DICE CLICKED");
                LUCKY_CHANCE *= 2.0f;
            }
            if (strcmp(state.shop.containers[i].item.title,  "PIERCING") == 0) {
                SDL_Log("PIERCING PROJECTILES CLICKED");
                HAS_PIERCING = true;
            }
            if (strcmp(state.shop.containers[i].item.title, "SPARE PARTS") == 0) {
                SDL_Log("SPARE PARTS CLICKED");
                state.lives++;
            }
            if (strcmp(state.shop.containers[i].item.title, "DYNAMITE") == 0) {
                SDL_Log("DYNAMITE CLICKED");
                CHAIN_CHANCE *= 2.0f;
            }
            if (strcmp(state.shop.containers[i].item.title, "SAFE WARP") == 0) {
                SDL_Log("SAFE WARP CLICKED");
                HAS_SAFE_WARP = true;
            }
            if (strcmp(state.shop.containers[i].item.title, "MAGNET") == 0) {
                SDL_Log("MAGNET CLICKED");
                HAS_MAGNET = true;
            }
            if (strcmp(state.shop.containers[i].item.title, "TARGET RADAR") == 0) {
                SDL_Log("TARGET RADAR CLICKED");
                RADAR_STACKS++;
            }
        }
    }
}

shop_item get_random_shop_item(void) {
    while (true) {
        const int n = randi(0, 8);
        if (state.shop.items_list[n].included == false) {
            if (strcmp(state.shop.items_list[n].title, "PIERCING") == 0 && HAS_PIERCING) continue;
            if (strcmp(state.shop.items_list[n].title, "SAFE WARP") == 0 && HAS_SAFE_WARP) continue;
            if (strcmp(state.shop.items_list[n].title, "MAGNET") == 0 && HAS_MAGNET) continue;
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
    s->cycle_init = false;

    s->padding = 50.0f;
    s->item_spacing = 15.0f;

    s->button_start_idx = 0;
    s->leaving = false;

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

            const v2 title_pos = (v2) {.x = container->title_rect.x + container->title_rect.w * 0.5f, .y = container->title_rect.y + container->title_rect.h * 0.3f};
            const v2 price_pos = (v2) {.x = container->title_rect.x + container->title_rect.w * 0.1f, .y = container->title_rect.y + container->title_rect.h * 0.3f};
            char price_str[8];
            sprintf(price_str, "%d", container->item.price);
            const SDL_Color text_color = container->item.is_affordable || container->item.is_purchased ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){255, 200, 200, 200};
            render_text_3d(state.renderer, container->item.title, title_pos, 25.0f, text_color);

            //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            const v2 desc_pos = (v2) {.x = container->desc_rect.x + container->desc_rect.w * 0.5f, .y = container->desc_rect.y + container->desc_rect.h * 0.5f};
            render_text(state.renderer, container->item.description,  desc_pos, 15.0f);

            if (!container->item.is_affordable) {
                set_shop_button(false, i);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);
                SDL_RenderRect(renderer, &container->desc_rect);
                SDL_RenderRect(renderer, &container->title_rect);
                SDL_RenderRect(renderer, &container->icon_rect);
                SDL_RenderRect(renderer, &container->outer_rect);
            }

            if (container->item.is_purchased) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_RenderRect(renderer, &container->desc_rect);
                SDL_RenderRect(renderer, &container->title_rect);
                SDL_RenderRect(renderer, &container->icon_rect);
                SDL_RenderRect(renderer, &container->outer_rect);
                render_text_3d(state.renderer, "BOUGHT",
                               (v2){
                                   .x = container->title_rect.x + container->title_rect.w * 0.14f,
                                   .y = container->title_rect.y + container->title_rect.h * 0.35f
                               }, 20.0f, (SDL_Color){.r = 255, .g = 255, .b = 100, .a = 255});
                state.button_system.buttons[state.shop.button_start_idx + (size_t) i].visible = false;
            } else {
                render_text_3d(state.renderer, price_str, price_pos, 25.0f,
                               (SDL_Color){.r = 255, .g = 255, .b = 100, .a = 255});
                render_coin(renderer, (v2){
                                .x = container->title_rect.x + container->title_rect.w * 0.04f,
                                .y = container->title_rect.y + container->title_rect.h * 0.5f
                            }, 10.0f);
            }

            render_coins_ui((v2) {s->inner_ship_rect.x + s->inner_ship_rect.w / 2 - 15, s->inner_ship_rect.y + 45});
            SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
            render_score((v2) {s->inner_ship_rect.x + s->inner_ship_rect.w / 2, s->inner_ship_rect.y + 80}, 20.0f);
            render_lives((v2) {s->inner_ship_rect.x + s->inner_ship_rect.w / 2 - 15, s->inner_ship_rect.y + 130});

            SDL_SetRenderDrawColor(state.renderer, 255, 255, 100, 255);
            //render_text_thick(state.renderer, "SHOP", (v2) {SCREEN_WIDTH / 2.0f, 100.0f}, 70.0f, 5.0f, 90.0f);
            render_text_3d_extruded(state.renderer, "SHOP", (v2) {SCREEN_WIDTH / 2.0f, 100.0f}, 75.0f);
        }
    }
}

void init_shop(void) {
    shop_init(&state.shop, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void update_shop(void) {
    shop_update_affordability(&state.shop, state.coins);
    shop_update_ship_preview(&state.shop, (float)global_time.dt);
}

void render_shop(void) {
    shop_render(&state.shop, state.renderer);
    render_3d_wireframe_ship(state.renderer, &state.shop.inner_ship_rect, state.shop.ship_rotation);
}

void set_shop_button(const bool val, const int i) {
    state.button_system.buttons[state.shop.button_start_idx + i].visible = val;
}

void set_shop_buttons(const bool val) {
    for (int i = 0; i < state.shop.item_count; i++) {
        set_shop_button(val, i);
    }
}

Uint32 enter_shop(void *userdata, SDL_TimerID timerID, const Uint32 interval) {
    if (!state.shop.cycle_init) {
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
        state.shop.cycle_init = true;
        state.state = SHOP_MENU;
    }
    if (!state.shop.item_button_init) {
        state.shop.button_start_idx = state.button_system.count;
        for (int i = 0; i < state.shop.item_count; i++) {
            const button b = {
                .draw_rect = state.shop.containers[i].outer_rect, .btn_color = (SDL_Color){0, 0, 0, 0},
                .label_color = (SDL_Color){0, 0, 0, 0}, .hover_color = (SDL_Color){0, 255, 255, 10},
                .is_hovered = false, .was_clicked = false, .on_click = shop_item_click_callback, .visible = false,
                .label = "", .display_state = SHOP_MENU
            };
            button_system_add_custom(&state.button_system, b);
        }
        state.shop.item_button_init = true;
    } else {
        set_shop_buttons(false);
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

        set_shop_buttons(true);
        return 0;
    }
    return 0;
}

Uint32 flag_leaving_false(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    state.shop.leaving = false;
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
    state.shop.cycle_init = false;

    set_shop_buttons(false);
    state.enter_shop = false;
    state.shop.leaving = true;
    const SDL_TimerID id = SDL_AddTimer(3000, flag_leaving_false, NULL);
    if (id == 0) {
        SDL_Log("SDL_AddTimer Error: %s", SDL_GetError());
    }
    state.state = GAME_VIEW;
}