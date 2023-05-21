#include "timer.h"
#include <common/hal_stm32.h>

void USER_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_Base_Stop_IT(htim);

    motor_pt->step_flag = !motor_pt->step_flag;
    HAL_TIM_Base_Start_IT(htim);
}

/**
 * @brief 
 * Note: half periold callback is not reachable, need to do investigation of the timer register later.
 * 
 * @param htim 
 */
void USER_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    // motor_pt->step_flag = MOTOR_STEP_LOW;
}

void timer_register_internal_callbacks(TIM_HandleTypeDef *htim)
{
#if USE_HAL_TIM_REGISTER_CALLBACKS
    HAL_TIM_RegisterCallback(htim, HAL_TIM_PERIOD_ELAPSED_CB_ID, USER_TIM_PeriodElapsedCallback);
    HAL_TIM_RegisterCallback(htim, HAL_TIM_PERIOD_ELAPSED_HALF_CB_ID, USER_TIM_PeriodElapsedHalfCpltCallback);

#else
#error USE_HAL_TIM_REGISTER_CALLBACKS must be 1
#endif
}

HAL_StatusTypeDef step_timer_init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

    gHAL->step_timer->Instance = TIM3;
    gHAL->step_timer->Init.Prescaler = (SystemCoreClock/1000000)-1;
    gHAL->step_timer->Init.CounterMode = TIM_COUNTERMODE_UP;
    gHAL->step_timer->Init.Period = motor_pt->step_timer_halfperiod - 1;
    gHAL->step_timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    gHAL->step_timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(gHAL->step_timer) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(gHAL->step_timer, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(gHAL->step_timer, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    timer_register_internal_callbacks(gHAL->step_timer);

    return HAL_OK;

}




void step_timer_start(void)
{
    HAL_TIM_Base_Start_IT(gHAL->step_timer);
}