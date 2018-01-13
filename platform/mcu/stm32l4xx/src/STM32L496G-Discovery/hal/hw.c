/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include "stm32l4xx_hal_cortex.h"

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

uart_dev_t uart_0;

void hal_reboot(void)
{
    HAL_NVIC_SystemReset();
}

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
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

extern hal_wifi_module_t sim_aos_wifi_stm23l496;
extern struct hal_ota_module_s stm32l496_ota_module;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_stm23l496);
    hal_ota_register_module(&stm32l496_ota_module);
}
