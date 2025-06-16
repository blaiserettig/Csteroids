
#ifndef V3_H
#define V3_H

#include "v2.h"

typedef struct {
    float x, y, z;
} v3;

static v3 rotate_3d_y(const v3 point, const float angle) {
    const float cos_a = cosf(angle);
    const float sin_a = sinf(angle);

    return (v3){
        point.x * cos_a - point.z * sin_a,
        point.y,
        point.x * sin_a + point.z * cos_a
    };
}

static v2 project_3d_to_2d(const v3 point, const float distance) {
    const float perspective_factor = distance / (distance + point.z);
    return (v2){
        point.x * perspective_factor,
        point.y * perspective_factor
    };
}

#endif //V3_H
