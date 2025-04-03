#include "dog/robot_main_c.h"
#include "dog/robot_main.hpp"

#define CALIBRATION_MODE

void robot_entrypoint() {
    #ifdef CALIBRATION_MODE
        /*
            1. set to center
            2. sweep non-flipped leg to find min and max
            3. flipped leg limits are min: center - (max - center) and max: center + (center - min)
        */

        Servo servo_BL2(
            &htim3,
            TIM_CHANNEL_3,
            (Servo::limits_t){0.f, 0.f, 62._deg, 205._deg, 0.f},
            false
        );
        Servo servo_BR2(
            &htim3,
            TIM_CHANNEL_4,
            (Servo::limits_t){0.f, 0.f, 65._deg, 208._deg, 0.f},
            true
        );

        servo_BL2.set_angle(Servo::preset_pos::CENTER);
        servo_BR2.set_angle(Servo::preset_pos::CENTER);

        Servo servo_BL1(
            &htim2,
            TIM_CHANNEL_1,
            (Servo::limits_t){0.f, 0.f, 85._deg, 195._deg, 0.f},
            false
        );
        Servo servo_BR1(
            &htim2,
            TIM_CHANNEL_2,
            (Servo::limits_t){0.f, 0.f, 75._deg, 195._deg, 0.f},
            true
        );

        servo_BL1.set_angle(Servo::preset_pos::CENTER);
        servo_BR1.set_angle(Servo::preset_pos::CENTER);

        // while (true) {
        //     for (float a = 80._deg; a < 190._deg; a += 1._deg) {
        //         servo_BL1.set_angle(a);
        //         servo_BR1.set_angle(a);
        //         HAL_Delay(50);
        //     }
        //     for (float a = 190._deg; a > 80._deg; a -= 1._deg) {
        //         servo_BL1.set_angle(a);
        //         servo_BR1.set_angle(a);
        //         HAL_Delay(50);
        //     }
        // }
    #endif

    // Robodog dog(
    //     std::make_unique<Leg>(
    //         Leg::position::BACK_LEFT,
    //         std::make_unique<Servo>(        // order: hip sideways, hip forward/back, knee
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim2,
    //             TIM_CHANNEL_1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //             false
    //         ),
    //         std::make_unique<Servo>(
    //             &htim3,
    //             TIM_CHANNEL_3,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         )
    //     ),
    //     std::make_unique<Leg>(
    //         Leg::position::BACK_RIGHT,
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim2,
    //             TIM_CHANNEL_2,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim3,
    //             TIM_CHANNEL_4,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         )
    //     ),
    //     std::make_unique<Leg>(
    //         Leg::position::FRONT_LEFT,
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         )
    //     ),
    //     std::make_unique<Leg>(
    //         Leg::position::FRONT_RIGHT,
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         ),
    //         std::make_unique<Servo>(
    //             &htim4,
    //             1,
    //             (Servo::limits_t){0.f, 0.f, 0.f, 0.f}
    //         )
    //     )
    // );

    LOG_OK("robodog", "init OK, running!");
    // dog.run();
}

void Robodog::run() {
    LOG_OK("robodog", "running!");

    exit = true;
    while (!exit) {
        this->tick();

        HAL_Delay(100);     // replace this with smarter delay logic
    }

    LOG_OK("robodog", "exited, bye :(");
}

void Robodog::tick() {
    // no op
}

// void init_robot() {
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_1);
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_2);
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_3);
// }

// void run_robot() {
//     while (true) {
//         glm::vec3 one(1.f, 1.f, 1.f);

//         state.target_up += one;
//         printf("up: %.4f %.4f %.4f", state.target_up.x, state.target_up.y, state.target_up.z);

//         HAL_Delay(100);
//     }
// }
