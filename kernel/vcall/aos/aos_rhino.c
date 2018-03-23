/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <errno.h>
#include <aos/aos.h>
#include "errno_mapping.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static unsigned int used_bitmap;

extern void hal_reboot(void);
void aos_reboot(void)
{
    hal_reboot();
}
AOS_EXPORT(void, aos_reboot, void);

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}
AOS_EXPORT(int, aos_get_hz, void);

const char *aos_version_get(void)
{
    return SYSINFO_KERNEL_VERSION;
}
AOS_EXPORT(const char *, aos_version_get, void);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
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
AOS_EXPORT(int, aos_task_new, const char *, void (*)(void *), void *, int);

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
AOS_EXPORT(int, aos_task_new_ext, aos_task_t *, const char *, void (*)(void *), void *, int, int);
void aos_task_exit(int code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}
AOS_EXPORT(void, aos_task_exit, int);
#endif

const char *aos_task_name(void)
{
    return krhino_cur_task_get()->task_name;
}
AOS_EXPORT(const char *, aos_task_name, void);

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
AOS_EXPORT(int, aos_task_key_create, aos_task_key_t *);

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}
AOS_EXPORT(void, aos_task_key_delete, aos_task_key_t);

#if (RHINO_CONFIG_TASK_INFO > 0)
int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    int ret;
    ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}
AOS_EXPORT(int, aos_task_setspecific, aos_task_key_t, void *);

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &vp);

    return vp;
}
AOS_EXPORT(void *, aos_task_getspecific, aos_task_key_t);
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
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
AOS_EXPORT(int, aos_mutex_new, aos_mutex_t *);

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    krhino_mutex_del(mutex->hdl);

    aos_free(mutex->hdl);

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

    if (k_mutex->blk_obj.obj_type != RHINO_MUTEX_OBJ_TYPE) {
        return 0;
    }

    return 1;
}
#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)&&(RHINO_CONFIG_SEM > 0))
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
AOS_EXPORT(int, aos_sem_new, aos_sem_t *, int);

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    krhino_sem_del(sem->hdl);

    aos_free(sem->hdl);

    sem->hdl = NULL;
}
AOS_EXPORT(void, aos_sem_free, aos_sem_t *);

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
    ksem_t *k_sem;

    if (sem == NULL) {
        return 0;
    }

    k_sem = sem->hdl;

    if (k_sem == NULL) {
        return 0;
    }

    if (k_sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
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

#if (RHINO_CONFIG_EVENT_FLAG > 0)

int aos_event_new(aos_event_t *event, unsigned int flags)
{
    int ret;
    ret = (int)krhino_event_dyn_create((kevent_t **)(&(event->hdl)), "AOS", flags);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

void aos_event_free(aos_event_t *event)
{
    if (event == NULL) {
        return;
    }

    krhino_event_dyn_del(event->hdl);

    event->hdl = NULL;
}

int aos_event_get
    (
    aos_event_t *event, 
    unsigned int flags, 
    unsigned char opt,
    unsigned int *actl_flags,
    unsigned int timeout
    )
{
    kstat_t ret;

    if (event == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_event_get(event->hdl, flags, opt, actl_flags, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_event_get(event->hdl, flags, opt, actl_flags, MS2TICK(timeout));
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}

int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt)
{
    kstat_t ret;

    if (event == NULL) {
        return -EINVAL;
    }

    ret = krhino_event_set(event->hdl, flags, opt);

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
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
AOS_EXPORT(int, aos_queue_new, aos_queue_t *, void *, unsigned int, int);

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL) {
        return;
    }

    krhino_buf_queue_del(queue->hdl);

    aos_free(queue->hdl);

    queue->hdl = NULL;
}
AOS_EXPORT(void, aos_queue_free, aos_queue_t *);

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
AOS_EXPORT(int, aos_queue_send, aos_queue_t *, void *, unsigned int);

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
AOS_EXPORT(int, aos_queue_recv, aos_queue_t *, unsigned int, void *, unsigned int *);

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

    if (k_queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
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
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    kstat_t   ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS", (timer_cb_t)fn, MS2TICK(ms), 0, arg, 1);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                  arg, 1);
    }

    if (ret != RHINO_SUCCESS) {
        ERRNO_MAPPING(ret);
    }

    return 0;
}
AOS_EXPORT(int, aos_timer_new, aos_timer_t *, void (*)(void *, void *), void *, int, int);

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat, unsigned char auto_run)
{
    kstat_t   ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS", (timer_cb_t)fn, MS2TICK(ms), 0, arg, auto_run);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                  arg, auto_run);
    }

    if (ret != RHINO_SUCCESS) {
        ERRNO_MAPPING(ret);
    }

    return 0;
}
AOS_EXPORT(int, aos_timer_new_ext, aos_timer_t *, void (*)(void *, void *), void *, int, int, unsigned char);

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL) {
        return;
    }

    krhino_timer_dyn_del(timer->hdl);
    timer->hdl = NULL;
}
AOS_EXPORT(void, aos_timer_free, aos_timer_t *);

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
AOS_EXPORT(int, aos_timer_start, aos_timer_t *);

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
AOS_EXPORT(int, aos_timer_stop, aos_timer_t *);

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
AOS_EXPORT(int, aos_timer_change, aos_timer_t *, int);

