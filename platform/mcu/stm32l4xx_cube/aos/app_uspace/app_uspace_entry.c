/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"

#ifdef AOS_BINS

extern unsigned int _app_data_ram_begin;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_data_flash_begin;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern unsigned int app_type;
extern int application_start(int argc, char **argv);

#define UTASK_STACK_SIZE 0x600

static ktask_t utask_obj;
cpu_stack_t utask_ubuf[UTASK_STACK_SIZE];

int aos_application_init(void)
{
    printf("aos application init.\r\n");
    return 0;
}

static void utask_run(void *arg)
{
    application_start(0, NULL);
}

static void app_entry(int argc, char *argv[])
{
    ktask_t *cur;

    aos_application_init();

    krhino_utask_create(&utask_obj, "utask", 0,AOS_DEFAULT_APP_PRI,
        (tick_t)0, utask_ubuf, UTASK_STACK_SIZE, UTASK_STACK_SIZE,
        (task_entry_t)utask_run, 1, 1, 0, 0);

    cur = krhino_cur_task_get();

    krhino_task_del(cur);

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

