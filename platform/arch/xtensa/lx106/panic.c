#include <k_api.h>
#include <k_overview.h>
#include "frxt/xtensa_api.h"

extern char _text_start[];
extern char _text_end[];
extern char _irom0_text_start[];
extern char _irom0_text_end[];

extern int ets_printf(const char *fmt, ...);

/* check if pc is valid, return 0 when illegel, other is offset */
static int panicCheckPcValid(char *pc)
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

/* find current function caller, update PC and SP */
static int panicFindRetAddr_callee(int **pSP, char **pPC, char *RA)
{
    int *       SP = *pSP;
    char *      PC = *pPC;
    int         lmt, i, j;
    signed char framesize = 0;

    /* stack use for callee function:
       1. "addi a1, a1, -N" to open stack frame, and "s32i a0, a1, N-4" to push
       RA
       2. "addi a1, a1, -N" to open stack frame, and do not push RA
       3. do not open frame, and do not push RA
       */

    lmt = panicCheckPcValid(PC);
    for (i = 0; i < lmt; i++) {
        /* find nearest "addi a1, a1, -N" */
        if (*(PC - i) == 0x12 && *(PC - i + 1) == 0xc1 &&
            (*(PC - i + 2)) % 16 == 0) {
            framesize = *(PC - i + 2);
            framesize /= -4;
            break;
        }
    }
    if (framesize == 0) {
        return 0;
    }

    if (PC - RA > 0 && PC - RA < i) {
        /* RA has changed in func, so find stack to get ReturnAddr */
        *pSP = SP + framesize;
        *pPC = ((char *)*(SP + framesize - 1)) - 3;
    } else {
        /* ReturnAddr is RA */
        *pPC = RA - 3;

        /* find "ret.n" */
        for (j = 0; j < i; j++) {
            if (*(PC - j) == 0x0d && *(PC - j + 1) == 0xf0) {
                break;
            }
        }
        if (i == j) {
            /* no ret.n finded, so SP is changed in function */
            *pSP = SP + framesize;
        } else {
            /* no ret.n finded, so SP is not changed in function */
            *pSP = SP;
        }
    }

    return 1;
}

/* find current function caller, update PC and SP */
static int panicFindRetAddr_caller(int **pSP, char **pPC)
{
    int *       SP = *pSP;
    char *      PC = *pPC;
    char *      RA;
    int         lmt, i;
    signed char framesize = 0;

    /* func call ways:
       1. "addi a1, a1, -N" to open stack frame, N is a multiplier of 16
          binary code: "12 c1 N"
       2. ReturnAddr always be pushed in N-4
       */

    lmt = panicCheckPcValid(PC);
    for (i = 0; i < lmt; i++) {
        /* find nearest "addi a1, a1, -N" */
        if (*(PC - i) == 0x12 && *(PC - i + 1) == 0xc1 &&
            (*(PC - i + 2)) % 16 == 0) {
            framesize = *(PC - i + 2);
            framesize /= -4;
            break;
        }
    }

    if (framesize == 0) {
        return 0;
    }

    *pSP = SP + framesize;
    *pPC = ((char *)*(SP + framesize - 1)) - 3;

    return 1;
}


