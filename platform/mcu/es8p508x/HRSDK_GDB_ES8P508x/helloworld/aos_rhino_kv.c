/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <errno.h>
#include <aos/aos.h>
#include "errno_mapping.h"

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{

    int ret;
	/*
    ktask_t *task_handle = NULL;

    ret = (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI, 0,
                                      stack_size / sizeof(cpu_stack_t), fn, 1u);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }
*/
    ERRNO_MAPPING(ret);
}
AOS_EXPORT(int, aos_task_new, const char *, void (*)(void *), void *, int);


void aos_task_exit(int code)
{
	/*
    (void)code;

    krhino_task_dyn_del(NULL);*/
}
AOS_EXPORT(void, aos_task_exit, int);

const char *aos_task_name(void)
{
    return krhino_cur_task_get()->task_name;
}
AOS_EXPORT(const char *, aos_task_name, void);

int aos_mutex_new(aos_mutex_t *mutex)
{
    kstat_t   ret;
    kmutex_t *m;

    if (mutex == NULL) {
        return -EINVAL;
    }

//    m = aos_malloc(sizeof(kmutex_t));
//    if (m == NULL) {
//        return -ENOMEM;
//    }

    ret = krhino_mutex_create(m, "AOS");
    if (ret != RHINO_SUCCESS) {
//        aos_free(m);
        ERRNO_MAPPING(ret);
    }

    mutex->hdl = m;

    return 0;
}
AOS_EXPORT(int, aos_mutex_new, aos_mutex_t *);

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    krhino_mutex_del(mutex->hdl);

//    aos_free(mutex->hdl);

    mutex->hdl = NULL;
}
AOS_EXPORT(void, aos_mutex_free, aos_mutex_t *);

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    kstat_t ret;

    if (mutex == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_mutex_lock(mutex->hdl, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_mutex_lock(mutex->hdl, MS2TICK(timeout));
    }

    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}
AOS_EXPORT(int, aos_mutex_lock, aos_mutex_t *, unsigned int);

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    kstat_t ret;

    if (mutex == NULL) {
        return -EINVAL;
    }

    ret = krhino_mutex_unlock(mutex->hdl);
    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}
AOS_EXPORT(int, aos_mutex_unlock, aos_mutex_t *);



int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    kstat_t ret;

    if (sem == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_sem_take(sem->hdl, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_sem_take(sem->hdl, MS2TICK(timeout));
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}
AOS_EXPORT(int, aos_sem_wait, aos_sem_t *, unsigned int);

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    krhino_sem_give(sem->hdl);
}
AOS_EXPORT(void, aos_sem_signal, aos_sem_t *);

int aos_sem_is_valid(aos_sem_t *sem)
{
    int ret;

    if (sem == NULL) {
        return false;
    }

    ret = krhino_sem_is_valid(sem->hdl);
    return (ret == RHINO_SUCCESS);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    krhino_sem_give_all(sem->hdl);
}



long long aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}
AOS_EXPORT(long long, aos_now, void);

long long aos_now_ms(void)
{
    return krhino_sys_time_get();
}
AOS_EXPORT(long long, aos_now_ms, void);


void aos_init(void)
{
    krhino_init();
}

void aos_start(void)
{
    krhino_start();
}

