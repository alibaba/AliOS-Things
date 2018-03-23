/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal/wifi.h"
#include "hal/ota.h"
#include <aos/log.h>

#define AOS_START_STACK 1536

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

extern void Message(const char *str);

static void platform_init(void);

ktask_t *g_aos_init;
ktask_t *g_slspawn;
ktask_t *g_aos_app = NULL;
extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);

extern hal_wifi_module_t cc3220s_wifi_nwp;

static void app_slspawn(void *arg)
{
    sl_Task(NULL);
}
    
static int init_wifi()
{
    int ret = 0;
    //hal_wifi_register_module(&qca_4002_wmi);
    krhino_task_dyn_create(&g_slspawn, "SLSPAWN", 0, AOS_DEFAULT_APP_PRI, 0, 1024, (task_entry_t)app_slspawn, 1);
    return ret;
}

extern void hw_start_hal(void);

static void sys_init(void)
{
    int i = 0;

    init_wifi();

    platform_init();

#ifdef BOOTLOADER

#else
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif

#ifdef CONFIG_AOS_CLI
    aos_cli_init();
#endif

#ifdef AOS_KV
    aos_kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

#ifdef AOS_FOTA 
    ota_service_init();
#endif

    aos_framework_init();
    application_start(0, NULL);	
#endif
}
#ifdef AOS_FOTA
extern struct hal_ota_module_s hal_cc3220_ota_module;
#endif
static void platform_init(void)
{
    board_init();

#ifdef AOS_FOTA
    hal_ota_register_module(&hal_cc3220_ota_module);
#endif

}


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


void hw_start_hal(void)
{
    Message("start-----------hal\n");
}


int main(void)
{
   aos_init();
   krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);

   aos_start();
   return 0;
}


