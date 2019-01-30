/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __ADAPTER_H__
#define __ADAPTER_H__

#include "ulog/ulog.h"
#include "aos/hal/can.h"
#include "aos/hal/timer.h"
#include "applicfg.h"
#include "data.h"

#define WAIT_FOREVER 0xFFFFFFFFU
#define MAX_COUNTER_VALUE 0xFFFF

extern uint32_t hal_timer_getcounter(timer_dev_t *tim);
extern void hal_timer_setcounter(timer_dev_t *tim, uint32_t counter);

int32_t timer_init(uint8_t port);
int32_t can_init(uint8_t port, uint32_t baud, CO_Data * d);

#endif    /* __ADAPTER_H__ */

