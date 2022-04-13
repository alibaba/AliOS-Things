/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/timer.h"
#define IGNORE_HAL_TIMER_RAW_API_CHECK
#include "hal_timer_raw.h"
#define BES_HAL_DEBUG 0
#include "app_hal.h"
#undef hal_timer_start
#undef hal_timer_stop
#include "hal_trace.h"

static timer_dev_t _tim;

static void oneshot_timer_handler(uint32_t elapsed)
{
    TRACE("%s: elapsed=%d", __FUNCTION__, elapsed);
    _tim.config.cb(_tim.config.arg);
}

static void periodic_timer_handler(uint32_t elapsed)
{
    TRACE("%s: elapsed=%d", __FUNCTION__, elapsed);
    _tim.config.cb(_tim.config.arg);
}

static void _hal_timer_stop(void)
{
    hal_timer_stop_nickname();
}

static int32_t _hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    int32_t ret = 0;
    enum HAL_TIMER_TYPE_T type = HAL_TIMER_TYPE_ONESHOT;
    HAL_TIMER_IRQ_HANDLER_T handler = oneshot_timer_handler;
    ENTER_FUNCTION();

    memcpy(&_tim, tim, sizeof(timer_dev_t));
    if (_tim.config.reload_mode == TIMER_RELOAD_AUTO) {
        type = HAL_TIMER_TYPE_PERIODIC;
        handler = periodic_timer_handler;
    }
    hal_timer_setup(type, handler);
    
    LEAVE_FUNCTION();
	return 0;
}


/**
 * init a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = 0;
    ENTER_FUNCTION();
    ret = _hal_timer_para_chg(tim, tim->config);
    LEAVE_FUNCTION();
	return 0;
}

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_start(timer_dev_t *tim)
{
    int32_t ret = 0;
    ENTER_FUNCTION();
    hal_timer_start_nickname(US_TO_TICKS(tim->config.period));
    LEAVE_FUNCTION();
	return 0;
}

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void hal_timer_stop(timer_dev_t *tim)
{
    ENTER_FUNCTION();
    _hal_timer_stop();
    LEAVE_FUNCTION();
}

/**
 * change the config of a hardware timer
 *
 * @param[in]  tim   timer device
 * @param[in]  para  timer config
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    int32_t ret = 0;
    ENTER_FUNCTION();
    ret = _hal_timer_para_chg(tim, para);
    LEAVE_FUNCTION();
	return 0;
}

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = 0;
    ENTER_FUNCTION();
    _hal_timer_stop();
    LEAVE_FUNCTION();
	return ret;
}

static void _hal_timer_test_cb(void *arg)
{
    TRACEME("%s arg=0x%X\n", arg);
}

void _hal_timer_test()
{
    uint32_t cur_ticks = hal_sys_timer_get();
    uint32_t st;
    timer_dev_t tim = {0, {1000, TIMER_RELOAD_MANU, _hal_timer_test_cb, 100}, NULL};

    hal_timer_init(&tim);
    hal_timer_start(&tim);
    krhino_task_sleep(3000);
    tim.config.period = 20;
    hal_timer_start(&tim);
    TRACEME("TIMER: oneshot start 20\n");
    hal_timer_stop(&tim);
    hal_timer_finalize(&tim);

    tim.config.reload_mode = TIMER_RELOAD_AUTO;
    hal_timer_init(&tim);
    hal_timer_start(&tim);
    krhino_task_sleep(3000);
    tim.config.period = 20;
    hal_timer_start(&tim);
    krhino_task_sleep(30000);
    TRACEME("TIMER: oneshot start 20\n");
    hal_timer_stop(&tim);
    hal_timer_finalize(&tim);
}


