/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
 * Copyright (c) 2017 Pycom Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "stdio.h"

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "mpthreadport.h"
#include "mpsalport.h"

#include "aos/kernel.h"
#include "mphalport.h"

#if MICROPY_PY_THREAD

#define LOG_TAG "MP_THREAD"

#define MP_THREAD_MIN_STACK_SIZE            MP_SAL_THREAD_MIN_STACK_SIZE
#define MP_THREAD_DEFAULT_STACK_SIZE        MP_SAL_THREAD_DEFAULT_STACK_SIZE
#define MP_THREAD_PRIORITY                  MP_SAL_THREAD_PRIORITY

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    mp_sal_taskhandler_obj_t handler;  // task handler of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    void *stack_addr;       // pointer to the stack
    size_t stack_len;       // number of bytes in the stack
    struct _thread_t *next;
    mp_state_thread_t *state; //state
} thread_t;

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;
STATIC thread_t thread_entry0;
STATIC thread_t *thread = NULL; // root pointer, handled by mp_thread_gc_others

void mp_thread_init(void *stack_addr, uint32_t stack_len) {
    mp_thread_mutex_init(&thread_mutex);
    // create the first entry in the linked list of all threads
    thread = &thread_entry0;
    thread->handler = mp_sal_get_taskhandler();
    thread->ready = 1;
    thread->arg = NULL;
    thread->stack_addr = stack_addr;
    thread->stack_len = stack_len;
    thread->next = NULL;
    thread->state = NULL;
    mp_thread_set_state(&mp_state_ctx.thread);
}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1);
        if (th->handler == mp_sal_get_taskhandler()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        gc_collect_root(th->stack_addr, th->stack_len);
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    mp_state_thread_t *state = NULL;

    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->handler == mp_sal_get_taskhandler()) {
            state = th->state;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return state;
}

void mp_thread_set_state(mp_state_thread_t *state) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->handler == mp_sal_get_taskhandler()) {
            th->state = state;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->handler == mp_sal_get_taskhandler()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create_ex(void (*entry)(void*), void *arg, size_t *stack_size, int priority, char *name) {

    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);
    if (th == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "thread_t alloc fail"));
    }

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    void *stack_addr = NULL;
    int32_t result = mp_sal_task_create(th->handler, entry, arg, stack_addr, *stack_size, priority, name);
    if (result != 0) {
        mp_thread_mutex_unlock(&thread_mutex);
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread"));
    }

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;

    // add thread to linked list of all threads
    th->ready = 0;
    th->arg = arg;
    th->stack_addr = stack_addr;
    th->stack_len = (*stack_size) / sizeof(cpu_stack_t);
    th->next = thread;
    thread = th;

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void*), void *arg, size_t *stack_size) {
    mp_thread_create_ex(entry, arg, stack_size, MP_THREAD_PRIORITY, "mp_thread");
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->handler == mp_sal_get_taskhandler()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    mp_sal_mutex_create(&(mutex->k_mutex));
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    if((mutex->k_mutex) == NULL)
    {
        LOGE(LOG_TAG, "mpthread mutex lock error!!");
        return 0;
    }
    return mp_sal_mutex_lock(&(mutex->k_mutex), wait ? AOS_WAIT_FOREVER : 0);
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    if((mutex->k_mutex) == NULL)
    {
        LOGE(LOG_TAG, "mpthread mutex unlock error!!");
        return 0;
    }
    mp_sal_mutex_unlock(&(mutex->k_mutex));
}

void mp_thread_deinit(void) {
    for (;;) {
        // Find a task to delete
        mp_sal_taskhandler_obj_t handler = NULL;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (thread_t *th = thread; th != NULL; th = th->next) {
            // Don't delete the current task
            if (th->handler != krhino_cur_task_get()) {
                handler = th->handler;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);

        if (handler == NULL) {
            // No tasks left to delete
            break;
        } else {
            int32_t status = -1;
            mp_sal_task_delete(handler, &status);
            if(status != 0) {
                LOGE(LOG_TAG, "Failed to delete task[id = 0x%X]");
            }
        }
    }
}

#else

#endif // MICROPY_PY_THREAD
