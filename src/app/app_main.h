#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "common/hal_stm32.h"
#include "common/interval.h"
#include "common/debug.h"
#include "timer.h"
#include <stdbool.h>


#define MAX_RPM_NUM             600


typedef enum {
    CLOCKWISE = GPIO_PIN_SET,
    COUNTERCLOCKWISE = GPIO_PIN_RESET,
} motor_dir_type;

typedef enum {
    MOTOR_ENABLE = GPIO_PIN_RESET,
    MOTOR_DISABLE = GPIO_PIN_SET,
} motor_enable_type;

typedef enum {
    MOTOR_STEP_HIGH = GPIO_PIN_SET,
    MOTOR_STEP_LOW = GPIO_PIN_RESET,
} motor_step_type;


typedef struct {
    // unsigned int step_timer_period;
    unsigned int step_timer_halfperiod;
    motor_step_type step_flag;
    motor_enable_type enable;
    bool timer_is_running;

} motor_status_t;
extern motor_status_t *motor_pt;


void app_init(void);
void app_run(void);
HAL_StatusTypeDef USER_TIM3_Init(void);


#endif