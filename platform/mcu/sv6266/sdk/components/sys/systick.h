#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>
#include "attrs.h"

void systick_set_period(uint32_t period);
void systick_start(void);
void systick_stop(void);
uint32_t systick_get_period();
uint32_t systick_get_timer_period();

uint32_t systick_remain_count(void);

#endif
