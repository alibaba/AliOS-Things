/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <prctl.h>
#include <pthread.h>
#include <k_api.h>

extern pthread_mutex_t g_pthread_lock;

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
        ret = pthread_mutex_lock(&g_pthread_lock);
        if (ret != 0) {
            return -1;
        }

        p_tcb = krhino_cur_task_get();
        p_tcb->task_name = p_str;

        pthread_mutex_unlock(&g_pthread_lock);

        return 0;
    } else {
        return 1;
    }
}

