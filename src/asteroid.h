
#ifndef ASTEROID_H
#define ASTEROID_H

#include "util/v2.h"

typedef enum {
    SMALL,
    MEDIUM,
    LARGE
} AsteroidSize;

typedef struct {
    v2 position;
    v2 velocity;
    v2 *points;
    int point_count;
    float angle;
    float scale;
    AsteroidSize size;
} asteroid;

float get_asteroid_scale(const AsteroidSize size);

v2 get_asteroid_velocity(const AsteroidSize size);

#endif //ASTEROID_H
