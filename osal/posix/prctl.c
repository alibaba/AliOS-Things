/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include "posix/prctl.h"

#if (POSIX_CONFIG_PRCTL_ENABLE > 0)

extern kmutex_t g_pthread_mutex;

int prctl(int option, ...)
{
    va_list       ap;
    unsigned long arg;
    char         *p_str;
    ktask_t      *p_tcb;
    int           ret = -1;

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
        ret = krhino_mutex_lock(&g_pthread_mutex, RHINO_WAIT_FOREVER);
        if (ret != 0) {
            return -1;
        }

        p_tcb = krhino_cur_task_get();
        p_tcb->task_name = p_str;

        krhino_mutex_unlock(&g_pthread_mutex);

        return 0;
    } else {
        return 1;
    }
}

#endif
