/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef POSIX_SIGNAL_H
#define POSIX_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <k_api.h>
#include <pthread.h>

#define SIGEV_NONE      0
#define SIGEV_SIGNAL    1
#define SIGEV_THREAD    2

union sigval
{
    int   sival_int;
    void  *sival_ptr;
};

struct sigevent {
    int            sigev_notify;
    int            sigev_signo;
    union sigval   sigev_value;
    void           (*sigev_notify_function)(union sigval);
    pthread_attr_t *sigev_notify_attributes;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
