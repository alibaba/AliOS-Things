/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "debug_api.h"
#include "k_backtrace.h"
#include "aos/cli.h"

void debug_backtrace_now()
{
    backtrace_now(NULL, NULL, BACKTRACE_KSPACE, printk);
}

void debug_panic_backtrace(char *PC, int *SP, char *LR,
                           int (*print_func)(const char *fmt, ...))
{
#if (RHINO_CONFIG_USER_SPACE > 0)
    static int  *U_SP = NULL;
    static char *U_PC = NULL;
    static char *U_LR = NULL;

    /* need uspace backtrace usr-kernel panic*/
    if (debug_uspace_panic_type_get() == PANIC_IN_USER_KERNEL) {
        debug_usapce_context_get(NULL, &U_PC, &U_LR, &U_SP);
        print_func("========== Uspace Call stack ==========\r\n");
        backtrace_handle(U_PC, U_SP, U_LR, BACKTRACE_USPACE, print_func);
    }

    if (debug_uspace_panic_type_get() == PANIC_IN_USER) {
        /* PS/SP/LR have saved */
        print_func("========== Uspace Call stack ==========\r\n");
        backtrace_handle(PC, SP, LR, BACKTRACE_USPACE, print_func);
        return;
    }
#endif
    print_func("========== Call stack ==========\r\n");
    backtrace_handle(PC, SP, LR, BACKTRACE_KSPACE, print_func);
}
