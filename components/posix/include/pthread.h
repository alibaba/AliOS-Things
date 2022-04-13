/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <stdint.h>
#include <time.h>
#include <sched.h> /* only for be compatible with toolchain 7.3.1.*/

#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_DEFAULT 1
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_SCOPE_PROCESS 1

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_MASKED 2
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCELED ((void *)-1)

#define PTHREAD_DYN_INIT    0X01
#define PTHREAD_STATIC_INIT 0X02
#define PTHREAD_MUTEX_INITIALIZER {PTHREAD_STATIC_INIT, NULL, {0}}
#define PTHREAD_COND_INITIALIZER  {PTHREAD_STATIC_INIT, 0, 0, NULL, NULL, NULL, {0}}
#define PTHREAD_ONCE_INIT 0

typedef void * pthread_t;
typedef int pthread_once_t;

typedef struct pthread_attr {
    uint8_t flag;
    void   *stackaddr;       /* the start addr of the stack of the pthead */
    size_t  stacksize;       /* the size of the stack of the pthead */
    size_t  guardsize;       /* guardsize is set to protect the stack, not supported */
    int     contentionscope; /* the scope of contention, only PTHREAD_SCOPE_SYSTEM is supported */
    int     inheritsched;    /* when set to PTHREAD_INHERIT_SCHED, specifies that the thread scheduling attributes
                                shall be inherited from the creating thread, and the scheduling attributes in this
                                attr argument shall be ignored */
    int     schedpolicy;     /* the sched policy of the thread */
    int     detachstate;     /* when set to PTHREAD_CREATE_JOINABLE, thread will not end untill the creating thread end */
    int     sched_priority;  /* The thread scheduling priority */
    uint64_t sched_slice;    /* The time slice in SCHED_RR mode (ms) */
} pthread_attr_t;

typedef struct pthread_mutexattr {
    uint8_t flag;
    int type;
    int protocol;
    int prioceiling;
    int pshared;
} pthread_mutexattr_t;

typedef struct pthread_mutex {
    uint8_t              flag;
    void                *mutex;
    pthread_mutexattr_t  attr;
} pthread_mutex_t;

typedef struct pthread_condattr {
    uint8_t flag;
    int     pshared; /* allow this to be shared amongst processes */
    clock_t clock;   /* specifiy clock for timeouts */
} pthread_condattr_t;

typedef struct pthread_cond {
    uint8_t flag;
    int     waiting;
    int     signals;
    void    *lock;
    void    *wait_sem;
    void    *wait_done;
    pthread_condattr_t attr;
} pthread_cond_t;

/********* Thread Specific Data *********/
typedef int pthread_key_t;

typedef struct key_value {
    pthread_t  thread;
    const uint32_t  *value;
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

int       pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
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
int       pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
pthread_t pthread_self(void);
int       pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id);
int       pthread_setconcurrency(int new_level);
int       pthread_getconcurrency(void);
int       pthread_setschedprio(pthread_t thread, int prio);
int       pthread_setname_np(pthread_t thread, const char *name);
int       pthread_timedjoin_np(pthread_t thread, void **retval, const struct timespec *abstime);

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
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_getinheritsched(const pthread_attr_t *__restrict attr, int *__restrict inheritsched);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int pthread_attr_setscope(pthread_attr_t *attr, int scope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_getprioceiling(const pthread_mutex_t *__restrict mutex, int *__restrict prioceiling);
int pthread_mutex_setprioceiling(pthread_mutex_t *__restrict mutex, int prioceiling, int *__restrict old_ceiling);

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict attr, int *__restrict protocol);
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *__restrict attr, int *__restrict prioceiling);
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_setclock(pthread_condattr_t *attr, clock_t clock);

int   pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int   pthread_setspecific(pthread_key_t key, const void *value);
void* pthread_getspecific(pthread_key_t key);
int   pthread_key_delete(pthread_key_t key);

#ifdef __cplusplus
}
#endif
#endif /* _PTHREAD_H */
