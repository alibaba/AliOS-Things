/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <sys/prctl.h>
#include <stdarg.h>

int prctl(int option, ...)
{
    va_list       ap;
    unsigned long arg;
    char         *p_str;
    ktask_t      *p_tcb;
    CPSR_ALLOC();

    if (option == PR_SET_NAME) {
        /* set ap to the start stack address of argument list */
        va_start(ap, option);

        /*
         * checkout the first argument from argument list, and force it
         * to  unsigned long, the ap address will be moved at same time.
         */

        arg = va_arg(ap, unsigned long);

        va_end(ap);

        p_str = (char *)arg;
        if (NULL == p_str) {
            return 1;
        }

        /* get TCB of current task */
        RHINO_CRITICAL_ENTER();

        p_tcb = g_active_task[cpu_cur_get()];
        p_tcb->task_name = p_str;

        RHINO_CRITICAL_EXIT();

        return 0;
    } else {
        return 1;
    }
}

