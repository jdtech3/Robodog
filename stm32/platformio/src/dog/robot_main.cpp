#include "dog/robot_main_c.h"
#include "dog/robot_main.hpp"


// const static run_mode_t run_mode = CALIBRATION;
// const static run_mode_t run_mode = PRESET;
const static run_mode_t run_mode = EXT_INPUT;

void robot_entrypoint() {
    SET_LED_GREEN(true);

    if (run_mode == CALIBRATION) {
        LOG_INFO("robodog", "mode: CALIBRATION");

        /*
            1. set to center
            2. put in servo horn screw
            3. sweep non-flipped leg with angles +/- from center to find min and max
            4. flipped leg limits are shift up/down until 270-max/min works (TODO: closed form formula)
        */

        // start with: (Servo::limits_t){0.f, 0.f, 0._deg, 270._deg, 0.f},

        Servo servo_BL2(
            &htim3,
            TIM_CHANNEL_3,
            (Servo::limits_t){0.f, 0.f, 62._deg, 205._deg, 0.f},
            false
        );
        Servo servo_BR2(
            &htim3,
            TIM_CHANNEL_4,
            (Servo::limits_t){0.f, 0.f, 63.5_deg, 206.5_deg, 0.f},
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
            (Servo::limits_t){0.f, 0.f, 80._deg, 190._deg, 0.f},
            true
        );

        servo_BL1.set_angle(Servo::preset_pos::CENTER);
        servo_BR1.set_angle(Servo::preset_pos::CENTER);

        Servo servo_FL2(
            &htim2,
            TIM_CHANNEL_3,
            (Servo::limits_t){0.f, 0.f, 65._deg, 205._deg, 0.f},
            true
        );
        Servo servo_FR2(
            &htim2,
            TIM_CHANNEL_4,
            (Servo::limits_t){0.f, 0.f, 55._deg, 195._deg, 0.f},
            false
        );

        servo_FL2.set_angle(Servo::preset_pos::CENTER);
        servo_FR2.set_angle(Servo::preset_pos::CENTER);

        Servo servo_FL1(
            &htim4,
            TIM_CHANNEL_1,
            (Servo::limits_t){0.f, 0.f, 80._deg, 190._deg, 0.f},
            true
        );
        Servo servo_FR1(
            &htim4,
            TIM_CHANNEL_2,
            (Servo::limits_t){0.f, 0.f, 85._deg, 195._deg, 0.f},
            false
        );

        servo_FL1.set_angle(Servo::preset_pos::CENTER);
        servo_FR1.set_angle(Servo::preset_pos::CENTER);

        Servo servo_BL0(
            &htim1,
            TIM_CHANNEL_1,
            (Servo::limits_t){0.f, 0.f, 105._deg, 150._deg, 0.f},
            false
        );
        Servo servo_BR0(
            &htim1,
            TIM_CHANNEL_2,
            (Servo::limits_t){0.f, 0.f, 112.5_deg, 157.5_deg, 0.f},
            true
        );
        Servo servo_FL0(
            &htim1,
            TIM_CHANNEL_3,
            (Servo::limits_t){0.f, 0.f, 112.5_deg, 157.5_deg, 0.f},
            true
        );
        Servo servo_FR0(
            &htim1,
            TIM_CHANNEL_4,
            (Servo::limits_t){0.f, 0.f, 105._deg, 150._deg, 0.f},
            false
        );

        servo_BL0.set_angle(Servo::preset_pos::CENTER);
        servo_BR0.set_angle(Servo::preset_pos::CENTER);
        servo_FL0.set_angle(Servo::preset_pos::CENTER);
        servo_FR0.set_angle(Servo::preset_pos::CENTER);

        // while (true) {
        //     for (float a = 85._deg; a < 195._deg; a += 1._deg) {
        //         LOG_INFO("calibrate", "setting angle %.2f deg", rad2deg(a));
        //         servo_FL1.set_angle(a);
        //         servo_FR1.set_angle(a);
        //         HAL_Delay(50);
        //     }
        //     for (float a = 195._deg; a > 85._deg; a -= 1._deg) {
        //         LOG_INFO("calibrate", "setting angle %.2f deg", rad2deg(a));
        //         servo_FL1.set_angle(a);
        //         servo_FR1.set_angle(a);
        //         HAL_Delay(50);
        //     }
        // }

    }

    else {

        Robodog dog(
            std::make_unique<Leg>(
                Leg::position::BACK_LEFT,
                std::make_unique<Servo>(        // order: hip sideways, hip forward/back, knee
                    &htim1,
                    TIM_CHANNEL_1,
                    (Servo::limits_t){0.f, 0.f, 105._deg, 150._deg, 0.f},
                    false
                ),
                std::make_unique<Servo>(
                    &htim2,
                    TIM_CHANNEL_1,
                    (Servo::limits_t){0.f, 0.f, 85._deg, 195._deg, 0.f},
                    false
                ),
                std::make_unique<Servo>(
                    &htim3,
                    TIM_CHANNEL_3,
                    (Servo::limits_t){0.f, 0.f, 62._deg, 205._deg, 0.f},
                    false
                )
            ),
            std::make_unique<Leg>(
                Leg::position::BACK_RIGHT,
                std::make_unique<Servo>(
                    &htim1,
                    TIM_CHANNEL_2,
                    (Servo::limits_t){0.f, 0.f, 112.5_deg, 157.5_deg, 0.f},
                    true
                ),
                std::make_unique<Servo>(
                    &htim2,
                    TIM_CHANNEL_2,
                    (Servo::limits_t){0.f, 0.f, 80._deg, 190._deg, 0.f},
                    true
                ),
                std::make_unique<Servo>(
                    &htim3,
                    TIM_CHANNEL_4,
                    (Servo::limits_t){0.f, 0.f, 63.5_deg, 206.5_deg, 0.f},
                    true
                )
            ),
            std::make_unique<Leg>(
                Leg::position::FRONT_LEFT,
                std::make_unique<Servo>(
                    &htim1,
                    TIM_CHANNEL_3,
                    (Servo::limits_t){0.f, 0.f, 112.5_deg, 157.5_deg, 0.f},
                    true
                ),
                std::make_unique<Servo>(
                    &htim4,
                    TIM_CHANNEL_1,
                    (Servo::limits_t){0.f, 0.f, 80._deg, 190._deg, 0.f},
                    true
                ),
                std::make_unique<Servo>(
                    &htim2,
                    TIM_CHANNEL_3,
                    (Servo::limits_t){0.f, 0.f, 65._deg, 205._deg, 0.f},
                    true
                )
            ),
            std::make_unique<Leg>(
                Leg::position::FRONT_RIGHT,
                std::make_unique<Servo>(
                    &htim1,
                    TIM_CHANNEL_4,
                    (Servo::limits_t){0.f, 0.f, 105._deg, 150._deg, 0.f},
                    false
                ),
                std::make_unique<Servo>(
                    &htim4,
                    TIM_CHANNEL_2,
                    (Servo::limits_t){0.f, 0.f, 85._deg, 195._deg, 0.f},
                    false
                ),
                std::make_unique<Servo>(
                    &htim2,
                    TIM_CHANNEL_4,
                    (Servo::limits_t){0.f, 0.f, 55._deg, 195._deg, 0.f},
                    false
                )
            )
        );
        
        if (run_mode == PRESET) {
            LOG_INFO("robodog", "mode: PRESET");

            dog.set_target(Robodog::POS_NEUTRAL);
            dog.tick();

            SET_LED_BLUE(true);
        }

        else if (run_mode == EXT_INPUT) {
            LOG_INFO("robodog", "mode: EXT INPUT");
            
            uint8_t data = 0x00;
            while (data != 0xFF) HAL_UART_Receive(&huart4, &data, 1, HAL_MAX_DELAY);
            LOG_OK("robodog", "ESP32 ready detected, proceeding...");

            dog.run();
        }

        else {
            LOG_ERR("robodog", "no/invalid run mode, doing nothing");
            while (1);
        }
    }
}

