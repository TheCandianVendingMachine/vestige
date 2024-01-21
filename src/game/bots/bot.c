#define STATE_DATA_BYTES 512

#include <stdlib.h>
#include <math.h>

#include "lib/time.h"
#include "game/bots/bot.h"
#include "game/bots/resource.h"

#include "logger.h"

typedef enum BotState {
    BOT_STATE_INITIAL,
    BOT_STATE_SEARCH_FOR_RESOURCE,
    BOT_STATE_GO_TO_RESOURCE,
    BOT_STATE_MINE_RESOURCE,
    BOT_STATE_RETURN_RESOURCE,
    BOT_STATE_UNLOAD_RESOURCE
} BotState;

typedef struct PID {
    float gain_proportional;
    float gain_derivative;
    float gain_integral;
    float _integral;
    float _last_error;
    Clock _query_clock;
} PID;

PID new_pid(float kp, float kd, float ki) {
    return (PID) {
        .gain_proportional = kp,
        .gain_derivative = kd,
        .gain_integral = ki,
        ._integral = 0.f,
        ._last_error = 0.f,
        ._query_clock = new_clock()
    };
}

float pid_step(PID* pid, float error) {
    float delta_time = time_as_seconds(get_elapsed_time(&pid->_query_clock));
    pid->_query_clock = new_clock();

    float p = pid->gain_proportional * error;

    float d = pid->gain_derivative * (error - pid->_last_error) / delta_time;
    if (d != d) {
        d = 0.f;
    }

    // trapezoidal method for calculating integral
    pid->_integral += 0.5f * delta_time * (error + pid->_last_error);
    float i = pid->gain_integral * pid->_integral;

    pid->_last_error = error;

    return p + i + d;
}

typedef struct BotDataSearch {
    unsigned char goal_material;
    Vector2f goal_position;
} BotDataSearch;

typedef struct BotDataGoTo {
    Vector2f resource_position;
    float target_radius;
} BotDataGoTo;

typedef struct BotDataMine {
    PID pid;
    float desired_radius;
    float allowed_radius_error;
    Vector2f resource_position;
    Time mine_enter_time;
    Time mine_time;
} BotDataMine;

typedef struct BotDataReturn {
    Vector2f base_position;
    float target_radius;
} BotDataReturn;

typedef struct BotDataUnload {
    Time unload_enter_time;
    Time unload_time;
} BotDataUnload;

union CombinedData {
    BotDataSearch search;
    BotDataGoTo go_to;
    BotDataMine mine;
    BotDataReturn go_back;
    BotDataUnload unload;
    uint8_t _memory[STATE_DATA_BYTES];
};

Bot create_bot(void) {
    return (Bot) {
        .id = -1,
        .transform = new_transform(),
        .internal_clock = new_clock(),
        .physics = {
            .acceleration = (Vector2f) { .x = 0.f, .y = 0.f },
            .position = (Vector2f) { .x = 0.f, .y = 0.f },
            .velocity = (Vector2f) { .x = 0.f, .y = 0.f },
            .max_acceleration = 500.f
        },
        .brain = {
            .state = BOT_STATE_INITIAL,
            .previous_state = BOT_STATE_INITIAL,
            .state_data = { calloc(1, STATE_DATA_BYTES), calloc(1, STATE_DATA_BYTES), calloc(1, STATE_DATA_BYTES) },
            .state_index = 0,
        }
    };
}

void bot_search_for_resource(Bot* bot, BotDataSearch* state_data) {
    bot->brain.state = BOT_STATE_GO_TO_RESOURCE;
    state_data->goal_position = (Vector2f) { .x = 3000.f, .y = 2700.f };
}

void bot_go_to_resource(Bot* bot, BotDataGoTo* state_data) {
    Vector2f direction_to_resource = sub_vector2f(state_data->resource_position, bot->physics.position);
    float distance_to_resource = length_vector2f(direction_to_resource);
    Vector2f normal_dir = normalise_vector2f(direction_to_resource);

    bot->physics.acceleration = mul_vector2f(normal_dir, bot->physics.max_acceleration);
    if (distance_to_resource < state_data->target_radius) {
        bot->brain.state = BOT_STATE_MINE_RESOURCE;
    }
}

