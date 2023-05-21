#include "app_main.h"

typedef struct {
    unsigned int step_timer_period;
    motor_step_type step_flag;
    motor_enable_type enable;
    bool timer_is_running;

} motor_status_t;

motor_status_t motor_data;
motor_status_t *motor_pt = &motor_data;


/**
 * @brief Set the direction object
 * 
 * @param dir 
 * * Two directions are: CLOCKWISE and COUNTER_CLOCKWISE
 * 
 */
void set_direction(motor_dir_type dir)
{
    HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, dir);
}

motor_dir_type read_motor_dir_pin(void)
{
    motor_dir_type status = (motor_dir_type) HAL_GPIO_ReadPin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin);
    return status;
}


/**
 * @brief Set the enable object
 * * 
 * @param en 
 * * MOTOR_ENABLE and MOTOR_DISABLE
 * 
 */
void set_motor_enabled(motor_enable_type en)
{
    HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port, MOTOR_ENABLE_Pin, en);
}

/**
 * @brief set_motor_step
 * MOTOR_STEP_HIGH and MOTOR_STEP_LOW
 * 
 */
void set_motor_step_pin(motor_step_type status)
{
    HAL_GPIO_WritePin(MOTOR_STEP_TEST_GPIO_Port, MOTOR_STEP_TEST_Pin, status);
}

motor_step_type read_motor_step_pin(void)
{
    motor_step_type status = (motor_step_type) HAL_GPIO_ReadPin(MOTOR_STEP_TEST_GPIO_Port, MOTOR_STEP_TEST_Pin);
    return status;
}

HAL_StatusTypeDef USER_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

    gHAL->step_timer->Instance = TIM3;
    gHAL->step_timer->Init.Prescaler = (SystemCoreClock/1000000)-1;
    gHAL->step_timer->Init.CounterMode = TIM_COUNTERMODE_UP;
    gHAL->step_timer->Init.Period = motor_pt->step_timer_period * 1000 - 1;
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
    return HAL_OK;

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == gHAL->step_timer->Instance) {
        motor_pt->step_flag = MOTOR_STEP_HIGH;
    }
}

void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == gHAL->step_timer->Instance) {
        motor_pt->step_flag = MOTOR_STEP_LOW;
    }
}

void led_toggle_test(void)
{
    HAL_GPIO_TogglePin(MOTOR_STEP_GPIO_Port, MOTOR_STEP_Pin); // ACT LED 
    HAL_GPIO_TogglePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin); // LNK LED
    HAL_GPIO_TogglePin(MOTOR_ENABLE_GPIO_Port, MOTOR_ENABLE_Pin); // HB LED
}

/**
 * @brief config_motor_step
 * The motor is 18 degrees per step (20 steps per revolution)
 * Constraints: The stepper motor cannot do more than 600 RPM
 * => 600 RPM  -> 10 RPS -> at least 100ms for one revolution -> 5ms one step
 * => As uTick is 10 milliseconds, which is slower than the requirement, a timer is needed here 
 * 
 * => one step period = (60 * 1000ms) / (PRM_NUM * 20) = 3000/PRM_NUM
 * 
 */

void config_motor(unsigned int RPM_NUM, motor_dir_type dir)
{
    unsigned int rpmnum = RPM_NUM;
    if(rpmnum >= MAX_RPM_NUM) {
        rpmnum = MAX_RPM_NUM;
    } else if(rpmnum < 1) {
        rpmnum = 1;
    }
    // set direction
    if(dir != read_motor_dir_pin())
        set_direction(dir);
    // set step period
    if(!motor_pt->timer_is_running) {
        motor_pt->step_timer_period = 3000/rpmnum;
        USER_TIM3_Init();
        // start timer
        HAL_TIM_Base_Start_IT(gHAL->step_timer);
        motor_pt->timer_is_running = true;
    }
}

/**
 * @brief motor_init
 * 
 */
void motor_init(void)
{
    motor_pt->enable = MOTOR_DISABLE;
    motor_pt->step_flag = 0;
    motor_pt->step_timer_period = 3000/MAX_RPM_NUM;
    set_motor_enabled(motor_pt->enable);
    set_direction(CLOCKWISE);
    motor_pt->timer_is_running = false;

}

/**
 * @brief motor_start
 * 
 */
void motor_run(void)
{
    if(motor_pt->enable != MOTOR_ENABLE) {
        motor_pt->enable = MOTOR_ENABLE;
        set_motor_enabled(motor_pt->enable);
    }
    if(motor_pt->step_flag != read_motor_step_pin()) {
        set_motor_step_pin(motor_pt->step_flag);
    }
}

/**
 * @brief app_init
 * 
 */
void app_init(void)
{
    interval_init();
    motor_init();

    SR_DEBUG(" TEST STEP MOTOR ");

}

/**
 * @brief app_run 
 * noted: This function is running inside while(1) loop
 * 
 */
void app_run(void)
{
    interval_step();

    /*
    if(fT100ms)
        led_toggle_test();
    */

    config_motor(MAX_RPM_NUM, CLOCKWISE);

    motor_run();

}