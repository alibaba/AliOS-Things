/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <uapp.h>

extern unsigned int _app_text_flash_start;
extern unsigned int _app_text_flash_end;
extern unsigned int _app_data_flash_start;
extern unsigned int _app_data_ram_start;
extern unsigned int _app_data_ram_end;
extern unsigned int _app_bss_start;
extern unsigned int _app_bss_end;
extern unsigned int _app_heap_start;
extern unsigned int _app_heap_end;
extern unsigned int _app_type;

#define UTASK_USTACK_SIZE 0x200
#define UTASK_KSTACK_SIZE 0x200

static cpu_stack_t  ustack[UTASK_USTACK_SIZE];
static ktask_t     *task_struct;

extern int application_start(int argc, char **argv);

void app_entry(int argc, char *argv[])
{
    ktask_t *cur;
    unsigned int mm_start;
    size_t mm_size;

    /* init app private heap */
    mm_start = &_app_heap_start;
    mm_size  = (size_t)&_app_heap_end - (size_t)&_app_heap_start;
    if (NULL == umm_init(mm_start, mm_size)) {
        return;
    }

    // Note: printf doesn't work in app untill app_mm_init is done

    timer_init();

    application_start(argc, argv);
}

__attribute__ ((used, section(".app_info"))) uapp_info_t app_info = {
     APP_INFO_MAGIC,
    &_app_type,
    &task_struct,
     app_entry,
     ustack,
     UTASK_USTACK_SIZE,
     UTASK_KSTACK_SIZE,
     AOS_DEFAULT_APP_PRI,
    &_app_text_flash_start,
    &_app_text_flash_end,
    &_app_data_ram_start,
    &_app_data_ram_end,
    &_app_data_flash_start,
    &_app_bss_start,
    &_app_bss_end,
    &_app_heap_start,
    &_app_heap_end,
     0, 0, 0, 0
};

