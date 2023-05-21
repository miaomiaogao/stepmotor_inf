#include "comm.h"
#include "app_main.h"
#include <string.h>


#define LINE_BUFFER_SIZE 128

uart_def(uart_dbg, gHAL->huart_dbg, NULL, LINE_BUFFER_SIZE, LINE_BUFFER_SIZE*2);
;

typedef struct {
    char linebuffer[LINE_BUFFER_SIZE];
    int pos;
    bool ready;
    char cache[LINE_BUFFER_SIZE+3];
    motor_dir_type dir;
    unsigned int rpm;
    unsigned int step;

} comm_data_t;

comm_data_t comm_data;
comm_data_t *comm_pt = &comm_data;

/**
 * @brief app_comm_init
 * 
 */
void app_comm_init(void)
{
    uart_init(uart_dbg, true);
}

/**
 * @brief app_comm_rx
 * 
 */
void app_comm_rx(void)
{
    while(uart_is_rx_available(uart_dbg)) {
        char ch = uart_getc(uart_dbg);
        debug_putchar(ch);
        if(ch == '\r') {
            comm_pt->linebuffer[comm_pt->pos] = 0x0;
            size_t len = strlen(comm_pt->linebuffer);
            if(len > 0)
                comm_pt->ready = true;
            comm_pt->pos = 0;
        } else if(ch != '\n') {
            comm_pt->linebuffer[comm_pt->pos++] = ch;
            if(comm_pt->pos > LINE_BUFFER_SIZE)
                comm_pt->pos = 0;
        }

    }
}

/**
 * @brief Construct a new parameter process object
 * 
 */
void parameter_process(char *key, char *value)
{
    if(strncmp(key, "dir", strlen("dir"))==0 && value) {
        if(strncmp(value, "clockwise", strlen("clockwise"))== 0) {
            comm_pt->dir = CLOCKWISE;
        } else {
            comm_pt->dir = COUNTERCLOCKWISE;
        }
    } else if(strncmp(key, "rpm", strlen("npm"))==0 && value) {
        comm_pt->rpm = pm_atoi(value);
    } else if(strncmp(key, "step", strlen("step"))==0 && value) {
        comm_pt->step = pm_atoi(value);
    }
}

/**
 * @brief comm_motor_parameter_process
 * "start:(dir=clockwise,rpm=600,step=20)"
 * dir includes: "clockwise" and "counterclockwise"
 * "stop:(ok)"
 */
void comm_motor_parameter_process(void)
{
    size_t len = strlen(comm_pt->cache);
    if(strncmp(comm_pt->cache, "start:(", strlen("start:(")) == 0) { // start command
        SR_DEBUG("start cmd");
        char *key = NULL;
        char *value = NULL;
        for(int i = strlen("start:("); i < len; i++) {
            char *cp = comm_pt->cache + i;
            if(!key && *cp != ' ') // start key
                key = cp;
            if(*cp == '=') {
                *cp = 0;
                value = cp + 1;
            }
            if(*cp == ')' || *cp == ',') {
                char cp_tmp = *cp;
                *cp = 0;
                if(key)
                    parameter_process(key, value);
                key = NULL;
                if(cp_tmp == ')') {
                    config_motor(comm_pt->rpm, comm_pt->dir, comm_pt->step);
                    motor_start();
                    break;
                }
            }
        }
        comm_pt->ready = false;
    } else if(strncmp(comm_pt->cache, "stop:(", strlen("stop:(")) == 0) { // stop command
        SR_DEBUG("stop cmd");
        motor_force_stop();
    } else { // ignore the command
        SR_DEBUG("invalid command ignored...");
    }

}

/**
 * @brief comm_receiver_process
 * 
 */
void comm_receiver_process(void)
{
    if(comm_pt->ready) {
        comm_pt->ready = false;
        unsigned int len = strlen(comm_pt->linebuffer);
        memcpy(comm_pt->cache, comm_pt->linebuffer, len);
        comm_pt->cache[len] = '\r';
        comm_pt->cache[len+1] = '\n';
        comm_pt->cache[len+2] = 0;
        SR_DEBUG(comm_pt->cache);
        comm_motor_parameter_process();
    }
}

/**
 * @brief app_comm_sm
 * 
 */
void app_comm_sm(void)
{
    app_comm_rx();
    comm_receiver_process();
}