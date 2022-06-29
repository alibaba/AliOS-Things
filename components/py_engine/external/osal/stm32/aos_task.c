#include "aos_kernel.h"
#include "aos_errno.h"
#include "py/mpthread.h"

typedef struct tmr_adapter {
    void (*func)(void *);
    void *func_arg;
} mpy_task_adapter_t;

static void* mpy_thread_fn(void *args)
{
    mpy_task_adapter_t *task_adapter = (mpy_task_adapter_t *)(args);
    task_adapter->func(task_adapter->func_arg);
    aos_free(task_adapter);
    return NULL;
}

aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                              void *arg, size_t stack_size, int32_t prio)
{
    mpy_task_adapter_t *task_adapter = aos_malloc(sizeof(mpy_task_adapter_t));

    if (task_adapter == NULL) {
        return -1;
    }

    task_adapter->func = fn;
    task_adapter->func_arg = arg;

    mp_thread_create(mpy_thread_fn, task_adapter, &stack_size);
    return 0;
}

void aos_task_exit(int32_t code)
{
    mp_thread_finish();
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    return 0;
}

aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size)
{
    return 0;
}


aos_status_t aos_task_resume(aos_task_t *task)
{
    return 0;
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    return 0;
}

aos_status_t aos_task_yield()
{
    return 0;
}


aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options)
{
    mp_thread_mutex_t *thread_mutex = m_new_obj(mp_thread_mutex_t);
    mp_thread_mutex_init(thread_mutex);
    mutex = (aos_mutex_t *)thread_mutex;
    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    mp_thread_mutex_unlock((mp_thread_mutex_t *)mutex);
    m_del_obj(mp_thread_mutex_t, mutex);
    return;
}

aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout)
{
    mp_thread_mutex_lock((mp_thread_mutex_t *)mutex, 1);
    return 0;
}

aos_status_t aos_mutex_unlock(aos_mutex_t *mutex)
{
    mp_thread_mutex_unlock((mp_thread_mutex_t *)mutex);
    return 0;
}

bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return true;
}