void Robodog::run() {
    LOG_OK("robodog", "running!");

    SET_LED_BLUE(true);

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    uint32_t last_tick = HAL_GetTick();
    uint32_t last_cyc = DWT->CYCCNT;
    uint32_t i = 0;

    exit = false;
    while (!exit) {
        uint8_t dummy = 0xFF;
        HAL_UART_Transmit(&huart4, &dummy, 1, 100);

        glm::quat q;
        if (HAL_UART_Receive(&huart4, (uint8_t*)&q, sizeof(q), 100) == HAL_TIMEOUT)
            LOG_WARN("robodog", "EXT input timed out");

        glm::quat quat_buf(q.x, q.y, q.z, q.w);

        set_target(quat_buf);
        tick();

        // set_target(Robodog::POS_NEUTRAL);
        // set_target(Robodog::POS_20DEG_PITCHUP);

        uint32_t cur_ms = HAL_GetTick();
        int ms_per_iter = cur_ms - last_tick;
        last_tick = cur_ms;

        uint32_t cur_cyc = DWT->CYCCNT;
        int cyc_per_iter = cur_cyc - last_cyc;
        last_cyc = cur_cyc;

        if (i % 10 == 0) {
            TOGGLE_LED_RED();
        }

        if (i % 1000 == 0) {
            printf("\n\n\n\n\n");
            LOG_INFO("robodog", "ms per iter: %d (%.2f/s), cycles per iter: %d (%.2f/s)", ms_per_iter, 1000.f/ms_per_iter, cyc_per_iter, static_cast<float>(CLOCK_SPEED)/cyc_per_iter);
            LOG_INFO("robodog", "input quat: %.6f x, %.6f y, %.6f z, %.6f w", quat_buf.x, quat_buf.y, quat_buf.z, quat_buf.w);
        }

        i++;
    }

    LOG_OK("robodog", "exited, bye :(");
}

