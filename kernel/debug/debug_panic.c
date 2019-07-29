/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include "debug_api.h"
#include "k_compiler.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#ifdef AOS_COMP_CLI
#include "cli/cli_api.h"
#endif

#ifdef AOS_UND
#include "und/und.h"
#include "aos/kv.h"
#include "aos/errno.h"
#define DEFAULT_REBOOT_REASON UND_STATIS_DEV_REPOWER_REASON
#endif

extern void hal_reboot(void);

#define DEBUG_PANIC_STEP_MAX    32

/* ARMCC and ICCARM do not use heap when printf a string, but gcc dose*/
#if defined(__CC_ARM)
#define print_str printf
#elif defined(__ICCARM__)
#define print_str printf
#elif defined(__GNUC__)
__attribute__((weak)) int print_str(const char *fmt, ...)
{
    int ret;

    va_list args;

    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
    return ret;
}

/* on some platform, the libc printf use the heap, while the heap maybe corrupt
   when panic.
   Redefining a new print_str without using heap is advised on these platform.
 */
#endif


#if (DEBUG_CONFIG_PANIC > 0)

/* functions followed should defined by arch\...\panic_c.c */
extern void panicShowRegs(void *context,
                          int (*print_func)(const char *fmt, ...));
extern void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP);

/* functions followed should defined by arch\...\backtrace.c */
extern int  backtrace_caller(char *PC, int *SP,
                             int (*print_func)(const char *fmt, ...));
extern int  backtrace_callee(char *PC, int *SP, char *LR,
                             int (*print_func)(const char *fmt, ...));

/* how many steps has finished when crash */
volatile uint32_t g_crash_steps = 0;


static void panic_goto_cli(void)
{
#if 0
#if (defined (AOS_COMP_CLI) && (RHINO_CONFIG_KOBJ_LIST > 0))

    klist_t      *listnode;
    ktask_t      *task;

    g_sched_lock[cpu_cur_get()]++;

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (0 != strcmp("cli", task->task_name)) {
            krhino_task_suspend(task);
        }
    }

    if(g_sched_lock[cpu_cur_get()] > 0) {
        g_sched_lock[cpu_cur_get()] = 0;
    }

    /*suspend current task*/
    task = g_active_task[cpu_cur_get()];
    if (0 != strcmp("cli", task->task_name)) {
        krhino_task_suspend(task);
    }
#endif
#endif
}

/* should exeception be restored?
   reture 1 YES, 0 NO*/
int panicRestoreCheck(void)
{
   return 0;
}

/* fault/exception entry
   notice: this function maybe reentried by double exception
   first exception, input context
   second exception, input NULL  */
void panicHandler(void *context)
{
    char prt_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \r\n";
    int x;
#if (DEBUG_CONFIG_BACKTRACE > 0)
    int lvl;
#endif
    static int  *SP = NULL;
    static char *PC = NULL;
    static char *LR = NULL;
    CPSR_ALLOC();

    /* g_crash_steps++ before panicHandler */
    if (g_crash_steps > 1 && g_crash_steps < DEBUG_PANIC_STEP_MAX) {
        print_str("......\r\n");
    }

    switch (g_crash_steps) {
        case 1:
            print_str("!!!!!!!!!! Exception  !!!!!!!!!!\r\n");
            if (context != NULL) {
                panicGetCtx(context, &PC, &LR, &SP);
            }
            panicShowRegs(context, print_str);
            g_crash_steps++;
        case 2:
            if (SP != NULL) {
                print_str("========== Stack info ==========\r\n");
                for (x = 0; x < 16; x++) {
                    k_int2str((int)&SP[x * 4], &prt_stack[8]);
                    k_int2str(SP[x * 4 + 0], &prt_stack[21]);
                    k_int2str(SP[x * 4 + 1], &prt_stack[32]);
                    k_int2str(SP[x * 4 + 2], &prt_stack[43]);
                    k_int2str(SP[x * 4 + 3], &prt_stack[54]);
                    print_str(prt_stack);
                }
            }
            g_crash_steps++;
#if (DEBUG_CONFIG_BACKTRACE > 0)
        /* 3 steps, 3 ways to try backtrace */
        case 3:
            /* Backtrace 1st try: assume ReturnAddr is saved in stack when exception */
            if (SP != NULL) {
                print_str("========== Call stack ==========\r\n");
                lvl = backtrace_caller(PC, SP, print_str);
                if (lvl > 0) {
                    /* backtrace success, do not try other way */
                    g_crash_steps += 2;
                }
                /* else, backtrace fail, try another way */
            }
            g_crash_steps++;
        case 4:
            if (g_crash_steps == 4) {
                /* Backtrace 2nd try: assume ReturnAddr is saved in LR when exception */
                if (SP != NULL) {
                    lvl = backtrace_callee(PC, SP, LR, print_str);
                    if (lvl > 0) {
                        /* backtrace success, do not try other way */
                        g_crash_steps += 1;
                    }
                    /* else, backtrace fail, try another way */
                }
                g_crash_steps++;
            }
        case 5:
            if (g_crash_steps == 5) {
                /* Backtrace 3rd try: assume PC is invalalb, backtrace from LR */
                if (SP != NULL) {
                    (void)backtrace_caller(LR, SP, print_str);
                }
                g_crash_steps++;
            }
#else
            g_crash_steps = 6;
#endif
        case 6:
#if (RHINO_CONFIG_MM_TLF > 0)
            print_str("========== Heap Info  ==========\r\n");
            debug_mm_overview(print_str);
#endif
            g_crash_steps++;
        case 7:
            print_str("========== Task Info  ==========\r\n");
            debug_task_overview(print_str);
            g_crash_steps++;
        case 8:
#if (RHINO_CONFIG_QUEUE > 0)
            print_str("========== Queue Info ==========\r\n");
            debug_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 9:
#if (RHINO_CONFIG_BUF_QUEUE > 0)
            print_str("======== Buf Queue Info ========\r\n");
            debug_buf_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 10:
#if (RHINO_CONFIG_SEM > 0)
            print_str("=========== Sem Info ===========\r\n");
            debug_sem_overview(print_str);
#endif
            g_crash_steps++;
        case 11:
            print_str("!!!!!!!!!! dump end   !!!!!!!!!!\r\n");
            g_crash_steps++;
        /* for debug version, last step is CLI */
        case 12:
            panic_goto_cli();
            g_crash_steps++;
        default:
            break;
    }
#ifdef AOS_UND
    debug_reboot_reason_update(UND_STATIS_DEV_PANIC_ERR_REASON);
#endif
    RHINO_CPU_INTRPT_DISABLE();
#if defined (DEBUG)
    while (1);
#else
    hal_reboot();  /*release version*/
#endif
    RHINO_CPU_INTRPT_ENABLE();
}
#endif

