#include "debug.h"
#include "hal_stm32.h"

void debug_putchar(char ch)
{
    if (ch == '\n') HAL_UART_Transmit(gHAL->huart_dbg, (uint8_t *)"\r", 1, HAL_MAX_DELAY);   // LF -> CR LF
    HAL_UART_Transmit(gHAL->huart_dbg, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
}

void SR_DEBUG(const char *str)
{
    const char *str_ptr = str;
    while(*str_ptr != 0) {
        debug_putchar(*str_ptr);
        str_ptr++;
    }
}

int pm_isdigit(char ch)
{
    return ('0' <= ch) && (ch <= '9');
}

int pm_atoi(const char *s)
{
    int result = 0;
    int sign = 1;
    if(s) {
        if(*s == '-') {
            sign = -1;
            s++;
        }
        while(*s) {
            if (!pm_isdigit(*s)) break;
            result = result * 10 + (*s++ - '0');
        }
    }
    return sign*result;
}