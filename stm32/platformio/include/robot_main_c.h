#pragma once

#include "main.h"       // upstream

#ifdef __cplusplus
extern "C" {
#endif

void robot_entrypoint(TIM_HandleTypeDef *_timer, UART_HandleTypeDef *_uart);

#ifdef __cplusplus
}
#endif
