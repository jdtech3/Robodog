#include "dog/servo.hpp"

// Magic values

constexpr static uint32_t US_PER_S =                    1000000;
constexpr static uint8_t PCLK_TO_TIMCLK_MULTIPLIER =    2;
constexpr static float EPSILON =                        0.001;

// Constructor

Servo::Servo(TIM_HandleTypeDef *_tim, uint32_t _ch, limits_t _limits, bool _flip) : tim(_tim), tim_ch(_ch), limits(_limits), flip(_flip) {
    TIM_US_PER_CYCLE = US_PER_S / (HAL_RCC_GetPCLK1Freq() * PCLK_TO_TIMCLK_MULTIPLIER / tim->Instance->PSC);

    if (limits.pwm_min_us == 0.f) limits.pwm_min_us = 500.f;
    if (limits.pwm_max_us == 0.f) limits.pwm_max_us = 2500.f;
    if (limits.abs_max_angle == 0.f) limits.abs_max_angle = 270._deg;

    LOG_DEBUG(
        "servo", "init OK: tim: 0x%x, ch: 0x%x, us per cycle: %d, pwm limits: [%.2f, %.2f], angle limits: [%.2f, %.2f]",
        &tim->Instance, tim_ch, TIM_US_PER_CYCLE,
        limits.pwm_min_us, limits.pwm_max_us, limits.min_angle, limits.max_angle
    );
};

// Private

void Servo::_set_width(float width_us) const {
    if (width_us != 0 && (width_us < limits.pwm_min_us-EPSILON || width_us > limits.pwm_max_us+EPSILON)) {
        LOG_WARN("servo", "ignoring width %.2f us, out of limits!", width_us);
        return;
    }

    TIM_OC_InitTypeDef sConfigOC = { 0 };

    uint16_t width_cycles = width_us / TIM_US_PER_CYCLE;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = width_cycles;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(tim, &sConfigOC, tim_ch);
    HAL_TIM_PWM_Start(tim, tim_ch);
    LOG_DEBUG("servo", "set width %.2f us (%d cycles, %.2f deg)", width_us, width_cycles,
        lerp(0.f, 270.f, (width_us-limits.pwm_min_us)/(limits.pwm_max_us-limits.pwm_min_us)));
}

// Public

void Servo::disable() const {
    _set_width(0);
    HAL_TIM_PWM_Stop(tim, tim_ch);
}

/**
 * @brief Send angle command to servo
 *
 * @param angle raw angle in radians
 */
void Servo::set_angle(float angle) const {
    if (flip) angle = limits.abs_max_angle - angle;

    if (angle < limits.min_angle-EPSILON || angle > limits.max_angle+EPSILON) {
        LOG_WARN("servo", "ignoring angle %.6f deg, out of limits!", rad2deg(angle));
        return;
    }

    _set_width(lerp(limits.pwm_min_us, limits.pwm_max_us, angle / limits.abs_max_angle));
}

/**
 * @brief Shortcut for common servo positions
 *
 * @param pos servo position
 */
void Servo::set_angle(preset_pos_t pos) const {
    switch (pos) {
        case MIN: set_angle(limits.min_angle); break;
        case MAX: set_angle(limits.max_angle); break;
        case CENTER: set_angle(lerp(limits.min_angle, limits.max_angle, 0.5f)); break;
    }
}
