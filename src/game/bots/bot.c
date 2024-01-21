#include "lib/math.h"
#define STATE_DATA_BYTES 512

#include <stdlib.h>
#include <math.h>

#include "lib/time.h"
#include "game/bots/bot.h"
#include "game/bots/resource.h"
#include "game/bots/world.h"

#include "logger.h"

typedef enum BotState {
    BOT_STATE_INITIAL,
    BOT_STATE_SEARCH_FOR_RESOURCE,
    BOT_STATE_GO_TO_RESOURCE,
    BOT_STATE_MINE_RESOURCE,
    BOT_STATE_RETURN_RESOURCE,
    BOT_STATE_UNLOAD_RESOURCE
} BotState;

typedef enum BotControllerState {
    BOT_CONTROLLER_STATE_MOVING,
    BOT_CONTROLLER_STATE_STOPPED,
    BOT_CONTROLLER_STATE_HOVERING,
} BotControllerState;

typedef enum BotControllerGoal {
    BOT_CONTROLLER_GOAL_NONE,
    BOT_CONTROLLER_GOAL_DISABLED,
    BOT_CONTROLLER_GOAL_MOVE_TO,
    BOT_CONTROLLER_GOAL_STOP,
    BOT_CONTROLLER_GOAL_HOVER
} BotControllerGoal;

typedef struct BotDataSearch {
    unsigned char goal_material;
    float goal_radius;
    Vector2f goal_position;
} BotDataSearch;

typedef struct BotDataGoTo {
    Vector2f resource_position;
    float target_radius;
} BotDataGoTo;

typedef struct BotDataMine {
    float desired_radius;
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
            .speed = 0.f,
            .force = (Vector2f) { .x = 0.f, .y = 0.f },
            .mass = 1000.f
        },
        .brain = {
            .state = BOT_STATE_INITIAL,
            .previous_state = BOT_STATE_INITIAL,
            .state_data = { calloc(1, STATE_DATA_BYTES), calloc(1, STATE_DATA_BYTES), calloc(1, STATE_DATA_BYTES) },
            .state_index = 0,
        },
        .controller = {
            .thrust = 1000.f * 1000.f,
            ._pid_x = new_pid(1000.f, 6000000.f, 0.f),
            ._pid_y = new_pid(1000.f, 6000000.f, 0.f),
            .state = BOT_CONTROLLER_STATE_STOPPED,
            .goal = BOT_CONTROLLER_GOAL_NONE,
            .target_distance = 0.f,
            .target_position = (Vector2f) { .x = 0.f, .y = 0.f }
        }
    };
}

void bot_search_for_resource(World* world, Bot* bot, BotDataSearch* state_data) {
    bot->brain.state = BOT_STATE_GO_TO_RESOURCE;
    Resource r = resource_map_get_closest_resource(
        world->resources,
        bot->physics.position,
        state_data->goal_material
    );
    state_data->goal_position = r.position;
    state_data->goal_radius = 500.f;
}

void bot_go_to_resource(Bot* bot, BotDataGoTo* state_data) {
    Vector2f direction_to_resource = sub_vector2f(state_data->resource_position, bot->physics.position);
    float distance_to_resource = length_vector2f(direction_to_resource);
    if (distance_to_resource < state_data->target_radius) {
        bot->brain.state = BOT_STATE_MINE_RESOURCE;
    }
}

void bot_mine(Bot* bot, BotDataMine* state_data) {
    bot->controller.target_position = state_data->resource_position;
    bot->controller.target_distance = state_data->desired_radius;

    if (bot->controller.state != BOT_CONTROLLER_STATE_HOVERING) {
        state_data->mine_enter_time = get_elapsed_time(&bot->internal_clock);
    }

    Time elapsed = time_elapsed(state_data->mine_enter_time, get_elapsed_time(&bot->internal_clock));
    if (time_as_nanoseconds(elapsed) >= time_as_nanoseconds(state_data->mine_time)) {
        bot->brain.state = BOT_STATE_RETURN_RESOURCE;
    }
}

void bot_return(Bot* bot, BotDataReturn* state_data) {
    if (bot->controller.state == BOT_CONTROLLER_STATE_STOPPED) {
        bot->brain.state = BOT_STATE_UNLOAD_RESOURCE;
    }
}

void bot_unload(Bot* bot, BotDataUnload* state_data) {
    bot->brain.state = BOT_STATE_SEARCH_FOR_RESOURCE;
}

