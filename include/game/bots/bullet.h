#ifndef BOTS_BULLET_H
#define BOTS_BULLET_H

#include "lib/math.h"
#include "lib/time.h"

typedef struct Bullet {
    Vector2f position;
    Vector2f velocity;
    Time init_time;
} Bullet;

#endif
