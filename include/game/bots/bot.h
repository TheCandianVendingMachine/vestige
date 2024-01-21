#ifndef BOTS_BOT_H
#define BOTS_BOT_H

#include "transform.h"
#include "lib/math.h"
#include "lib/clock.h"

typedef struct Bot {
    Clock internal_clock;
    size_t id;
    Transform transform;
    struct BotPhysics {
        Vector2f position;
        Vector2f velocity;
        Vector2f acceleration;
        float max_acceleration;
    } physics;
    struct BotBrain {
        unsigned char state;
        unsigned char previous_state;
        void* state_data[3];
        size_t state_index;
    } brain;
} Bot;

Bot create_bot(void);
void bot_think(Bot* bot);
void bot_update(Bot* bot, float delta_time);

#endif
