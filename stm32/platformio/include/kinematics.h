#pragma once

#include "cglm/cglm.h"

// typedef

typedef struct leg_state {
    vec3 target_joint_angle;        // order is: hip sideways, hip forward/back, knee
    vec3 cur_joint_angle;
    vec3 cur_angular_speed;
} leg_state_t;

// 

leg_state_t* update_leg_target(leg_state_t* leg, vec3 target_r);  // r vec measured wrt to hip
leg_state_t* step_leg_angle(leg_state_t* leg);
