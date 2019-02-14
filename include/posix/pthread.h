/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PTHREAD_H
#define PTHREAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <time.h>
#include <sys/time.h>

#include "k_api.h"
#include "posix_config.h"

typedef ktask_t *pthread_t;
typedef int      pthread_once_t;

/* the value of contentionscope in struct pthread_attr_t */
#define PTHREAD_SCOPE_PROCESS   0
#define PTHREAD_SCOPE_SYSTEM    1

/* the value of inheritsched in struct pthread_attr_t */
#define PTHREAD_INHERIT_SCHED   1
#define PTHREAD_EXPLICIT_SCHED  2

/* the value of detachstate in struct pthread_attr_t */
#define PTHREAD_CREATE_DETACHED 0
#define PTHREAD_CREATE_JOINABLE 1

/* the value of scheduling policy */
#define SCHED_OTHER 0
#define SCHED_FIFO  1
#define SCHED_RR    2

#define RH_LOW_PRI     RHINO_CONFIG_PRI_MAX /* low priority rhino numbering */
#define RH_HIGH_PRI    0                    /* high priority rhino numbering */
#define POSIX_LOW_PRI  0                    /* low priority POSIX numbering */
#define POSIX_HIGH_PRI RHINO_CONFIG_PRI_MAX /* high priority POSIX numbering */

/* conversion pri between POSIX and RHINO */
#define PRI_CONVERT_PX_RH(prior) (POSIX_HIGH_PRI - prior)

#define DEFAULT_THREAD_STACK_SIZE 2048
#define DEFAULT_THREAD_PRIORITY   30
#define DEFAULT_THREAD_SLICE      10

enum {
    PTHREAD_CANCEL_ASYNCHRONOUS,
    PTHREAD_CANCEL_ENABLE,
    PTHREAD_CANCEL_DEFERRED,
    PTHREAD_CANCEL_DISABLE,
    PTHREAD_CANCELED
};

typedef int pid_t;

struct sched_param {
    int    sched_priority; /* process execution scheduling priority */
    size_t slice;          /* time slice in SCHED_RR mode (ms) */
};

typedef struct pthread_attr {
    unsigned char      is_initialized;  /* if the attr is initialized set to 1, otherwise set to 0 */
    void              *stackaddr;       /* the start addr of the stack of the pthead */
    size_t             stacksize;       /* the size of the stack of the pthead */
    unsigned char      contentionscope; /* the scope of contention, only PTHREAD_SCOPE_SYSTEM is supported */
    unsigned char      inheritsched;    /* when set to PTHREAD_INHERIT_SCHED, specifies that the thread scheduling attributes
                                           shall be inherited from the creating thread, and the scheduling attributes in this
                                           attr argument shall be ignored */
    unsigned char      schedpolicy;     /* the sched policy of the thread */
    struct sched_param schedparam;      /* the parameter of the thread scheduling */
    size_t             guardsize;       /* guardsize is set to protect the stack, not supported */
    unsigned char      detachstate;     /* when set to PTHREAD_CREATE_JOINABLE, thread will not end untill the creating thread end */
} pthread_attr_t;

#define PTHREAD_MAGIC 0x11223344

typedef struct _pthread_cleanup {
    void                    *para;
    int                      cancel_type;
    struct _pthread_cleanup *prev;

    void (*cleanup_routine)(void *para);
} _pthread_cleanup_t;

typedef struct _pthread_tcb {
    unsigned int   magic;
    pthread_attr_t attr;
    ktask_t       *tid;

    void *(*thread_entry)(void *para);
    void *thread_para;

    ksem_t *join_sem;

    unsigned char          cancel_state;
    volatile unsigned char cancel_type;
    volatile unsigned char canceled;

    _pthread_cleanup_t *cleanup;

    void **tls;
    void *return_value;
} _pthread_tcb_t;

