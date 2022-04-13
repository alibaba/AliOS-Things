/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __POSIX_INTERNAL_PTHREAD_H
#define __POSIX_INTERNAL_PTHREAD_H

#include <pthread.h>
#include <aos/kernel.h>

#define PTHREAD_DEFAULT_STACK_SIZE 2048
#define PTHREAD_DEFAULT_GUARD_SIZE 256
#define PTHREAD_DEFAULT_PRIORITY   30
#define PTHREAD_DEFAULT_SLICE      10
#define PTHREAD_NAME_MAX_LEN  16

#define PTHREAD_TCB_MAGIC 0X11223344

#define DEFAULT_MUTEX_TYPE        PTHREAD_MUTEX_DEFAULT
#define DEFAULT_MUTEX_PROCOCOL    PTHREAD_PRIO_INHERIT
#define DEFAULT_MUTEX_PRIOCEILING 30
#define DEFAULT_MUTEX_PSHARED     PTHREAD_PROCESS_PRIVATE

#define DEFAULT_COND_CLOCK  CLOCK_REALTIME
#define DEFAULT_COND_SHARED PTHREAD_PROCESS_PRIVATE


typedef struct pthread_cleanup {
    int cancel_type;
    struct pthread_cleanup *prev;
    void (*cleanup_routine)(void *para);
    void *para;
} pthread_cleanup_t;

typedef struct pthread_tcb {
    aos_task_t task;    /* The rhino task handle. */
    unsigned int magic; /* The pthread tcb memory magic number. */
    void *(*thread_entry)(void *para); /* The start routine of the thread. */
    void *thread_para;   /* The parameter of start routine. */
    aos_sem_t join_sem;  /* The semaphore for pthread_join. */
    pthread_cleanup_t *cleanup; /* The registered cleanup function for the thread.*/
    void *environ;
    void **tls;
    void  *return_value; /* The thread's return value. */
    pthread_attr_t  attr; /* The thread's attribute. */
    char thread_name[PTHREAD_NAME_MAX_LEN + 1];  /* The thread's name. */
} pthread_tcb_t;

static inline pthread_tcb_t* __pthread_get_tcb(pthread_t thread)
{
    pthread_tcb_t* ptcb = (pthread_tcb_t*)thread;

    if ((ptcb == NULL) || (ptcb->magic != PTHREAD_TCB_MAGIC)) {
        return NULL;
    }

    return ptcb;
}

void pthread_tsd_dtors(void);

#endif /* __POSIX_INTERNAL_PTHREAD_H */
