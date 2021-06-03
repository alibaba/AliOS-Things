/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include "debug_api.h"
#include "k_compiler.h"
#include "aos/kernel.h"
#include "aos/debug.h"
#include "aos/errno.h"

int backtrace_now(int (*print_func)(const char *fmt, ...));
void debug_panic_backtrace(char *PC, int *SP, char *LR,
                           int (*print_func)(const char *fmt, ...));

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif

/* reboot reason*/
#define DEFAULT_REBOOT_REASON DEBUG_REBOOT_REASON_REPOWER

#define SYS_REBOOT_REASON "reboot reason"
#ifndef panic_print
#define panic_print printk
#endif
#define panic_print_direct printk_direct

#if (RHINO_CONFIG_CPU_NUM > 1)
kspinlock_t g_panic_print_lock = {KRHINO_SPINLOCK_FREE_VAL, 0, 0};
#endif

#if DEBUG_ULOG_FLUSH
extern void uring_fifo_flush(void);
void debug_log_flush(void)
{
    uring_fifo_flush();
}
#endif

/* use cli in panic depends on mcu*/
__attribute__((weak)) void alios_cli_panic_hook()
{
    return;
}

extern void hal_reboot(void);
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
#define DEBUG_PANIC_STEP_MAX    32
volatile uint32_t g_crash_steps = 0;
volatile uint32_t g_crash_by_NMI = 0;
volatile uint32_t g_crash_not_reboot = 0;

void debug_cpu_stop(void)
{
#if (RHINO_CONFIG_CPU_NUM > 1)
    cpu_freeze_others();
#endif
    krhino_sched_disable();
}

void debug_cpu_goto_cli(void)
{
    g_crash_not_reboot = OS_PANIC_NOT_REBOOT;
}

uint32_t debug_cpu_in_crash(void)
{
    return g_crash_steps;
}

void panicNmiFlagSet()
{
    g_crash_by_NMI = OS_PANIC_BY_NMI;
}

int panicNmiFlagCheck()
{
    return (g_crash_by_NMI == OS_PANIC_BY_NMI);
}

static void panic_goto_cli(void)
{
    cpu_intrpt_save();

    extern uint8_t g_sched_lock[];
    if (0 == g_sched_lock[cpu_cur_get()]) {
        g_sched_lock[cpu_cur_get()]++;
    }

    extern int32_t g_cli_direct_read;
    g_cli_direct_read = 1;

    alios_cli_panic_hook();
#if AOS_COMP_CLI
    extern void cli_main_panic(void);
    cli_main_panic();
#endif
    while (1);
}

/* should exeception be restored?
   reture 1 YES, 0 NO*/
int panicRestoreCheck(void)
{
   return 0;
}

static void debug_panic_end(void)
{
#if !DEBUG_PANIC_CLI
    if (g_crash_not_reboot == OS_PANIC_NOT_REBOOT) {
        panic_goto_cli();
    } else if (panicNmiFlagCheck() == 0) {
        hal_reboot();
    } else { /* '$' is also effective in release version*/
        panic_goto_cli();
    }
#else /* debug version*/
    panic_goto_cli();
#endif
}

void stack_dump(cpu_stack_t *stack, uint32_t size)
{
    uint32_t zero_cnt, zero_prt, idx;

    for (idx = 0, zero_cnt = 0, zero_prt = 0; idx < size; idx += 4) {
        if (stack[idx] == 0 && stack[idx + 1] == 0
            && stack[idx + 2] == 0 && stack[idx + 3] == 0) {
            zero_cnt++;
            if (zero_cnt == 1) {
                panic_print(".........................( All Zeros ).........................\r\n");
                zero_prt = 1;
                continue;
            }
            if (zero_prt == 1) {
                continue;
            }
        }
        panic_print("(0x%08X): 0x%08X 0x%08X 0x%08X 0x%08X\r\n",
                    &stack[idx],
                    (void *)stack[idx], (void *)stack[idx + 1], (void *)stack[idx + 2], (void *)stack[idx + 3]);
        zero_cnt = 0;
        zero_prt = 0;
    }
}

