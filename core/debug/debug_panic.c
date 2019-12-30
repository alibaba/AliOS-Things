/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include "debug_api.h"
#include "debug_panic.h"
#include "k_compiler.h"
#include "aos/kernel.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#include "cli_api.h"
#endif

#ifdef AOS_COMP_KV
#include "aos/kv.h"
#endif

#include "aos/errno.h"

/* reboot reason*/
#define DEFAULT_REBOOT_REASON DEBUG_REBOOT_REASON_REPOWER

#define SYS_REBOOT_REASON "reboot reason"

typedef struct {
    unsigned int return_reason;
    unsigned int update_reason;
} debug_reason_t;


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

#if (RHINO_CONFIG_USER_SPACE > 0)
#include "task_group.h"

typedef enum {
    PANIC_IN_KERNEL,
    PANIC_IN_USER_KERNEL,
    PANIC_IN_USER,
    PANIC_UNDEFINED
} uspace_panic_type;

volatile uspace_panic_type g_uspace_panic_type = PANIC_UNDEFINED;

__attribute__((always_inline)) RHINO_INLINE unsigned int get_control(void)
{
    unsigned int ctl;
    asm volatile("mrs %0, control":"=r"(ctl));
    return ctl;
}

void uspace_type_show(void)
{
    ktask_t *task;
    task_group_t *group;

    task = krhino_cur_task_get();

    switch(g_uspace_panic_type) {
        case PANIC_IN_KERNEL:
            print_str("\r\nkernel space exception\r\n");
            break;
        case PANIC_IN_USER_KERNEL:
            print_str("\r\nuser space result exception in kernel\r\n");
            break;
         case PANIC_IN_USER:
            group = task->task_group;

            print_str("\r\nuser space app %s exception\r\n", group->tg_name);
            break;
        default:
            print_str("\r\ncan not identify usapce exception type\r\n");
            break;
    }
}
void panicGetUspaceCtx(void *context, char **PC, char **LR, int **SP)
{
    int *ptr = context - 32;

    *SP = context;
    /*  reference to svc */
    *PC = (char *)ptr[6];
    *LR = (char *)ptr[5];
}
#endif

static void panic_goto_cli(void)
{
    /* to be continiued..*/
}

/* should exeception be restored?
   reture 1 YES, 0 NO*/
int panicRestoreCheck(void)
{
   return 0;
}
#if (RHINO_CONFIG_USER_SPACE > 0)
static void backtrace(char *PC, int *SP, char *LR,
        int (*print_func)(const char *fmt, ...))
{
    int lvl;

    if (SP == NULL)
        return;

    lvl = backtrace_caller(PC, SP, print_str);
    if (lvl > 0) {
        return;
    } else {
        lvl = backtrace_callee(PC, SP, LR, print_str);
        if (lvl > 0) {
            return;
        } else {
            (void)backtrace_caller(LR, SP, print_str);
        }
    }
}
#endif

/* fault/exception entry
   notice: this function maybe reentried by double exception
   first exception, input context
   second exception, input NULL  */
