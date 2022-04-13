/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "k_arch.h"
#include "k_api.h"

//#define OS_BACKTRACE_DEBUG

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

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

#define BT_PC2ADDR(pc)          ((char *)(((uintptr_t)(pc)) & 0xfffffffe))
#define BT_CHK_PC_AVAIL(pc)     ((uintptr_t)(pc)<(uintptr_t)(&_mmu_text_end) \
                              && (uintptr_t)(pc)>(uintptr_t)(&_mmu_text_start))
#define BT_CHK_PC_THUMB(pc)     ((uintptr_t)(pc) & 0x1)
#define BT_MODESET_THUMB(pc)    do{(pc) = (void *)((uintptr_t)(pc) | 0x1);}while(0)

/* part of ktask_t */
typedef struct {
    void *task_stack;
} ktask_t_shadow;

extern char *k_int2str(int num, char *str);
extern void *debug_task_stack_bottom(ktask_t *task);
extern ktask_t *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t *task);

extern char *_mmu_text_start;
extern char *_mmu_text_end;

extern char *k_int2str(int num, char *str);
extern void *debug_task_stack_bottom(ktask_t *task);
extern ktask_t *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t *task);
extern ktask_t *debug_task_find_running(char **name);
extern void krhino_task_deathbed(void);
extern void _interrupt_return_address(void);

int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

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

static void getPLSfromTaskCtx(void *context, char **PC, char **LR, int **SP)
{
    context_t *task_ctx;

    task_ctx = (context_t *)context;

    *PC = (char *)task_ctx->PC;
    *LR = (char *)task_ctx->LR;
    *SP = (int *)(task_ctx + 1);
    /* CPSR T, bit[5]  Thumb execution state bit. */
    if (task_ctx->CPSR & 0x20) {
        BT_MODESET_THUMB(*PC);
    }
}

static void getPLSfromFaultCtx(void *context, char **PC, char **LR, int **SP)
{
    fault_context_t *task_ctx;

    task_ctx = (fault_context_t *)context;

    *PC = (char *)task_ctx->cntx.PC;
    *LR = (char *)task_ctx->cntx.LR;
    *SP = (int *)task_ctx->SP;;
    /* CPSR T, bit[5]  Thumb execution state bit. */
    if (task_ctx->cntx.CPSR & 0x20) {
        BT_MODESET_THUMB(*PC);
    }
}

/* get "blx" or "bl" before LR, return offset */
static int backtraceFindLROffset(char *LR,
                                 int (*print_func)(const char *fmt, ...))
{
    int offset = 0;
    char *LR_indeed;
    unsigned short ins16;
    char           s_panic_call[] = "backtrace : 0x         \r\n";

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

    /* check if thumb mode for next level */
    if (BT_CHK_PC_THUMB(LR)) {
        ins16 = *(unsigned short *)(LR_indeed - 4);
        if ((ins16 & 0xf000) == 0xf000) {
            offset = 4;
        } else {
            offset = 2;
        }
    } else {
        offset = 4;
    }

    if (print_func != NULL) {
        k_int2str((int)LR_indeed - offset, &s_panic_call[14]);
        print_func(s_panic_call);
    }

    return offset;
}