void panicHandler(XtExcFrame *frame)
{
    /* how many steps has finished when crash */
    static uint32_t g_crash_steps = 0;

    static char g_panic_info[] = "PC       0x         \n"
                                 "PS       0x         \n"
                                 "A0       0x         \n"
                                 "A1       0x         \n"
                                 "A2       0x         \n"
                                 "A3       0x         \n"
                                 "A4       0x         \n"
                                 "A5       0x         \n"
                                 "A6       0x         \n"
                                 "A7       0x         \n"
                                 "A8       0x         \n"
                                 "A9       0x         \n"
                                 "A10      0x         \n"
                                 "A11      0x         \n"
                                 "A12      0x         \n"
                                 "A13      0x         \n"
                                 "A14      0x         \n"
                                 "A15      0x         \n"
                                 "SAR      0x         \n"
                                 "EXCCAUSE 0x         \n"
                                 "EXCVADDR 0x         \n";
    static char g_panic_stack[] =
      "stack(0x        ): 0x         0x         0x         0x         \n";

    static char g_panic_call[] = "backtrace : 0x         \n";

    int * regs = (int *)frame;
    int   x, y;
    int * SP = (int *)regs[4];
    char *PC = (char *)regs[1];

    /* avoid to use printf, because printf depend on malloc,
       malloc may trig another exception */

    /* NMI can interrupt exception. */

    vPortETSIntrLock();

    g_crash_steps++;
    if (g_crash_steps > 1) {
        ets_printf("double exception occur!\n");
    }

    switch (g_crash_steps) {
        case 1:
            ets_printf("!!!!!!!!!! Exception  !!!!!!!!!!\n");
            ets_printf("========== Regs info  ==========\n");
            for (x = 0; x < 21; x++) {
                k_int2str(regs[x + 1], &g_panic_info[21 * x + 11]);
            }
            ets_printf(g_panic_info);
            g_crash_steps++;
        case 2:
            ets_printf("========== Stack info ==========\n");
            for (x = 0; x < 16; x++) {
                k_int2str((int)&SP[x * 4], &g_panic_stack[8]);
                k_int2str(SP[x * 4 + 0], &g_panic_stack[21]);
                k_int2str(SP[x * 4 + 1], &g_panic_stack[32]);
                k_int2str(SP[x * 4 + 2], &g_panic_stack[43]);
                k_int2str(SP[x * 4 + 3], &g_panic_stack[54]);
                ets_printf(g_panic_stack);
            }
            g_crash_steps++;
        case 3:
            ets_printf("========== Call stack ==========\n");
            k_int2str((int)PC, &g_panic_call[14]);
            ets_printf(g_panic_call);
            if (0 == panicCheckPcValid(PC)) {
                /* invalid pc, set Return Addr as pc */
                PC = (char *)regs[3];
                k_int2str((int)PC, &g_panic_call[14]);
                ets_printf(g_panic_call);
            } else {
                panicFindRetAddr_callee(&SP, &PC, (char *)regs[3]);
                k_int2str((int)PC, &g_panic_call[14]);
                ets_printf(g_panic_call);
            }
            for (x = 0; x < 32; x++) {
                if (0 == panicFindRetAddr_caller(&SP, &PC)) {
                    break;
                }
                __asm__ volatile("" ::: "memory"); // for gcc bug
                if (PC + 3 == (char *)krhino_task_deathbed) {
                    ets_printf("backtrace : ^task entry^\n");
                    break;
                }
                k_int2str((int)PC, &g_panic_call[14]);
                ets_printf(g_panic_call);
            }
            g_crash_steps++;
        case 4:
#if (RHINO_CONFIG_MM_TLF > 0)
            ets_printf("========== Heap Info  ==========\n");
            krhino_mm_overview(ets_printf);
#endif
            g_crash_steps++;
        case 5:
            ets_printf("========== Task Info  ==========\n");
            krhino_task_overview(ets_printf);
            g_crash_steps++;
        case 6:
#if (RHINO_CONFIG_QUEUE > 0)
            ets_printf("========== Queue Info ==========\n");
            krhino_queue_overview(ets_printf);
#endif
            g_crash_steps++;
        case 7:
#if (RHINO_CONFIG_BUF_QUEUE > 0)
            ets_printf("======== Buf Queue Info ========\n");
            krhino_buf_queue_overview(ets_printf);
#endif
            g_crash_steps++;
        case 8:
#if (RHINO_CONFIG_SEM > 0)
            ets_printf("=========== Sem Info ===========\n");
            krhino_sem_overview(ets_printf);
#endif
            g_crash_steps++;
        case 9:
            ets_printf("!!!!!!!!!! dump end   !!!!!!!!!!\n");
            g_crash_steps++;
        default:
            break;
    }

    while (1)
        ;
}
