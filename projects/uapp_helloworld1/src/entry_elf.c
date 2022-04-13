/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <locale.h>
#include "u_api.h"
#include "preamble.h"
#include <pthread.h>

#if defined (__CC_ARM)
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
#elif defined (__GNUC__)
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
#endif

extern char *_app_heap_start;
extern char *_app_heap_end;

/* utask's stack */
static cpu_stack_t ustack[RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE + sizeof(struct utask_couple_data)];

extern int application_start(int argc, char **argv);

k_mm_region_t g_mm_region[1];
int           g_region_num;

extern int __ctors_start__;
extern int __ctors_end__;
extern int __dtors_start__;
extern int __dtors_end__;

static void append_start_init(void)
{
    unsigned int a = (unsigned int)&__ctors_start__;
    for (; a<(unsigned int)&__ctors_end__; a+=sizeof(void(*)()))
        (*(void (**)(void))a)();
}

static void append_end_destroy(void)
{
    unsigned int a = (unsigned int)&__dtors_start__;
    for (; a<(unsigned int)&__dtors_end__; a+=sizeof(void(*)()))
        (*(void (**)(void))a)();
}

void usr_mm_init(void)
{
    /* init app private heap */
    g_mm_region[0].start = (void *)&_app_heap_start;
    g_mm_region[0].len   = (size_t)&_app_heap_end - (size_t)&_app_heap_start;
    g_region_num = 1;

    umm_init();
}

static int usr_ptcb_init(void)
{
    void *ptcb;
    ktask_t *cur_task;

    ptcb = calloc(1, PTHREAD_TCB_SIZE);
    if (ptcb == NULL) {
        printf("FATAL ERROR:%s:%d:%s: out of memory.\n", __FILE__, __LINE__, __func__);
        return -1;
    }
    cur_task = krhino_cur_task_get();
    krhino_task_ptcb_set(cur_task, ptcb);

    return 0;
}

void app_entry(int argc, char *argv[])
{
    usr_mm_init();
    if (usr_ptcb_init() != 0) {
        return;
    }

    setlocale(LC_ALL, "C");

#if (RHINO_CONFIG_URES_TASK > 0)
    res_task_start(RHINO_CONFIG_URES_TASK_KSTACK_SIZE,
                   RHINO_CONFIG_URES_TASK_USTACK_SIZE,
                   RHINO_CONFIG_URES_TASK_PRIO);
#endif

#if (RHINO_CONFIG_UCALLBACK_TASK > 0)
    cb_task_start(RHINO_CONFIG_UCALLBACK_TASK_KSTACK_SIZE,
                  RHINO_CONFIG_UCALLBACK_TASK_USTACK_SIZE,
                  RHINO_CONFIG_UCALLBACK_TASK_PRIO);
#endif

#if (RHINO_CONFIG_UTIMER_TASK > 0)
    timer_task_start(RHINO_CONFIG_UTIMER_TASK_KSTACK_SIZE,
                     RHINO_CONFIG_UTIMER_TASK_USTACK_SIZE,
                     RHINO_CONFIG_UTIMER_TASK_PRIO,
                     RHINO_CONFIG_UTIMER_MSG_NUM);
#endif

#if (RHINO_CONFIG_UCLI_TASK > 0)
    ucli_task_start(RHINO_CONFIG_UCLI_TASK_KSTACK_SIZE,
                    RHINO_CONFIG_UCLI_TASK_USTACK_SIZE,
                    RHINO_CONFIG_UCLI_TASK_PRIO);
#endif

    append_start_init();
    #ifdef AOS_COMP_CPLUSPLUS
    cpp_init();
    #endif

#ifdef AOS_COMP_ULOG
    /* uspace ulog init, same as kspace ulog*/
    ulog_init();
#endif

    application_start(0, NULL);

    append_end_destroy();

    /* Note: return from application_start(), the process will exit */

    krhino_uprocess_exit();
}

#if defined (__CC_ARM) || defined (__GNUC__)
const static preamble_t preamble __attribute__((used, section(".preamble"))) =
#elif defined (__ICCARM__)
#pragma location=".preamble"
const static __root preamble_t preamble =
#endif
{
    PREAMBLE_MAGIC,
    RHINO_CONFIG_APP_NAME,
    (void*)APP_TEXT_START,
    (void*)APP_TEXT_END,
    (void*)APP_DATA_START,
    (void*)APP_DATA_END,
    (void*)APP_ZERO_START,
    (void*)APP_ZERO_END,
    RHINO_CONFIG_APP_ENTRY_TASK_NAME,
    app_entry,
    ustack,
    RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_PRIO,
    krhino_utask_deathbed,
    RHINO_CONFIG_APP_VERSION,
    {0},
};

