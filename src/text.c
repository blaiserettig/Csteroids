#include "text.h"

#include <string.h>
#include <ctype.h>

number NUMBERS[10] = {
    {
        {
            // 0
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 1.0f},
            {0.0f, 1.0f},
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 1.0f}
        },
        7
    },
    {
        {
            // 1
            {0.36f, 0.0f},
            {0.36f, 1.0f}
        },
        2
    },
    {
        {
            // 2
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        6
    },
    {
        {
            // 3
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 1.0f},
            {0.0f, 1.0f}

        },
        7
    },
    {
        {
            // 4
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 0.0f},
            {0.72f, 1.0f}

        },
        5
    },
    {
        {
            // 5
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 1.0f},
            {0.0f, 1.0f}

        },
        6
    },
    {
        {
            // 6
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f}

        },
        5
    },
    {
        {
            // 7
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 1.0f}
        },
        3
    },
    {
        {
            // 8
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 0.0f},
            {0.0f, 0.0f}
        },
        8
    },
    {
        {
            // 9
            {0.72f, 1.0f},
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.72f, 0.4f}
        },
        5
    }
};

letter LETTERS[26] = {
    {
        {
            // A
            {0.36f, 0.0f},
            {0.72f, 0.25f},
            {0.72f, 1.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f},
            {0.0f, 1.0f},
            {0.0f, 0.25f},
            {0.36f, 0.0f}
        },
        8
    },
    {
        {
            // B
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.6486f, 1.0f},
            {0.72f, 0.75f},
            {0.72f, 0.54f},
            {0.63f, 0.45f},
            {0.0f, 0.45f},
            {0.63f, 0.45f},
            {0.72f, 0.35f},
            {0.72f, 0.26f},
            {0.48f, 0.0f},
            {0.0f, 0.0f}
        },
        12
    },
    {
        {
            // C
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        4
    },
    {
        {
            // D
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.36f, 1.0f},
            {0.72f, 0.6f},
            {0.72f, 0.4f},
            {0.36f, 0.0f},
            {0.0f, 0.0f}
        },
        7
    },
    {
        {
            // E
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.54f, 0.4f},
            {0.0f, 0.4f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        7
    },
    {
        {
            // F
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.54f, 0.4f}
        },
        6
    },
    {
        {
            // G
            {0.72f, 0.1f},
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f},
            {0.72f, 0.4f},
            {0.45f, 0.4f}
        },
        7
    },
    {
        {
            // H
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 0.0f},
            {0.72f, 1.0f}
        },
        6
    },
    {
        {
            // I
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.36f, 0.0f},
            {0.36f, 1.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        6
    },
    {
        {
            // J
            {0.36f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 1.0f},
            {0.36f, 1.0f},
            {0.0f, 0.75f}
        },
        5
    },
    {
        {
            // K
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.0f, 0.4f},
            {0.72f, 0.0f},
            {0.0f, 0.4f},
            {0.72f, 1.0f}
        },
        6
    },
    {
        {
            // L
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        3
    },
    {
        {
            // M
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {0.36f, 0.4f},
            {0.72f, 0.0f},
            {0.72f, 1.0f}
        },
        5
    },
    {
        {
            // N
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {0.72f, 1.0f},
            {0.72f, 0.0f}
        },
        4
    },
    {
        {
            // O
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f}
        },
        5
    },
    {
        {
            // P
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f}
        },
        5
    },
    {
        {
            // Q
            {0.0f, 0.2f},
            {0.0f, 0.8f},
            {0.18f, 1.0f},
            {0.54f, 1.0f},
            {0.72f, 0.8f},
            {0.72f, 0.2f},
            {0.54f, 0.0f},
            {0.18f, 0.0f},
            {0.0f, 0.2f},
            {0.45f, 0.7f},
            {0.72f, 1.0f}
        },
        11
    },
    {
        {
            // R
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.72f, 0.4f},
            {0.0f, 0.4f},
            {0.72f, 1.0f}
        },
        6
    },
    {
        {
            // S
            {0.72f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.4f},
            {0.72f, 0.4f},
            {0.72f, 1.0f},
            {0.0f, 1.0f}

        },
        6
    },
    {
        {
            // T
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.36f, 0.0f},
            {0.36f, 1.0f}
        },
        4
    },
    {
        {
            // U
            {0.0f, 0.0f},
            {0.0f, 0.8f},
            {0.18f, 1.0f},
            {0.54f, 1.0f},
            {0.72f, 0.8f},
            {0.72f, 0.0f}
        },
        6
    },
    {
        {
            // V
            {0.0f, 0.0f},
            {0.36f, 1.0f},
            {0.72f, 0.0f}
        },
        3
    },
    {
        {
            // W
            {0.0f, 0.0f},
            {0.0f, 1.0f},
            {0.36f, 0.6f},
            {0.72f, 1.0f},
            {0.72f, 0.0f}
        },
        5
    },
    {
        {
            // X
            {0.0f, 0.0f},
            {0.72f, 1.0f},
            {0.36f, 0.4f},
            {0.0f, 1.0f},
            {0.36f, 0.4f},
            {0.72f, 0.0f}
        },
        6
    },
    {
        {
            // Y
            {0.0f, 0.0f},
            {0.36f, 0.4f},
            {0.72f, 0.0f},
            {0.36f, 0.4f},
            {0.36f, 1.0f}
        },
        5
    },
    {
        {
            // Z
            {0.0f, 0.0f},
            {0.72f, 0.0f},
            {0.0f, 1.0f},
            {0.72f, 1.0f}
        },
        4
    }
};