void debug_cur_task_stack_dump(void)
{
    cpu_stack_t *stack;
    uint32_t     stack_size;

    ktask_t *task = g_active_task[cpu_cur_get()];
    stack         = task->task_stack_base;
    stack_size    = task->stack_size;

    if (stack != NULL) {
        panic_print("========== Stack info ==========\r\n");
        stack_dump(stack, stack_size);
    }
}

void debug_cur_task_show(void)
{
    ktask_t *task;
    uint8_t  time_buffer[30];

    long long ms   = aos_calendar_localtime_get();
    time_t rawtime = ms / 1000;
    struct tm *tm  = localtime(&rawtime);

    memset(time_buffer, 0, sizeof(time_buffer));

    if (tm) {
        strftime((char *)time_buffer, sizeof(time_buffer), "%F %H:%M:%S", tm);
        panic_print("crash   time : %s\r\n", time_buffer);
    }

    /* output crash task's name */
    task = g_active_task[cpu_cur_get()];
    if (task->task_name != NULL) {
        panic_print("current task : %s\r\n", task->task_name);
    } else {
        panic_print("cur task name is NULL\r\n");
    }
}

#if RHINO_CONFIG_MM_DEBUG
static void debug_print_block(k_mm_list_t *b, int (*print_func)(const char *fmt, ...))
{
    if (!b) {
        return;
    }

    if (print_func == NULL) {
        print_func = panic_print_direct;
    }

    print_func("0x%08x ", (uintptr_t)b);

    if (b->buf_size & MM_BUFF_FREE) {
        if (b->dye != MM_DYE_FREE) {
            print_func("!");
        } else {
            print_func(" ");
        }
        print_func("free ");
    } else {
        if (b->dye != MM_DYE_USED) {
            print_func("!");
        } else {
            print_func(" ");
        }
        print_func("used ");
    }

    if (MM_GET_BUF_SIZE(b)) {
        print_func(" %6lu ", (unsigned long)MM_GET_BUF_SIZE(b));
    } else {
        print_func(" sentinel ");
    }

    if (b->buf_size & MM_BUFF_FREE) {
        if (b->dye != MM_DYE_FREE) {
            print_func(" %8x ", b->dye);
        } else {
            print_func("  OK ");
        }
    } else {
        if (b->dye != MM_DYE_USED) {
            print_func(" %8x ", b->dye);
        } else {
            print_func("  OK ");
        }
    }

    print_func(" 0x%-8x ", b->owner);

#if (RHINO_CONFIG_MM_TRACE_LVL > 0)
    /* If double free, print last alloc trace maybe useful.
    This info is not useful if this mem alloc-and-freed by another module between.
    */
    //if ((b->buf_size & MM_BUFF_FREE) == 0)
    {
        int idx;
        print_func(" (%p", b->trace[0]);
        for (idx = 1 ; idx < RHINO_CONFIG_MM_TRACE_LVL ; idx++) {
            print_func(" <- %p", b->trace[idx]);
        }
        print_func(")");
    }
#endif

    print_func("\r\n");
}


static bool debug_blk_damaged(k_mm_list_t *mm_list)
{
    if (!mm_list) {
        return false;
    }

    if (mm_list->dye != MM_DYE_USED && mm_list->dye != MM_DYE_FREE) {
        return true;
    }

    if (mm_list->buf_size & MM_BUFF_FREE) {
        if (mm_list->dye != MM_DYE_FREE) {
            return true;
        }
    } else {
        if (mm_list->dye != MM_DYE_USED) {
            return true;
        }
    }
    /* detect bufsize skip lastblk */
    if (mm_list->owner != MM_LAST_BLK_MAGIC) {
        k_mm_list_t *next_b = MM_GET_NEXT_BLK(mm_list);
        if (next_b->dye != MM_DYE_USED && next_b->dye != MM_DYE_FREE) {
            return true;
        }
    }

    return false;
}

