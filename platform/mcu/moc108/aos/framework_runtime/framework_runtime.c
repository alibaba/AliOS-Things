/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"

#ifdef AOS_BINS
const int *syscall_ktbl = NULL;
extern void *syscall_ftbl[];

extern char  app_info_addr;
struct app_info_t *app_info = (struct app_info_t *)&app_info_addr;

extern unsigned int _framework_data_ram_begin;
extern unsigned int _framework_data_ram_end;
extern unsigned int _framework_data_flash_begin;
extern unsigned int _framework_bss_start;
extern unsigned int _framework_bss_end;
extern unsigned int _framework_heap_start;
extern unsigned int _framework_heap_end;

static void framework_entry(void *syscall_tbl, int argc, char *argv[])
{
    /* syscall_ktbl assignment must be first */
    syscall_ktbl = (int *)syscall_tbl;

#ifdef AOS_FRAMEWORK_COMMON
    aos_framework_init();
#endif

    /*app_pre_init();*/
    if (app_info->app_entry) {
        app_info->app_entry(syscall_tbl, (void *)syscall_ftbl, 0, NULL);
    }
}

__attribute__ ((used, section(".framework_info"))) struct framework_info_t framework_info = {
    framework_entry,
    &_framework_data_ram_begin,
    &_framework_data_ram_end,
    &_framework_data_flash_begin,
    &_framework_bss_start,
    &_framework_bss_end,
    &_framework_heap_start,
    &_framework_heap_end
};
#endif

