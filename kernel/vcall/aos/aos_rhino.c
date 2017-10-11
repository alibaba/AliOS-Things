/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <errno.h>
#include <aos/aos.h>
#include "errno_mapping.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#error "RHINO_CONFIG_KOBJ_DYN_ALLOC must be configured!"
#endif

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static unsigned int used_bitmap;

extern void hal_reboot(void);
void aos_reboot(void)
{
    hal_reboot();
}

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return krhino_version_get();
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    int ret;

    ktask_t *task_handle = NULL;

    ret = (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI, 0,
                                      stack_size / sizeof(cpu_stack_t), fn, 1u);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    int ret;
    ret = (int)krhino_task_dyn_create((ktask_t **)(&(task->hdl)), name, arg, prio, 0,
                                      stack_size / sizeof(cpu_stack_t), fn, 1u);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

void aos_task_exit(int code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}

const char *aos_task_name(void)
{
    return krhino_cur_task_get()->task_name;
}

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

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    int ret;
    ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &vp);

    return vp;
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    kstat_t   ret;
    kmutex_t *m;

    if (mutex == NULL) {
        return -EINVAL;
    }

    m = aos_malloc(sizeof(kmutex_t));
    if (m == NULL) {
        return -ENOMEM;
    }

    ret = krhino_mutex_create(m, "AOS");
    if (ret != RHINO_SUCCESS) {
        aos_free(m);
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

    krhino_mutex_del(mutex->hdl);

    aos_free(mutex->hdl);

    mutex->hdl = NULL;
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
    int ret;

    if (mutex == NULL) {
        return false;
    }

    ret = krhino_mutex_is_valid(mutex->hdl);
    return (ret == RHINO_SUCCESS);
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    kstat_t ret;
    ksem_t *s;

    if (sem == NULL) {
        return -EINVAL;
    }

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
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

    krhino_sem_del(sem->hdl);

    aos_free(sem->hdl);

    sem->hdl = NULL;
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

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    kstat_t       ret;
    kbuf_queue_t *q;

    if ((queue == NULL) || (buf == NULL)) {
        return -EINVAL;
    }

    q = aos_malloc(sizeof(kbuf_queue_t));
    if (q == NULL) {
        return -ENOMEM;
    }

    ret = krhino_buf_queue_create(q, "AOS", buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
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

    krhino_buf_queue_del(queue->hdl);

    aos_free(queue->hdl);

    queue->hdl = NULL;
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
    int ret;

    if (queue == NULL) {
        return false;
    }

    ret = krhino_buf_queue_is_valid(queue->hdl);
    return (ret == RHINO_SUCCESS);
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue)) {
        return NULL;
    }

    return ((kbuf_queue_t *)queue->hdl)->buf;
}

int aos_sched_disable()
{
    int ret;

    ret = (int)krhino_sched_disable();
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_sched_enable()
{
    int ret;

    ret = (int)krhino_sched_enable();
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    kstat_t   ret;
    ktimer_t *t;

    if (timer == NULL) {
        return -EINVAL;
    }

    t = aos_malloc(sizeof(ktimer_t));
    if (t == NULL) {
        return -ENOMEM;
    }

    if (repeat == 0) {
        ret = krhino_timer_create(t, "AOS", (timer_cb_t)fn, MS2TICK(ms), 0, arg, 1);
    } else {
        ret = krhino_timer_create(t, "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                  arg, 1);
    }

    if (ret != RHINO_SUCCESS) {
        aos_free(t);
        ERRNO_MAPPING(ret);
    }

    timer->hdl = t;

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL) {
        return;
    }

    krhino_timer_del(timer->hdl);

    aos_free(timer->hdl);

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

#if (RHINO_CONFIG_WORKQUEUE  > 0)
int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    kstat_t ret;

    cpu_stack_t  *stk;
    kworkqueue_t *wq;

    if (workqueue == NULL) {
        return -EINVAL;
    }

    if (stack_size < sizeof(cpu_stack_t)) {
        return -EINVAL;
    }

    stk = aos_malloc(stack_size);
    if (stk == NULL) {
        return -ENOMEM;
    }

    wq = aos_malloc(sizeof(kworkqueue_t));
    if (wq == NULL) {
        aos_free(stk);
        return -ENOMEM;
    }

    ret = krhino_workqueue_create(wq, "AOS", pri, stk,
                                  stack_size / sizeof(cpu_stack_t));
    if (ret != RHINO_SUCCESS) {
        aos_free(wq);
        aos_free(stk);
        ERRNO_MAPPING(ret);
    }

    workqueue->hdl = wq;
    workqueue->stk = stk;

    return 0;
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    if (workqueue == NULL) {
        return;
    }

    krhino_workqueue_del(workqueue->hdl);

    aos_free(workqueue->hdl);
    aos_free(workqueue->stk);

    workqueue->hdl = NULL;
    workqueue->stk = NULL;
}

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    kstat_t  ret;
    kwork_t *w;

    if (work == NULL) {
        return -EINVAL;
    }

    w = aos_malloc(sizeof(kwork_t));
    if (w == NULL) {
        return -ENOMEM;
    }

    ret = krhino_work_init(w, fn, arg, MS2TICK(dly));
    if (ret != RHINO_SUCCESS) {
        aos_free(w);
        ERRNO_MAPPING(ret);
    }

    work->hdl = w;

    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    if (work == NULL) {
        return;
    }

    aos_free(work->hdl);
    work->hdl = NULL;
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    int ret;

    if ((workqueue == NULL) || (work == NULL)) {
        return -EINVAL;
    }

    ret = krhino_work_run(workqueue->hdl, work->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_work_sched(aos_work_t *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_sched(work->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_work_cancel(aos_work_t *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_cancel(work->hdl);
    if (ret == RHINO_WORKQUEUE_WORK_RUNNING) {
        return -EBUSY;
    }

    return 0;
}
#endif

void *aos_zalloc(unsigned int size)
{
    void *tmp = NULL;
    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);

#ifndef AOS_BINS
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif
    } else {
        tmp = krhino_mm_alloc(size);
    }

#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {
        bzero(tmp, size);
    }

    return tmp;
}

void *aos_malloc(unsigned int size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);

#ifndef AOS_BINS
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif
    } else {
        tmp = krhino_mm_alloc(size);
    }

#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void *aos_realloc(void *mem, unsigned int size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);

#ifndef AOS_BINS
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif
    } else {
        tmp = krhino_mm_realloc(mem, size);
    }

#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
}

void aos_alloc_trace(void *addr, size_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    krhino_owner_attach(g_kmm_head, addr, allocator);
#endif
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}

long long aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

long long aos_now_ms(void)
{
    return krhino_sys_time_get();
}

void aos_msleep(int ms)
{
    krhino_task_sleep(MS2TICK(ms));
}

void aos_init(void)
{
    krhino_init();
}

void aos_start(void)
{
    krhino_start();
}

