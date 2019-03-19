/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <preamble.h>

#if defined (__GNUC__)
extern unsigned int _app_text_start[];
extern unsigned int _app_text_end[];
extern unsigned int _app_data_start[];
extern unsigned int _app_data_end[];
extern unsigned int _app_zero_start[];
extern unsigned int _app_zero_end[];
#define APP_TEXT_START _app_text_start
#define APP_TEXT_END   _app_text_end
#define APP_DATA_START _app_data_start
#define APP_DATA_END   _app_data_end
#define APP_ZERO_START _app_zero_start
#define APP_ZERO_END   _app_zero_end
#elif defined (__CC_ARM)
extern unsigned int Image$$APP_TEXT$$Base[];
extern unsigned int Image$$APP_TEXT$$Limit[];
extern unsigned int Image$$APP_DATA$$Base[];
extern unsigned int Image$$APP_DATA$$Limit[];
extern unsigned int Image$$APP_ZERO$$Base[];
extern unsigned int Image$$APP_ZERO$$ZI$$Limit[];
#define APP_TEXT_START Image$$APP_TEXT$$Base
#define APP_TEXT_END   Image$$APP_TEXT$$Limit
#define APP_DATA_START Image$$APP_DATA$$Base
#define APP_DATA_END   Image$$APP_DATA$$Limit
#define APP_ZERO_START Image$$APP_ZERO$$Base
#define APP_ZERO_END   Image$$APP_ZERO$$ZI$$Limit

#endif

#define APP_HEAP_SIZE     0x10000
#define UTASK_USTACK_SIZE 0x200
#define UTASK_KSTACK_SIZE 0x200

static unsigned int app_heap[APP_HEAP_SIZE/sizeof(unsigned int)];

static cpu_stack_t ustack[UTASK_USTACK_SIZE];
static ktask_t *task_struct;

extern int application_start(int argc, char **argv);

void app_entry(int argc, char *argv[])
{
    ktask_t *cur;
    unsigned int mm_start;
    size_t mm_size;

    /* init app private heap */
    mm_start = (unsigned int)app_heap;
    mm_size = APP_HEAP_SIZE;

    if (NULL == umm_init((void*)mm_start, mm_size)) {
        return;
    }

    // Note: printf doesn't work in app untill app_mm_init is done

    res_task_start();

    timer_init();

    application_start(argc, argv);
}

static const preamble_t preamble __attribute__((used, section (".preamble"))) = {
    PREAMBLE_MAGIC,
    (uintptr_t)APP_TEXT_START,
    (uintptr_t)APP_TEXT_END,
    (uintptr_t)APP_DATA_START,
    (uintptr_t)APP_DATA_END,
    (uintptr_t)APP_ZERO_START,
    (uintptr_t)APP_ZERO_END,
    &task_struct,
    app_entry,
    ustack,
    UTASK_USTACK_SIZE,
    UTASK_KSTACK_SIZE,
    AOS_DEFAULT_APP_PRI,
};

