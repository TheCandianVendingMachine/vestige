#ifndef VESTIGE_SIMULATION_FLAGS_H
#define VESTIGE_SIMULATION_FLAGS_H

typedef enum SimulationFlags {
    SIMULATION_DEFAULT          = 0,
    SIMULATION_DISABLE_GRAVITY  = 1 << 0,
    SIMULATION_DISABLE          = 1 << 63,
} SimulationFlags;

#endif
