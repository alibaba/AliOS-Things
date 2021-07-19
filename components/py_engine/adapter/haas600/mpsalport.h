#ifndef MICROPY_INCLUDED_SAL_H
#define MICROPY_INCLUDED_SAL_H

#include <stdio.h>
#include "py/obj.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"

#include "repl.h"
#include "amp_platform.h"
// #include "ql_rtos.h"

#define MP_SAL_THREAD_MIN_STACK_SIZE      (1024 + 256) 
#define MP_SAL_THREAD_DEFAULT_STACK_SIZE  (MP_THREAD_MIN_STACK_SIZE + 1024)
#define MP_SAL_THREAD_PRIORITY            (AOS_DEFAULT_APP_PRI)

#define mp_sal_taskhandler_obj_t      ql_task_t
#define mp_sal_mutex_obj_t            ql_mutex_t

uint32_t mp_sal_get_stack_size();
void* mp_sal_get_stack_addr();
mp_sal_taskhandler_obj_t mp_sal_get_taskhandler();
// int32_t mp_sal_thread_set_state(void *state);
// int32_t mp_sal_thread_get_state(void **state);
int32_t mp_sal_task_create(mp_sal_taskhandler_obj_t id,
                            void (*entry)(void*),
                            void *arg,
                            void *stack_buf,
                            size_t *stack_size_byte,
                            int priority,
                            char *name);
void mp_sal_task_delete(mp_sal_taskhandler_obj_t id, int32_t *status);
int32_t mp_sal_mutex_create(mp_sal_mutex_obj_t *mutex);
int32_t mp_sal_mutex_lock(mp_sal_mutex_obj_t *mutex, uint32_t timeout);
int32_t mp_sal_mutex_unlock(mp_sal_mutex_obj_t *mutex);
void mp_sal_mutex_delete(mp_sal_mutex_obj_t *mutex);

#endif
