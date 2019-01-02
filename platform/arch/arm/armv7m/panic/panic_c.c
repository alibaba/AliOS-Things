/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_dbg_api.h"

#if (RHINO_CONFIG_BACKTRACE > 0)

#if defined(__CC_ARM)
#ifdef __BIG_ENDIAN
#error "Not support big-endian!"
#endif
#elif defined(__ICCARM__)
#if (__LITTLE_ENDIAN__ == 0)
#error "Not support big-endian!"
#endif
#elif defined(__GNUC__)
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error "Not support big-endian!"
#endif
#endif

#define FUNC_SIZE_LIMIT  0x2000
#define BACK_TRACE_LIMIT 64
#define LR_2_ADDR(lr)    ((char *)(((int)(lr)) & 0xfffffffe))

#if defined(__ICCARM__)
static unsigned int __builtin_popcount(unsigned int u)
{
    unsigned int ret = 0;
    while (u) {
        u = (u & (u - 1));
        ret++;
    }
    return ret;
}
#endif

void getPLSfromCtx(void *context, char **PC, char **LR, int **SP)
{
    int *ptr = context;
    int  exc_return;

    /* reference to cpu_task_stack_init */
    exc_return = ptr[8];

    if ((exc_return & 0x10) == 0x10) {
        *PC = (char *)ptr[15];
        *LR = (char *)ptr[14];
        *SP = ptr + 17;
    } else {
        *PC = (char *)ptr[31];
        *LR = (char *)ptr[30];
        *SP = ptr + 51;
    }
}

