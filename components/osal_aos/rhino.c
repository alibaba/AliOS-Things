/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <time.h>

extern const char* sysinfo_kernel_version;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static unsigned int used_bitmap;
static long long start_time_ms = 0;

static aos_status_t rhino2stderrno(int ret)
{
    switch (ret) {
    case RHINO_SUCCESS:
    case RHINO_MUTEX_OWNER_NESTED:
    case RHINO_STOPPED:
        return 0;
    case RHINO_SYS_SP_ERR:
    case RHINO_NULL_PTR:
    case RHINO_MM_FREE_ADDR_ERR:
        return -EFAULT;

    case RHINO_INV_PARAM:
    case RHINO_INV_ALIGN:
    case RHINO_KOBJ_TYPE_ERR:
    case RHINO_MM_POOL_SIZE_ERR:
    case RHINO_MM_ALLOC_SIZE_ERR:
    case RHINO_INV_SCHED_WAY:
    case RHINO_TASK_INV_STACK_SIZE:
    case RHINO_BEYOND_MAX_PRI:
    case RHINO_BUF_QUEUE_INV_SIZE:
    case RHINO_BUF_QUEUE_SIZE_ZERO:
    case RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW:
    case RHINO_QUEUE_FULL:
    case RHINO_QUEUE_NOT_FULL:
    case RHINO_SEM_OVF:
    case RHINO_WORKQUEUE_EXIST:
    case RHINO_WORKQUEUE_NOT_EXIST:
    case RHINO_WORKQUEUE_WORK_EXIST:
        return -EINVAL;

    case RHINO_BLK_TIMEOUT:
        return -ETIMEDOUT ;
    case RHINO_KOBJ_BLK:
        return -EAGAIN;

    case RHINO_NO_MEM:
        return -ENOMEM;

    case RHINO_KOBJ_DEL_ERR:
    case RHINO_SCHED_DISABLE:
    case RHINO_SCHED_ALREADY_ENABLED:
    case RHINO_SCHED_LOCK_COUNT_OVF:
    case RHINO_TASK_NOT_SUSPENDED:
    case RHINO_TASK_DEL_NOT_ALLOWED:
    case RHINO_TASK_SUSPEND_NOT_ALLOWED:
    case RHINO_SUSPENDED_COUNT_OVF:
    case RHINO_PRI_CHG_NOT_ALLOWED:
    case RHINO_NOT_CALLED_BY_INTRPT:
    case RHINO_NO_THIS_EVENT_OPT:
    case RHINO_TIMER_STATE_INV:
    case RHINO_BUF_QUEUE_FULL:
    case RHINO_SEM_TASK_WAITING:
    case RHINO_MUTEX_NOT_RELEASED_BY_OWNER:
    case RHINO_WORKQUEUE_WORK_RUNNING:
        return -EPERM;

    case RHINO_TRY_AGAIN:
    case RHINO_WORKQUEUE_BUSY:
        return -EAGAIN;

    default:
        return -1;
    }

    return -1;
}

#define CHECK_HANDLE(hdl)  do{                                               \
                                if(hdl == NULL || (void *)(*hdl) == NULL) {  \
                                    return -EINVAL;                          \
                                }                                            \
                            } while(0)

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

aos_status_t aos_task_new(const char *name, void (*fn)(void *), void *arg, size_t stack_size)
{
    int ret;

    ktask_t *task_handle = NULL;

#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                          stack_size / sizeof(cpu_stack_t), fn, 1u);
#else
    ret = (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                      0, stack_size / sizeof(cpu_stack_t), fn, 1u);
#endif

    return rhino2stderrno(ret);
}

aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int32_t prio)
{
    int ret;
    if(task == NULL) {
        return -EINVAL;
    }
#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_dyn_create((ktask_t **)task, name, arg, AOS_DEFAULT_APP_PRI,
                                          stack_size / sizeof(cpu_stack_t), fn, 1u);
#else
    ret = (int)krhino_task_dyn_create((ktask_t **)task, name, arg, prio,
                                      0, stack_size / sizeof(cpu_stack_t), fn, 1u);
#endif
    return rhino2stderrno(ret);
}

aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, void *stack, size_t stack_size, int32_t prio, uint32_t options)
{
    int       ret;
    cpu_stack_t  *task_stack = NULL;
    ktask_t      *task_obj;

    if(task == NULL) {
        return -EINVAL;
    }

    if (stack == NULL) {
        task_stack = aos_malloc(stack_size * sizeof(cpu_stack_t));
        if (task_stack == NULL) {
            return -ENOMEM;
        }
        stack = task_stack;
    }

    task_obj = aos_malloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        aos_free(task_stack);
        return -ENOMEM;
    }

    *task = task_obj;
