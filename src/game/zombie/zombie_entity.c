#include "game/zombie/zombie_entity.h"

void behaviour_boid(ZombieEntity* zombie) {

}

void update_zombie(ZombieEntity* zombie, float delta_time) {
    switch (zombie->ai_state) {
        case WANDERING:
            behaviour_boid(zombie);
            break;
        case CHASING:
            break;
    }
}
