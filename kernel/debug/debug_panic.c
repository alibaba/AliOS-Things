/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include "debug_api.h"
#include "aos/cli.h"
#include "aos/kernel.h"
#ifdef AOS_COMP_CLI
#include "cli_api.h"
#endif

extern void hal_reboot(void);

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
#if (DEBUG_CONFIG_BACKTRACE > 0)
extern int  panicBacktraceCaller(char *PC, int *SP,
                                 int (*print_func)(const char *fmt, ...));
extern int  panicBacktraceCallee(char *PC, int *SP, char *LR,
                                 int (*print_func)(const char *fmt, ...));
#endif

/* how many steps has finished when crash */
volatile uint32_t g_crash_steps = 0;

#if defined (DEBUG)
static void panic_debug(void)
{
#if (defined (AOS_COMP_CLI) && (RHINO_CONFIG_KOBJ_LIST > 0))

    klist_t      *listnode;
    ktask_t      *task;

    extern void panic_cli_board_config(void);
    panic_cli_board_config();

    for (listnode = g_kobj_list.task_head.next;
         listnode != &g_kobj_list.task_head; listnode = listnode->next) {
        task = krhino_list_entry(listnode, ktask_t, task_stats_item);
        if (0 != strcmp("cli", task->task_name)) {
            krhino_task_suspend(task);
        }
    }

    krhino_sched_enable();

    debug_task_overview(print_str);

    /*suspend current task*/
    task = g_active_task[cpu_cur_get()];
    if (0 != strcmp("cli", task->task_name)) {
        krhino_task_suspend(task);
    }
#else
    while (1);
#endif
}
#endif


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

    krhino_sched_disable();

    if (context != NULL) {
        panicGetCtx(context, &PC, &LR, &SP);
    }

    /* g_crash_steps++ before panicHandler */
    if (g_crash_steps > 1) {
        print_str("......\r\n");
    }

    switch (g_crash_steps) {
        case 1:
            print_str("!!!!!!!!!! Exception  !!!!!!!!!!\r\n");
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
        case 3:
#if (DEBUG_CONFIG_BACKTRACE > 0)
            /* Backtrace: assume ReturnAddr is saved in stack when exception */
            if (SP != NULL) {
                print_str("========== Call stack ==========\r\n");
                lvl = panicBacktraceCaller(PC, SP, print_str);
                if (lvl > 0) {
                    /* trace success, jumpover next step */
                    g_crash_steps++;
                }
            }
#endif
            g_crash_steps++;
        case 4:
            if (g_crash_steps == 4) {
#if (DEBUG_CONFIG_BACKTRACE > 0)
                /* Backtrace: assume ReturnAddr is saved in LR when exception */
                if (SP != NULL) {
                    (void)panicBacktraceCallee(PC, SP, LR, print_str);
                }
#endif
                g_crash_steps++;
            }
        case 5:
#if (RHINO_CONFIG_MM_TLF > 0)
            print_str("========== Heap Info  ==========\r\n");
            debug_mm_overview(print_str);
#endif
            g_crash_steps++;
        case 6:
            print_str("========== Task Info  ==========\r\n");
            debug_task_overview(print_str);
            g_crash_steps++;
        case 7:
#if (RHINO_CONFIG_QUEUE > 0)
            print_str("========== Queue Info ==========\r\n");
            debug_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 8:
#if (RHINO_CONFIG_BUF_QUEUE > 0)
            print_str("======== Buf Queue Info ========\r\n");
            debug_buf_queue_overview(print_str);
#endif
            g_crash_steps++;
        case 9:
#if (RHINO_CONFIG_SEM > 0)
            print_str("=========== Sem Info ===========\r\n");
            debug_sem_overview(print_str);
#endif
            g_crash_steps++;
        case 10:
            print_str("!!!!!!!!!! dump end   !!!!!!!!!!\r\n");
            g_crash_steps++;
        default:
            break;
    }

#if defined (DEBUG)
    panic_debug(); /*debug version*/
#else
    hal_reboot();  /*release version*/
#endif
}
#endif

#if (DEBUG_CONFIG_ERRDUMP > 0)
void debug_fatal_error(kstat_t err, char *file, int line)
{
    char prt_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \r\n";
    int  x;
    int *SP = RHINO_GET_SP();

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

#if 0
#if (RHINO_CONFIG_QUEUE > 0)
    print_str("========== Queue Info ==========\r\n");
    debug_queue_overview(print_str);
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)
    print_str("======== Buf Queue Info ========\r\n");
    debug_buf_queue_overview(print_str);
#endif

#if (RHINO_CONFIG_SEM > 0)
    print_str("=========== Sem Info ===========\r\n");
    debug_sem_overview(print_str);
#endif
#endif
}
#endif


