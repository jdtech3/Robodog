#include "robot_main.h"


// Globals
TIM_HandleTypeDef *tim_BL;      // back left
TIM_HandleTypeDef *tim_BR;      // back right
TIM_HandleTypeDef *tim_FL;      // front left
TIM_HandleTypeDef *tim_FR;      // front right
UART_HandleTypeDef *uart;

global_state_t state;

void set_pwm_val(TIM_HandleTypeDef *tim, int width_us, uint32_t channel) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    uint16_t width_cycles = width_us / PWM_TIM_US_PER_CYCLE;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = width_cycles;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(tim, &sConfigOC, channel);
    HAL_TIM_PWM_Start(tim, channel);
}

void init_robot() {

}

void run_robot() {
    while (true) {
        vec3 one;
        glm_vec3_one(one);

        glm_vec3_add(one, state.target_up, state.target_up);
        printf("up: %.4f %.4f %.4f", state.target_up[0], state.target_up[1], state.target_up[2]);

        HAL_Delay(100);
    }
}
