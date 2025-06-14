#ifndef TEXT_H
#define TEXT_H

#include "SDL3/SDL_render.h"
#include "util/v2.h"

typedef struct {
    v2 points[12];
    int count;
} number, letter;

extern number NUMBERS[10];

extern letter LETTERS[26];

void render_text(SDL_Renderer *renderer, char c[], v2 pos, float scale);

void render_text_3d(SDL_Renderer *renderer, char c[], v2 pos, float scale, SDL_Color color);

void render_text_3d_extruded(SDL_Renderer *renderer, char c[], v2 pos, float scale);

void render_text_thick(SDL_Renderer *renderer, char c[], v2 pos, float scale, float thickness, float offset);

void draw_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float thickness);

#endif //TEXT_H
