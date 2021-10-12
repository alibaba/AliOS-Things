/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

/* Here is only signal definitions, no signal function is supported. */
#define SIGHUP    1
#define SIGINT    2
#define SIGQUIT   3
#define SIGILL    4
#define SIGTRAP   5
#define SIGABRT   6
#define SIGIOT    SIGABRT
#define SIGBUS    7
#define SIGFPE    8
#define SIGKILL   9
#define SIGUSR1   10
#define SIGSEGV   11
#define SIGUSR2   12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGTERM   15
#define SIGSTKFLT 16
#define SIGCHLD   17
#define SIGCONT   18
#define SIGSTOP   19
#define SIGTSTP   20
#define SIGTTIN   21
#define SIGTTOU   22
#define SIGURG    23
#define SIGXCPU   24
#define SIGXFSZ   25
#define SIGVTALRM 26
#define SIGPROF   27
#define SIGWINCH  28
#define SIGIO     29
#define SIGPOLL   29
#define SIGPWR    30
#define SIGSYS    31
#define SIGUNUSED SIGSYS
#define NSIG 32

#define SIG_BLOCK     0
#define SIG_UNBLOCK   1
#define SIG_SETMASK   2


typedef uint32_t sigset_t;
typedef void (*sighandler_t)(int);
typedef sighandler_t _sig_func_ptr;

#define SIG_ERR  ((sighandler_t)-1)
#define SIG_DFL  ((sighandler_t) 0)
#define SIG_IGN  ((sighandler_t) 1)

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
#endif

#endif /* _SIGNAL_H */