void panicHandler(void *context)
{
    char prt_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \r\n";
    int x;

    int lvl;
    static int  *SP = NULL;
    static char *PC = NULL;
    static char *LR = NULL;

#if (RHINO_CONFIG_USER_SPACE > 0)
    static int  *U_SP = NULL;
    static char *U_PC = NULL;
    static char *U_LR = NULL;
#endif

    CPSR_ALLOC();

    /* g_crash_steps++ before panicHandler */
    if (g_crash_steps > 1 && g_crash_steps < DEBUG_PANIC_STEP_MAX) {
        print_str("......\r\n");
    }

    switch (g_crash_steps) {
        case 1:
            print_str("!!!!!!!!!! Exception  !!!!!!!!!!\r\n");
#if (RHINO_CONFIG_USER_SPACE > 0)
            uspace_type_show();
            if (g_uspace_panic_type == PANIC_IN_USER_KERNEL) {
                panicGetUspaceCtx(g_active_task[cpu_cur_get()]->task_ustack, &U_PC, &U_LR, &U_SP);
            }
#endif
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
#if (RHINO_CONFIG_USER_SPACE > 0)
            if (U_SP != NULL) {
                U_SP -= 8;
                print_str("========== Uspace Stack info ==========\r\n");
                for (x = 0; x < 16; x++) {
                    k_int2str((int)&U_SP[x * 4], &prt_stack[8]);
                    k_int2str(U_SP[x * 4 + 0], &prt_stack[21]);
                    k_int2str(U_SP[x * 4 + 1], &prt_stack[32]);
                    k_int2str(U_SP[x * 4 + 2], &prt_stack[43]);
                    k_int2str(U_SP[x * 4 + 3], &prt_stack[54]);
                    print_str(prt_stack);
                }
                U_SP += 8;
            }
            if (g_uspace_panic_type == PANIC_IN_USER_KERNEL) {
                print_str("========== Uspace backtrace  ==========\r\n");
                backtrace(U_PC, U_SP, U_LR, print_str);
            }
#endif /* #if (RHINO_CONFIG_USER_SPACE > 0) */
            g_crash_steps++;
            /*  3 steps, 3 ways to try backtrace */
        case 3:
            /*  Backtrace 1st try: assume ReturnAddr is saved in stack when exception */
            if (SP != NULL) {
                print_str("========== Call stack ==========\r\n");
                lvl = backtrace_caller(PC, SP, print_str);
                if (lvl > 0) {
                    /*  backtrace success, do not try other way */
                    g_crash_steps += 2;
                }
                /*  else, backtrace fail, try another way */
            }
            g_crash_steps++;
        case 4:
            if (g_crash_steps == 4) {
                /*  Backtrace 2nd try: assume ReturnAddr is saved in LR when exception */
                if (SP != NULL) {
                    lvl = backtrace_callee(PC, SP, LR, print_str);
                    if (lvl > 0) {
                        /*  backtrace success, do not try other way */
                        g_crash_steps += 1;
                    }
                    /*  else, backtrace fail, try another way */
                }
                g_crash_steps++;
            }
        case 5:
            if (g_crash_steps == 5) {
                /*  Backtrace 3rd try: assume PC is invalalb, backtrace from LR */
                if (SP != NULL) {
                    (void)backtrace_caller(LR, SP, print_str);
                }
                g_crash_steps++;
            }
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

#ifdef AOS_COMP_KV
    debug_reboot_reason_update(DEBUG_REBOOT_REASON_PANIC);
#endif

#if (RHINO_CONFIG_USER_SPACE > 0)
    if (g_uspace_panic_type == PANIC_IN_USER) {
        return; /* return to panic_gcc.S for process_exit */
    }
#endif

    RHINO_CPU_INTRPT_DISABLE();
#if defined (DEBUG)
    while (1);
#else
    hal_reboot();  /*release version*/
#endif
    RHINO_CPU_INTRPT_ENABLE();
}

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

    debug_backtrace_now();

#ifdef AOS_COMP_KV
    debug_reboot_reason_update(DEBUG_REBOOT_REASON_FATAL_ERR);
#endif

    /*panic_goto_cli();*/

    RHINO_CPU_INTRPT_DISABLE();
    while (1) {
        /* aos_reboot(); */
    }
    RHINO_CPU_INTRPT_ENABLE();
}

static debug_reason_t debug_reason;

void debug_reboot_reason_update(unsigned int reason)
{
#ifdef AOS_COMP_KV
    int ret, len;

    if (debug_reason.update_reason == reason)
        return;

    len = sizeof(debug_reason_t);

    debug_reason.update_reason = reason;

    ret = aos_kv_set(SYS_REBOOT_REASON, &debug_reason, len, 1);
    if (ret) {
        print_str("reboot reason set err\n");
    }
#endif
}

unsigned int debug_reboot_reason_get()
{
#ifdef AOS_COMP_KV
    int ret = -1, len;

    len = sizeof(debug_reason_t);

    ret = aos_kv_get(SYS_REBOOT_REASON, &debug_reason, &len);
    if (ret == -ENOENT ||
        debug_reason.update_reason > DEBUG_REBOOT_REASON_FATAL_ERR) {
        /* key not found*/
        debug_reason.return_reason = DEFAULT_REBOOT_REASON;
        debug_reason.update_reason = DEFAULT_REBOOT_REASON;
        aos_kv_set(SYS_REBOOT_REASON,  &debug_reason, len, 1);
        return DEFAULT_REBOOT_REASON;
    }

    if (debug_reason.return_reason != debug_reason.update_reason ||
        debug_reason.update_reason != DEFAULT_REBOOT_REASON) {
        debug_reason.return_reason = debug_reason.update_reason;
        debug_reason.update_reason = DEFAULT_REBOOT_REASON;
        ret = aos_kv_set(SYS_REBOOT_REASON,  &debug_reason, len, 1);
        if (ret)
            return DEFAULT_REBOOT_REASON;
    }

    return debug_reason.return_reason;
#else
    return DEFAULT_REBOOT_REASON;
#endif
}