void render_text(SDL_Renderer *renderer, char c[], v2 pos, const float scale) {
    const float x_offset = ((float)strlen(c) * scale) / 2.0f;
    const float y_offset = scale / 2.0f;
    pos.x -= x_offset;
    pos.y -= y_offset;
    for (int i = 0; i < strlen(c); i++) {
        if (isalpha(c[i]) != 0) {
            const char upper = (char) toupper(c[i]);
            const int k = upper - 'A';
            for (int j = 0; j < LETTERS[k].count - 1; j++) {
                SDL_RenderLine(renderer, pos.x + LETTERS[k].points[j].x * scale, pos.y + LETTERS[k].points[j].y * scale,
                               pos.x + LETTERS[k].points[j + 1].x * scale, pos.y + LETTERS[k].points[j + 1].y * scale);
            }
        } else if (isdigit(c[i]) != 0) {
            for (int j = 0; j < NUMBERS[c[i] - '0'].count - 1; j++) {
                SDL_RenderLine(renderer, pos.x + NUMBERS[c[i] - '0'].points[j].x * scale,
                               pos.y + NUMBERS[c[i] - '0'].points[j].y * scale,
                               pos.x + NUMBERS[c[i] - '0'].points[j + 1].x * scale,
                               pos.y + NUMBERS[c[i] - '0'].points[j + 1].y * scale);
            }
        } else if (isspace(c[i]) != 0) {
        }
        pos.x += scale;
    }
}

void render_text_3d(SDL_Renderer *renderer, char c[], const v2 pos, const float scale) {
    const float depth_offset_x = scale * 0.1f;
    const float depth_offset_y = scale * 0.1f;

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    v2 shadow_pos = {pos.x + depth_offset_x, pos.y + depth_offset_y};
    render_text(renderer, c, shadow_pos, scale);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    render_text(renderer, c, pos, scale);
}

void render_text_3d_extruded(SDL_Renderer *renderer, char c[], v2 pos, const float scale) {
    const float depth_x = scale * 0.15f;
    const float depth_y = scale * 0.1f;

    const float x_offset = ((float)strlen(c) * scale) / 2.0f;
    const float y_offset = scale / 2.0f;
    pos.x -= x_offset;
    pos.y -= y_offset;

    for (int i = 0; i < strlen(c); i++) {
        if (isalpha(c[i]) != 0) {
            const char upper = (char) toupper(c[i]);
            const int k = upper - 'A';

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int j = 0; j < LETTERS[k].count - 1; j++) {
                SDL_RenderLine(renderer,
                    pos.x + LETTERS[k].points[j].x * scale,
                    pos.y + LETTERS[k].points[j].y * scale,
                    pos.x + LETTERS[k].points[j + 1].x * scale,
                    pos.y + LETTERS[k].points[j + 1].y * scale);
            }

            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            for (int j = 0; j < LETTERS[k].count - 1; j++) {
                SDL_RenderLine(renderer,
                    pos.x + LETTERS[k].points[j].x * scale + depth_x,
                    pos.y + LETTERS[k].points[j].y * scale + depth_y,
                    pos.x + LETTERS[k].points[j + 1].x * scale + depth_x,
                    pos.y + LETTERS[k].points[j + 1].y * scale + depth_y);
            }

            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            for (int j = 0; j < LETTERS[k].count; j++) {
                SDL_RenderLine(renderer,
                    pos.x + LETTERS[k].points[j].x * scale,
                    pos.y + LETTERS[k].points[j].y * scale,
                    pos.x + LETTERS[k].points[j].x * scale + depth_x,
                    pos.y + LETTERS[k].points[j].y * scale + depth_y);
            }
        }
        pos.x += scale;
    }
}
