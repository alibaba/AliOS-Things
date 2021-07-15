#include <aos/kernel.h>
#include "mpsalport.h"
#include "repl.h"
#include "amp_platform.h"

#if MICROPY_PY_THREAD

#define LOG_TAG "MP_SALPORT"

uint32_t mp_sal_get_stack_size() {
    return 1024*32;
}

void* mp_sal_get_stack_addr() {
    return NULL;
}

mp_sal_taskhandler_obj_t mp_sal_get_taskhandler() {
    mp_sal_taskhandler_obj_t	task_ref;
    ql_rtos_task_get_current_ref(&task_ref);
	return task_ref;
}

// int32_t mp_sal_thread_set_state(void *state) {
//     int status = krhino_task_info_set(krhino_cur_task_get(), 1, state);
//     return status;
// }

// int32_t mp_sal_thread_get_state(void **state) {
//     int status = krhino_task_info_get(krhino_cur_task_get(), 1, state);
//     return status;
// }

int32_t mp_sal_task_create(mp_sal_taskhandler_obj_t id,
                            void (*entry)(void*),
                            void *arg,
                            void *stack_buf,
                            size_t *stack_size_byte,
                            int priority,
                            char *name) {
    QlOSStatus status;
    status = ql_rtos_task_create(&id, stack_size_byte, 100, name, entry, arg);
    return status;
}

void mp_sal_task_delete(mp_sal_taskhandler_obj_t id, int32_t *status) {
    if(id == NULL)
    {
        LOGE(LOG_TAG, "mpthread task delete error!!");
        return;
    }
    *status = ql_rtos_task_delete(&id);
}

int32_t mp_sal_mutex_create(mp_sal_mutex_obj_t *mutex) {
    int status = aos_mutex_new(mutex);
    return status;
}

int32_t mp_sal_mutex_lock(mp_sal_mutex_obj_t *mutex, uint32_t timeout) {
    if((timeout == 0) || (mutex == NULL)){
        LOGE(LOG_TAG, "mpthread mutex lock error!!");
        return 0;
    }
    int status = aos_mutex_lock(mutex, timeout);
    return status;
}

int32_t mp_sal_mutex_unlock(mp_sal_mutex_obj_t *mutex) {
    if(mutex == NULL)
    {
        LOGE(LOG_TAG, "mpthread mutex unlock error!!");
        return 0;
    }
    int status = aos_mutex_unlock(mutex);
    return status;
}

void mp_sal_mutex_delete(mp_sal_mutex_obj_t *mutex) {
    if(mutex == NULL)
    {
        LOGE(LOG_TAG, "mpthread mutex delete error!!");
        return 0;
    }
    aos_mutex_free(mutex);
}
#else

#endif