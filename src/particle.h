
#ifndef PARTICLE_H
#define PARTICLE_H

#include "util/v2.h"

enum ParticleType {
    LINE,
    DOT
};

typedef struct Particle {
    enum ParticleType type;
    v2 pos;
    v2 vel;
    v2 *points;
    int point_count;
    float angle;
    float lifetime;
};

#endif //PARTICLE_H