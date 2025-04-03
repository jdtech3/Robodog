#pragma once

#include <cmath>
#include <utility>
#include <limits>

#include "stm32f4xx_hal.h"

#include "util/logging.h"
#include "util/util.hpp"

class Servo {
    public:
        typedef struct limits {
            float pwm_min_us;
            float pwm_max_us;
            float min_angle;
            float max_angle;
            float abs_max_angle;
        } limits_t;

        typedef enum preset_pos {
            MIN,
            MAX,
            CENTER,
        } preset_pos_t;

        TIM_HandleTypeDef *tim;
        uint32_t tim_ch;
        limits_t limits;
        bool flip;

        uint32_t TIM_US_PER_CYCLE = 0;

        Servo(TIM_HandleTypeDef *_tim, uint32_t _ch, limits_t _limits, bool _flip);

        void disable() const;

        void set_angle(float angle) const;
        void set_angle(preset_pos_t pos) const;

    private:
        void _set_width(float width_us) const;
};
