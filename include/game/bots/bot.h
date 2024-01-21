#ifndef BOTS_BOT_H
#define BOTS_BOT_H

#include "transform.h"
#include "lib/math.h"
#include "lib/clock.h"
#include "lib/pid.h"

typedef struct Bot {
    Clock internal_clock;
    size_t id;
    Transform transform;
    struct BotPhysics {
        Vector2f position;
        Vector2f velocity;
        float speed;
        Vector2f acceleration;
        Vector2f force;
        float mass;
    } physics;
    struct BotBrain {
        unsigned char state;
        unsigned char previous_state;
        void* state_data[3];
        size_t state_index;
    } brain;
    struct BotController {
        float thrust;
        PID _pid_x;
        PID _pid_y;
        unsigned char state;
        unsigned char goal;
        Vector2f target_position;
        float target_distance;
        float allowed_distance_error;
    } controller;
} Bot;

struct World;
Bot create_bot(void);
void bot_think(struct World* world, Bot* bot);
void bot_controller(Bot* bot, float delta_time);
void bot_physics(Bot* bot, float delta_time);

#endif
