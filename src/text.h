#ifndef TEXT_H
#define TEXT_H

#include "util/v2.h"

typedef struct {
    v2 points[8];
    int count;
} number;

extern number NUMBERS[10];

#endif //TEXT_H
