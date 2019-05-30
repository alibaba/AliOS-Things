/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include "k_dbg_api.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#if (RHINO_CONFIG_PANIC > 0)
#include "k_compiler.h"
#endif

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

#if (RHINO_CONFIG_PANIC > 0)

/* follow functions should defined by arch\...\panic_c.c */
extern void panicShowRegs(void *context,
                          int (*print_func)(const char *fmt, ...));
extern void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP);
extern int  panicBacktraceCaller(char *PC, int *SP,
                                 int (*print_func)(const char *fmt, ...));
extern int  panicBacktraceCallee(char *PC, int *SP, char *LR,
                                 int (*print_func)(const char *fmt, ...));

/* how many steps has finished when crash */
volatile uint32_t g_crash_steps = 0;

static void panic_goto_cli(void)
{
#if 0 /* (defined (CONFIG_AOS_CLI) && (RHINO_CONFIG_SYSTEM_STATS > 0)) */
    klist_t      *listnode;
    ktask_t      *task;

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (0 != strcmp("cli", task->task_name)) {
            krhino_task_suspend(task);
        }
    }

    krhino_sched_enable();

    krhino_task_overview(print_str);

    /*suspend current task*/
    task = g_active_task[cpu_cur_get()];
    if (0 != strcmp("cli", task->task_name)) {
        krhino_task_suspend(task);
    }
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
#if (RHINO_CONFIG_BACKTRACE > 0)
    int lvl;
#endif
    static int  *SP = NULL;
    static char *PC = NULL;
    static char *LR = NULL;
    CPSR_ALLOC();

    krhino_sched_disable();

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
#if (RHINO_CONFIG_BACKTRACE > 0)
        /* 3 steps, 3 ways to try backtrace */
        case 3:
            /* Backtrace 1st try: assume ReturnAddr is saved in stack when exception */
            if (SP != NULL) {
                print_str("========== Call stack ==========\r\n");
                lvl = panicBacktraceCaller(PC, SP, print_str);
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
                    lvl = panicBacktraceCallee(PC, SP, LR, print_str);
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
                    (void)panicBacktraceCaller(LR, SP, print_str);
                }
                g_crash_steps++;
            }
#else
            g_crash_steps = 6;
#endif
#if (RHINO_CONFIG_PANIC_OVERVIEW > 0)
        case 6:
#if (RHINO_CONFIG_MM_TLF > 0)
            print_str("========== Heap Info  ==========\r\n");
            krhino_mm_overview(print_str);
#endif
            g_crash_steps++;
        case 7:
            print_str("========== Task Info  ==========\r\n");
            krhino_task_overview(print_str);
            g_crash_steps++;
        case 8:
#if (RHINO_CONFIG_QUEUE > 0)
            print_str("========== Queue Info ==========\r\n");
            krhino_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 9:
#if (RHINO_CONFIG_BUF_QUEUE > 0)
            print_str("======== Buf Queue Info ========\r\n");
            krhino_buf_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 10:
#if (RHINO_CONFIG_SEM > 0)
            print_str("=========== Sem Info ===========\r\n");
            krhino_sem_overview(print_str);
#endif
            g_crash_steps++;
#else
            g_crash_steps = 11;
#endif
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


    RHINO_CPU_INTRPT_DISABLE();
    while (1) {
        /* aos_reboot(); */
    }
    RHINO_CPU_INTRPT_ENABLE();
}


void debug_fatal_error(kstat_t err, char *file, int line)
{
    char prt_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \r\n";
    int  x;
    int *SP = (int *)RHINO_GET_SP();
    CPSR_ALLOC();

    krhino_sched_disable();

    printf("!!!!!!!!!! Fatal Error !!!!!!!!!!\r\n");
    printf("errno:%d , file:%s, line:%d\r\n", err, file, line);

    if ( SP != NULL ) {
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

    print_str("========== Heap Info  ==========\r\n");
    krhino_mm_overview(print_str);

    print_str("========== Task Info  ==========\r\n");
    krhino_task_overview(print_str);

#if (RHINO_CONFIG_BACKTRACE > 0)
    krhino_backtrace_now();
#endif

    panic_goto_cli();

    RHINO_CPU_INTRPT_DISABLE();
    while (1) {
        /* aos_reboot(); */
    }
    RHINO_CPU_INTRPT_ENABLE();
}
#endif