/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include "rda5981.h"

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

//uart_dev_t uart_0;

void hal_reboot(void)
{
    rda_ccfg_ckrst();
    rda_ccfg_perrst();
    /* Ensure all outstanding memory accesses included buffered write are completed before reset */
    __DSB();
    SCB->AIRCR = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                            (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | /* Keep priority group unchanged */
                             SCB_AIRCR_VECTRESET_Msk);
    /* Ensure completion of memory access */
    __DSB();
    /* wait until reset */
    while(1) { __NOP(); }
}
#if 0
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
#endif
extern hal_wifi_module_t aos_wifi_rda59xx;
//extern struct hal_ota_module_s stm32l475_ota_module;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&aos_wifi_rda59xx);
    aos_wifi_rda59xx.init(&aos_wifi_rda59xx);
    //hal_ota_register_module(&stm32l475_ota_module);
}

