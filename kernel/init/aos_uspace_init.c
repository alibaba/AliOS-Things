/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <arch_mpu.h>

extern char  app_info_addr;
extern char  app2_info_addr;
extern char  app3_info_addr;

extern k_mm_head  *g_kmm_head;

struct m_app_info_t *g_app_info[MAX_APP_BINS] =  {
    (struct m_app_info_t *) &app_info_addr,
    (struct m_app_info_t *) &app2_info_addr,
    (struct m_app_info_t *) &app3_info_addr,
};

static uint32_t g_pid = 1;

static void app_pre_init(struct m_app_info_t *app_info)
{
    memcpy((void *)(app_info->data_ram_start),
            (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0,
           app_info->bss_end - app_info->bss_start);

/*
    krhino_add_mm_region(g_kmm_head,
                         (void *)(app_info->heap_start),
                         app_info->heap_end - app_info->heap_start);

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
    krhino_mm_leak_region_init((void *)(app_info->data_ram_start),
                               (void *)(app_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(app_info->bss_start),
                               (void *)(app_info->bss_end));
#endif
*/
}

static int aos_app_init(void)
{
    int ret;

    ret = arch_app_init(g_app_info);

    if (ret < 0)
        return ret;

    // init app data, bss section
    app_pre_init(g_app_info[0]);

    return 0;
}

static void app_entry_enter(void *arg)
{
    int ret;
    struct m_app_info_t *app_info = arg;
    int pid = g_pid;

    g_pid++;
    ret = arch_app_prepare(pid - 1);

    if (ret) {
        aos_task_exit(0);
    }

    if (app_info->app_entry) {
        app_info->app_entry(pid, 0, NULL);
    }
}

static void aos_app_start(void)
{
    int i;

    for (i = 0; i < 1 /*MAX_APP_BINS*/; i++) {
        aos_task_new("app", app_entry_enter, g_app_info[i], 0x400);
    }
}


int aos_kernel_init(kinit_t *kinit)
{
    int ret;

    ret = aos_app_init();
    if (ret) {
        return ret;
    }

    aos_app_start();

    return 0;
}


