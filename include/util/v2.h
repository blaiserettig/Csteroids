
#ifndef V2_H
#define V2_H

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
    float x;
    float y;
} v2;

static v2 v2_intersection(const v2 a0, const v2 a1, const v2 b0, const v2 b1) {
    const float d =
        (a0.x - a1.x) *  (b0.y - b1.y) - (a0.y - a1.y) *  (b0.x - b1.x);

    if (fabsf(d) < 0.000001f) {
        return (v2) {NAN, NAN};
    }

    const float t = ((a0.x - b0.x) * (b0.y - b1.y) - (a0.y - b0.y) * (b0.x - b1.x)) / d;
    const float u = ((a0.x - b0.x) * (a0.y - a1.y) - (a0.y - b0.y) * (a0.x - a1.x)) / d;

    return t >= 0 && t <= 1 && u >= 0 && u <= 1 ?
        (v2) {
            a0.x + t * (a1.x - a0.x),
            a0.y + t * (a1.y - a0.y),
        } : (v2) {NAN, NAN};
}

static v2 v2_sum(const v2 a0, const v2 a1) {
    return (v2) {a0.x + a1.x, a0.y + a1.y};
}

static float v2_dist_sqr(const v2 a0, const v2 b0) {
    return (b0.x - a0.x) * (b0.x - a0.x) + (b0.y - a0.y) * (b0.y - a0.y);
}

static v2 v2_scale(const v2 a0, const float b0) {
    return (v2) {a0.x * b0, a0.y * b0};
}

static v2 v2_inverse(const v2 a0) {
    return (v2) {-1.0 *  a0.x, -1.0 * a0.y};
}

#endif //V2_H