/* check if push lr instruction, return framesize pushed */
static int arm_backtraceInsPushLR(unsigned int inst, int *lr_offset)
{
    /* push lr */
    if ((inst & 0xFFFF4000) == 0xE92D4000) {
        *lr_offset = (inst & 0x8000) == 0x8000 ? 2 : 1;
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* str lr, [sp, #-4]! */
    if (inst == 0xE52DE004) {
        *lr_offset = 1;
        return 1;
    }

    return -1;
}

/* check if Function return instruction, return framesize popped */
static int arm_backtraceInsFuncRet(unsigned int inst)
{
    /* pop pc */
    if ((inst & 0x0FFF8000) == 0x08BD8000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* bx lr */
    if ((inst & 0x0FFFFFFF) == 0x012FFF1E) {
        return 0;
    }

    /* ldr pc, [sp], #4 */
    if ((inst & 0x0FFFFFFF) == 0x049DF004) {
        return 1;

    }

    return -1;
}

/* check if open stack instruction, return framesize pushed */
static int arm_backtraceInsStackOpen(unsigned int inst)
{
    unsigned int   sub;
    unsigned int   shift;

    /* find "sub sp, sp, ..." */
    if ((inst & 0x0FFFF000) == 0x024DD000) {
        sub = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if (shift != 0) {
            shift = 32 - 2 * shift;
            sub = sub << shift;
        }
        return sub / 4;
    }

    /* find "push" */
    if ((inst & 0x0FFF0000) == 0x092D0000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* find "vpush" */
    if ((inst & 0x0FBF0F00) == 0x0D2D0B00) {
        return inst & 0xFF;
    }

    /* find "str *, [sp, #-4]!" */
    if ((inst & 0xFFFF0FFF) == 0xE52D0004) {
        return 1;
    }

    return -1;
}

/* check if close stack instruction, return framesize pushed */
static int arm_backtraceInsStackClose(unsigned int inst)
{
    unsigned int   sub;
    unsigned int   shift;

    /* find "add sp, sp, ..." */
    if ((inst & 0x0FFFF000) == 0x028DD000) {
        sub = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if (shift != 0) {
            shift = 32 - 2 * shift;
            sub = sub << shift;
        }
        return sub / 4;
    }

    /* find "pop" */
    if ((inst & 0x0FFF0000) == 0x08BD0000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* find "vpop" */
    if ((inst & 0x0FBF0F00) == 0x0CBD0B00) {
        return inst & 0xFF;
    }

    /* find "ldr *, [sp], #4" */
    if ((inst & 0x0FFF0FFF) == 0x049D0004) {
        return 1;
    }

    return -1;
}


/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int arm_backtraceFromStack(int **pSP, char **pPC,
                                  int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    int  *SP       = *pSP;
    char *PC       = *pPC;
    char *LR;
    int   i;
    int   temp;
    int   framesize;
    int   offset = 0;
    unsigned int   ins32;

#ifdef OS_BACKTRACE_DEBUG
    printf("[arm_backtraceFromStack in ] SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if (SP == debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    }

    /* 1. scan code, find lr pushed */
    for (i = 4; i < BT_FUNC_LIMIT; i += 4) {
        CodeAddr = PC - i;
        ins32 = *(unsigned int *)CodeAddr;
        framesize = arm_backtraceInsPushLR(ins32, &offset);
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

    /* 2. scan code, find frame size from "sub" and "vpush" */
    for (i = 4; CodeAddr + i < PC; i += 4) {
        ins32 = *(unsigned int *)(CodeAddr + i);
        temp = arm_backtraceInsStackOpen(ins32);
        if (temp >= 0) {
            framesize += temp;
        }
    }

    ins32 = *(unsigned int *)(CodeAddr - 4);
    temp = arm_backtraceInsStackOpen(ins32);
    if (temp >= 0) {
        framesize += temp;
        offset += temp;
    }

#ifdef OS_BACKTRACE_DEBUG
    printf("[arm_backtraceFromStack out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR     = (char *) * (SP + framesize - offset);
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

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int arm_backtraceFromLR(int **pSP, char **pPC, char *LR,
                               int (*print_func)(const char *fmt, ...))
{
    int  *SP       = *pSP;
    char *PC       = *pPC;
    char *CodeAddr = NULL;
    int   i;
    int   temp;
    int   framesize = 0;
    int   offset;
    unsigned int   ins32;

#ifdef OS_BACKTRACE_DEBUG
    printf("[arm_backtraceFromLR in ] SP = %p, PC = %p, LR = %p\n\r", *pSP, *pPC, LR);
#endif

    if (BT_CHK_PC_AVAIL(PC) == 0) {
        if (BT_CHK_PC_AVAIL(LR) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid lr\r\n");
            }
            return -1;
        }
        offset = backtraceFindLROffset(LR, print_func);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    /* 1. scan code, find frame size from "pop" or "bx lr" or "ldr pc" */
    for (i = 0; i < BT_FUNC_LIMIT; i += 4) {
        CodeAddr = PC + i;
        ins32 = *(unsigned int *)CodeAddr;
        framesize = arm_backtraceInsFuncRet(ins32);
        if (framesize >= 0) {
            break;
        }

        temp = arm_backtraceInsPushLR(ins32, &offset);
        if (temp >= 0) {
            framesize = 0;
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

    /* 2. scan code, find frame size from "sub" and "vpush" */
    for (i = 4; CodeAddr - i >= PC; i += 4) {
        ins32 = *(unsigned int *)(CodeAddr - i);
        temp = arm_backtraceInsStackClose(ins32);
        if (temp >= 0) {
            framesize += temp;
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    printf("[arm_backtraceFromLR out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
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

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int thumb_backtraceFromStack(int **pSP, char **pPC,
                                    int (*print_func)(const char *fmt, ...))
{
    char          *CodeAddr = NULL;
    int           *SP       = *pSP;
    char          *PC       = BT_PC2ADDR(*pPC);
    char          *LR;
    int            i;
    unsigned short ins16;
    unsigned int   ins32;
    unsigned int   framesize = 0;
    unsigned int   shift = 0;
    unsigned int   sub = 0;
    unsigned int   offset    = 1;

#ifdef OS_BACKTRACE_DEBUG
    printf("[thumb_backtraceFromStack in ] SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if (SP == debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
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

        /* find "sub.w  sp, sp, ..." */
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
    printf("[thumb_backtraceFromStack out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR     = (char *) * (SP + framesize - offset);
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

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int thumb_backtraceFromLR(int **pSP, char **pPC, char *LR,
                                 int (*print_func)(const char *fmt, ...))
{
    int           *SP       = *pSP;
    char          *PC       = BT_PC2ADDR(*pPC);
    char          *CodeAddr = NULL;
    int            i;
    unsigned short ins16;
    unsigned int   framesize = 0;
    unsigned int   offset;

#ifdef OS_BACKTRACE_DEBUG
    printf("[thumb_backtraceFromLR in ] SP = %p, PC = %p, LR = %p\n\r", *pSP, *pPC, LR);
#endif

    if (BT_CHK_PC_AVAIL(PC) == 0) {
        if (BT_CHK_PC_AVAIL(LR) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid lr\r\n");
            }
            return -1;
        }
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
    /* printf("[arm_backtraceFromLR out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP); */
#endif

    /* 3. output */
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


/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int backtraceFromStack(int **pSP, char **pPC,
                              int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_AVAIL(*pPC) == 0) {
        return -1;
    }

    if (BT_CHK_PC_THUMB(*pPC)) {
        return thumb_backtraceFromStack(pSP, pPC, print_func);
    } else {
        return arm_backtraceFromStack(pSP, pPC, print_func);
    }
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int backtraceFromLR(int **pSP, char **pPC, char *LR,
                           int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_THUMB(*pPC)) {
        return thumb_backtraceFromLR(pSP, pPC, LR, print_func);
    } else {
        return arm_backtraceFromLR(pSP, pPC, LR, print_func);
    }
}

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    int  *SP;
    int   CPSR;
    int   lvl;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    /* compiler specific */
#if defined(__CC_ARM)
    register int temp __asm("cpsr");
    CPSR = temp;
    SP = (int *)__current_sp();
    PC = (char *)__current_pc();
#elif defined(__ICCARM__)
    asm volatile("mov %0, sp\n" : "=r"(SP));
    asm volatile("mov %0, pc\n" : "=r"(PC));
    asm volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#elif defined(__GNUC__)
    __asm__ volatile("mov %0, sp\n" : "=r"(SP));
    __asm__ volatile("mov %0, pc\n" : "=r"(PC));
    __asm__ volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#endif

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if (CPSR & 0x20) {
        BT_MODESET_THUMB(PC);
    }

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
int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    int      lvl;
    int      ret;
    char     panic_call[] = "backtrace : 0x         \r\n";
    ktask_t_shadow *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    task = (ktask_t_shadow *)debug_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }

    getPLSfromTaskCtx(task->task_stack, &PC, &LR, &SP);

#ifdef OS_BACKTRACE_DEBUG
    print_func("[backtrace_task] %s %p %p %p %p %p\n", taskname,
               task, task->task_stack, PC, LR, SP);
#endif

    print_func("========== Call stack ==========\r\n");
    k_int2str((int)BT_PC2ADDR(PC), &panic_call[14]);
    if (print_func != NULL) {
        print_func(panic_call);
    }

    ret = debug_task_is_running((ktask_t *)task);
    switch (ret) {
        case 0 :
            lvl = backtrace_caller(PC, SP, print_func);
            break;
        case 1 :
        case 2 :
            lvl = backtrace_caller(PC, SP, print_func);
            if (lvl == 0) {
                lvl = backtrace_callee(PC, SP, LR, print_func);
            }
            break;

        default:
            print_func("Status of task \"%s\" is 'Running', Can not backtrace!\n",
                       taskname);
            break;
    }
    print_func("==========    End     ==========\r\n");

    return lvl;
}


/* call from panic, show running task info */
int backtrace_task_now(int (*print_func)(const char *fmt, ...))
{
    char *taskname = NULL;
    char *PC;
    char *LR;
    int  *SP;
    int   i;
    int   lvl;
    char  panic_call[] = "backtrace : 0x         \r\n";
    char  name_show[]  = "TaskName  :                      \r\n";
    ktask_t_shadow *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    print_func("========= Running Task =========\r\n");
    task = (ktask_t_shadow *)debug_task_find_running(&taskname);
    if (task == NULL) {
        print_func("Task not found\n");
        return 0;
    }

    getPLSfromTaskCtx(task->task_stack, &PC, &LR, &SP);

#ifdef OS_BACKTRACE_DEBUG
    print_func("[backtrace_task] %s %p %p %p %p %p\n", taskname,
               task, task->task_stack, PC, LR, SP);
#endif

    /* set name */
    for (i = 0; i < 20; i++) {
        name_show[12 + i] = ' ';
    }
    for (i = 0; i < 20; i++) {
        if (taskname[i] == '\0') {
            break;
        }
        name_show[12 + i] = taskname[i];
    }
    print_func(name_show);

    k_int2str((int)BT_PC2ADDR(PC), &panic_call[14]);
    print_func(panic_call);

    lvl = backtrace_caller(PC, SP, print_func);
    if (lvl == 0) {
        lvl = backtrace_callee(PC, SP, LR, print_func);
    }

    print_func("==========    End     ==========\r\n");

    return lvl;
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

    bt_sp = SP;
    bt_pc = PC;
    ret   = -1;
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, NULL);
        if (ret != 0) {
            break;
        }
    }
    if (ret == 1) {
        /* assume right! print */
        k_int2str((int)BT_PC2ADDR(PC), &s_panic_call[14]);
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
    k_int2str((int)BT_PC2ADDR(PC), &s_panic_call[14]);
    if (print_func != NULL) {
        print_func(s_panic_call);
    }
    lvl   = 1;
    bt_sp = SP;
    bt_pc = PC;
    bt_lr = LR;
    ret   = backtraceFromLR(&bt_sp, &bt_pc, bt_lr, print_func);
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
    int   CPSR;
    int   lvl;
    int   ret;

    /* compiler specific */
#if defined(__CC_ARM)
    register int temp __asm("cpsr");
    CPSR = temp;
    SP = (int *)__current_sp();
    PC = (char *)__current_pc();
#elif defined(__ICCARM__)
    asm volatile("mov %0, sp\n" : "=r"(SP));
    asm volatile("mov %0, pc\n" : "=r"(PC));
    asm volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#elif defined(__GNUC__)
    __asm__ volatile("mov %0, sp\n" : "=r"(SP));
    __asm__ volatile("mov %0, pc\n" : "=r"(PC));
    __asm__ volatile("mrs %0, CPSR\n" : "=r"(CPSR));
#endif

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if (CPSR & 0x20) {
        BT_MODESET_THUMB(PC);
    }

    memset(trace, 0, size * sizeof(void *));

    g_back_trace = trace;

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, NULL);
        if (ret != 0) {
            break;
        }
        if
        (lvl >= offset && lvl - offset < size) {
            trace[lvl - offset] = PC;
        }
        if
        (lvl - offset >= size) {
            break;
        }
    }
    return lvl - offset < 0 ? 0 : lvl - offset;
}

