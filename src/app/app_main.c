#include "app_main.h"

motor_status_t motor_data;
motor_status_t *motor_pt = &motor_data;

/**
 * @brief watchdog_pat
 * 
 * pres prescaler IWDG_PRESCALER_32 (IWDG_PRESCALER_4 to IWDG_PRESCALER_256)
 * IWDG_RLR (12-bit value, 0 to 4095)
 *
 * T(iwdg) = T(lsi)*pres*(rlr+1)
 * 
 * LSI Clock (BOSCH_L071)37KHz => T(lsi) = 1/37000 = 27.03us
 * T(iwdg) = T(lsi) * (4*2exp(IWDG_PRESCALER_32)) * (rlr + 1) = 1/37000 * 32 * 4096 = 3.542s
 * 
 */
void watchdog_pat(void)
{
    if(gHAL->iwdg)
        HAL_IWDG_Refresh(gHAL->iwdg);
}

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
    HAL_GPIO_WritePin(MOTOR_STEP_GPIO_Port, MOTOR_STEP_Pin, status);
    HAL_GPIO_WritePin(MOTOR_STEP_TEST_GPIO_Port, MOTOR_STEP_TEST_Pin, status);

}

motor_step_type read_motor_step_pin(void)
{
    motor_step_type status = (motor_step_type) HAL_GPIO_ReadPin(MOTOR_STEP_TEST_GPIO_Port, MOTOR_STEP_TEST_Pin);
    return status;
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
 * set the timer half of one step period
 * 
 */

void config_motor(unsigned int RPM_NUM, motor_dir_type dir, unsigned int step)
{

    // set direction
    if(motor_pt->dir != dir) {
        motor_pt->dir = dir;
        set_direction(motor_pt->dir);
    }

    // set steps
    motor_pt->step = step;
    if(motor_pt->step > MAX_STEPS_SUPPORT)
        motor_pt->step = MAX_STEPS_SUPPORT;


    // set step period
    unsigned int rpmnum = RPM_NUM;
    if(rpmnum >= MAX_RPM_NUM_SUPPORT) {
        rpmnum = MAX_RPM_NUM_SUPPORT;
    } else if(rpmnum < 1) {
        rpmnum = 1;
    }
    if(motor_pt->rpm != rpmnum) {
        motor_pt->rpm = rpmnum;
        if(motor_pt->timer_is_running) {
            HAL_TIM_Base_DeInit(gHAL->step_timer);
            motor_pt->timer_is_running = false;
        }
    }

    if(!motor_pt->timer_is_running) {
        // motor_pt->step_timer_period = 3000/rpmnum * 1000;
        motor_pt->step_timer_halfperiod = (3000/motor_pt->rpm * 1000) / 2;
        step_timer_init();
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
    motor_pt->step_flag = MOTOR_STEP_DEFAULT;
    motor_pt->dir = CLOCKWISE;
    // motor_pt->step_timer_period = (3000/MAX_RPM_NUM_SUPPORT) * 1000;
    motor_pt->step_timer_halfperiod = ((3000/MAX_RPM_NUM_SUPPORT) * 1000)/ 2;
    set_motor_enabled(motor_pt->enable);
    set_direction(motor_pt->dir);
    motor_pt->timer_is_running = false;

}

/**
 * @brief motor_start
 * 
 */
void motor_start(void)
{
    if(motor_pt->enable != MOTOR_ENABLE) {
        motor_pt->enable = MOTOR_ENABLE;
        set_motor_enabled(motor_pt->enable);
        // start timer
        step_timer_start();
    }
}

/**
 * @brief motor_stop
 * 
 */
void motor_stop(void)
{
    if(motor_pt->enable != MOTOR_DISABLE) {
        motor_pt->enable = MOTOR_DISABLE;
        set_motor_enabled(motor_pt->enable);
    }
    if(motor_pt->step_flag != MOTOR_STEP_DEFAULT || read_motor_step_pin() != MOTOR_STEP_DEFAULT) {
        motor_pt->step_flag = MOTOR_STEP_DEFAULT;
        set_motor_step_pin(motor_pt->step_flag);
    }
    step_timer_stop();
    motor_pt->step = 0;
}

/**
 * @brief motor_force_stop
 * 
 */
void motor_force_stop(void)
{
    motor_stop();
}

/**
 * @brief motor is running
 * 
 */
void motor_run(void)
{
    if(motor_pt->step > 0) {
        if(motor_pt->step_flag != read_motor_step_pin()) {
            set_motor_step_pin(motor_pt->step_flag);
            if(motor_pt->step_flag == MOTOR_STEP_DEFAULT)
                motor_pt->step--;
        }
    } else {
        motor_stop();
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
    app_comm_init();
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
    watchdog_pat();

    /*
    if(fT100ms)
        led_toggle_test();
    */

    if(motor_pt->timer_is_running)
        motor_run();

    app_comm_sm();

}