#if (RHINO_CONFIG_ERR_DUMP > 0)
void debug_fatal_error(kstat_t err, char *file, int line)
{
    char prt_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \r\n";
    int  x;
    int *SP = RHINO_GET_SP();
    CPSR_ALLOC();

    krhino_sched_disable();

    printf("!!!!!!!!!! Fatal Error !!!!!!!!!!\r\n");
    printf("errno:%d , file:%s, line:%d\r\n", err, file, line);

    if ( SP != NULL )
    {
        print_str("========== Stack info ==========\r\n");
        for (x = 0; x < 16; x++) {
            k_int2str((int)&SP[x * 4], &prt_stack[8]);
            k_int2str(SP[x * 4 + 0], &prt_stack[21]);
            k_int2str(SP[x * 4 + 1], &prt_stack[32]);
            k_int2str(SP[x * 4 + 2], &prt_stack[43]);
            k_int2str(SP[x * 4 + 3], &prt_stack[54]);
            print_str(prt_stack);
        }
    }

#if (RHINO_CONFIG_MM_TLF > 0)
    print_str("========== Heap Info  ==========\r\n");
    debug_mm_overview(print_str);
#endif

    print_str("========== Task Info  ==========\r\n");
    debug_task_overview(print_str);

#if (DEBUG_CONFIG_BACKTRACE > 0)
    debug_backtrace_now();
#endif

#ifdef AOS_UND
    debug_reboot_reason_update(UND_STATIS_DEV_FATAL_ERR_REASON);
#endif

    /*panic_goto_cli();*/

    RHINO_CPU_INTRPT_DISABLE();
    while (1) {
        /* aos_reboot(); */
    }
    RHINO_CPU_INTRPT_ENABLE();
}
#endif

#ifdef AOS_UND
static debug_reason_t g_debug_reason;

void debug_reboot_reason_update(unsigned int reason)
{
    int ret, len;

    if (g_debug_reason.update_reason == reason)
        return;

    len = sizeof(debug_reason_t);

    g_debug_reason.update_reason = reason;

    ret = aos_kv_set(SYS_REBOOT_REASON, &g_debug_reason, len, 1);
    if (ret) {
        print_str("reboot reason set err\n");
    }
}

unsigned int debug_reboot_reason_get()
{
    int ret = -1, len;

    len = sizeof(debug_reason_t);

    ret = aos_kv_get(SYS_REBOOT_REASON, &g_debug_reason, &len);
    if (ret == -ENOENT ||
        g_debug_reason.update_reason > UND_STATIS_DEV_FATAL_ERR_REASON) {
        /* key not found*/
        g_debug_reason.return_reason = DEFAULT_REBOOT_REASON;
        g_debug_reason.update_reason = DEFAULT_REBOOT_REASON;
        aos_kv_set(SYS_REBOOT_REASON,  &g_debug_reason, len, 1);
        return DEFAULT_REBOOT_REASON;
    }

    if (g_debug_reason.return_reason != g_debug_reason.update_reason ||
        g_debug_reason.update_reason != DEFAULT_REBOOT_REASON) {
        g_debug_reason.return_reason = g_debug_reason.update_reason;
        g_debug_reason.update_reason = DEFAULT_REBOOT_REASON;
        ret = aos_kv_set(SYS_REBOOT_REASON,  &g_debug_reason, len, 1);
        if (ret)
            return DEFAULT_REBOOT_REASON;
    }

    return g_debug_reason.return_reason;
}
#endif /* AOS_UND */



