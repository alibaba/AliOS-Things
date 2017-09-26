/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <k_api.h>
#include <aos/aos.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <hal/ota.h>

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

static void _timer_cb(void *timer, void *arg)
{
    hal_timer_t *tmr = arg;
    tmr->cb(tmr->arg);
}

void hal_timer_init(hal_timer_t *tmr, unsigned int period, unsigned char auto_reload, unsigned char ch, hal_timer_cb_t cb, void *arg)
{
    (void)ch;
    bzero(tmr, sizeof(*tmr));
    tmr->cb = cb;
    tmr->arg = arg;
    if (auto_reload > 0u) {
        krhino_timer_dyn_create((ktimer_t **)&tmr->priv, "hwtmr", _timer_cb,
                                us2tick(period), us2tick(period), tmr, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tmr->priv, "hwtmr", _timer_cb,
                                us2tick(period), 0, tmr, 0);
    }
}


void hal_timer_stop(hal_timer_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

extern hal_wifi_module_t sim_aos_wifi_beken;
extern struct hal_ota_module_s moc108_ota_module;
void hw_start_hal(void)
{
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_beken);
    hal_ota_register_module(&moc108_ota_module);
#ifdef CONFIG_AOS_MESH
    extern void beken_wifi_mesh_register(void);
    beken_wifi_mesh_register();
#endif
}
