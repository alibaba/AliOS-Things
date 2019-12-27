/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SIGNAL_H
#define SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"
#include "pthread.h"
#include "posix_config.h"

#define SIGEV_NONE   0
#define SIGEV_SIGNAL 1
#define SIGEV_THREAD 2

union sigval {
    int   sival_int;
    void *sival_ptr;
};

struct sigevent {
    int             sigev_notify;
    int             sigev_signo;
    union sigval    sigev_value;
    pthread_attr_t *sigev_notify_attributes;

    void (*sigev_notify_function)(union sigval);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SIGNAL_H */