#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_create(task_obj, name, arg, prio, (cpu_stack_t *)stack,
                                      stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#else
    ret = (int)krhino_task_create(task_obj, name, arg, prio, 0, (cpu_stack_t *)stack,
                                  stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#endif
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        aos_free(task_stack);
        aos_free(task_obj);
        *task = NULL;
    }

    return rhino2stderrno(ret);
}

void aos_task_exit(int32_t code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);

    ret = (int)krhino_task_dyn_del(ktask);

    return rhino2stderrno(ret);
}
#endif /* RHINO_CONFIG_KOBJ_DYN_ALLOC */

aos_status_t aos_task_resume(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);
    ret = krhino_task_resume(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);
    ktask = (ktask_t *)(*task);
    ret = krhino_task_suspend(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_yield()
{
    int ret;
    ret = krhino_task_yield();
    return rhino2stderrno(ret);
}

aos_task_t aos_task_self()
{
    return (aos_task_t)krhino_cur_task_get();

}

aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size)
{
    const char *name;
    int str_len, copy_len;

    CHECK_HANDLE(task);

    if (buf == NULL || buf_size == 0) {
        return -EINVAL;
    }

    name = ((ktask_t*)*task)->task_name;

    str_len = strlen(name);

    copy_len = (str_len > (buf_size - 1)) ? buf_size - 1 : str_len;
    memcpy(buf, name, copy_len);
    return 0;

}

aos_status_t aos_task_key_create(aos_task_key_t *key)
{
    int i;
    if(key == NULL) {
        return -EINVAL;
    }
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

#if (RHINO_CONFIG_TASK_INFO > 0)
aos_status_t aos_task_setspecific(aos_task_key_t key, void *vp)
{
    int ret;
    ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
    return rhino2stderrno(ret);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &vp);

    return vp;
}
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
aos_status_t aos_mutex_new(aos_mutex_t *mutex)
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
        return rhino2stderrno(ret);
    }
    *mutex = m;

    return 0;
}

aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options)
{
    kstat_t   ret;
    kmutex_t *m;

    (void)options;
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
        return rhino2stderrno(ret);
    }
    *mutex = m;

    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    if(mutex == NULL || *mutex == NULL) {
        return;
    }
    krhino_mutex_del((kmutex_t *)*mutex);

    aos_free(*mutex);

    *mutex = NULL;
}

aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, MS2TICK(timeout));
    }

    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

aos_status_t aos_mutex_unlock(aos_mutex_t *mutex)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    ret = krhino_mutex_unlock((kmutex_t *)*mutex);
    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    kmutex_t *k_mutex;

    if (mutex == NULL) {
        return false;
    }

    k_mutex = (kmutex_t *)*mutex;

    if (k_mutex == NULL) {
        return false;
    }

    if (k_mutex->blk_obj.obj_type != RHINO_MUTEX_OBJ_TYPE) {
        return false;
    }

    return true;
}
#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC > 0) && (RHINO_CONFIG_SEM > 0))
aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
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
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    kstat_t ret;
    ksem_t *s;

    (void)options;
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
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if(sem == NULL || *sem == NULL) {
        return;
    }
    krhino_sem_del((ksem_t *)*sem);

    aos_free(*sem);

    *sem = NULL;
}

aos_status_t aos_sem_wait(aos_sem_t *sem, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(sem);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_sem_take((ksem_t *)*sem, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_sem_take((ksem_t *)*sem, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL) {
        return;
    }

    krhino_sem_give((ksem_t *)*sem);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL) {
        return;
    }

    krhino_sem_give_all((ksem_t *)*sem);
}

bool aos_sem_is_valid(aos_sem_t *sem)
{
    ksem_t *k_sem;

    if (sem == NULL) {
        return false;
    }

    k_sem = *sem;

    if (k_sem == NULL) {
        return false;
    }

    if (k_sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        return false;
    }

    return true;
}

#endif

#if (RHINO_CONFIG_EVENT_FLAG > 0)

aos_status_t aos_event_new(aos_event_t *event, uint32_t value)
{
    int ret;
    if(event == NULL) {
        return -EINVAL;
    }

    ret = krhino_event_dyn_create((kevent_t **)(event), "AOS", value);

    return rhino2stderrno(ret);
}

aos_status_t aos_event_create(aos_event_t *event, uint32_t value, uint32_t options)
{
    int ret;

    (void)options;
    if(event == NULL) {
        return -EINVAL;
    }
    ret = krhino_event_dyn_create((kevent_t **)(event), "AOS", value);

    return rhino2stderrno(ret);
}

