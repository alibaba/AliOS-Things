/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <errno.h>
#include "aos/kernel.h"
#include <stdio.h>
#include "errno_mapping.h"
#include <time.h>
#include "aos_common.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

#define KERNEL_STACK_SIZE 1024

static unsigned int used_bitmap;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    int ret;

    ktask_t *task_handle = NULL;

    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(stack_size * sizeof(cpu_stack_t));

    if (NULL == ustack_buf) {
        printf("%s: alloc stack failed\r\n", __func__);
        return -ENOMEM;
    }

    ret = (int)krhino_utask_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                   (tick_t)0u, ustack_buf, stack_size,
                                    KERNEL_STACK_SIZE / sizeof(cpu_stack_t),
                                    fn, 1u);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, int stack_size, int prio)
{
    int ret;
    cpu_stack_t *ustack_buf;

    ustack_buf = malloc(stack_size * sizeof(cpu_stack_t));

    if (NULL == ustack_buf) {
        printf("%s: alloc stack failed\r\n", __func__);
        return -ENOMEM;
    }

    ret = (int)krhino_utask_create((ktask_t **)(&(task->hdl)), name, arg,
                                    prio, (tick_t)0u,
                                    ustack_buf, stack_size,
                                    KERNEL_STACK_SIZE / sizeof(cpu_stack_t),
                                    fn, 1u);

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

void aos_task_exit(int code)
{
    (void)code;

    krhino_utask_del(NULL);
}

#endif

int aos_task_key_create(aos_task_key_t *key)
{
    int i;

    for (i = RHINO_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
int aos_mutex_new(aos_mutex_t *mutex)
{
    kstat_t   ret;
    kmutex_t *m;

    if (mutex == NULL) {
        return -EINVAL;
    }

#ifdef USPACE_DYN
    ret = (int)krhino_mutex_dyn_create((kmutex_t **)(&(mutex->hdl)), "AOS");
    ERRNO_MAPPING(ret);
#endif

    m = malloc(sizeof(kmutex_t));
    if (m == NULL) {
        return -ENOMEM;
    }

    ret = krhino_mutex_create(m, "AOS");
    if (ret != RHINO_SUCCESS) {
        free(m);
        m = NULL;
        ERRNO_MAPPING(ret);
    }

    mutex->hdl = m;

    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

#ifdef USPACE_DYN
    krhino_mutex_dyn_del(mutex->hdl);
#else
    krhino_mutex_del(mutex->hdl);

    free(mutex->hdl);

    mutex->hdl = NULL;
#endif
}

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

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    kmutex_t *k_mutex;

    if (mutex == NULL) {
        return 0;
    }

    k_mutex = mutex->hdl;

    if (k_mutex == NULL) {
        return 0;
    }

    return 1;
}
#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC > 0) && (RHINO_CONFIG_SEM > 0))
int aos_sem_new(aos_sem_t *sem, int count)
{
    kstat_t ret;
    ksem_t *s;

    if (sem == NULL) {
        return -EINVAL;
    }

#ifdef USPACE_DYN
    ret = (int)krhino_sem_dyn_create((ksem_t **)(&(sem->hdl)), "AOS", count);
    ERRNO_MAPPING(ret);
#endif

    s = malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        free(s);
        s = NULL;
        ERRNO_MAPPING(ret);
    }

    sem->hdl = s;

    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

#ifdef USPACE_DYN
    krhino_sem_dyn_del(sem->hdl);
#else
    krhino_sem_del(sem->hdl);

    free(sem->hdl);

    sem->hdl = NULL;
#endif
}

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

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    krhino_sem_give(sem->hdl);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    ksem_t *k_sem;

    if (sem == NULL) {
        return 0;
    }

    k_sem = sem->hdl;

    if (k_sem == NULL) {
        return 0;
    }

    return 1;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    krhino_sem_give_all(sem->hdl);
}

#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    kstat_t       ret;
    kbuf_queue_t *q;

    if ((queue == NULL) || (buf == NULL)) {
        return -EINVAL;
    }

#ifdef USPACE_DYN
    ret = (int)krhino_buf_queue_dyn_create((kbuf_queue_t **)(&(queue->hdl)), "AOS", size, max_msg);
    ERRNO_MAPPING(ret);
#endif

    q = malloc(sizeof(kbuf_queue_t));
    if (q == NULL) {
        return -ENOMEM;
    }

    ret = krhino_buf_queue_create(q, "AOS", buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        free(q);
        q = NULL;
        ERRNO_MAPPING(ret);
    }

    queue->hdl = q;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL) {
        return;
    }

#ifdef USPACE_DYN
    krhino_buf_queue_dyn_del(queue->hdl);
#else
    krhino_buf_queue_del(queue->hdl);

    free(queue->hdl);

    queue->hdl = NULL;
#endif
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    int ret;

    if ((queue == NULL) || (msg == NULL)) {
        return -EINVAL;
    }

    ret = krhino_buf_queue_send(queue->hdl, msg, size);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    int ret;

    if (queue == NULL) {
        return -EINVAL;
    }

    ret = krhino_buf_queue_recv(queue->hdl, MS2TICK(ms), msg, size);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    kbuf_queue_t *k_queue;

    if (queue == NULL) {
        return 0;
    }

    k_queue = queue->hdl;

    if (k_queue == NULL) {
        return 0;
    }

    return 1;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue)) {
        return NULL;
    }

    return ((kbuf_queue_t *)queue->hdl)->buf;
}
#endif

#if (RHINO_CONFIG_TIMER > 0)
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                  int ms, int repeat)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), 0, arg, 1);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                      arg, 1);
    }

    if (ret != RHINO_SUCCESS) {
        ERRNO_MAPPING(ret);
    }

    return 0;
}

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      int ms, int repeat, unsigned char auto_run)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), 0, arg,
                                      auto_run);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                      arg, auto_run);
    }

    if (ret != RHINO_SUCCESS) {
        ERRNO_MAPPING(ret);
    }

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL) {
        return;
    }

    krhino_timer_dyn_del(timer->hdl);
    timer->hdl = NULL;
}

int aos_timer_start(aos_timer_t *timer)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_start(timer->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_timer_stop(aos_timer_t *timer)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_stop(timer->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_change(timer->hdl, MS2TICK(ms), MS2TICK(ms));
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}
#endif

long long aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

long long aos_now_ms(void)
{
    return krhino_sys_time_get();
}

char *aos_now_time_str(char *buffer, const int len)
{
    const long long ms = aos_now_ms();
    if (buffer != NULL && len > 0) {
        time_t actualTime = { ms / 1000 };
        memset(buffer, 0, len);
        strftime(buffer, len, "%m-%d %H:%M:%S", gmtime(&actualTime));
        if ((int)(len - strlen(buffer) - 1) > 0) {
            const int milli    = ms % 1000;
            char      msStr[8] = "";
	    memset(msStr, 0, sizeof(msStr));
            snprintf(msStr, sizeof(msStr), ".%03d", milli);
            strncat(buffer, msStr, len - strlen(buffer) - 1);
        }
    }

    return buffer;
}

void aos_msleep(int ms)
{
    krhino_task_sleep(MS2TICK(ms));
}

