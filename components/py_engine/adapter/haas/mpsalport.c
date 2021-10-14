#include "mpsalport.h"

#include "aos/kernel.h"
#include "k_api.h"
#include "py/mperrno.h"
#include "ulog/ulog.h"

#define LOG_TAG "mp_sal_port"

mp_uint_t mp_sal_get_stack_size()
{
    ktask_t *task = krhino_cur_task_get();
    return task->stack_size;
}

void *mp_sal_get_stack_addr()
{
    ktask_t *task = krhino_cur_task_get();
    return (void *)(task->task_stack_base);
}

void mp_sal_task_delete(aos_task_t *id, mp_int_t *status)
{
    *status = aos_task_delete(id);
}

mp_int_t mp_sal_mutex_create(mp_sal_mutex_obj_t *mutex)
{
#ifndef AOS_BOARD_HAAS700
    int status = aos_mutex_create(mutex, 0);
#else
    int status = aos_mutex_new((aos_mutex_t *)mutex);
#endif
    return status;
}

mp_int_t mp_sal_mutex_lock(mp_sal_mutex_obj_t *mutex, mp_uint_t timeout)
{
    if (mutex == NULL) {
        LOGE(LOG_TAG, "mpthread mutex lock with mutex NULL !!");
        return -MP_EINVAL;
    }

    if (timeout == 0) {
        return 0;
    }
    int status = aos_mutex_lock(mutex, timeout);
    return status;
}

mp_int_t mp_sal_mutex_unlock(mp_sal_mutex_obj_t *mutex)
{
    if (mutex == NULL) {
        LOGE(LOG_TAG, "mpthread mutex unlock with mutex NULL !!");
        return -MP_EINVAL;
    }
    int status = aos_mutex_unlock(mutex);
    return status;
}

void mp_sal_mutex_delete(mp_sal_mutex_obj_t *mutex)
{
    aos_mutex_free(mutex);
}

/* APIs for semphone */

mp_int_t mp_sal_sem_create(mp_sal_sem_obj_t *sem)
{
    int status = aos_sem_create(sem, 0, 0);
    return status;
}

mp_int_t mp_sal_sem_take(mp_sal_sem_obj_t *sem, mp_uint_t timeout)
{
    if (sem == NULL) {
        LOGE(LOG_TAG, "mpthread sem lock with sem NULL !!");
        return -MP_EINVAL;
    }

    if (timeout == 0) {
        return 0;
    }

    int status = aos_sem_wait(sem, timeout);
    return status;
}

mp_int_t mp_sal_sem_give(mp_sal_sem_obj_t *sem)
{
    if (sem == NULL) {
        LOGE(LOG_TAG, "mpthread sem unlock with sem NULL !!");
        return -MP_EINVAL;
    }
    aos_sem_signal(sem);
    return 0;
}

void mp_sal_sem_delete(mp_sal_sem_obj_t *sem)
{
    aos_sem_free(sem);
}
