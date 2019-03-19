/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <preamble.h>
#include <locale.h>

extern uintptr_t Image$$ER_RO$$Base;
extern uintptr_t Image$$ER_RO$$Limit;
extern uintptr_t Image$$ER_RW$$Base;
extern uintptr_t Image$$ER_RW$$Limit;
extern uintptr_t Image$$ER_ZI$$Base;
extern uintptr_t Image$$ER_ZI$$ZI$$Limit;

#define APP_TEXT_START       Image$$ER_RO$$Base
#define APP_TEXT_END         Image$$ER_RO$$Limit
#define APP_DATA_START       Image$$ER_RW$$Base
#define APP_DATA_END         Image$$ER_RW$$Limit
#define APP_ZERO_START       Image$$ER_ZI$$Base
#define APP_ZERO_END         Image$$ER_ZI$$ZI$$Limit

#define APP_HEAP_SIZE     0x10000 // 32k
#define UTASK_USTACK_SIZE 0x4000 // 16k
#define UTASK_KSTACK_SIZE 0x4000 // 16k

static unsigned int app_heap[APP_HEAP_SIZE/sizeof(unsigned int)];

static cpu_stack_t ustack[UTASK_USTACK_SIZE];
static ktask_t *task_struct;

extern int application_start(int argc, char **argv);

extern void res_task_start(void);

extern int ucli_task_start(void);

void app_entry(int argc, char *argv[])
{
    unsigned int mm_start;
    size_t mm_size;

    /* init app private heap */
    mm_start = (unsigned int)app_heap;
    mm_size = APP_HEAP_SIZE;

    if (NULL == umm_init((void*)mm_start, mm_size)) {
        return;
    }

    // Note: printf doesn't work in app untill app_mm_init is done

    setlocale(LC_ALL, "C");

    res_task_start();

    cb_task_start();

    timer_init();

    ucli_task_start();

    application_start(argc, argv);
}

static const preamble_t preamble __attribute__((used, section (".preamble"))) = {
    PREAMBLE_MAGIC,
    "linkkit_app",
    (uintptr_t *) &APP_TEXT_START,
    (uintptr_t *) &APP_TEXT_END,
    (uintptr_t *) &APP_DATA_START,
    (uintptr_t *) &APP_DATA_END,
    (uintptr_t *) &APP_ZERO_START,
    (uintptr_t *) &APP_ZERO_END,
    &task_struct,
    app_entry,
    ustack,
    UTASK_USTACK_SIZE,
    UTASK_KSTACK_SIZE,
    AOS_DEFAULT_APP_PRI,
};

