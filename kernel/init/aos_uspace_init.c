/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

extern char  app_info_addr;
extern k_mm_head  *g_kmm_head;
struct m_app_info_t *app_info = (struct m_app_info_t *) &app_info_addr;

static void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start),
            (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0,
           app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head,
                         (void *)(app_info->heap_start),
                         app_info->heap_end - app_info->heap_start);

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
    krhino_mm_leak_region_init((void *)(app_info->data_ram_start),
                               (void *)(app_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(app_info->bss_start),
                               (void *)(app_info->bss_end));
#endif

}


int aos_kernel_init(kinit_t *kinit)
{

    app_pre_init();

    if (app_info->app_entry) {
        app_info->app_entry(0, NULL);
    }

    return 0;
}


