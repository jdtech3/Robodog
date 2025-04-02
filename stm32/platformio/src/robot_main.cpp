#include "robot_main_c.h"
#include "robot_main.hpp"


void set_pwm_val(TIM_HandleTypeDef *tim, int width_us, uint32_t channel) {
    HAL_RCC_GetPCLK1Freq() * 2;     // TIMx clocks are always 2x PCLK

    TIM_OC_InitTypeDef sConfigOC = {0};

    uint16_t width_cycles = width_us / PWM_TIM_US_PER_CYCLE;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = width_cycles;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(tim, &sConfigOC, channel);
    HAL_TIM_PWM_Start(tim, channel);
}

void robot_entrypoint(
    TIM_HandleTypeDef *tim_BL,
    TIM_HandleTypeDef *tim_BR,
    TIM_HandleTypeDef *tim_FL,
    TIM_HandleTypeDef *tim_FR,
    UART_HandleTypeDef *uart
) {
    Robodog dog(tim_BL, tim_BR, tim_FL, tim_FR, uart);
    dog.run();
}

Robodog::Robodog(TIM_HandleTypeDef *tim_BL, TIM_HandleTypeDef *tim_BR, TIM_HandleTypeDef *tim_FL, TIM_HandleTypeDef *tim_FR, UART_HandleTypeDef *uart) {
    this->tim_BL = tim_BL;
    this->tim_BR = tim_BR;
    this->tim_FL = tim_FL;
    this->tim_FR = tim_FR;

    this->uart = uart;
}

void Robodog::run() {
    while (!exit) {
        this->tick();

        
        HAL_Delay(100);     // replace this with smarter delay logic
    }
}

// void init_robot() {
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_1);
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_2);
//     HAL_TIM_PWM_Start(tim_BL, TIM_CHANNEL_3);
// }

void run_robot() {
    while (true) {
        glm::vec3 one(1.f, 1.f, 1.f);

        state.target_up += one;
        printf("up: %.4f %.4f %.4f", state.target_up.x, state.target_up.y, state.target_up.z);

        HAL_Delay(100);
    }
}
