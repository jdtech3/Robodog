#pragma once

#include "cglm/cglm.h"

#include "main.h"       // upstream

// Typedefs

typedef enum movement_type {
    STATIONARY,
    WALK_FORWARD,
    WALK_BACKWARD,
    WALK_LEFT,
    WALK_RIGHT,
} movement_type_t;

typedef struct global_state {
    // target
    vec3 target_up;
    float target_z;

    // performance
    int theoretical_tps;            // loops per sec
    int actual_tps;
    uint32_t last_update_cyccnt;
} global_state_t;

// Constants

#define PWM_TIM_US_PER_CYCLE    10
#define CLOCK_SPEED             180*1000000

// Function prototypes

void set_pwm_val(TIM_HandleTypeDef *tim, int width_us, uint32_t channel);

void init_robot();
void run_robot();