void aos_event_free(aos_event_t *event)
{
    if (event == NULL || *event == NULL) {
        return;
    }

    (void)krhino_event_dyn_del((kevent_t *)*event);

    *event = NULL;
}

aos_status_t aos_event_get(aos_event_t *event, uint32_t value, uint32_t opt,
                           uint32_t *actl_value, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(event);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_event_get((kevent_t *)*event, value, opt, actl_value, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_event_get((kevent_t *)*event, value, opt, actl_value, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

aos_status_t aos_event_set(aos_event_t *event, uint32_t flags, uint32_t opt)
{
    kstat_t ret;

    CHECK_HANDLE(event);

    ret = krhino_event_set((kevent_t *)*event, flags, opt);

    return rhino2stderrno(ret);
}

bool aos_event_is_valid(aos_event_t *event)
{
    kevent_t *k_event;

    if (event == NULL) {
        return false;
    }

    k_event = (kevent_t *)*event;

    if (k_event == NULL) {
        return false;
    }

    if (k_event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        return false;
    }

    return true;
}
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)

aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(kbuf_queue_t) + (buf == NULL? size : 0);
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (buf == NULL) ? ((void *)q + sizeof(kbuf_queue_t)) : buf;

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msg, uint32_t options)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    (void)options;
    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(kbuf_queue_t) + size;
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (void *)q + sizeof(kbuf_queue_t);

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL || *queue == NULL) {
        return;
    }

    krhino_buf_queue_del((kbuf_queue_t *)*queue);

    aos_free(*queue);

    *queue = NULL;
}

aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    int ret;

    CHECK_HANDLE(queue);

    ret = krhino_buf_queue_send((kbuf_queue_t *)*queue, msg, size);
    return rhino2stderrno(ret);
}

aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg,
                   size_t *size)
{
    int ret;

    CHECK_HANDLE(queue);
    if (ms == AOS_WAIT_FOREVER) {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, RHINO_WAIT_FOREVER, msg, size);
    } else {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, MS2TICK(ms), msg, size);
    }
    return rhino2stderrno(ret);
}

bool aos_queue_is_valid(aos_queue_t *queue)
{
    kbuf_queue_t *k_queue;

    if (queue == NULL) {
        return false;
    }

    k_queue = (kbuf_queue_t *)*queue;

    if (k_queue == NULL) {
        return false;
    }

    if (k_queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        return false;
    }

    return true;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue)) {
        return NULL;
    }

    return ((kbuf_queue_t *)*queue)->buf;
}

aos_status_t aos_queue_get_count(aos_queue_t *queue)
{
    kbuf_queue_info_t info;
    CHECK_HANDLE(queue);
    kstat_t ret = krhino_buf_queue_info_get((kbuf_queue_t *)*queue, &info);

    return ret == RHINO_SUCCESS ? info.cur_num : -EPERM;
}
#endif

#if (RHINO_CONFIG_TIMER > 0)
aos_status_t aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, bool repeat)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }
	
	sys_time_t round = (repeat == false ? 0 : ms);

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, 1);


    return rhino2stderrno(ret);
}

aos_status_t aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, bool repeat, bool auto_run)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

	sys_time_t round = repeat == 0 ? 0 : ms;

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, auto_run? 1 : 0);

    return rhino2stderrno(ret);
}

aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint32_t options)
{
    kstat_t ret;
    sys_time_t round;
    if (timer == NULL) {
        return -EINVAL;
    }

    round = (options & AOS_TIMER_REPEAT) ? ms : 0;

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, (options & AOS_TIMER_AUTORUN) ? 1 : 0);

    return rhino2stderrno(ret);
}

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL || *timer == NULL) {
        return;
    }

    krhino_timer_dyn_del((ktimer_t *)*timer);
    *timer = NULL;
}

