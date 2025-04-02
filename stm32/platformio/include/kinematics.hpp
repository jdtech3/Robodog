#pragma once

#define CGLM_OMIT_NS_FROM_STRUCT_API
#include "cglm/struct.h"

#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"

// typedef

typedef enum leg_position {
    BACK_LEFT,
    BACK_RIGHT,
    FRONT_LEFT,
    FRONT_RIGHT,
} leg_position_t;

typedef struct leg_state {
    leg_position_t leg;
    vec3s target_joint_angle;       // order is: hip sideways, hip forward/back, knee
    vec3s cur_joint_angle;
    vec3s cur_angular_speed;
} leg_state_t;

class Leg {
    public:
        void update_target(glm::vec3 target_r);
        void step_angle();
    private:
        leg_position_t leg;
        glm::vec3 target_joint_angle;
        glm::vec3 cur_joint_angle;
        glm::vec3 cur_angular_speed;
};

// Constants

#define LEG_L   1.f

// Function prototypes

leg_state_t* update_leg_target(leg_state_t* leg, vec3s target_r);  // r vec measured wrt to hip
leg_state_t* step_leg_angle(leg_state_t* leg);
