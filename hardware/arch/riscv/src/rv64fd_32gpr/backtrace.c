/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "k_api.h"
#include "aos/debug.h"

/* part of ktask_t */
typedef struct
{
    void *task_stack;
} ktask_t_shadow;

//#define OS_BACKTRACE_DEBUG

extern char *__etext;
extern char *__stext;

#define BT_CHK_PC_AVAIL(pc)   ((uintptr_t)(pc) < (uintptr_t)(&__etext) \
                              && (uintptr_t)(pc) > (uintptr_t)(&__stext))

#define BT_PC2ADDR(pc)        ((char *)(((uintptr_t)(pc))))

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

extern void krhino_task_deathbed(void);
extern ktask_t_shadow *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t_shadow *task);
extern void *debug_task_stack_bottom(ktask_t_shadow *task);
extern char *k_int2str(int num, char *str);
extern void _interrupt_return_address(void);

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...));

/* get framesize from c ins */
static int riscv_backtrace_framesize_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* addi sp, sp, -im */
    if ((inst & 0xFF83) == 0x1101) {
        imm = (inst >> 2) & 0x1F;
        imm = (~imm & 0x1F) + 1;
        return imm >> 3;
    }

    /* c.addi16sp sp, nzuimm6<<4 */
    if ((inst & 0xFF83) == 0x7101) {
        imm = (inst >> 3) & 0x3;
        imm = (imm << 1) | ((inst >> 5) & 0x1);
        imm = (imm << 1) | ((inst >> 2) & 0x1);
        imm = (imm << 1) | ((inst >> 6) & 0x1);
        imm = ((~imm & 0x1f) + 1) << 4;
        return imm >> 3;
    }

    return -1;
}

/* get ra position in the stach */
static int riscv_backtrace_ra_offset_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* c.fsdsp rs2, uimm6<<3(sp) */
    if ((inst & 0xE07F) == 0xE006) {
        imm = (inst >> 7) & 0x7;
        imm = (imm << 3) | ((inst >> 10) & 0x7);
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm;
    }

    return -1;
}

/* get the offset between the jump instruction and the return address */
static int backtraceFindLROffset(char *LR, int (*print_func)(const char *fmt, ...))
{
    int offset = 0;
    char *LR_indeed;
    unsigned int ins32;
    char         s_panic_call[] = "backtrace : 0x         \r\n";

    LR_indeed = BT_PC2ADDR(LR);

   /* callstack bottom */
    if (LR_indeed == BT_PC2ADDR(&_interrupt_return_address)) {
        /* EXC_RETURN, so here is callstack bottom of interrupt handler */
        if (print_func != NULL) {
            print_func("backtrace : ^interrupt^\r\n");
        }
        return 0;
    }

    if (LR_indeed == BT_PC2ADDR(&krhino_task_deathbed)) {
        /* task delete, so here is callstack bottom of task */
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 0;
    }

    /* Usually jump using the JAL instruction */
    ins32 = *(unsigned int *)(LR_indeed - 4);
    if ((ins32 & 0x3) == 0x3) {
        offset = 4;
    } else {
        offset = 2;
    }

    if (print_func != NULL) {
        k_int2str((int)LR_indeed - offset, &s_panic_call[14]);
        print_func(s_panic_call);
    }

    return offset;
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int riscv_backtraceFromStack(long **pSP, char **pPC,
                                  int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    long  *SP      = *pSP;
    char *PC       = *pPC;
    char *LR;
    int   i;
    int   framesize;
    int   offset = 0;

    unsigned short ins16;

#ifdef OS_BACKTRACE_DEBUG
    printk("[riscv_backtraceFromStack in ] SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if (SP == debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    }

    /* 1. scan code, find lr pushed */
    for (i = 0; i < BT_FUNC_LIMIT; i += 4) {
        CodeAddr = (char *)(((long)PC & (~0x3)) - i);

        ins16 = *(unsigned short *)(CodeAddr + 2);
        framesize = riscv_backtrace_framesize_get(ins16);
        if (framesize >= 0) {
            break;
        }

        ins16 = *(unsigned short *)(CodeAddr);
        framesize = riscv_backtrace_framesize_get(ins16);
        if (framesize >= 0) {
            break;
        }
    }

    if (i == BT_FUNC_LIMIT) {
        /* error branch */
        if (print_func != NULL) {
            print_func("Backtrace fail!\r\n");
        }
        return -1;
    }

    /* 2. scan code, find ins: sd ra,24(sp) */
    for (i = 0; CodeAddr + i < PC; i += 4) {
        ins16 = *(unsigned short *)(CodeAddr + i + 2);
        offset = riscv_backtrace_ra_offset_get(ins16);
        if (offset >= 0) {
            break;
        }

        ins16 = *(unsigned short *)(CodeAddr + i);
        offset = riscv_backtrace_ra_offset_get(ins16);
        if (offset >= 0) {
            break;
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    printk("[arm_backtraceFromStack out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR     = (char *)(*(SP + offset));

    if (BT_CHK_PC_AVAIL(LR) == 0) {
        if (print_func != NULL) {
            print_func("backtrace : invalid lr\r\n");
        }
        return -1;
    }
    *pSP   = SP + framesize;
    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

static int backtraceFromStack(long **pSP, char **pPC,
                              int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_AVAIL(*pPC) == 0) {
        return -1;
    }

    return riscv_backtraceFromStack(pSP, pPC, print_func);
}

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    long  *SP;
    int   lvl;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    SP = RHINO_GET_SP();
    PC = RHINO_GET_PC();

    print_func("========== Call stack ==========\r\n");
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, print_func);
        if (ret != 0) {
            break;
        }
    }
    print_func("==========    End     ==========\r\n");
    return lvl;
}

/* printf call stack for task
   return levels of call stack */
void backtrace_task(void *task, int (*print_func)(const char *fmt, ...))
{

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
    return 0;
}


/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(void *trace[], int size, int offset)
{
    char *PC;
    long  *SP;
    int   lvl;
    int   ret;

    SP = RHINO_GET_SP();
    PC = RHINO_GET_PC();

    memset(trace, 0, size * sizeof(void *));

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, NULL);
        if (ret != 0) {
            break;
        }
        if (lvl >= offset && lvl - offset < size) {
            trace[lvl - offset] = PC;
        }
        if (lvl - offset >= size) {
            break;
        }
    }

    return lvl - offset < 0 ? 0 : lvl - offset;
}

void backtrace_handle(char *PC, int *SP, char *LR,
                      int (*print_func)(const char *fmt, ...))
{
    int   lvl;
    int   ret;
    long  *pSP = (long *)SP;

    if (print_func == NULL) {
        print_func = printf;
    }

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&pSP, &PC, print_func);
        if (ret != 0) {
            break;
        }
    }
    return;
}