RHINO_INLINE _pthread_tcb_t *_pthread_get_tcb(pthread_t thread)
{
    _pthread_tcb_t *ptcb;

    if (thread == NULL) {
        return NULL;
    }

    ptcb = (_pthread_tcb_t *)thread->user_info[PTHREAD_CONFIG_USER_INFO_POS];
    if (ptcb != NULL) {
        if (ptcb->magic != PTHREAD_MAGIC) {
            ptcb = NULL;
        }
    }

    return ptcb;
}

RHINO_INLINE pid_t _pthread_to_pid(pthread_t thread)
{
    if (thread == NULL) {
        return -1;
    }

    return (pid_t)thread;
}

RHINO_INLINE pthread_t _pid_to_pthread(pid_t pid)
{
    if (pid == -1) {
        return NULL;
    }

    return (pthread_t)pid;
}

int       pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                         void *(*start_routine)(void *), void *arg);
void      pthread_exit(void *value_ptr);
int       pthread_detach(pthread_t thread);
int       pthread_join(pthread_t thread, void **retval);
int       pthread_cancel(pthread_t thread);
void      pthread_testcancel(void);
int       pthread_setcancelstate(int state, int *oldstate);
int       pthread_setcanceltype(int type, int *oldtype);
int       pthread_kill(pthread_t thread, int sig);
int       pthread_equal(pthread_t t1, pthread_t t2);
int       pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *pParam);
void      pthread_cleanup_pop(int execute);
void      pthread_cleanup_push(void (*routine)(void *), void *arg);
pthread_t pthread_self(void);

int sched_yield(void);
int sched_get_priority_min(int policy);
int sched_get_priority_max(int policy);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int pthread_attr_setscope(pthread_attr_t *attr, int scope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);

typedef struct pthread_mutexattr {
    int is_initialized;
    int recursive;
} pthread_mutexattr_t;

typedef struct pthread_mutex {
    kmutex_t *mutex;
    int       initted;
} pthread_mutex_t;

#define PTHREAD_INITIALIZED_OBJ 0xABCDEFEF
#define PTHREAD_UNUSED_YET_OBJ  -1
#define PTHREAD_ONCE_INIT       0

#define PTHREAD_MUTEX_INITIALIZER { NULL, PTHREAD_UNUSED_YET_OBJ }

/* Mutex types */
enum {
    PTHREAD_MUTEX_NORMAL     = 0, /* Normal mutex        */
    PTHREAD_MUTEX_RECURSIVE  = 1, /* Recursive mutex     */
    PTHREAD_MUTEX_ERRORCHECK = 2, /* Error checking mutex*/

    PTHREAD_MUTEX_ERRORCHECK_NP = PTHREAD_MUTEX_ERRORCHECK, /* Error checking mutex*/
    PTHREAD_MUTEX_RECURSIVE_NP  = PTHREAD_MUTEX_RECURSIVE,  /* Recursive mutex */
    PTHREAD_MUTEX_DEFAULT       = PTHREAD_MUTEX_NORMAL      /* Default mutex */
};

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);

typedef struct pthread_cond {
    kmutex_t *lock;
    int       waiting;
    int       signals;
    ksem_t   *wait_sem;
    ksem_t   *wait_done;
} pthread_cond_t;

typedef struct pthread_condattr {
    int __dummy;
} pthread_condattr_t;

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);

/********* Thread Specific Data *********/
typedef int pthread_key_t;

typedef struct key_value {
    pthread_t thread;
    uint32_t *value;
} key_value_t;

typedef struct pthread_key_value {
    key_value_t key_value;

    struct pthread_key_value *next;
} pthread_key_value_t;

typedef struct pthread_key_value_head {
    void (*fun)(void*);

    pthread_key_value_t *next;
} pthread_key_value_head_t;

typedef struct pthread_key_list_s {
    pthread_key_t            key_num;
    pthread_key_value_head_t head;

    struct pthread_key_list_s *next;
} pthread_key_list_t;

int   pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int   pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
int   pthread_key_delete(pthread_key_t key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PTHREAD_H */
