/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_DEBUG

#include <stdio.h>
#include "debug_overview.h"

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
    int SP;      // Stack Pointer
    int LR;      // Link Register
    int PC;      // Program Counter
    int CPSR;    // Current Program Status Registers
    int EXCTYPE; // 1-undefined; 2-prefetch; 3-data
} PANIC_CONTEXT;

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    PANIC_CONTEXT *arm_context = (PANIC_CONTEXT *)context;

    *pSP = (int *)arm_context->SP;
    *pPC = (char *)arm_context->PC;
    *pLR = (char *)arm_context->LR;
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int  x;
    int *regs = (int *)context;
    char s_panic_regs[REG_NAME_WIDTH + 14];
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
                         "SP     "
                         "LR     "
                         "PC     "
                         "CPSR   "
                         "EXCTYPE";

    if (regs == NULL) {
        return;
    }

    print_func("========== Regs info  ==========\r\n");

    /* show PANIC_CONTEXT */
    for (x = 0; x < sizeof(s_panic_ctx) / REG_NAME_WIDTH - 1; x++) {
        memcpy(&s_panic_regs[0], &s_panic_ctx[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_int2str(regs[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13] = 0;
        print_func(s_panic_regs);
    }
    switch (regs[x]) {
        case 1:
            print_func("ExcCasue: Undefined instruction\r\n");
            break;
        case 2:
            print_func("ExcCasue: Prefetch Abort\r\n");
            break;
        case 3:
            print_func("ExcCasue: Data Abort\r\n");
            break;
        default:
            print_func("ExcCasue: Unknown\r\n");
            break;
    }
}
#endif