void bot_mine(Bot* bot, BotDataMine* state_data) {
    Vector2f direction_to_resource = sub_vector2f(bot->physics.position, state_data->resource_position);
    float distance_to_resource = length_vector2f(direction_to_resource);
    float error = state_data->desired_radius - distance_to_resource;
    float pid = pid_step(&state_data->pid, error);
    Vector2f normal_dir = normalise_vector2f(direction_to_resource);

    log_debug("distance: %f, desired: %f, error: %f", distance_to_resource, state_data->desired_radius, error);
    log_debug("pid: %f", pid);

    bot->physics.acceleration = mul_vector2f(normal_dir, pid);

    if (distance_to_resource > state_data->allowed_radius_error) {
        state_data->mine_enter_time = get_elapsed_time(&bot->internal_clock);
    }

    Time elapsed = time_elapsed(state_data->mine_enter_time, get_elapsed_time(&bot->internal_clock));
    if (time_as_nanoseconds(elapsed) >= time_as_nanoseconds(state_data->mine_time)) {
        //bot->brain.state = BOT_STATE_RETURN_RESOURCE;
    }
}

void bot_return(Bot* bot, BotDataReturn* state_data) {
    bot->brain.state = BOT_STATE_UNLOAD_RESOURCE;
}

void bot_unload(Bot* bot, BotDataUnload* state_data) {
    bot->brain.state = BOT_STATE_SEARCH_FOR_RESOURCE;
}

void bot_think(Bot* bot) {
    bot->brain.previous_state = bot->brain.state;

    union CombinedData state_data;
    memcpy(state_data._memory, bot->brain.state_data[bot->brain.state_index], STATE_DATA_BYTES);

    switch (bot->brain.state) {
        case BOT_STATE_INITIAL:
            bot->brain.state = BOT_STATE_SEARCH_FOR_RESOURCE;
            break;
        case BOT_STATE_SEARCH_FOR_RESOURCE:
            bot_search_for_resource(bot, &state_data.search);
            break;
        case BOT_STATE_GO_TO_RESOURCE:
            bot_go_to_resource(bot, &state_data.go_to);
            break;
        case BOT_STATE_MINE_RESOURCE:
            bot_mine(bot, &state_data.mine);
            break;
        case BOT_STATE_RETURN_RESOURCE:
            bot_return(bot, &state_data.go_back);
            break;
        case BOT_STATE_UNLOAD_RESOURCE:
            bot_unload(bot, &state_data.unload);
            break;
    }

    memcpy(bot->brain.state_data[bot->brain.state_index], state_data._memory, STATE_DATA_BYTES);

    if (bot->brain.previous_state != bot->brain.state) {
        bot->brain.state_index = (bot->brain.state_index + 1) % 3;

        union CombinedData new_state_data;
        memcpy(new_state_data._memory, bot->brain.state_data[bot->brain.state_index], STATE_DATA_BYTES);

        switch (bot->brain.state) {
            case BOT_STATE_SEARCH_FOR_RESOURCE:
                new_state_data.search.goal_material = RESOURCE_TYPE_FUEL;
                break;
            case BOT_STATE_GO_TO_RESOURCE:
                new_state_data.go_to.resource_position = state_data.search.goal_position;
                new_state_data.go_to.target_radius = 300.f;
                break;
            case BOT_STATE_MINE_RESOURCE:
                new_state_data.mine.mine_time = time_from_seconds(5.f);
                new_state_data.mine.mine_enter_time = get_elapsed_time(&bot->internal_clock);
                new_state_data.mine.desired_radius = state_data.go_to.target_radius;
                new_state_data.mine.resource_position = state_data.go_to.resource_position;
                float allowed_radius_error = state_data.go_to.target_radius / 2.f;
                new_state_data.mine.allowed_radius_error = allowed_radius_error;
                new_state_data.mine.pid = new_pid(1.f, 3.f, 0.f);
                break;
            case BOT_STATE_RETURN_RESOURCE:
                break;
            case BOT_STATE_UNLOAD_RESOURCE:
                break;
        }

        memcpy(bot->brain.state_data[bot->brain.state_index], new_state_data._memory, STATE_DATA_BYTES);
    }
}

void bot_update(Bot* bot, float delta_time) {
    bot->physics.velocity = add_vector2f(bot->physics.velocity, mul_vector2f(bot->physics.acceleration, delta_time));
    bot->physics.position = add_vector2f(bot->physics.position, mul_vector2f(bot->physics.velocity, delta_time));

    transform_set_position(&bot->transform, (Vector3f) {
        .x = bot->physics.position.x, .y = bot->physics.position.y, 0.f
    });

    bot->physics.acceleration = (Vector2f) { .x = 0.f, .y = 0.f };
}
