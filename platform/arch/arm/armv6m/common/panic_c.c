/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "debug_api.h"

#if (DEBUG_CONFIG_PANIC > 0)
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
                         "LR     "
                         "PC     "
                         "xPSR   "
                         "SP     "
                         "EXC_RET"
                         "EXC_NUM"
                         "PRIMASK";

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
}
#endif