void Robodog::tick() {
    glm::vec3 BR_foot( BODY_X/2.f, -BODY_Y/2.f, 0);
    glm::vec3 FR_foot( BODY_X/2.f,  BODY_Y/2.f, 0);
    glm::vec3 FL_foot(-BODY_X/2.f,  BODY_Y/2.f, 0);
    glm::vec3 BL_foot(-BODY_X/2.f, -BODY_Y/2.f, 0);
    glm::vec3 body_center(0.f, 0.f, (LEG_L1+LEG_L2)/glm::sqrt(2.f));

    glm::vec3 BR_body = target * BR_foot;
    glm::vec3 FR_body = target * FR_foot;
    glm::vec3 FL_body = target * FL_foot;
    glm::vec3 BL_body = target * BL_foot;
    BR_body += body_center;
    FR_body += body_center;
    FL_body += body_center;
    BL_body += body_center;

    BR_foot += glm::vec3(+LEG_L0, (-LEG_L1+LEG_L2)/glm::sqrt(2.f), 0.f);
    FR_foot += glm::vec3(+LEG_L0, (+LEG_L1-LEG_L2)/glm::sqrt(2.f), 0.f);
    FL_foot += glm::vec3(-LEG_L0, (+LEG_L1-LEG_L2)/glm::sqrt(2.f), 0.f);
    BL_foot += glm::vec3(-LEG_L0, (-LEG_L1+LEG_L2)/glm::sqrt(2.f), 0.f);

    glm::vec3 BL_final = BL_foot - BL_body;
    glm::vec3 BR_final = BR_foot - BR_body;
    glm::vec3 FL_final = FL_foot - FL_body;
    glm::vec3 FR_final = FR_foot - FR_body;

    leg_BL->update_target(BL_final); leg_BL->step_angle();
    leg_BR->update_target(BR_final); leg_BR->step_angle();
    leg_FL->update_target(FL_final); leg_FL->step_angle();
    leg_FR->update_target(FR_final); leg_FR->step_angle();
}

void Robodog::set_target(const glm::quat& _target) {
    target = _target;
    LOG_DEBUG("robodog", "set target quat: %.4f %.4f %.4f %.4f", target.x, target.y, target.z, target.w);
}
