#ifndef __INTERVAL_H__
#define __INTERVAL_H__

#include "common/hal_stm32.h"
typedef enum
{
    ti10ms  = 1,                    // x1
    ti100ms = 2,                    // x10
    ti1sec  = 4,                    // x10x10
    ti1min  = 8,                    // x10x10x60
    tiBase  = ti10ms,
} ETimeInterval;

// As of 2021
// PM54(10ms) + 1sec + 1min
// PM45(10ms) + 100ms + 1sec + 1min

#define INTERVAL_BASETICKS      10
#define INTERVAL_STEPCOUNT      3
#define INTERVAL_STEPS          { 10, 10, 60 }              // base=10ms + steps(100ms, 1sec, 1min)

extern unsigned char interval_counter[INTERVAL_STEPCOUNT];

extern unsigned char interval_flags;

#define INTERVAL_IS(x)          (interval_flags & (x))

// Backward Compatibility
#define fT10ms                  INTERVAL_IS(ti10ms)
#define fT100ms                 INTERVAL_IS(ti100ms)
#define fT1sec                  INTERVAL_IS(ti1sec)
#define fT1min                  INTERVAL_IS(ti1min)

void interval_init(void);
void interval_fini(void);
void interval_step(void);

#endif
