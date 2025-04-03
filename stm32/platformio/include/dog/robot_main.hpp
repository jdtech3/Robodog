#pragma once

#include <memory>

#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

#include "util/logging.h"
#include "dog/leg.hpp"
#include "dog/servo.hpp"

class Robodog {
    public:
        typedef enum movement_type {
            STATIONARY,
            WALK_FORWARD,
            WALK_BACKWARD,
            WALK_LEFT,
            WALK_RIGHT,
        } movement_type_t;

        constexpr static float BODY_X = 0.0720f;
        constexpr static float BODY_Y = 0.3180f;
        constexpr static float BODY_Z = 0.0575f;
        constexpr static float LEG_S  = 0.0250f;
        constexpr static float LEG_L0 = 0.0765f;
        constexpr static float LEG_L1 = 0.1330f;
        constexpr static float LEG_L2 = 0.1140f;

        constexpr static auto POS_NEUTRAL = glm::quat(1.f, 0.f, 0.f, 0.f);
        constexpr static auto POS_10DEG_PITCHUP = glm::quat(0.9961947f, 0.0871557f, 0.f, 0.f);
        constexpr static auto POS_20DEG_PITCHUP = glm::quat(0.9848078f, 0.1736482f, 0.f, 0.f);
        constexpr static auto POS_20DEG_YAWLEFT = glm::quat(0.9848078f, 0.f, 0.f, 0.1736482f);

        Robodog(
            std::unique_ptr<Leg> leg_BL,
            std::unique_ptr<Leg> leg_BR,
            std::unique_ptr<Leg> leg_FL,
            std::unique_ptr<Leg> leg_FR
        ) : leg_BL(std::move(leg_BL)), leg_BR(std::move(leg_BR)), leg_FL(std::move(leg_FL)), leg_FR(std::move(leg_FR)) {};

        void run();
        void tick();

        void set_target(const glm::quat& _target);

    private:
        UART_HandleTypeDef *uart;

        std::unique_ptr<Leg> leg_BL;
        std::unique_ptr<Leg> leg_BR;
        std::unique_ptr<Leg> leg_FL;
        std::unique_ptr<Leg> leg_FR;

        // target
        glm::quat target;
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
