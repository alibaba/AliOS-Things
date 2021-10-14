#ifndef MICROPY_INCLUDED_SAL_H
#define MICROPY_INCLUDED_SAL_H

#include <stdio.h>

#include "aos/kernel.h"
#include "py/obj.h"
#include "ulog/ulog.h"

#define MP_SAL_THREAD_MIN_STACK_SIZE     (1024 + 256)
#define MP_SAL_THREAD_DEFAULT_STACK_SIZE (MP_THREAD_MIN_STACK_SIZE + 1024)
#define MP_SAL_THREAD_PRIORITY           (AOS_DEFAULT_APP_PRI)

#define mp_sal_mutex_obj_t               aos_mutex_t
#define mp_sal_sem_obj_t                 aos_sem_t

mp_uint_t mp_sal_get_stack_size();
void *mp_sal_get_stack_addr();

void mp_sal_task_delete(aos_task_t *id, mp_int_t *status);
mp_int_t mp_sal_mutex_create(mp_sal_mutex_obj_t *mutex);
mp_int_t mp_sal_mutex_lock(mp_sal_mutex_obj_t *mutex, mp_uint_t timeout);
mp_int_t mp_sal_mutex_unlock(mp_sal_mutex_obj_t *mutex);
void mp_sal_mutex_delete(mp_sal_mutex_obj_t *mutex);

mp_int_t mp_sal_sem_create(mp_sal_sem_obj_t *sem);
mp_int_t mp_sal_sem_take(mp_sal_sem_obj_t *sem, mp_uint_t timeout);
mp_int_t mp_sal_sem_give(mp_sal_sem_obj_t *sem);
void mp_sal_sem_delete(mp_sal_sem_obj_t *sem);

#endif
