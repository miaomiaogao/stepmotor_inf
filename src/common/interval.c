#include "interval.h"
#include <stdint.h>
#include <string.h>

const unsigned char interval_steps[INTERVAL_STEPCOUNT] =  INTERVAL_STEPS;

uint32_t interval_lasttick = 0;

unsigned char interval_flags = 0;
unsigned char interval_counter[INTERVAL_STEPCOUNT] =       { 0, 0, 0 };



void interval_init(void)
{
    interval_flags = 0;
    memset(&interval_counter, 0, INTERVAL_STEPCOUNT);
}

void interval_fini(void)
{

}

void interval_step(void)
{
    interval_flags = 0;

    // Platform Specific  -----------------
    uint32_t diff = HAL_GetTick()  - interval_lasttick;
    // ------------------------------------

    if (diff >= INTERVAL_BASETICKS) {
        // Platform Specific  -----------------
        interval_lasttick = HAL_GetTick();
        // ------------------------------------


        interval_flags = tiBase;
        for (int i = 0; i < INTERVAL_STEPCOUNT; i++) {
            interval_counter[i]++;
            if (interval_counter[i] >= interval_steps[i]) {
                interval_counter[i] = 0;
                interval_flags |= 1 << (i + 1);
            } else break;
        }

    }
}
