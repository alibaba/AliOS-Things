#include <aos/kernel.h>
#include "mpsalport.h"
#include "k_api.h"
#include "ulog/ulog.h"

#define LOG_TAG "MP-SAL"

uint32_t mp_sal_get_stack_size() {
    ktask_t *task = krhino_cur_task_get();
    return task->stack_size;
}

void* mp_sal_get_stack_addr() {
    ktask_t *task = krhino_cur_task_get();
    return (void*)(task->task_stack_base);
}

void mp_sal_task_delete(aos_task_t *id, int32_t *status) {
    *status = aos_task_delete(id);
}

int32_t mp_sal_mutex_create(mp_sal_mutex_obj_t *mutex) {
#ifndef AOS_BOARD_HAAS700
    int status = aos_mutex_create(mutex, 0);
#else
    int status = aos_mutex_new((aos_mutex_t *)mutex);
#endif
    return status;
}

int32_t mp_sal_mutex_lock(mp_sal_mutex_obj_t *mutex, uint32_t timeout) {

    if(mutex == NULL){
        LOGE(LOG_TAG, "mpthread mutex lock with mutex NULL !!");
        return -1;
    }

    if(timeout == 0) {
        return 0;
    }
    int status = aos_mutex_lock(mutex, timeout);
    return status;
}

int32_t mp_sal_mutex_unlock(mp_sal_mutex_obj_t *mutex) {
    if(mutex == NULL){
        LOGE(LOG_TAG, "mpthread mutex unlock with mutex NULL !!");
        return -1;
    }
    int status = aos_mutex_unlock(mutex);
    return status;
}

void mp_sal_mutex_delete(mp_sal_mutex_obj_t *mutex) {
    aos_mutex_free(mutex);
}
