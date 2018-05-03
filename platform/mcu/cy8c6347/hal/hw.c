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

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)


void hal_reboot(void)
{
    //NVIC_SystemReset();
}

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    kstat_t status;

    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        status = krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        status = krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }

    return status;
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

#if defined(DEV_SAL_MK3060)
    extern hal_wifi_module_t aos_wifi_module_mk3060;
#else
    extern hal_wifi_module_t sim_aos_wifi_stm32l475;
#endif

extern struct hal_ota_module_s cy8c6347_ota_module;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    
#if defined(DEV_SAL_MK3060)
    hal_wifi_register_module(&aos_wifi_module_mk3060);
#else
    hal_wifi_register_module(&sim_aos_wifi_stm32l475);
#endif
    hal_ota_register_module(&cy8c6347_ota_module);
    hal_wifi_init();    
}

