#pragma once

#include <memory>

#include "glm/vec3.hpp"

#include "util/logging.h"
#include "dog/leg.hpp"
#include "dog/servo.hpp"

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
            std::unique_ptr<Leg> leg_BL,
            std::unique_ptr<Leg> leg_BR,
            std::unique_ptr<Leg> leg_FL,
            std::unique_ptr<Leg> leg_FR
        ) : leg_BL(std::move(leg_BL)), leg_BR(std::move(leg_BR)), leg_FL(std::move(leg_FL)), leg_FR(std::move(leg_FR)) {};

        void run();
        void tick();

    private:
        UART_HandleTypeDef *uart;

        std::unique_ptr<Leg> leg_BL;
        std::unique_ptr<Leg> leg_BR;
        std::unique_ptr<Leg> leg_FL;
        std::unique_ptr<Leg> leg_FR;

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

// #define PWM_TIM_US_PER_CYCLE    10
#define CLOCK_SPEED             180*1000000

// Function prototypes

void set_pwm_val(TIM_HandleTypeDef *tim, int width_us, uint32_t channel);
