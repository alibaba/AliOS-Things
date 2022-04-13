/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include "ulog/ulog.h"

#include "aos/hal/uart.h"
#include "aos/hal/timer.h"
#include <aos/hal/wifi.h>
#include "board.h"


#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

uart_dev_t uart_0;
uart_dev_t uart_1;

#if 0
extern hal_wifi_module_t sim_aos_wifi_mico;
#endif

void hal_reboot(void)
{
    //sys_reset();
}

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    kstat_t   ret;
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        ret = krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        ret = krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
    return ret;
}

int32_t hal_timer_start(timer_dev_t *tmr)
{
    return krhino_timer_start(tmr->priv);
}

void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

int32_t hal_timer_finalize(timer_dev_t *tim) {
    return 0;
}
int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para) {
    return 0;
}

#if 0
extern hal_wifi_module_t rtl8710bn_wifi_module;
#endif

void hw_start_hal(void)
{
    //DBG_8195A("start hal-----------\n");

    uart_0.port                = MICO_UART_0;
    uart_0.config.baud_rate    = 921600;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
}

