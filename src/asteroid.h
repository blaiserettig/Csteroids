
#ifndef ASTEROID_H
#define ASTEROID_H

#include <stdint.h>

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
    uint8_t color;
    AsteroidSize size;
} asteroid;

float get_asteroid_scale(AsteroidSize size);

float get_asteroid_velocity_scale(AsteroidSize size);

float get_asteroid_check_distance(AsteroidSize size);

#endif //ASTEROID_H
