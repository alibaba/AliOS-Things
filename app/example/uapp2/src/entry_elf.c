/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <locale.h>
#include "u_api.h"
#include "preamble.h"

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


static unsigned int app_heap[APP_HEAP_SIZE/sizeof(unsigned int)];

static ktask_t *utask;
/* utask's stack */
static cpu_stack_t ustack[RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE];

extern int application_start(int argc, char **argv);

#if (RHINO_CONFIG_UCLI_TASK > 0)
static void uapp_heap_stat(char *pbuffer, int outlen, int argc, char **argv)
{
    umm_show();
}

static const struct cli_command_st uapp_builtin_cmds[] = {
    {"uapp1_heap", "dump uapp1 heap statistics", uapp_heap_stat},
};
#endif

void app_entry(int argc, char *argv[])
{
    void    *mm_start;
    size_t   mm_size;

    /* init app private heap */
    mm_start = (void*)app_heap;
    mm_size  = APP_HEAP_SIZE;

    if (NULL == umm_init(mm_start, mm_size)) {
        return;
    }

    /* Note: printf doesn't work in app untill umm_init is done */

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

    cli_register_commands(uapp_builtin_cmds,
                          sizeof(uapp_builtin_cmds)/sizeof(uapp_builtin_cmds[0]));
#endif

    application_start(0, NULL);

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
    &utask,
    app_entry,
    ustack,
    RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_PRIO,
    RHINO_CONFIG_APP_VERSION,
    {0},
};

