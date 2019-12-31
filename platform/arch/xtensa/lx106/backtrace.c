#ifdef AOS_COMP_DEBUG

#include <stdio.h>
#include "k_api.h"
#include "debug_api.h"

#define BACK_TRACE_LIMIT 64

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

/* find current function caller, update PC and SP */
static int findRetAddr_Callee(int **pSP, char **pPC, char *RA)
{
    int  *SP = *pSP;
    char *PC = *pPC;
    int   isPush = 0;
    int   isRet  = 0;
    int   lmt, i, j;
    int   framesize = 0;

    /* stack use for callee function:
       1. "addi a1, a1, -N" to open stack frame, and "s32i a0, a1, N-4" to push
       RA
       2. "movi a9, N; sub a1, a1, a9" to open stack frame, N is a multiplier of
       16 binary code: "92 aN NN" "90 11 c0"
       3. "addi a1, a1, -N" to open stack frame, and do not push RA
       4. do not open frame, and do not push RA
       */

    lmt = checkPcValid(PC);
    if ( lmt == 0 )
    {
        return 0;
    }

    for (i = 0; i < lmt; i++) {
        /* find nearest "addi a1, a1, -N" */
        if (*(PC - i) == 0x12 && *(PC - i + 1) == 0xc1 &&
            (*(PC - i + 2)) % 16 == 0) {
            framesize = (int)(*(signed char *)(PC - i + 2));
            framesize /= -4;
            break;
        }
        /* find nearest "sub a1, a1, a9" */
        if (*(PC - i) == 0x90 && *(PC - i + 1) == 0x11 &&
            *(PC - i + 2) == 0xc0) {
            i += 3;
            if (*(PC - i) == 0x92 && (*(PC - i + 2)) % 16 == 0) {
                framesize = (int)(*(unsigned char *)(PC - i + 2));
                framesize += (*(unsigned char *)(PC - i + 1) - 0xa0) << 8;
                framesize /= 4;
                break;
            }
        }
        /* find "ret.n" */
        if (*(PC - i) == 0x0d && *(PC - i + 1) == 0xf0) {
            isRet++;
            continue;
        }
        /* find "s32i.n a0, a1, *" */
        if (*(PC - i) == 0x09 && (*(PC - i + 1)) % 16 == 1) {
            isPush++;
            continue;
        }
        /* find "s32i   a0, a1, *" */
        if (*(PC - i) == 0x02 && *(PC - i + 1) == 0x61) {
            isPush++;
            continue;
        }

    }

    if (isRet > 0 || framesize == 0) {
        *pPC = RA - 3;
        *pSP = SP;
    }
    else if (isPush == 0) {
        *pPC = RA - 3;
        *pSP = SP + framesize;
    }
    else {
        *pPC = ((char *)*(SP + framesize - 1)) - 3;
        *pSP = SP + framesize;
    }

    return 1;
}

/* find current function caller, update PC and SP */
static int findRetAddr_Caller(int **pSP, char **pPC)
{
    int  *SP = *pSP;
    char *PC = *pPC;
    char *RA;
    int   lmt, i;
    int   framesize = 0;

    /* func call ways:
       1. "addi a1, a1, -N" to open stack frame, N is a multiplier of 16
          binary code: "12 c1 N"
       2. "movi a9, N; sub a1, a1, a9" to open stack frame, N is a multiplier of
       16 binary code: "92 aN NN" "90 11 c0"
       3. ReturnAddr always be pushed in N-4
       */

    lmt = checkPcValid(PC);
    for (i = 0; i < lmt; i++) {
        /* find nearest "addi a1, a1, -N" */
        if (*(PC - i) == 0x12 && *(PC - i + 1) == 0xc1 &&
            (*(PC - i + 2)) % 16 == 0) {
            framesize = (int)(*(signed char *)(PC - i + 2));
            framesize /= -4;
            break;
        }
        /* find nearest "sub a1, a1, a9" */
        if (*(PC - i) == 0x90 && *(PC - i + 1) == 0x11 &&
            *(PC - i + 2) == 0xc0) {
            i += 3;
            if (*(PC - i) == 0x92 && (*(PC - i + 2)) % 16 == 0) {
                framesize = (int)(*(unsigned char *)(PC - i + 2));
                framesize += (*(unsigned char *)(PC - i + 1) - 0xa0) << 8;
                framesize /= 4;
                break;
            }
        }
    }

    if (framesize == 0) {
        return 0;
    }

    *pSP = SP + framesize;
    *pPC = ((char *)*(SP + framesize - 1)) - 3;

    return 1;
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

/* printf call stack for context, return lvl */
int backtraceContext(char *PC, char *LR, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    int  lvl       = 0;
    char prtbuff[] = "backtrace : 0x         \n";

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    print_func("======= Call stack Begin =======\n");
    k_int2str((int)PC, &prtbuff[14]);

    print_func(prtbuff);
    if (0 == checkPcValid(PC)) {
        /* invalid pc, set Return Addr as pc */
        PC = LR;
        k_int2str((int)PC, &prtbuff[14]);
        print_func(prtbuff);
    } else {
        findRetAddr_Callee(&SP, &PC, LR);
        k_int2str((int)PC, &prtbuff[14]);
        print_func(prtbuff);
    }
    lvl++;

    while (lvl < BACK_TRACE_LIMIT) {
        if (0 == findRetAddr_Caller(&SP, &PC)) {
            break;
        }
        lvl++;
        __asm__ volatile("" ::: "memory"); // for gcc bug
        if (PC + 3 == (char *)krhino_task_deathbed) {
            print_func("backtrace : ^task entry^\n");
            break;
        }
        k_int2str((int)PC, &prtbuff[14]);
        print_func(prtbuff);
    }

    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    int  *SP;
    int   lvl       = 0;
    char  prtbuff[] = "backtrace : 0x        \n";

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    getPCnSP(&PC, &SP);

    print_func("======= Call stack Begin =======\n");
    while (lvl < BACK_TRACE_LIMIT) {
        if (0 == findRetAddr_Caller(&SP, &PC)) {
            break;
        }
        lvl++;
        __asm__ volatile("" ::: "memory"); // for gcc bug
        if (PC + 3 == (char *)krhino_task_deathbed) {
            lvl++;
            print_func("backtrace : ^task entry^\n");
            break;
        }
        k_int2str((int)PC, &prtbuff[14]);
        print_func(prtbuff);
    }

    print_func("======== Call stack End ========\n");
    return lvl;
}

/* printf call stack for task
   return levels of call stack */
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    ktask_t *task;

    if (print_func == NULL) {
        print_func = ets_printf;
    }

    task = krhino_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }

    getPLSfromCtx(task->task_stack, &PC, &LR, &SP);

    print_func("TaskName  : %s\n", taskname);
    return backtraceContext(PC, LR, SP, print_func);
}

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    return 0;
}

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...))
{
    backtraceContext(PC, LR, SP, print_func);
    return 1;
}
#endif