void debug_dump_mm_error(k_mm_head *mmhead, int (*print_func)(const char *fmt, ...))
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur, *prev, *pprev;

    if (!mmhead) {
        return;
    }

    if (print_func == NULL) {
        print_func = panic_print_direct;
    }

    print_func("ALL BLOCKS\r\n");
    print_func("Blk_Addr    Stat     Len  Chk      Caller    Point\r\n");
    reginfo = mmhead->regioninfo;
    prev = NULL;
    pprev = NULL;
    while (reginfo) {
        cur = MM_GET_THIS_BLK(reginfo);
        while (cur) {
            if (debug_blk_damaged(cur)) {
                debug_print_block(pprev, print_func);
                debug_print_block(prev, print_func);
                debug_print_block(cur, print_func);
            }

            if (MM_GET_BUF_SIZE(cur)) {
                next = MM_GET_NEXT_BLK(cur);
            } else {
                next = NULL;
            }
            pprev = prev;
            prev = cur;
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }
}

void dump_mm_all_error_block(void *pmm_head, int (*print_func)(const char *fmt, ...))
{
    if (print_func == NULL) {
        print_func = panic_print_direct;
    }

    print_func("g_kmm_head = %8x\r\n", (unsigned int)pmm_head);
    /* kernel and user space use the same mm head file */
    debug_dump_mm_error(pmm_head, print_func);
}

void dump_mm_sys_error_info(int (*print_func)(const char *fmt, ...))
{
    void *pmm_head = g_kmm_head;
    print_func("kernel space mem layout:\r\n");
    dump_mm_all_error_block(pmm_head, print_func);
}
#endif /* RHINO_CONFIG_MM_DEBUG */

void fiqafterpanicHandler(void *context)
{
    static int  *SP = NULL;
    static char *PC = NULL;
    static char *LR = NULL;

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(&g_panic_print_lock);
#endif

    panic_print("\r\n!!!!!!!!!! core %d Enter fiq  !!!!!!!!!!\r\n", cpu_cur_get());
    if (context != NULL) {
        panicGetCtx(context, &PC, &LR, &SP);
    }
    panicShowRegs(context, panic_print);

    debug_panic_backtrace(PC, SP, LR, panic_print);

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock(&g_panic_print_lock);
#endif

    return;
}


/* fault/exception entry
   notice: this function maybe reentried by double exception
   first exception, input context
   second exception, input NULL  */
