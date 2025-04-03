#pragma once

#include <memory>
#include <utility>

#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"

// #include "dog/robot_main.hpp"
#include "dog/servo.hpp"

// typedef

class Leg {
    public:
        typedef enum position {
            BACK_LEFT,
            BACK_RIGHT,
            FRONT_LEFT,
            FRONT_RIGHT,
        } position_t;

        constexpr static float SERVO_NEUTRAL = 135._deg;
        constexpr static float JOINT_0_IK_NEUTRAL = 0._deg;
        constexpr static float JOINT_1_IK_NEUTRAL = 135._deg;
        constexpr static float JOINT_2_IK_NEUTRAL = 90._deg;

        constexpr static float LEG_L0 = 0.0765f;
        constexpr static float LEG_L1 = 0.1330f;
        constexpr static float LEG_L2 = 0.1140f;

        Leg(
            position_t leg,
            std::unique_ptr<Servo> servo_0,
            std::unique_ptr<Servo> servo_1,
            std::unique_ptr<Servo> servo_2
        ) : leg(leg), servo_0(std::move(servo_0)), servo_1(std::move(servo_1)), servo_2(std::move(servo_2)) {};

        void enable() const;
        void disable() const;
        void update_target(const glm::vec3& target_r);
        void step_angle();

    private:
        position_t leg;
        std::unique_ptr<Servo> servo_0;     // order is: hip sideways, hip forward/back, knee
        std::unique_ptr<Servo> servo_1;
        std::unique_ptr<Servo> servo_2;

        glm::vec3 target_joint_angle;       // same order as above
        // glm::vec3 cur_joint_angle;
        // glm::vec3 cur_angular_speed;

        void _ik_BL(glm::vec3 r);
        void _ik_BR(glm::vec3 r);
        void _ik_FL(glm::vec3 r);
        void _ik_FR(glm::vec3 r);
};

// Constants

#define LEG_L   1.f

// Function prototypes

