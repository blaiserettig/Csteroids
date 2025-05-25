#include "asteroid.h"
#include "util/math_ext.h"

float get_asteroid_scale(const AsteroidSize size) {
    switch (size) {
        case SMALL:
            return 20.0f;
        case MEDIUM:
            return 40.0f;
        case LARGE:
            return 60.0f;
        default:
            return 0.0f;
    }
}

v2 get_asteroid_velocity(const AsteroidSize size) {
    switch (size) {
        case SMALL:
            return (v2) {randf(-3.0f, 3.0f), randf(-3.0f, 3.0f)};
        case MEDIUM:
            return (v2) {randf(-2.0f, 2.0f), randf(-2.0f, 2.0f)};
        case LARGE:
            return (v2) {randf(-1.0f, 1.0f), randf(-1.0f, 1.0f)};
        default:
            return (v2) {0, 0};
    }
}