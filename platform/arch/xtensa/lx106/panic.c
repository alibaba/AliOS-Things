#include "k_dbg_api.h"
#include "backtrace.h"
#include "frxt/xtensa_api.h"

extern int               ets_printf(const char *fmt, ...);
extern volatile uint32_t g_crash_steps;

int print_str(const char *fmt, ...)
{
    /* panic print do not need fmt parse(like %d etg) */
    return ets_printf(fmt);
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int  x;
    int *regs       = (int *)context; // refer to XtExcFrame
    char prt_info[] = "PC       0x        \n"
                      "PS       0x        \n"
                      "A0       0x        \n"
                      "A1       0x        \n"
                      "A2       0x        \n"
                      "A3       0x        \n"
                      "A4       0x        \n"
                      "A5       0x        \n"
                      "A6       0x        \n"
                      "A7       0x        \n"
                      "A8       0x        \n"
                      "A9       0x        \n"
                      "A10      0x        \n"
                      "A11      0x        \n"
                      "A12      0x        \n"
                      "A13      0x        \n"
                      "A14      0x        \n"
                      "A15      0x        \n"
                      "SAR      0x        \n"
                      "EXCCAUSE 0x        \n"
                      "EXCVADDR 0x        \n";

    ets_printf("========== Regs info  ==========\n");
    for (x = 0; x < 21; x++) {
        k_int2str(regs[x + 1], &prt_info[20 * x + 11]);
    }
    ets_printf(prt_info);
}

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    int *regs = (int *)context; // refer to XtExcFrame

    *pPC = (char *)regs[1];
    *pLR = (char *)regs[3];
    *pSP = (int *)regs[4];
}

int panicBacktraceCaller(char *PC, int *SP,
                         int (*print_func)(const char *fmt, ...))
{
    return 0;
}

int panicBacktraceCallee(char *PC, int *SP, char *LR,
                         int (*print_func)(const char *fmt, ...))
{
    backtraceContext(PC, LR, SP, print_func);
    return 1;
}

void xtensaPanic(void *context)
{
    vPortETSIntrLock();
    if(g_crash_steps == 0x87654321) {
        while (1);
    }

    g_crash_steps++;
    if (g_crash_steps > 1) {
        ets_printf("double exception occur!\n");
        context = NULL;
    }

#if (RHINO_CONFIG_PANIC > 0)
    panicHandler(context);
#else
    ets_printf("exception occur!\n");
    while (1)
        ;
#endif
}