void panicHandler(void *context)
{
    static int  *SP = NULL;
    static char *PC = NULL;
    static char *LR = NULL;
    kstat_t stat_save;

#if RHINO_CONFIG_MM_DEBUG
    g_mmlk_cnt = 0;
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(&g_panic_print_lock);
#endif

    stat_save  = g_sys_stat;
    g_sys_stat = RHINO_STOPPED;

    /* g_crash_steps++ before panicHandler */
    if (g_crash_steps > 1 && g_crash_steps < DEBUG_PANIC_STEP_MAX) {
        panic_print("......\r\n");
    }

    switch (g_crash_steps) {
        case 1:
            if (panicNmiFlagCheck()) { /*for $#@! feature*/
                panic_print("\r\n!!!!!!!! Stopped by '$#@!' !!!!!!!!\r\n");
            }
            panic_print("\r\n!!!!!!!!!! Exception  !!!!!!!!!!\r\n");

            debug_cur_task_show();

            if (context != NULL) {
                panicGetCtx(context, &PC, &LR, &SP);
            }

            panicShowRegs(context, panic_print);
            g_crash_steps++;
        case 2:
            panic_print("========== Call stack ==========\r\n");
            debug_panic_backtrace(PC, SP, LR, panic_print);
            g_crash_steps++;
        case 3:
#if (RHINO_CONFIG_MM_TLF > 0)
            panic_print("========== Heap Info  ==========\r\n");
            debug_mm_overview(panic_print);
#endif
            g_crash_steps++;
        case 4:
            panic_print("========== Task Info  ==========\r\n");
            debug_task_overview(panic_print);
            g_crash_steps++;
        case 5:
#if (RHINO_CONFIG_QUEUE > 0)
            panic_print("========== Queue Info ==========\r\n");
            debug_queue_overview(panic_print);
#endif
            g_crash_steps++;
        case 6:
#if (RHINO_CONFIG_BUF_QUEUE > 0)
            panic_print("======== Buf Queue Info ========\r\n");
            debug_buf_queue_overview(panic_print);
#endif
            g_crash_steps++;
        case 7:
#if (RHINO_CONFIG_SEM > 0)
            panic_print("========= Sem Waiting ==========\r\n");
            debug_sem_overview(panic_print);
#endif
            g_crash_steps++;
        case 8:
            panic_print("======== Mutex Waiting =========\r\n");
            debug_mutex_overview(panic_print);
            g_crash_steps++;
        case 9:
#if RHINO_CONFIG_MM_DEBUG
            panic_print("======== all memory error blocks =========\r\n");
            dump_mm_sys_error_info(panic_print);
#endif
            g_crash_steps++;
        case 10:
            if (SP != NULL) {
                debug_cur_task_stack_dump();
            }
            g_crash_steps++;
        case 11:
            panic_print("!!!!!!!!!! dump end   !!!!!!!!!!\r\n");
            g_crash_steps++;
        default:
            break;
    }

    g_crash_steps = DEBUG_PANIC_STEP_MAX;

#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_unlock(&g_panic_print_lock);
#endif

#if DEBUG_LAST_WORD_ENABLE
    debug_reboot_reason_update(DEBUG_REBOOT_REASON_PANIC);
#endif

#if DEBUG_ULOG_FLUSH
    debug_log_flush();
#endif

    debug_panic_end();

    g_sys_stat = stat_save;
    g_crash_steps   = 0;
    g_crash_by_NMI  = 0;
}

void debug_fatal_error(kstat_t err, char *file, int line)
{
    void *pmm_head = NULL;
    (void)pmm_head;

#if (RHINO_CONFIG_CPU_NUM > 1)
    cpu_freeze_others();
#endif

    krhino_sched_disable();

    g_crash_steps = 1;

    panic_print("!!!!!!!!!! Fatal Error !!!!!!!!!!\r\n");

    if (err == RHINO_TASK_STACK_OVF) {
        panic_print("Task : %s  Stack Overflow!\r\n", g_active_task[cpu_cur_get()]->task_name);
    }

    debug_cur_task_show();

#if (RHINO_CONFIG_MM_TLF > 0)
    panic_print("========== Heap Info  ==========\r\n");
    debug_mm_overview(panic_print);
#endif

    panic_print("========== Task Info  ==========\r\n");
    debug_task_overview(panic_print);

    //debug_backtrace_now();
    backtrace_now(panic_print);

#if RHINO_CONFIG_MM_DEBUG
    panic_print("======== all memory error blocks =========\r\n");
    dump_mm_sys_error_info(panic_print);
#endif

    debug_cur_task_stack_dump();

    panic_print("!!!!!!!!!! dump end   !!!!!!!!!!\r\n");

#if DEBUG_LAST_WORD_ENABLE
    debug_reboot_reason_update(DEBUG_REBOOT_REASON_FATAL_ERR);
#endif

#if DEBUG_ULOG_FLUSH
    debug_log_flush();
#endif

    debug_panic_end();

    g_crash_steps = 0;
}

void debug_init(void)
{
#if AOS_COMP_CLI
    debug_cli_cmd_init();
#endif

#if DEBUG_LAST_WORD_ENABLE
    debug_lastword_init();
#endif
}
