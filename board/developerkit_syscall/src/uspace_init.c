/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/kernel.h"
#include <aos/kernel.h>
#include <k_api.h>
#include <uapp.h>
#include <arch_mpu.h>

extern char  app1_info_addr;
extern char  app2_info_addr;
extern char  app3_info_addr;

/*
 * to match nature number,
 * valid app bin starts from 1
 */
uapp_info_t *g_app_info[MAX_APP_BINS] =  {
    (uapp_info_t*) &app1_info_addr,
    (uapp_info_t*) &app2_info_addr,
    (uapp_info_t*) &app3_info_addr,
};

/*
 * Why g_pid starts from 1?
 * PID 0 is reserved for kernel,
 * so user process starts from 1
 */
static uint32_t g_pid = 1;

static void app_pre_init(uapp_info_t *app_info)
{
    memcpy((void *)(app_info->data_ram_start),
            (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0,
           app_info->bss_end - app_info->bss_start);
}

static void app_start(void)
{
    int i;
    int ret;
    int pid;
    uapp_info_t *app_info;

    for (i = 0; i < MAX_APP_BINS; i++) {
        app_info = g_app_info[i];

        if (app_info == NULL ||
            app_info->magic != APP_INFO_MAGIC) {
            continue;
        }

        /* init data, bss section */
        app_pre_init(app_info);

        pid = g_pid;

        ret = arch_app_prepare(app_info, pid);
        if (ret != 0)
            continue;

        krhino_uprocess_create(app_info->task, "utask", 0,
                               app_info->priority, (tick_t)0,
                               app_info->ustack,
                               app_info->ustack_size, // ustasck size
                               app_info->kstack_size, // kstasck size
                               (task_entry_t)app_info->main_entry,
                               pid, 1);

        g_pid++;
    }
}

int aos_run_app(void)
{
    int ret;

    ret = arch_mpu_init();
    if (ret) {
        return ret;
    }

    app_start();

    return 0;
}


