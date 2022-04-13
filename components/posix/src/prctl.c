/*
 * Copyright (C) 2018-2021 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <sys/prctl.h>
#include <aos/errno.h>
#include <pthread.h>

__attribute__((weak)) int prctl(int option, ...)
{
    int ret = 0;
    va_list ap;
    unsigned long arg;

    if (option == PR_SET_NAME) {
        va_start(ap, option);
        arg = va_arg(ap, unsigned long);
        va_end(ap);

        if (arg == 0) {
            errno = EINVAL;
            return -1;
        }

        pthread_setname_np(pthread_self(), (char *)arg);
        return 0;
    } else {
        errno = ENOTSUP;
        return -1;
    }
}
