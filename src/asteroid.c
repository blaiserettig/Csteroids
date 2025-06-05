#include "asteroid.h"

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

float get_asteroid_velocity_scale(const AsteroidSize size) {
    switch (size) {
        case SMALL:
            return 1.6f;
        case MEDIUM:
            return 1.0f;
        case LARGE:
            return 0.6f;
        default:
            return 0.0f;
    }
}

float get_asteroid_check_distance(const AsteroidSize size) {
    switch (size) {
        case SMALL:
            return 200.0f;
        case MEDIUM:
            return 500.0f;
        case LARGE:
            return 700.0f;
        default:
            return 0.0f;
    }
}