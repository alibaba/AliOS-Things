/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <debug_api.h>

#if (DEBUG_CONFIG_BACKTRACE > 0)

/** WARNING: to use call trace facilities, enable
 *  compiler's frame pointer feature:
 *  -fno-omit-frame-pointer
 */


/* function call stack graph
 *
 *                                   low addr
 *                                  |___..___|
 *                               ---|___fp___|<--- previous fp
 * func_A stack start -->        |  |___lr___|<--- current fp
 *                               |  |        |
 *                               |  |   ..   |
 *                               |  |________|
 *                             --|--|___fp___|
 * func_B stack start -->      | -->|___lr___|
 *                             |    |        |
 *                             |    |        |
 *                             |    |___..___|
 *                           --|----|___fp___|
 * func_C stack start -->    |  --->|___lr___|
 *                           |      |        |
 *                           |       high addr
 *                           ------>
 *
 *                   func_C () {
 *                      ...
 *                      func_B();
 *                      ...
 *                   }
 *
 *                   func_B() {
 *                      ...
 *                      func_A();
 *                      ...
 *                   }
 */

static void backtrace_stack(int (*print_func)(const char *fmt, ...),
                            unsigned long *fp)
{
    unsigned long *lr;
    unsigned long *pc;

    while (1) {
        lr = (unsigned long *)(*fp);
        pc = lr - 1;

        if (lr == (unsigned long *)krhino_task_deathbed) {
            print_func("backtrace: reached task deathbed!!!\r\n");
            break;
        }

        print_func("backtrace: %p\r\n", pc);

        fp = (unsigned long *)*(fp - 1);
    }
}

void backtrace_now(int (*print_func)(const char *fmt, ...))
{
    static int     processing_backtrace = 0;
    unsigned long *fp;
    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    if (processing_backtrace == 0) {
        processing_backtrace = 1;
    } else {
        print_func("backtrace nested...\r\n");
        return;
    }

#if defined(__GNUC__)
    __asm__("mov %0, fp" : "=r"(fp));
#else
#error "Compiler is not gcc!"
#endif

    print_func("=== backtrace start ===\r\n");

    backtrace_stack(print_func, fp);

    print_func("=== backtrace end ===\r\n\r\n");

    processing_backtrace = 0;

    RHINO_CPU_INTRPT_ENABLE();
}

#endif