aos_status_t aos_timer_start(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_start((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

aos_status_t aos_timer_stop(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_stop((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

aos_status_t aos_timer_change(aos_timer_t *timer, uint32_t ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), MS2TICK(ms));
    return rhino2stderrno(ret);
}
aos_status_t aos_timer_change_once(aos_timer_t *timer, uint32_t ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), 0);

    return rhino2stderrno(ret);
}

int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    ktimer_t *ktimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;

    CHECK_HANDLE(timer);
    ktimer = (ktimer_t *)*timer;
    init_ms = (uint64_t)krhino_ticks_to_ms(ktimer->init_count);
    round_ms = (uint64_t)krhino_ticks_to_ms(ktimer->round_ticks);

    value[0] = init_ms / 1000;
    value[1] = (init_ms % 1000) * 1000000UL;
    value[2] = round_ms / 1000;
    value[3] = (round_ms % 1000) * 1000000UL ;

    return 0;
}
#endif



void *aos_zalloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {
        memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void *aos_calloc(size_t nitems, size_t size)
{
    void *tmp = NULL;
    size_t len = (size_t)nitems*size;

    if (len == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(len | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(len);
#endif

    if (tmp) {
        memset(tmp, 0, len);
    }

    return tmp;
}

void *aos_realloc(void *mem, size_t size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
}

void aos_alloc_trace(void *addr, uintptr_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    krhino_owner_attach(addr, allocator);
#endif
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - krhino_sys_time_get();
}

uint64_t aos_calendar_time_get(void)
{
    return krhino_sys_time_get() + start_time_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    if ((int64_t)(aos_calendar_time_get() - 8 * 3600 * 1000) < 0) {
        return aos_calendar_time_get();
    }
    return aos_calendar_time_get() + 8 * 3600 * 1000;
}

uint64_t aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

uint64_t aos_now_ms(void)
{
    return krhino_sys_time_get();
}


aos_status_t aos_now_time_str(char *buffer, size_t len)
{

    uint64_t ms = aos_now_ms();

    if (buffer == NULL || len == 0) {
        return -EINVAL;
    } 

    time_t actualTime = { ms / 1000 };
    memset(buffer, 0, len);
    strftime(buffer, len, "%m-%d %H:%M:%S", gmtime(&actualTime));
    if ((int)(len - strlen(buffer) - 1) > 0) {
        const int milli    = ms % 1000;
        char      msStr[8] = {0};
        snprintf(msStr, sizeof(msStr), ".%03d", milli);
        strncat(buffer, msStr, len - strlen(buffer) - 1);
    } else {
        return -EINVAL;
    }

    return 0;
}

void aos_msleep(uint32_t ms)
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

uint32_t aos_sched_get_priority_max(uint32_t policy)
{
    return RHINO_CONFIG_PRI_MAX;
}

aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (ptcb == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *(ptcb) = ((ktask_t *)*task)->ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);

    RHINO_CRITICAL_ENTER();
    ((ktask_t *)*task)->ptcb = ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    ret = krhino_task_pri_change((ktask_t *)*task, pri, old_pri);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (priority == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *priority = ((ktask_t *)*task)->b_prio;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    if (policy != KSCHED_FIFO && policy != KSCHED_RR && policy != KSCHED_CFS) {
        return -EINVAL;
    }

    ret = krhino_sched_param_set((ktask_t *)*task, policy, pri);
    return rhino2stderrno(ret);
}

aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy)
{
    kstat_t ret = 0;

    CHECK_HANDLE(task);
    if (policy == NULL) {
        return -EINVAL;
    }

    ret = krhino_sched_policy_get((ktask_t *)*task, policy);
    return rhino2stderrno(ret);
}

uint32_t aos_task_sched_policy_get_default()
{
#if (RHINO_CONFIG_SCHED_CFS > 0)
    return KSCHED_CFS;
#else
    return KSCHED_RR;
#endif
}

aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice)
{
    kstat_t ret;
    CHECK_HANDLE(task);

    ret = krhino_task_time_slice_set((ktask_t *)*task, MS2TICK(slice));
    return rhino2stderrno(ret);
}

aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice)
{
    uint32_t time_slice;
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (slice == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    time_slice = ((ktask_t *)*task)->time_slice;
    RHINO_CRITICAL_EXIT();
    *slice = (uint32_t)krhino_ticks_to_ms(time_slice);

    return 0;
}

void aos_spin_lock_init(aos_spinlock_t *spinlock)
{
    krhino_spin_lock_init((kspinlock_t *)spinlock);
}

void aos_spin_lock(aos_spinlock_t *spinlock)
{
    krhino_spin_lock((kspinlock_t *)spinlock);
}

void aos_spin_unlock(aos_spinlock_t *spinlock)
{
    krhino_spin_unlock((kspinlock_t *)spinlock);
}

aos_irqsave_t aos_spin_lock_irqsave(aos_spinlock_t *spinlock)
{
    aos_irqsave_t flag;
    krhino_spin_lock_irq_save((kspinlock_t *)spinlock, flag);
    return flag;
}

void aos_spin_unlock_irqrestore(aos_spinlock_t *spinlock, aos_irqsave_t flag)
{
    krhino_spin_unlock_irq_restore((kspinlock_t *)spinlock, flag);
}
