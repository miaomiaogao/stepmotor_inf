#ifndef __HAL_STM32_H__
#define __HAL_STM32_H__

// Pin UserLabels are in Core/Inc/main.h
//  - Note that STMCubeMX projects include "Core/Inc" search path by default
#include <main.h>

#include "stm32l0xx_hal_uart.h"
#include "stm32l0xx_it.h"
#include "stm32l071xx.h"

typedef struct {
    UART_HandleTypeDef *huart_dbg;
    TIM_HandleTypeDef *step_timer;
    IWDG_HandleTypeDef *iwdg;
} HAL_Context;

extern HAL_Context *gHAL;

#endif
