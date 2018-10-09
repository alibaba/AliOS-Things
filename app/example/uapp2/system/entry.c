/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"
#include <app_mm.h>
#include <uapp.h>

extern unsigned int _app_text_flash_begin;
extern unsigned int _app_text_flash_end;
extern unsigned int _app_data_ram_begin;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_data_flash_begin;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern unsigned int app_type;
extern int application_start(int argc, char **argv);

#define UTASK_STACK_SIZE 0x200

static ktask_t utask_obj;
cpu_stack_t utask_ubuf[UTASK_STACK_SIZE];

static void utask_run(void *arg)
{
    application_start(0, NULL);
}

static void app_entry(int id, int argc, char *argv[])
{
    ktask_t *cur;
    unsigned int mm_start;
    size_t mm_size;

    /* init app private heap */
    mm_start = &_app_heap_start;
    mm_size = (size_t)&_app_heap_end - (size_t)&_app_heap_start;
    if (NULL == app_mm_init(mm_start, mm_size)) {
        goto out;
    }

    // Note: printf doesn't work in app untill app_mm_init is done

    krhino_uprocess_create(&utask_obj, "utask", 0, AOS_DEFAULT_APP_PRI,
        (tick_t)0, utask_ubuf, UTASK_STACK_SIZE, UTASK_STACK_SIZE,
        (task_entry_t)utask_run, id, 1);

out:
    cur = krhino_cur_task_get();
    krhino_task_del(cur);
}

__attribute__ ((used, section(".app_info"))) uapp_info_t app_info = {
    app_entry,
    &_app_text_flash_begin,
    &_app_text_flash_end,
    &_app_data_ram_begin,
    &_app_data_ram_end,
    &_app_data_flash_begin,
    &_app_bss_start,
    &_app_bss_end,
    &_app_heap_start,
    &_app_heap_end,
    APP_INFO_MAGIC,
    0x0,
    &app_type
};

