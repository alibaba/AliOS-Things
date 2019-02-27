#include <stdio.h>
#include "k_api.h"
#include "debug_api.h"

#define BACK_TRACE_LIMIT 64

#define Valid(a) ((a & 0x80000000) ? ((a & 0x3fffffff) | 0x40000000) : a)


/* defined in *.ld file */
extern char _text_start[];
extern char _text_end[];
extern char _irom0_text_start[];
extern char _irom0_text_end[];

extern int  ets_printf(const char *fmt, ...);
extern void getPLSfromCtx(void *context, char **PC, char **LR, int **SP);

/* check if pc is valid, return 0 when illegel, other is offset */
static int checkPcValid(char *pc)
{
    if ((unsigned int)(pc - _text_start) <
        (unsigned int)(_text_end - _text_start)) {
        return pc - _text_start;
    } else if ((unsigned int)(pc - _irom0_text_start) <
               (unsigned int)(_irom0_text_end - _irom0_text_start)) {
        return pc - _irom0_text_start;
    } else {
        return 0;
    }
}

__attribute__((__noinline__, __noclone__)) static void getPCnSP(char **PC,
                                                                int  **SP)
{
    int   a;
    char *lr;

    asm volatile("mov %0, a0" : "=r"(lr));
    *PC = lr;
    *SP = (int *)&a + 4;
}

static int windowed_register_backtrace(uint32_t pc, uint32_t sp,
                                       int (*print_func)(const char *fmt, ...))
{
    int       lvl = 0;
    uint32_t  backtrace_pc, backtrace_sp, psp;

    char prtbuff[] = "backtrace : 0x         \n";

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    backtrace_pc = pc;
    backtrace_sp = sp;

    while (lvl++ < BACK_TRACE_LIMIT) {
        psp = backtrace_sp;
        if ((backtrace_sp > 0x3fffffff0UL) || ((backtrace_sp & 0xf) != 0))
            break;

        backtrace_sp = *((uint32_t*) (backtrace_sp - 0x10 + 4));
        k_int2str((int)Valid((uint32_t)backtrace_pc -3), &prtbuff[14]);
        print_func(prtbuff);

        backtrace_pc = *((uint32_t *) (psp - 0x10));
        if (backtrace_pc < 0x40000000)
            break;

        if (backtrace_pc == (char *)krhino_task_deathbed) {
            print_func("backtrace : ^task entry^\n");
            break;
        }
    }
    return lvl;
}

/* printf call stack for context, return lvl */
int backtraceContext(char *PC, char *LR, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    int  lvl = 0;
    uint32_t  backtrace_pc, backtrace_sp;
    char prtbuff[] = "backtrace : 0x         \n";

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    print_func("======= Call stack Begin =======\n");
    k_int2str((int)Valid((uint32_t)PC), &prtbuff[14]);

    print_func(prtbuff);

    backtrace_pc = LR;
    backtrace_sp = SP;

    lvl = windowed_register_backtrace(backtrace_pc, backtrace_sp, print_func);
    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    int  *SP;
    int   lvl       = 0;

    uint32_t  backtrace_pc, backtrace_sp;

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    getPCnSP(&PC, &SP);

    print_func("======= Call stack Begin =======\n");

    backtrace_pc = PC;
    backtrace_sp = (uint32_t)SP + 32; /*call getPCnSP result to sp -32*/

    lvl = windowed_register_backtrace(backtrace_pc, backtrace_sp, print_func);
    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack for task */
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    int      lvl = 0;
    ktask_t *task;

    uint32_t  backtrace_pc, backtrace_sp;

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    task = debug_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }

    print_func("TaskName  : %s\n", taskname);
    print_func("======= Call stack Begin =======\n");

    SP = task->task_stack;

    backtrace_pc = *((uint32_t *) (SP - 0x10));
    backtrace_sp = SP;

    lvl = windowed_register_backtrace(backtrace_pc, backtrace_sp, print_func);
    print_func("======== Call stack End ========\n");

    return lvl;
}
