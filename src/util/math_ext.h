#ifndef MATH_EXT_H
#define MATH_EXT_H

#include <math.h>
#include <stdlib.h>

static float wrap0f(const float given, const float max) {
    if (given < 0) return max;
    if (given > max) return 0;
    return given;
}

static float clampf(const float val, const float min, const float max) {
    return fmaxf(min, fminf(max, val));
}

static float randf(const float min, const float max) {
    const float scale = (float) rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

static int randi(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

static bool point_in_rect(const float x, const float y, const SDL_FRect *rect) {
    return x >= rect->x && x <= rect->x + rect->w &&
           y >= rect->y && y <= rect->y + rect->h;
}

#endif //MATH_EXT_H
