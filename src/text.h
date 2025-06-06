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

#endif //TEXT_H
