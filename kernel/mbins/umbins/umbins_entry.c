/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/init.h"

#ifdef AOS_BINS

const int *mbins_tbl = NULL;

extern unsigned int _app_data_ram_begin;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_data_flash_begin;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern unsigned int app_type;
extern int application_start(int argc, char **argv);

int aos_application_init(void)
{
    printf("aos application init.");
    return 0;
}

static void app_entry(void *ksyscall_tbl, int argc, char *argv[])
{
    /* syscall_tbl assignment must be first, every syscall need array address */
    mbins_tbl = (int *)ksyscall_tbl;

    aos_application_init();
    application_start(argc, argv);
}

__attribute__ ((used, section(".app_info"))) struct m_app_info_t app_info = {
    app_entry,
    &_app_data_ram_begin,
    &_app_data_ram_end,
    &_app_data_flash_begin,
    &_app_bss_start,
    &_app_bss_end,
    &_app_heap_start,
    &_app_heap_end,
    0x0,
    0x0,
    &app_type
};
#endif

