#pragma once

#include "glm/vec3.hpp"

#include "logging.h"
#include "kinematics.hpp"

// Typedefs

typedef enum movement_type {
    STATIONARY,
    WALK_FORWARD,
    WALK_BACKWARD,
    WALK_LEFT,
    WALK_RIGHT,
} movement_type_t;

class Robodog {
    public:
        Robodog(
            TIM_HandleTypeDef *tim_BL,      // back left
            TIM_HandleTypeDef *tim_BR,      // back right
            TIM_HandleTypeDef *tim_FL,      // front left
            TIM_HandleTypeDef *tim_FR,      // front right
            UART_HandleTypeDef *uart
        );
        void run();
        void tick();

    private:
        UART_HandleTypeDef *uart;

        Leg leg_BL;
        Leg leg_BR;
        Leg leg_FL;
        Leg leg_FR;

        // target
        glm::vec3 target_up;
        float target_z;

        // performance
        int theoretical_tps;            // loops per sec
        int actual_tps;
        uint32_t last_update_cyccnt;

        // flags
        bool exit;
};

// Constants

#define PWM_TIM_US_PER_CYCLE    10
#define CLOCK_SPEED             180*1000000

// Function prototypes

void init_robot();
void run_robot();

void set_pwm_val(TIM_HandleTypeDef *tim, int width_us, uint32_t channel);
