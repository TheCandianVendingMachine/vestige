#ifndef BOTS_RESOURCE_H
#define BOTS_RESOURCE_H

#include "lib/math.h"

typedef struct Resource {
    Vector2i position;
    unsigned int remaining;
    enum {
        RESOURCE_TYPE_NONE = -1,
        RESOURCE_TYPE_FUEL = 0,
        RESOURCE_TYPE_BUILD,
        RESOURCE_TYPE_COUNT
    } type;
} Resource;

#endif
