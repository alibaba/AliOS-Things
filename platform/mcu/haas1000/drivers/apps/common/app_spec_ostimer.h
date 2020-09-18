/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_SPEC_OSTIMER__
#define __APP_SPEC_OSTIMER__
#include "cmsis_os.h"

typedef struct{
    osTimerId timerid;
    os_timer_type type;
    os_ptimer ptimer;
    uint32_t interval;
    uint32_t ctx;    
    void *argument;
}SPEC_TIMER_CTX_T;

#define specTimerDef(name, function)  \
SPEC_TIMER_CTX_T spec_timer_ctx_##name = \
{NULL, osTimerOnce, function, 0, 0, NULL}; \
osTimerDef(name, app_spec_timer_handler)

#define specTimer osTimer

#define specTimerCtx(name) \
&spec_timer_ctx_##name

void app_spec_timer_handler(void const *para);
osStatus app_spec_timer_create (SPEC_TIMER_CTX_T *spec_timer_ctx, const osTimerDef_t *timer_def, os_timer_type type, void *argument);
osStatus app_spec_timer_start (SPEC_TIMER_CTX_T *spec_timer_ctx, uint32_t millisec);
osStatus app_spec_timer_stop (SPEC_TIMER_CTX_T *spec_timer_ctx);
osStatus app_spec_timer_delete (SPEC_TIMER_CTX_T *spec_timer_ctx);

#endif