void bot_think(World* world, Bot* bot) {
    bot->brain.previous_state = bot->brain.state;

    union CombinedData state_data;
    memcpy(state_data._memory, bot->brain.state_data[bot->brain.state_index], STATE_DATA_BYTES);

    switch (bot->brain.state) {
        case BOT_STATE_INITIAL:
            bot->brain.state = BOT_STATE_SEARCH_FOR_RESOURCE;
            break;
        case BOT_STATE_SEARCH_FOR_RESOURCE:
            bot_search_for_resource(world, bot, &state_data.search);
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

        bot->controller._pid_x._last_error = 0.f;
        bot->controller._pid_y._last_error = 0.f;

        bot->controller._pid_x._integral = 0.f;
        bot->controller._pid_y._integral = 0.f;

        switch (bot->brain.state) {
            case BOT_STATE_SEARCH_FOR_RESOURCE:
                new_state_data.search.goal_material = RESOURCE_TYPE_FUEL;
                break;
            case BOT_STATE_GO_TO_RESOURCE:
                new_state_data.go_to.resource_position = state_data.search.goal_position;
                new_state_data.go_to.target_radius = state_data.search.goal_radius * 3.f;
                bot->controller.goal = BOT_CONTROLLER_GOAL_MOVE_TO;
                bot->controller.target_position = new_state_data.go_to.resource_position;
                break;
            case BOT_STATE_MINE_RESOURCE:
                new_state_data.mine.mine_time = time_from_seconds(5.f);
                new_state_data.mine.mine_enter_time = get_elapsed_time(&bot->internal_clock);
                new_state_data.mine.desired_radius = state_data.go_to.target_radius / 2.f;
                new_state_data.mine.resource_position = state_data.go_to.resource_position;
                new_state_data.mine.mine_enter_time = get_elapsed_time(&bot->internal_clock);

                bot->controller.target_position = state_data.go_to.resource_position;
                bot->controller.target_distance = state_data.go_to.target_radius;
                bot->controller.allowed_distance_error = 50.f;
                bot->controller.goal = BOT_CONTROLLER_GOAL_HOVER;
                break;
            case BOT_STATE_RETURN_RESOURCE:
                bot->controller.target_position = (Vector2f) { .x = 0.f, .y = 0.f };
                bot->controller.goal = BOT_CONTROLLER_GOAL_MOVE_TO;
                break;
            case BOT_STATE_UNLOAD_RESOURCE:
                bot->controller.goal = BOT_CONTROLLER_GOAL_STOP;
                break;
        }

        memcpy(bot->brain.state_data[bot->brain.state_index], new_state_data._memory, STATE_DATA_BYTES);
    }
}

void bot_controller(Bot* bot, float delta_time) {
    bot->controller.state = BOT_CONTROLLER_STATE_MOVING;
    if (length_vector2f(bot->physics.velocity) < 1.f) {
        bot->controller.state = BOT_CONTROLLER_STATE_STOPPED;
    }

    if (bot->controller.goal == BOT_CONTROLLER_GOAL_DISABLED) {
        return;
    }

    Vector2f desired_velocity;
    switch (bot->controller.goal) {
        case BOT_CONTROLLER_GOAL_NONE:
            desired_velocity = bot->physics.velocity;
            break;
        case BOT_CONTROLLER_GOAL_STOP:
            desired_velocity = (Vector2f) { .x = 0.f, .y = 0.f };
            break;
        case BOT_CONTROLLER_GOAL_MOVE_TO: {
            Vector2f direction = sub_vector2f(bot->controller.target_position, bot->physics.position);
            desired_velocity = sub_vector2f(direction, bot->physics.velocity);
            break;
        }
        case BOT_CONTROLLER_GOAL_HOVER: {
            Vector2f direction = sub_vector2f(bot->controller.target_position, bot->physics.position);
            Vector2f normal_dir = normalise_vector2f(direction);
            Vector2f wanted_position = add_vector2f(
                bot->controller.target_position,
                mul_vector2f(normal_dir, -bot->controller.target_distance)
            );

            direction = sub_vector2f(wanted_position, bot->physics.position);
            desired_velocity = sub_vector2f(direction, bot->physics.velocity);

            // lookahead 10 seconds, and if we are in range of our allowed error
            // then we are fine with coasting
            Vector2f lookahead_pos = add_vector2f(bot->physics.position, mul_vector2f(bot->physics.velocity, 10.f));
            float range = distance_vector2f(lookahead_pos, wanted_position);
            if (range <= bot->controller.allowed_distance_error) {
                desired_velocity = mul_vector2f(desired_velocity, 1.f / 10.f);
                bot->controller.state = BOT_CONTROLLER_STATE_HOVERING;
            }
            break;
        }
    };

    Vector2f error = sub_vector2f(desired_velocity, bot->physics.velocity);
    Vector2f instant_accel = mul_vector2f(error, 1.f / delta_time);

    Vector2f correction;
    correction.x = pid_step(&bot->controller._pid_x, error.x, delta_time);
    correction.y = pid_step(&bot->controller._pid_y, error.y, delta_time);

    correction = add_vector2f(mul_vector2f(correction, 1.f / 10.f), instant_accel);

    bot->physics.force = clamp_vector2f(correction, bot->controller.thrust);
}

void bot_physics(Bot* bot, float delta_time) {
    bot->physics.acceleration = mul_vector2f(bot->physics.force, 1.f / bot->physics.mass);
    bot->physics.velocity = add_vector2f(bot->physics.velocity, mul_vector2f(bot->physics.acceleration, delta_time));
    bot->physics.position = add_vector2f(bot->physics.position, mul_vector2f(bot->physics.velocity, delta_time));

    transform_set_position(&bot->transform, (Vector3f) {
        .x = bot->physics.position.x, .y = bot->physics.position.y, 0.f
    });

    bot->physics.speed = length_vector2f(bot->physics.position);
    bot->physics.force = (Vector2f) { .x = 0.f, .y = 0.f };
}
