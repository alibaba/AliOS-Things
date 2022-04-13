/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <aos/kernel.h>
#include "preamble.h"
#include "umm.h"
#include "u_config.h"

#if defined (__CC_ARM)

#elif defined (__ICCARM__)
    #pragma segment=".data"
    #pragma segment=".data_init"
    #pragma segment=".bss"
    extern unsigned int __flash_start[];
    extern unsigned int __flash_end[];
    extern unsigned int __ram_start[];
    extern unsigned int __ram_end[];
    extern unsigned int __heap_end[];
    #define APP_FLASH_START      __flash_start
    #define APP_FLASH_END        __flash_end
    #define APP_RAM_START        __ram_start
    #define APP_RAM_END          __ram_end
    #define APP_DATA_FLASH_START __sfb(".data_init")
    #define APP_DATA_RAM_START   __sfb(".data")
    #define APP_DATA_RAM_END     __sfe(".data")
    #define APP_BSS_START        __sfb(".bss")
    #define APP_BSS_END          __sfe(".bss")
    #define APP_HEAP_START       __sfe(".bss")
    #define APP_HEAP_END         __heap_end
#else /* __GNUC__ */
    extern unsigned char _app_flash_start[];
    extern unsigned char _app_flash_end[];
    extern unsigned char _app_ram_start[];
    extern unsigned char _app_ram_end[];
    extern unsigned char _app_data_ram_start[];
    extern unsigned char _app_data_ram_end[];
    extern unsigned char _app_data_flash_start[];
    extern unsigned char _app_bss_start[];
    extern unsigned char _app_bss_end[];
    extern unsigned char _app_heap_start[];
    extern unsigned char _app_heap_end[];

    #define APP_FLASH_START      _app_flash_start
    #define APP_FLASH_END        _app_flash_end
    #define APP_RAM_START        _app_ram_start
    #define APP_RAM_END          _app_ram_end
    #define APP_DATA_FLASH_START _app_data_flash_start
    #define APP_DATA_RAM_START   _app_data_ram_start
    #define APP_DATA_RAM_END     _app_data_ram_end
    #define APP_BSS_START        _app_bss_start
    #define APP_BSS_END          _app_bss_end
    #define APP_HEAP_START       _app_heap_start
    #define APP_HEAP_END         _app_heap_end
#endif

static ktask_t *utask;
/* utask's stack */
static cpu_stack_t ustack[RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE];

extern int application_start(int argc, char **argv);

#if (RHINO_CONFIG_UTIMER_TASK > 0)
    extern int timer_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio, size_t msg_num);
#endif

#if (RHINO_CONFIG_URES_TASK > 0)
    extern int res_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);
#endif

#if (RHINO_CONFIG_UCLI_TASK > 0)
    extern int ucli_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);
#endif

#if (RHINO_CONFIG_UCALLBACK_TASK > 0)
    extern int cb_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);
#endif

#if (AOS_COMP_SOCKET_RPC_CLIENT > 0)
    extern void ulwip_client_attach_vfs(void);
#endif
#ifdef AOS_COMP_CPLUSPLUS
    extern void cpp_init(void);
#endif

k_mm_region_t g_mm_region[1];
int           g_region_num;

void usr_mm_init(void)
{
    /* init app private heap */
    g_mm_region[0].start = (void *)APP_HEAP_START;
    g_mm_region[0].len   = (size_t)APP_HEAP_END - (size_t)APP_HEAP_START;
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

__attribute__((weak)) int aos_uspace_vendor_init(void)
{
    return 0;
}

void app_entry(int argc, char *argv[])
{
    usr_mm_init();

    aos_uspace_vendor_init();

    if (usr_ptcb_init() != 0) {
        return;
    }

    /* Note: printf doesn't work in app untill umm_init is done */

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

    //aos_cli_register_commands(uapp_builtin_cmds, sizeof(uapp_builtin_cmds)/sizeof(uapp_builtin_cmds[0]));
    umm_cli_cmds_register();
#ifdef AOS_COMP_CPLUSPLUS
    cpp_init();
#endif

#endif
#if (AOS_COMP_SOCKET_RPC_CLIENT > 0)
    ulwip_client_attach_vfs();
#endif
    application_start(0, NULL);

    /* Note: return from application_start(), the process will exit */
    aos_exit(0);
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
    (void *)APP_FLASH_START,
    (void *)APP_FLASH_END,
    (void *)APP_RAM_START,
    (void *)APP_RAM_END,
    (void *)APP_DATA_RAM_START,
    (void *)APP_DATA_RAM_END,
    (void *)APP_DATA_FLASH_START,
    (void *)APP_BSS_START,
    (void *)APP_BSS_END,
    &utask,
    app_entry,
    ustack,
    RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE,
    RHINO_CONFIG_APP_ENTRY_TASK_PRIO,
    aos_task_exit,
    RHINO_CONFIG_APP_VERSION,
    {0},
};

