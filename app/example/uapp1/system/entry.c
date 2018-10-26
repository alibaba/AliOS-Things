/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "hal/soc/soc.h"
#include <uapp.h>
#include <app_mm.h>

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

#define UTASK_STACK_SIZE 0x200

static cpu_stack_t app_stack[UTASK_STACK_SIZE * sizeof(cpu_stack_t)];
static ktask_t app_task_strcut;

extern int application_start(int argc, char **argv);

static void app_entry(int argc, char *argv[])
{
    ktask_t *cur;
    unsigned int mm_start;
    size_t mm_size;

    /* init app private heap */
    mm_start = &_app_heap_start;
    mm_size = (size_t)&_app_heap_end - (size_t)&_app_heap_start;
    if (NULL == app_mm_init(mm_start, mm_size)) {
        return;
    }

    // Note: printf doesn't work in app untill app_mm_init is done

    application_start(argc, argv);
}

__attribute__ ((used, section(".app_info"))) uapp_info_t app_info = {
    APP_INFO_MAGIC,
    &app_type,
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
    &app_task_strcut,
    app_stack,
    UTASK_STACK_SIZE,
    AOS_DEFAULT_APP_PRI,
    0, 0, 0, 0
};

