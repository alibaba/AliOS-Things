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

#include "mpthreadport.h"

#include "aos/kernel.h"
#include "mphalport.h"
#include "mpsalport.h"
#include "py/gc.h"
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mpstate.h"
#include "py/mpthread.h"
#include "stdio.h"

#if MICROPY_PY_THREAD

#define LOG_TAG                      "MP_THREAD"

#define MP_THREAD_MIN_STACK_SIZE     MP_SAL_THREAD_MIN_STACK_SIZE
#define MP_THREAD_DEFAULT_STACK_SIZE MP_SAL_THREAD_DEFAULT_STACK_SIZE
#define MP_THREAD_PRIORITY           MP_SAL_THREAD_PRIORITY

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    mp_task_t id;      // task id of thread
    int ready;         // whether the thread is ready and running
    void *arg;         // thread Python args, a GC root pointer
    void *stack_addr;  // pointer to the stack
    size_t stack_len;  // number of bytes in the stack
    struct _thread_t *next;
} thread_t;

STATIC aos_task_key_t tls_key;

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;
STATIC thread_t thread_entry0;
STATIC thread_t *thread = NULL;  // root pointer, handled by mp_thread_gc_others

void mp_thread_init(void *stack_addr, mp_uint_t stack_len)
{
    aos_task_key_create(&tls_key);
    aos_task_setspecific(tls_key, &mp_state_ctx.thread);

    mp_thread_set_state(&mp_state_ctx.thread);
    // create the first entry in the linked list of all threads
    thread_entry0.id = aos_task_self();
    thread_entry0.ready = 1;
    thread_entry0.arg = NULL;
    thread_entry0.stack_addr = stack_addr;
    thread_entry0.stack_len = stack_len;
    thread_entry0.next = NULL;
    mp_thread_mutex_init(&thread_mutex);

    // vPortCleanUpTCB needs the thread ready after thread_mutex is ready
    thread = &thread_entry0;
}

void mp_thread_gc_others(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1);
        if (th->id == aos_task_self()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        gc_collect_root(th->stack_addr, th->stack_len);
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void)
{
    mp_state_thread_t *state = (mp_state_thread_t *)aos_task_getspecific(tls_key);
    if (state == NULL) {
        state = &mp_state_ctx.thread;
    }
    return state;
}

void mp_thread_set_state(mp_state_thread_t *state)
{
    aos_task_setspecific(tls_key, state);
}

void mp_thread_start(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == aos_task_self()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

STATIC void *(*ext_thread_entry)(void *) = NULL;

STATIC void aos_entry(void *arg)
{
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }

    aos_task_exit(0);
}

void mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name)
{
    // store thread entry function into a global variable so we can access it
    ext_thread_entry = entry;

    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE;  // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE;  // minimum stack size
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);
    if (th == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "thread_t alloc fail"));
    }

    mp_thread_mutex_lock(&thread_mutex, 1);

    aos_task_t engine_task;
    mp_int_t result = aos_task_new_ext(&th->id, name, aos_entry, arg, *stack_size, priority);
    if (result != 0) {
        mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
    }

    // add thread to linked list of all threads
    th->ready = 0;
    th->arg = arg;
    th->stack_addr = mp_sal_get_stack_addr();
    th->stack_len = (*stack_size) / sizeof(cpu_stack_t);
    th->next = thread;
    thread = th;

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size)
{
    mp_thread_create_ex(entry, arg, stack_size, MP_THREAD_PRIORITY, "mp_thread");
}

void mp_thread_finish(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == aos_task_self()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex)
{
    aos_mutex_new((aos_mutex_t *)&(mutex->k_mutex));
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait)
{
    if (mutex->k_mutex == NULL) {
        LOGE(LOG_TAG, "mp_thread_mutex_lock with k_mutex NULL!!");
        return MP_EINVAL;
    }

    int status = aos_mutex_lock(&(mutex->k_mutex), wait ? AOS_WAIT_FOREVER : 0);
    return (status == 0);
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex)
{
    if ((mutex->k_mutex) == NULL) {
        LOGE(LOG_TAG, "mpthread mutex unlock error!!");
        return;
    }
    mp_sal_mutex_unlock(&(mutex->k_mutex));
}

void mp_thread_deinit(void)
{
    for (;;) {
        // Find a task to delete
        mp_task_t id = NULL;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (thread_t *th = thread; th != NULL; th = th->next) {
            // Don't delete the current task
            if (th->id != aos_task_self()) {
                id = th->id;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);

        if (id == NULL) {
            // No tasks left to delete
            break;
        } else {
            mp_int_t status = -1;
            mp_sal_task_delete(id, &status);
            if (status != 0) {
                LOGE(LOG_TAG, "Failed to delete task[id = 0x%X]");
            }
            aos_task_key_delete(tls_key);
        }
    }
}

#else

#endif  // MICROPY_PY_THREAD
