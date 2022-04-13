/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_TIMER_RAW_H__
#define __HAL_TIMER_RAW_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IGNORE_HAL_TIMER_RAW_API_CHECK
#if !(defined(ROM_BUILD) || defined(SIMU))
#error "Raw timer API can only be used in rom or simulation environment!"
#endif
#endif

#include "hal_timer.h"

#define HAL_TIMER_LOAD_DELTA            1

enum HAL_TIMER_TYPE_T {
    HAL_TIMER_TYPE_FREERUNNING = 0,
    HAL_TIMER_TYPE_ONESHOT,
    HAL_TIMER_TYPE_PERIODIC,
    HAL_TIMER_TYPE_QTY
};

typedef void (*HAL_TIMER_IRQ_HANDLER_T)(uint32_t elapsed);

void hal_timer_setup(enum HAL_TIMER_TYPE_T type, HAL_TIMER_IRQ_HANDLER_T handler);

#if 1 // conflict with alios things, need rename
void hal_timer_start_nickname(uint32_t load);
void hal_timer_stop_nickname(void);
#define hal_timer_start hal_timer_start_nickname
#define hal_timer_stop hal_timer_stop_nickname
#else
void hal_timer_start(uint32_t load);
void hal_timer_stop(void);
#endif

void hal_timer_continue(void);

int hal_timer_is_enabled(void);

void hal_timer_reload(uint32_t load);

uint32_t hal_timer_get(void);

uint32_t hal_timer_get_overrun_time(void);

int hal_timer_irq_active(void);

int hal_timer_irq_pending(void);

uint32_t hal_timer_get_elapsed_time(void);

#ifdef __cplusplus
}
#endif

#endif

