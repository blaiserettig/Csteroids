
#ifndef F_OPS_H
#define F_OPS_H

#include <math.h>
#include <stdlib.h>

static float wrapf(const float given, const float max) {
    if (given < 0) return max;
    if (given > max) return 0;
    return given;
}

static float clampf(const float val, const float min, const float max) {
    return fmaxf(min, fminf(max, val));
}

static float randf(const float min, const float max) {
    const float scale = (float)rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

#endif //F_OPS_H
