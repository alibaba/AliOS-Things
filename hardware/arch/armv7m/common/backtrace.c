/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "k_api.h"

/* part of ktask_t */
typedef struct
{
    void *task_stack;
}ktask_t_shadow;

//#define OS_BACKTRACE_DEBUG

extern void krhino_task_deathbed(void);
extern ktask_t_shadow *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t_shadow *task);
extern void *debug_task_stack_bottom(ktask_t_shadow *task);
extern char *k_int2str(int num, char *str);

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...));

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

#define BT_FUNC_LIMIT       0x2000
#define BT_LVL_LIMIT        64
#define BT_PC2ADDR(pc)      ((char *)(((uintptr_t)(pc)) & 0xfffffffe))

/* alios_debug_pc_check depends on mcu*/
__attribute__((weak)) int alios_debug_pc_check(char *pc)
{
    return 0;
}

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

    LR = BT_PC2ADDR(LR);

    /* callstack bottom */
    if (((int)LR & 0xffffffe0) == 0xffffffe0) {
        /* EXC_RETURN, so here is callstack bottom of interrupt handler */
        if (print_func != NULL) {
            print_func("backtrace : ^interrupt^\r\n");
        }
        return 0;
    }

    if (LR == BT_PC2ADDR(&krhino_task_deathbed)) {
        /* task delete, so here is callstack bottom of task */
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 0;
    }

    if (alios_debug_pc_check(LR) != 0) {
        if (print_func) {
            print_func("backtrace : invalid pc : 0x%x\r\n", LR);
        }
        return -1;
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
static int backtraceFromStack(int **pSP, char **pPC,
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
    unsigned int   shift = 0;
    unsigned int   sub = 0;
    unsigned int   offset    = 1;
#ifdef OS_BACKTRACE_DEBUG
    printf("[backtraceFromStack in ] SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if (SP == debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    }

    if (alios_debug_pc_check(*pPC) != 0) {
        if (print_func) {
            print_func("backtrace : invalid pc : 0x%x\r\n", *pPC);
        }
        return -1;
    }

    /* func call ways:
       1. "stmdb sp!, ..." or "push ..." to open stack frame and save LR
       2. "sub  sp, ..." or "sub.w  sp, ..." to open stack more
       3. call
       */

    /* 1. scan code, find frame size from "push" or "stmdb sp!" */
    for (i = 2; i < BT_FUNC_LIMIT; i += 2) {
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

    /* 2. scan code, find frame size from "sub" or "sub.w" */
    for (i = 0; i < BT_FUNC_LIMIT;) {
        if (CodeAddr + i > PC) {
            break;
        }
        /* find "sub    sp, ..." */
        ins16 = *(unsigned short *)(CodeAddr + i);
        if ((ins16 & 0xff80) == 0xb080) {
            framesize += (ins16 & 0x7f);
            break;
        }

        /* find "sub.w	sp, sp, ..." */
        ins32 = *(unsigned short *)(CodeAddr + i);
        ins32 <<= 16;
        ins32 |= *(unsigned short *)(CodeAddr + i + 2);
        if ((ins32 & 0xFBFF8F00) == 0xF1AD0D00) {
            sub = 128 + (ins32 & 0x7f);
            shift  = (ins32 >> 7) & 0x1;
            shift += ((ins32 >> 12) & 0x7) << 1;
            shift += ((ins32 >> 26) & 0x1) << 4;
            framesize += sub << (30 - shift);
            break;
        }

        if ((ins16 & 0xf800) >= 0xe800) {
            i += 4;
        } else {
            i += 2;
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    printf("[backtraceFromStack out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif
    /* 3. output */
    LR     = (char *)*(SP + framesize - offset);
    offset = backtraceFindLROffset(LR, print_func);
    if (offset < 0) {
        return -1;
    }
    *pSP   = SP + framesize;
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int backtraceFromLR(int **pSP, char **pPC, char *LR,
                    int (*print_func)(const char *fmt, ...))
{
    int           *SP       = *pSP;
    char          *PC       = *pPC;
    char          *CodeAddr = NULL;
    int            i;
    unsigned short ins16;
    unsigned int   framesize = 0;
    unsigned int   offset;

#ifdef OS_BACKTRACE_DEBUG
    printf("[backtraceFromLR in ] SP = %p, PC = %p, LR = %p\n\r", *pSP, *pPC, LR);
#endif
    if (alios_debug_pc_check(*pPC) != 0) {
        offset = backtraceFindLROffset(LR, print_func);
        if ( offset < 0 ){
            return -1;
        }
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
    for (i = 2; i < BT_FUNC_LIMIT; i += 2) {
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

#ifdef OS_BACKTRACE_DEBUG
    printf("[backtraceFromLR out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif
    /* 3. output */
    offset = backtraceFindLROffset(LR, print_func);
    if ( offset < 0 ){
        return -1;
    }
    *pSP   = SP + framesize;
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
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
    char    *PC;
    char    *LR;
    int     *SP;
    int      lvl = 0;
    int      ret;
    char     panic_call[] = "backtrace : 0x         \r\n";
    //ktask_t_shadow *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    getPLSfromCtx(((ktask_t *)task)->task_stack, &PC, &LR, &SP);

#ifdef OS_BACKTRACE_DEBUG
    printf("[backtrace_task] SP = %p, PC = %p, LR = %p\r\n", SP, PC, LR);
#endif

    print_func("========== Call stack ==========\r\n");
    k_int2str((int)BT_PC2ADDR(PC), &panic_call[14]);
    if (print_func != NULL) {
        print_func(panic_call);
    }

    ret = debug_task_is_running(task);
    switch (ret) {
        case 0 :
        case 1 :
        case 2 :
            backtrace_handle(PC, SP, LR, print_func);
            break;

        default:
            print_func("Status of task \"%s\" is 'Running', Can not backtrace!\n",
                       ((ktask_t *)task)->task_name ? ((ktask_t *)task)->task_name : "anonym");
            break;
    }

    print_func("==========    End     ==========\r\n");
}

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
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

    /* try with no LR */
    bt_sp = SP;
    bt_pc = BT_PC2ADDR(PC);
    ret   = -1;
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, NULL);
        if (ret != 0) {
            break;
        }
    }
    if (ret == 1) {
        /* try success, output */
        k_int2str((int)PC, &s_panic_call[14]);
        if (print_func != NULL) {
            print_func(s_panic_call);
        }
        bt_sp = SP;
        bt_pc = PC;
        ret   = -1;
        for (lvl = 1; lvl < BT_LVL_LIMIT; lvl++) {
            ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
            if (ret != 0) {
                break;
            }
        }
        return lvl;
    }

    return 0;
}

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
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

    /* try, with LR */
    ret = backtraceFromLR(&bt_sp, &bt_pc, bt_lr, print_func);
    if (ret == 0) {
        for (; lvl < BT_LVL_LIMIT; lvl++) {
            ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
            if (ret != 0) {
                break;
            }
        }
    }

    return lvl;
}


void *g_back_trace;
/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(void *trace[], int size, int offset)
{
    char *PC;
    int  *SP;
    int   lvl;
    int   ret;

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

    memset(trace, 0, size*sizeof(void *));

    g_back_trace = trace;

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
    if (SP == NULL) {
        print_func("SP is NULL, Can't backtrace\r\n");
        return;
    }

    if (backtrace_caller(PC, SP, print_func) > 0) {
        /* Backtrace 1st try: assume ReturnAddr is saved in stack when exception */
        /* backtrace success, do not try other way */
        goto exit;
    } else if (backtrace_callee(PC, SP, LR, print_func) > 0) {
        /* Backtrace 2nd try: assume ReturnAddr is saved in LR when exception */
        /* backtrace success, do not try other way */
        goto exit;
    } else {
        /* Backtrace 3rd try: assume PC is invalalb, backtrace from LR */
        backtrace_caller(LR, SP, print_func);
    }

exit:
    return;
}
