/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_TIMER_H
#define UR_TIMER_H

#include <stdint.h>

typedef void (* timer_handler_t)(void *args);
typedef void *ur_timer_t;

void ur_start_timer(ur_timer_t *timer, uint32_t dt, timer_handler_t handler, void *args);
void ur_stop_timer(ur_timer_t *timer, void *args);
uint32_t umesh_now_ms(void);
uint32_t umesh_get_timestamp(void);
void umesh_set_timestamp(uint32_t timestamp);

#endif  /* UR_TIMER_H */
