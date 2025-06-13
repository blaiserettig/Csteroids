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
    if (min >= max) return min;

    const int range = max - min + 1;
    const int limit = RAND_MAX - (RAND_MAX % range);
    int value;

    do {
        value = rand();
    } while (value >= limit);

    return min + (value % range);
}

#endif //MATH_EXT_H
