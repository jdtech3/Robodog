#pragma once

#include <cmath>
#include <utility>

#include "stm32f4xx_hal.h"

class Motor {
    typedef struct limits {
        int pwm_min_us;
        int pwm_max_us;
        float min_angle;
        float max_angle;
    } limits_t;

    public:
        Motor(TIM_HandleTypeDef *tim, uint32_t ch, limits_t limits);
        void set_width(int width_us);
        void set_angle(float angle);
};