#endif
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
AOS_EXPORT(int, aos_workqueue_create, aos_workqueue_t *, int, int);

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    kstat_t ret;

    if (workqueue == NULL) {
        return;
    }

    while (1) {
        ret = krhino_workqueue_del(workqueue->hdl);
        if (ret != RHINO_SUCCESS) {
            /* give time for work queue handler to finish */
            krhino_task_sleep(1);
            continue;
        }
        else {
            break;
        }
    }

    aos_free(workqueue->hdl);
    aos_free(workqueue->stk);

    workqueue->hdl = NULL;
    workqueue->stk = NULL;
}
AOS_EXPORT(void, aos_workqueue_del, aos_workqueue_t *);

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
AOS_EXPORT(int, aos_work_init, aos_work_t *, void (*)(void *), void *, int);

void aos_work_destroy(aos_work_t *work)
{
    kwork_t *w;

    if (work == NULL) {
        return;
    }

    w = work->hdl;

    if (w->timer != NULL) {
        krhino_timer_stop(w->timer);
        krhino_timer_dyn_del(w->timer);
    }

    aos_free(work->hdl);
    work->hdl = NULL;
}
AOS_EXPORT(void, aos_work_destroy, aos_work_t *);

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
AOS_EXPORT(int, aos_work_run, aos_workqueue_t *, aos_work_t *);

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
AOS_EXPORT(int, aos_work_sched, aos_work_t *);

int aos_work_cancel(aos_work_t *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_cancel(work->hdl);
    if (ret != RHINO_SUCCESS) {
        return -EBUSY;
    }

    return 0;
}
AOS_EXPORT(int, aos_work_cancel, aos_work_t *);
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
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
#if defined (__CC_ARM)
        krhino_owner_attach(g_kmm_head, tmp, __return_address());
#elif defined (__GNUC__)
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif /* __CC_ARM */
#endif
    } else {
        tmp = krhino_mm_alloc(size);
    }

#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {		
        memset(tmp, 0, size);
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
#if defined (__CC_ARM)
        krhino_owner_attach(g_kmm_head, tmp, __return_address());
#elif defined (__GNUC__)
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif /* __CC_ARM */
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
#if defined (__CC_ARM)
        krhino_owner_attach(g_kmm_head, tmp, __return_address());
#elif defined (__GNUC__)
        krhino_owner_attach(g_kmm_head, tmp, (size_t)__builtin_return_address(0));
#endif /* __CC_ARM */
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
AOS_EXPORT(void, aos_alloc_trace, void *, size_t);

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}
AOS_EXPORT(void, aos_free, void *);
#endif

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

void aos_msleep(int ms)
{
    krhino_task_sleep(MS2TICK(ms));
}
AOS_EXPORT(void, aos_msleep, int);

void aos_init(void)
{
    krhino_init();
}

void aos_start(void)
{
    krhino_start();
}

