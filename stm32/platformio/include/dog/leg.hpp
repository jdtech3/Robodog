#pragma once

#include <memory>
#include <utility>

#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"

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

        Leg(
            position_t leg,
            std::unique_ptr<Servo> servo_0,
            std::unique_ptr<Servo> servo_1,
            std::unique_ptr<Servo> servo_2
        ) : leg(leg), servo_0(std::move(servo_0)), servo_1(std::move(servo_1)), servo_2(std::move(servo_2)) {};

        void enable() const;
        void disable() const;
        void update_target(glm::vec3 target_r);
        void step_angle();

    private:
        position_t leg;
        std::unique_ptr<Servo> servo_0;     // order is: hip sideways, hip forward/back, knee
        std::unique_ptr<Servo> servo_1;
        std::unique_ptr<Servo> servo_2;

        glm::vec3 target_joint_angle;       // same order as above
        // glm::vec3 cur_joint_angle;
        // glm::vec3 cur_angular_speed;
};

// Constants

#define LEG_L   1.f

// Function prototypes

