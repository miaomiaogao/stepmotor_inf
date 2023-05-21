#include "timer.h"
#include <common/hal_stm32.h>

void USER_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    motor_pt->step_flag = !motor_pt->step_flag;

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

    /** Note:
     * TIMx->ARR only support 16 bits, even the definition in the source code is uint32_t
     * => If motor_pt->step_timer_halfperiod > 65535 then modify the prescaler and period to fit for the bits
     * 
    */
    uint32_t prescaler, period, times_temp;

    if(motor_pt->step_timer_halfperiod > 65535) {
        times_temp = (motor_pt->step_timer_halfperiod >> 16) + 1; 
        if(times_temp <= 0) times_temp = 1; // just in case
        prescaler = (SystemCoreClock/1000000) * times_temp - 1;
        period = motor_pt->step_timer_halfperiod / times_temp;
    } else {
        prescaler = (SystemCoreClock/1000000) - 1;
        period = motor_pt->step_timer_halfperiod;
    }

    gHAL->step_timer->Instance = TIM3;
    gHAL->step_timer->Init.Prescaler = prescaler;
    gHAL->step_timer->Init.CounterMode = TIM_COUNTERMODE_UP;
    gHAL->step_timer->Init.Period = period;
    gHAL->step_timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    gHAL->step_timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    
    __HAL_TIM_SET_COUNTER(gHAL->step_timer, 0);

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


void step_timer_stop(void)
{
    HAL_TIM_Base_Stop_IT(gHAL->step_timer);
}

void step_timer_start(void)
{
    HAL_TIM_Base_Start_IT(gHAL->step_timer);
}