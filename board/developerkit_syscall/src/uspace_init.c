/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <k_api.h>
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
}

static int app_init(void)
{
    int i;
    int ret;

    ret = arch_app_init(g_app_info);

    if (ret < 0)
        return ret;

    // init app data, bss section
    for (i = 0; i < MAX_APP_BINS; i++) {
        if (g_app_info[i]->magic == APP_INFO_MAGIC) {
            app_pre_init(g_app_info[i]);
        }
    }

    return 0;
}

static void app_entry_enter(void *arg)
{
    int ret;
    struct m_app_info_t **app_info = arg;
    int pid = g_pid;

    g_pid++;

    ret = arch_app_prepare(app_info- g_app_info);

    if (!ret) {
        if ((*app_info)->app_entry) {
            // jump to uapp, it will never return
            (*app_info)->app_entry(pid, 0, NULL);
        }
    }

    aos_task_exit(0);
}

static void app_start(void)
{
    int i;

    for (i = 0; i < MAX_APP_BINS; i++) {
        aos_task_new("app", app_entry_enter, &g_app_info[i], 0x400);
    }
}

int aos_app_init(void)
{
    int ret;

    ret = app_init();
    if (ret) {
        return ret;
    }

    app_start();

    return 0;
}


