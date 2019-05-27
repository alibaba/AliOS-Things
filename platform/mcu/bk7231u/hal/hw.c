/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include "aos/kernel.h"

#include "aos/hal/timer.h"
#include "network/hal/base.h"
#include <hal/wifi.h>

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

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

void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

extern hal_wifi_module_t sim_aos_wifi_beken;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_beken);
#ifdef CONFIG_AOS_MESH
    extern void beken_wifi_mesh_register(void);
    beken_wifi_mesh_register();
#endif
}
