/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <csi_core.h>
//#include "debug_api.h"

#define REG_NAME_WIDTH 7
extern volatile uint32_t g_crash_steps;
extern void panicHandler(void *context);

typedef struct
{
    /* saved in assembler */
    long LR; // X1/ra
    long SP; // X2
    long X3;
    long X4;
    long X5;
    long X6;
    long X7;
    long X8;
    long X9;
    long X10;
    long X11;
    long X12;
    long X13;
    long X14;
    long X15;
    long X16;
    long X17;
    long X18;
    long X19;
    long X20;
    long X21;
    long X22;
    long X23;
    long X24;
    long X25;
    long X26;
    long X27;
    long X28;
    long X29;
    long X30;
    long X31;
    long mepc;
    long mstatus;
} PANIC_CONTEXT;

typedef struct
{
    long mcause;
} FAULT_REGS;

static char *k_ll2str(intptr_t num, char *str)
{
    int i = 15;
    char         index[] = "0123456789ABCDEF";
    uintptr_t usnum   = (uintptr_t)num;

    for(; i >= 0; i--){
        str[i] = index[usnum % 16];
        usnum /= 16;
    }

    return str;
}

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    PANIC_CONTEXT *rv64_context = (PANIC_CONTEXT *)context;

    *pSP = (int *)rv64_context->SP;
    *pPC = (char *)rv64_context->mepc;
    *pLR = (char *)rv64_context->LR;
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int        x;
    long       *regs = (long *)context;
    char       s_panic_regs[REG_NAME_WIDTH + 14 + 8];
    FAULT_REGS stFregs;
    /* PANIC_CONTEXT */
    char s_panic_ctx[] = "X1     "
                         "X2     "
                         "X3     "
                         "X4     "
                         "X5     "
                         "X6     "
                         "X7     "
                         "X8     "
                         "X9     "
                         "X10    "
                         "X11    "
                         "X12    "
                         "X13    "
                         "X14    "
                         "X15    "
                         "X16    "
                         "X17    "
                         "X18    "
                         "X19    "
                         "X20    "
                         "X21    "
                         "X22    "
                         "X23    "
                         "X24    "
                         "X25    "
                         "X26    "
                         "X27    "
                         "X28    "
                         "X29    "
                         "X30    "
                         "X31    "
                         "MEPC   "
                         "MSTAT  ";

    /* FAULT_REGS */
    char s_panic_reg[] = "MCAUSE ";

    if (regs == NULL) {
        return;
    }

    print_func("========== Regs info  ==========\r\n");

    /* show PANIC_CONTEXT */
    for (x = 0; x < sizeof(s_panic_ctx) / REG_NAME_WIDTH; x++) {
        memcpy(&s_panic_regs[0], &s_panic_ctx[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_ll2str(regs[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11 + 8] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12 + 8] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13 + 8] = 0;
        print_func(s_panic_regs);
    }

    /* show FAULT_REGS */
    stFregs.mcause  = __get_MCAUSE();
    for (x = 0; x < sizeof(stFregs) / sizeof(long); x++) {
        memcpy(&s_panic_regs[0], &s_panic_reg[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_ll2str(((long *)(&stFregs))[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11 + 8] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12 + 8] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13 + 8] = 0;
        print_func(s_panic_regs);
    }
}

#if (RHINO_CONFIG_CLI_AS_NMI > 0)
void panicNmiInputFilter(uint8_t ch)
{
    static int  check_cnt = 0; /* for '$#@!' */

    if ( ch == '$' && check_cnt == 0) {
        check_cnt++;
    }
    else if ( ch == '#' && check_cnt == 1) {
        check_cnt++;
    }
    else if ( ch == '@' && check_cnt == 2) {
        check_cnt++;
    }
    else if ( ch == '!' && check_cnt == 3) {
        panicNmiFlagSet();
        __asm__ __volatile__("udf":::"memory");
    }
    else {
        check_cnt = 0;
    }
}
#else
void panicNmiInputFilter(uint8_t ch){}
#endif

void exceptionHandler(void *context)
{
    g_crash_steps++;
    if (g_crash_steps > 1) {
        context = NULL;
    } 
    panicHandler(context);
}