/* get "blx" or "bl" before LR, return offset */
static int backtraceFindLROffset(char *LR,
                                 int (*print_func)(const char *fmt, ...))
{
    unsigned short ins16;
    char           s_panic_call[] = "backtrace : 0x         \r\n";

    LR = LR_2_ADDR(LR);

    /* callstack bottom */
    if (((int)LR & 0xffffffe0) == 0xffffffe0) {
        /* EXC_RETURN, so here is callstack bottom of interrupt handler */
        if (print_func != NULL) {
            print_func("backtrace : ^interrupt^\r\n");
        }
        return 0;
    }

    if (LR == LR_2_ADDR(&krhino_task_deathbed)) {
        /* task delete, so here is callstack bottom of task */
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 0;
    }

    ins16 = *(unsigned short *)(LR - 4);
    if ((ins16 & 0xf000) == 0xf000) {
        if (print_func != NULL) {
            k_int2str((int)LR - 4, &s_panic_call[14]);
            print_func(s_panic_call);
        }
        return 5;
    } else {
        if (print_func != NULL) {
            k_int2str((int)LR - 2, &s_panic_call[14]);
            print_func(s_panic_call);
        }
        return 3;
    }
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
int backtraceFromStack(int **pSP, char **pPC,
                       int (*print_func)(const char *fmt, ...))
{
    char          *CodeAddr = NULL;
    int           *SP       = *pSP;
    char          *PC       = *pPC;
    char          *LR;
    int            i;
    unsigned short ins16;
    unsigned int   ins32;
    unsigned int   framesize = 0;
    unsigned int   offset    = 1;

    if (SP == krhino_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    }

    /* func call ways:
       1. "stmdb sp!, ..." or "push ..." to open stack frame and save LR
       2. "sub  sp, ..." to open stack more
       3. call
       */

    /* 1. scan code, find frame size from "push" or "stmdb sp!" */
    for (i = 2; i < FUNC_SIZE_LIMIT; i += 2) {
        /* find nearest "push   {..., lr}" */
        ins16 = *(unsigned short *)(PC - i);
        if ((ins16 & 0xff00) == 0xb500) {
            framesize = __builtin_popcount((unsigned char)ins16);
            framesize++;
            /* find double push */
            ins16 = *(unsigned short *)(PC - i - 2);
            if ((ins16 & 0xff00) == 0xb400) {
                offset += __builtin_popcount((unsigned char)ins16);
                framesize += __builtin_popcount((unsigned char)ins16);
            }
            CodeAddr = PC - i;
            break;
        }

        /* find "stmdb sp!, ..." */
        /* The Thumb instruction stream is a sequence of halfword-aligned
         * halfwords */
        ins32 = *(unsigned short *)(PC - i);
        ins32 <<= 16;
        ins32 |= *(unsigned short *)(PC - i + 2);
        if ((ins32 & 0xFFFFF000) == 0xe92d4000) {
            framesize = __builtin_popcount(ins32 & 0xfff);
            framesize++;
            CodeAddr = PC - i;
            break;
        }
    }

    if (CodeAddr == NULL) {
        /* error branch */
        if (print_func != NULL) {
            print_func("Backtrace fail!\r\n");
        }
        return -1;
    }

    /* 2. scan code, find frame size from "sub" */
    for (i = 2; i < FUNC_SIZE_LIMIT; i += 2) {
        if (PC - i < CodeAddr) {
            break;
        }
        /* find "sub    sp, ..." */
        ins16 = *(unsigned short *)(PC - i);
        if ((ins16 & 0xff80) == 0xb080) {
            framesize += (ins16 & 0x7f);
            break;
        }
    }

    /* 3. output */
    *pSP   = SP + framesize;
    LR     = (char *)*(SP + framesize - offset);
    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
int backtraceFromLR(int **pSP, char **pPC, char *LR,
                    int (*print_func)(const char *fmt, ...))
{
    int           *SP       = *pSP;
    char          *PC       = *pPC;
    char          *CodeAddr = NULL;
    int            i;
    unsigned short ins16;
    unsigned int   framesize = 0;
    unsigned int   offset;

    if (PC == NULL) {
        offset = backtraceFindLROffset(LR, print_func);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    /*find stack framesize:
       1. "push ..." to open stack
       2. "sub  sp, ..." to open stack
       3. 1 + 2
       4. do not open stack
       */

    /* 1. scan code, find frame size from "push" or "sub" */
    for (i = 2; i < FUNC_SIZE_LIMIT; i += 2) {
        ins16 = *(unsigned short *)(PC - i);
        /* find "push   {..., lr}" */
        if ((ins16 & 0xff00) == 0xb500) {
            /* another function */
            break;
        }
        /* find "push   {...}" */
        if ((ins16 & 0xff00) == 0xb400) {
            framesize = __builtin_popcount((unsigned char)ins16);
            CodeAddr  = PC - i;
            break;
        }
        /* find "sub    sp, ..." */
        if ((ins16 & 0xff80) == 0xb080) {
            framesize = (ins16 & 0x7f);
            CodeAddr  = PC - i;
            /* find push before sub */
            ins16 = *(unsigned short *)(PC - i - 2);
            if ((ins16 & 0xff00) == 0xb400) {
                framesize += __builtin_popcount((unsigned char)ins16);
                CodeAddr = PC - i - 2;
            }
            break;
        }
    }

    /* 2. check the "push" or "sub sp" belongs to another function */
    if (CodeAddr != NULL) {
        for (i = 2; i < PC - CodeAddr; i += 2) {
            ins16 = *(unsigned short *)(PC - i);
            /* find "pop   {..., pc}" or "bx   lr" */
            if ((ins16 & 0xff00) == 0xbd00 || ins16 == 0x4770) {
                /* SP no changed */
                framesize = 0;
            }
        }
    } /* else: SP no changed */

    /* 3. output */
    *pSP   = SP + framesize;
    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

/* printf call stack */
int backtraceNow(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    int  *SP;
    int   lvl;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    /* compiler specific */
#if defined(__CC_ARM)
    SP = (int *)__current_sp();
    PC = (char *)__current_pc();
#elif defined(__ICCARM__)
    asm volatile("mov %0, sp\n" : "=r"(SP));
    asm volatile("mov %0, pc\n" : "=r"(PC));
#elif defined(__GNUC__)
    __asm__ volatile("mov %0, sp\n" : "=r"(SP));
    __asm__ volatile("mov %0, pc\n" : "=r"(PC));
#endif

    print_func("========== Call stack ==========\r\n");
    for (lvl = 0; lvl < BACK_TRACE_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, print_func);
        if (ret != 0) {
            break;
        }
    }
    print_func("==========    End     ==========\r\n");
    return lvl;
}

int backtraceTask(char *taskname, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    int      lvl;
    int      ret;
    ktask_t *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    task = krhino_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }

    if (krhino_is_task_ready(task)) {
        print_func("Status of task \"%s\" is 'Ready', Can not backtrace!\n",
                   taskname);
        return 0;
    }

    getPLSfromCtx(task->task_stack, &PC, &LR, &SP);

    print_func("TaskName  : %s\n", taskname);
    print_func("========== Call stack ==========\r\n");
    for (lvl = 0; lvl < BACK_TRACE_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, print_func);
        if (ret != 0) {
            break;
        }
    }
    print_func("==========    End     ==========\r\n");
    return lvl;
}

#endif

#if (RHINO_CONFIG_PANIC > 0)
#define REG_NAME_WIDTH 7

typedef struct
{
    /* saved in assembler */
    int R0;
    int R1;
    int R2;
    int R3;
    int R4;
    int R5;
    int R6;
    int R7;
    int R8;
    int R9;
    int R10;
    int R11;
    int R12;
    int LR;         // Link Register (LR)
    int PC;         // Program Counter (PC)
    int xPSR;       // Program Status Registers
    int SP;         // Stack Pointer
    int EXC_RETURN; // Exception Return
    int EXC_NUMBER; // Exception Num
    int PRIMASK;    // Interrupt Mask
    int FAULTMASK;  // Interrupt Mask
    int BASEPRI;    // Interrupt Mask
} PANIC_CONTEXT;

typedef struct
{
    int CFSR;
    int HFSR;
    int MMFAR;
    int BFAR;
    int AFSR;
} FAULT_REGS;

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    PANIC_CONTEXT *arm_context = (PANIC_CONTEXT *)context;

    *pSP = (int *)arm_context->SP;
    *pPC = (char *)arm_context->PC;
    *pLR = (char *)arm_context->LR;
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int        x;
    int       *regs = (int *)context;
    char       s_panic_regs[REG_NAME_WIDTH + 14];
    FAULT_REGS stFregs;
    /* PANIC_CONTEXT */
    char s_panic_ctx[] = "R0     "
                         "R1     "
                         "R2     "
                         "R3     "
                         "R4     "
                         "R5     "
                         "R6     "
                         "R7     "
                         "R8     "
                         "R9     "
                         "R10    "
                         "R11    "
                         "R12    "
                         "LR     "
                         "PC     "
                         "xPSR   "
                         "SP     "
                         "EXC_RET"
                         "EXC_NUM"
                         "PRIMASK"
                         "FLTMASK"
                         "BASEPRI";
    /* FAULT_REGS */
    char s_panic_reg[] = "CFSR   "
                         "HFSR   "
                         "MMFAR  "
                         "BFAR   "
                         "AFSR   ";

    if (regs == NULL) {
        return;
    }

    print_func("========== Regs info  ==========\r\n");

    /* show PANIC_CONTEXT */
    for (x = 0; x < sizeof(s_panic_ctx) / REG_NAME_WIDTH; x++) {
        memcpy(&s_panic_regs[0], &s_panic_ctx[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_int2str(regs[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13] = 0;
        print_func(s_panic_regs);
    }

    /* show FAULT_REGS */
    stFregs.CFSR  = (*((volatile int *)(0xE000ED28)));
    stFregs.HFSR  = (*((volatile int *)(0xE000ED2C)));
    stFregs.MMFAR = (*((volatile int *)(0xE000ED34)));
    stFregs.BFAR  = (*((volatile int *)(0xE000ED38)));
    stFregs.AFSR  = (*((volatile int *)(0xE000ED3C)));
    for (x = 0; x < sizeof(stFregs) / sizeof(int); x++) {
        memcpy(&s_panic_regs[0], &s_panic_reg[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_int2str(((int *)(&stFregs))[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13] = 0;
        print_func(s_panic_regs);
    }
}

#if (RHINO_CONFIG_BACKTRACE > 0)
/* backtrace start with PC and SP, find LR from stack memory
   return levels os callstack */
int panicBacktraceCaller(char *PC, int *SP,
                         int (*print_func)(const char *fmt, ...))
{
    int  *bt_sp;
    char *bt_pc;
    int   lvl, ret;
    char  s_panic_call[] = "backtrace : 0x         \r\n";

    /* caller must save LR in stack, so find LR from stack */

    if (SP == NULL) {
        return 0;
    }

    bt_sp = SP;
    bt_pc = PC;
    ret   = -1;
    for (lvl = 0; lvl < BACK_TRACE_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, NULL);
        if (ret != 0) {
            break;
        }
    }
    if (ret == 1) {
        /* assume right! print */
        k_int2str((int)PC, &s_panic_call[14]);
        if (print_func != NULL) {
            print_func(s_panic_call);
        }
        bt_sp = SP;
        bt_pc = PC;
        ret   = -1;
        for (lvl = 1; lvl < BACK_TRACE_LIMIT; lvl++) {
            ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
            if (ret != 0) {
                break;
            }
        }
        return lvl;
    }

    return 0;
}

/* backtrace start with PC SP and LR */
int panicBacktraceCallee(char *PC, int *SP, char *LR,
                         int (*print_func)(const char *fmt, ...))
{
    int  *bt_sp;
    char *bt_pc;
    char *bt_lr;
    int   lvl, ret;
    char  s_panic_call[] = "backtrace : 0x         \r\n";

    if (SP == NULL) {
        return 0;
    }

    /* Backtrace: assume ReturnAddr is saved in LR when exception */
    k_int2str((int)PC, &s_panic_call[14]);
    if (print_func != NULL) {
        print_func(s_panic_call);
    }
    lvl   = 1;
    bt_sp = SP;
    bt_pc = PC;
    bt_lr = LR;
    ret   = backtraceFromLR(&bt_sp, &bt_pc, bt_lr, print_func);
    if (ret == 0) {
        for (; lvl < BACK_TRACE_LIMIT; lvl++) {
            ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
            if (ret != 0) {
                break;
            }
        }
    }

    return lvl;
}
#endif

#endif
