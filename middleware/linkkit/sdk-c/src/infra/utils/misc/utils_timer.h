/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _IOTX_COMMON_TIMER_H_
#define _IOTX_COMMON_TIMER_H_

#include "iot_import.h"

typedef struct {
    uint64_t time;
} iotx_time_t;


void iotx_time_start(iotx_time_t *timer);

uint64_t utils_time_spend(iotx_time_t *start);

uint64_t iotx_time_left(iotx_time_t *end);

int utils_time_is_expired(iotx_time_t *timer);

void iotx_time_init(iotx_time_t *timer);

void utils_time_countdown_ms(iotx_time_t *timer, uint64_t millisecond);

uint64_t utils_time_get_ms(void);

#endif /* _IOTX_COMMON_TIMER_H_ */
