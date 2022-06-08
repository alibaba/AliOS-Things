/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#include <freertos/event_groups.h>

static uint32_t CK_IN_INTRP(void)
{
    return 0;
}

typedef struct tmr_adapter {
    TimerHandle_t timer;
    void (*func)(void *, void *);
    void *func_arg;
    uint8_t bIsRepeat;
    uint64_t init_ms;
    uint64_t round_ms;
} tmr_adapter_t;

static void tmr_adapt_cb(TimerHandle_t xTimer)
{
    tmr_adapter_t *pTmrObj = (tmr_adapter_t *)pvTimerGetTimerID(xTimer);
    pTmrObj->func(pTmrObj, pTmrObj->func_arg);
    return;
}

aos_status_t aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, uint32_t ms, bool repeat, bool auto_run)
{
    /* verify param */
    if (timer == NULL || ms == 0 || fn == NULL) {
        return -1;
    }

    /* create timer wrap object ,then initlize timer object */
    tmr_adapter_t *tmr_adapter = pvPortMalloc(sizeof(tmr_adapter_t));

    if (tmr_adapter == NULL) {
        return -1;
    }

    tmr_adapter->func = fn;
    tmr_adapter->func_arg = arg;
    tmr_adapter->bIsRepeat = repeat;
    tmr_adapter->init_ms = ms;
    tmr_adapter->round_ms = (repeat == true) ? ms : 0;
    /* create timer by kernel api */
    TimerHandle_t ptimer = xTimerCreate("Timer", pdMS_TO_TICKS(ms), repeat, tmr_adapter, tmr_adapt_cb);

    if (ptimer == NULL) {
        vPortFree(tmr_adapter);
        return -1;
    }

    /* start ptimer if auto_run == TRUE */
    if (auto_run) {
        if (pdPASS != xTimerStart(ptimer, 0)) {
            return -1;
        }
    }

    tmr_adapter->timer = ptimer;
    *timer = (void *)tmr_adapter;
    return 0;
}

aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint32_t options)
{
    bool repeat, auto_run;

    if (timer == NULL) {
        return -EINVAL;
    }
    repeat = (options & AOS_TIMER_REPEAT) ? true : false;
    auto_run = (options & AOS_TIMER_AUTORUN) ? true : false;
    return aos_timer_new_ext(timer, fn, arg, ms, repeat, auto_run);
}

aos_status_t aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, uint32_t ms, bool repeat)
{
    return aos_timer_new_ext(timer, fn, arg, ms, repeat, 1);
}

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL) {
        return;
    }

    tmr_adapter_t *tmr_adapter = (tmr_adapter_t *)*timer;
    int ret = xTimerDelete(tmr_adapter->timer, 0);

    if (!ret) {
        return ;
    }

    vPortFree(tmr_adapter);
    *timer = NULL;

    return ;
}

aos_status_t aos_timer_start(aos_timer_t *timer)
{
    /* verify param */
    if (timer == NULL) {
        return -1;
    }

    /* start timer  */
    tmr_adapter_t *tmr_adapter = (tmr_adapter_t *)*timer;
    int tmp = xTimerStart(tmr_adapter->timer, 0);

    if (tmp != pdPASS) {
        return -1;
    }

    return 0;
}

aos_status_t aos_timer_stop(aos_timer_t *timer)
{
    /* verify param */
    if (timer == NULL) {
        return -1;
    }

    /* stop timer */
    tmr_adapter_t *tmr_adapter = (tmr_adapter_t *)*timer;
    int tmp;
    tmp = xTimerStop(tmr_adapter->timer, 0);

    if (tmp != pdPASS) {
        return -1;
    }

    return 0;
}

aos_status_t aos_timer_change(aos_timer_t *timer, uint32_t ms)
{
    /* verify param */
    if (timer == NULL) {
        return -1;
    }
    BaseType_t  xHigherProTskWoken  = pdFALSE;
    /* change timer period value */
    tmr_adapter_t *pTimer = (tmr_adapter_t *)*timer;
    int ret;
    if (CK_IN_INTRP()) {
        ret = xTimerChangePeriodFromISR(pTimer->timer, pdMS_TO_TICKS(ms), &xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 10);
    }
    pTimer->init_ms = ms;
    pTimer->round_ms = (pTimer->bIsRepeat == 1) ? ms : 0;

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

aos_status_t aos_timer_change_once(aos_timer_t *timer, uint32_t ms)
{
    /* verify param */
    if (timer == NULL) {
        return -1;
    }
    BaseType_t  xHigherProTskWoken  = pdFALSE;
    /* change timer period value */
    tmr_adapter_t *pTimer = (tmr_adapter_t *)*timer;
    int ret;

    /* remove as not supported by IDF-V4.2 */
    // vTimerSetReloadMode(pTimer->timer, 0);
    if (CK_IN_INTRP()) {
        ret = xTimerChangePeriodFromISR(pTimer->timer, pdMS_TO_TICKS(ms), &xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 10);
    }
    pTimer->init_ms = ms;
    pTimer->bIsRepeat = 0;
    pTimer->round_ms = 0;

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}
int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    tmr_adapter_t *pTimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;

    pTimer = (tmr_adapter_t *)*timer;
    init_ms = pTimer->init_ms;
    round_ms = pTimer->round_ms;

    value[0] = init_ms / 1000;
    value[1] = (init_ms % 1000) * 1000000UL;
    value[2] = round_ms / 1000;
    value[3] = (round_ms % 1000) * 1000000UL ;

    return 0;
}
