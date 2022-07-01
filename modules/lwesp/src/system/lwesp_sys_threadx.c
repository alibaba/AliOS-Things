/**
 * \file            lwesp_sys_threadx.c
 * \brief           System dependant functions
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the follwing conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Author:          Neo Xiong <xiongyu0523@gmail.com>
 * Version:         v1.1.2-dev
 */
#include "system/lwesp_sys.h"
#include "lwesp/lwesp_mem.h"
#include "tx_api.h"

/* Custom memory ThreadX implementation must be done to use this feature */
#if LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION && !LWESP_CFG_MEM_CUSTOM
#error "LWESP_CFG_MEM_CUSTOM must be enabled to use LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION feature"
#endif

#if !__DOXYGEN__

/* Main LwESP byte pool handle */
TX_BYTE_POOL*       lwesp_threadx_byte_pool;

/* If user will not provide its own byte pool from app, create one here */
#if !LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL

/* ThreadX memory pool definition */
#if !defined(LWESP_MEM_SIZE)
#define LWESP_MEM_SIZE                    0x2000
#endif
static UCHAR        byte_pool_mem[LWESP_MEM_SIZE];
static TX_BYTE_POOL byte_pool;

#else

/**
 * \brief           Set byte pool handle for memory allocation
 * Byte pools are usually externally created by users
 *
 * This function shall be called before \ref lwesp_init
 *
 * \param[in]       bp: Handle to external byte pool
 */
void
lwesp_sys_preinit_threadx_set_bytepool_handle(TX_BYTE_POOL* bp) {
    lwesp_threadx_byte_pool = bp;
}

#endif

/* Module private variables */
static TX_MUTEX sys_mutex;

/* Macros to convert from milliseconds to ticks and opposite */
#define TICKS_TO_MS(ticks)              ((ticks) * (1000 / TX_TIMER_TICKS_PER_SECOND))
#define MS_TO_TICKS(ms)                 ((ms) * TX_TIMER_TICKS_PER_SECOND / 1000)

uint8_t
lwesp_sys_init(void) {
    UINT status = TX_SUCCESS;

#if !LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL
    status = tx_byte_pool_create(&byte_pool, "lwesp_byte_pool", byte_pool_mem, LWESP_MEM_SIZE);
    if (status == TX_SUCCESS) {
        status = lwesp_sys_mutex_create(&sys_mutex) ? TX_SUCCESS : TX_NO_MEMORY;
    }
    lwesp_threadx_byte_pool = &byte_pool;
#else /* LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL */
    lwesp_sys_mutex_create(&sys_mutex);
#endif /* !LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL */
    return status == TX_SUCCESS ? 1 : 0;
}

uint32_t
lwesp_sys_now(void) {
    return TICKS_TO_MS(tx_time_get());
}

uint8_t
lwesp_sys_protect(void) {
    return lwesp_sys_mutex_lock(&sys_mutex);
}

uint8_t
lwesp_sys_unprotect(void) {
    return lwesp_sys_mutex_unlock(&sys_mutex);
}

uint8_t
lwesp_sys_mutex_create(lwesp_sys_mutex_t* p) {
    return tx_mutex_create(p, TX_NULL, TX_INHERIT) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mutex_delete(lwesp_sys_mutex_t* p) {
    return tx_mutex_delete(p) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mutex_lock(lwesp_sys_mutex_t* p) {
    return tx_mutex_get(p, TX_WAIT_FOREVER) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mutex_unlock(lwesp_sys_mutex_t* p) {
    return tx_mutex_put(p) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mutex_isvalid(lwesp_sys_mutex_t* p) {
    return p->tx_mutex_id != TX_CLEAR_ID ? 1 : 0;
}

uint8_t
lwesp_sys_mutex_invalid(lwesp_sys_mutex_t* p) {
    /* No need actions since all invalid are following delete, and delete make sure it is invalid */
    return 1;
}

uint8_t
lwesp_sys_sem_create(lwesp_sys_sem_t* p, uint8_t cnt) {
    return tx_semaphore_create(p, "lwesp_sem", cnt) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_sem_delete(lwesp_sys_sem_t* p) {
    return tx_semaphore_delete(p) == TX_SUCCESS ? 1 : 0;
}

uint32_t
lwesp_sys_sem_wait(lwesp_sys_sem_t* p, uint32_t timeout) {
    ULONG start = tx_time_get();
    return tx_semaphore_get(p, !timeout ? TX_WAIT_FOREVER : MS_TO_TICKS(timeout)) == TX_SUCCESS ? TICKS_TO_MS(tx_time_get() - start) : LWESP_SYS_TIMEOUT;
}

uint8_t
lwesp_sys_sem_release(lwesp_sys_sem_t* p) {
    return tx_semaphore_put(p) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_sem_isvalid(lwesp_sys_sem_t* p) {
    return p->tx_semaphore_id != TX_CLEAR_ID ? 1 : 0;
}

uint8_t
lwesp_sys_sem_invalid(lwesp_sys_sem_t* p) {
    /* No need actions since all invalid are following delete, and delete make sure it is invalid */
    return 1;
}

uint8_t
lwesp_sys_mbox_create(lwesp_sys_mbox_t* b, size_t size) {
    uint8_t rt = 0;
    ULONG queue_total_size = size * sizeof(void*);
    void* queue_mem = lwesp_mem_malloc(queue_total_size);
    if (queue_mem != NULL) {
        if (tx_queue_create(b, TX_NULL, sizeof(void*) / sizeof(ULONG), queue_mem, queue_total_size) == TX_SUCCESS) {
            rt = 1;
        } else {
            lwesp_mem_free(queue_mem);
        }
    }
    return rt;
}

uint8_t
lwesp_sys_mbox_delete(lwesp_sys_mbox_t* b) {
    (VOID)tx_queue_delete(b);
    lwesp_mem_free(b->tx_queue_start);
    return 1;
}

uint32_t
lwesp_sys_mbox_put(lwesp_sys_mbox_t* b, void* m) {
    ULONG start = tx_time_get();
    (VOID)tx_queue_send(b, &m, TX_WAIT_FOREVER);
    return tx_time_get() - start;
}

uint32_t
lwesp_sys_mbox_get(lwesp_sys_mbox_t* b, void** m, uint32_t timeout) {
    ULONG start = tx_time_get();
    return tx_queue_receive(b, m, !timeout ? TX_WAIT_FOREVER : MS_TO_TICKS(timeout)) == TX_SUCCESS ? TICKS_TO_MS(tx_time_get() - start) : LWESP_SYS_TIMEOUT;
}

uint8_t
lwesp_sys_mbox_putnow(lwesp_sys_mbox_t* b, void* m) {
    return tx_queue_send(b, &m, TX_NO_WAIT) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mbox_getnow(lwesp_sys_mbox_t* b, void** m) {
    return tx_queue_receive(b, m, TX_NO_WAIT) == TX_SUCCESS ? 1 : 0;
}

uint8_t
lwesp_sys_mbox_isvalid(lwesp_sys_mbox_t* b) {
    return b->tx_queue_id != TX_CLEAR_ID ? 1 : 0;
}

uint8_t
lwesp_sys_mbox_invalid(lwesp_sys_mbox_t* b) {
    /* No need actions since all invalid are following delete, and delete make sure it is invalid */
    return 1;
}

#if LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION

uint8_t
lwesp_sys_thread_create(lwesp_sys_thread_t* t, const char* name, lwesp_sys_thread_fn thread_func, void* const arg, size_t stack_size, lwesp_sys_thread_prio_t prio) {
    void* stack_ptr = NULL;
    lwesp_sys_thread_t* t_handle;
    uint8_t t_handle_dynamic = 0;

    /* First process thread object */
    if (t != NULL) {
        t_handle = t;                           /* Use static handle from parameter */
    } else if (tx_byte_allocate(lwesp_threadx_byte_pool, (void*)&t_handle, sizeof(*t_handle), TX_NO_WAIT) == TX_SUCCESS) {
        t_handle_dynamic = 1;                   /* Handle has been dynamically allocated */
    } else {
        goto cleanup;
    }

    /* Allocate memory for stack */
    if (tx_byte_allocate(lwesp_threadx_byte_pool, &stack_ptr, stack_size, TX_NO_WAIT) != TX_SUCCESS) {
        goto cleanup;
    }

    /* Allocate thread stack */
    if (tx_thread_create(t_handle, (CHAR*)name, (VOID (*)(ULONG))(thread_func), (ULONG)arg,
                         stack_ptr, stack_size, prio, 0, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        goto cleanup;
    }

    /*
     * ThreadX does not allow self-termination feature,
     * hence dynamic thread creation or termination isn't supported.
     *
     * To overcome this "problem", application can create an idle thread,
     * with lowest priority, being involved only when no other threads are being used.
     *
     * Idle thread is then in charge to check every thread in terminated or completed state
     * and can cleanup dynamically allocated memory
     *
     * User must add below code to the "tx_user.h" file
     *
     * #define TX_THREAD_USER_EXTENSION         UINT tx_thread_user_is_handle_alloc_dynamic; \
     *                                          UINT tx_thread_user_is_stack_alloc_dynamic;
     *
     * And later for sure create idle thread in charge of ThreadX cleanup process
     */
    t_handle->tx_thread_user_is_handle_alloc_dynamic = t_handle_dynamic;
    t_handle->tx_thread_user_is_stack_alloc_dynamic = 1;
    return 1;

cleanup:
    if (t_handle_dynamic && t_handle != NULL) {
        tx_byte_release(t_handle);
    }
    if (stack_ptr != NULL) {
        tx_byte_release(stack_ptr);
    }
    return 0;
}

uint8_t
lwesp_sys_thread_terminate(lwesp_sys_thread_t* t) {
    uint8_t rt = 0;

    /*
     * See thread creation feature to understand this properly
     *
     * We only need to terminate thread and let idle thread to do the things for us
     * with memory cleanups. ThreadX extension at user side must be provided
     */
    rt = tx_thread_terminate(t != NULL ? t : tx_thread_identify()) == TX_SUCCESS;
    return rt;
}

#else /* LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION */

uint8_t
lwesp_sys_thread_create(lwesp_sys_thread_t* t, const char* name, lwesp_sys_thread_fn thread_func, void* const arg, size_t stack_size, lwesp_sys_thread_prio_t prio) {

    typedef VOID (*threadx_entry_t)(ULONG);
    uint8_t rt = 0;

    void* stack_mem = lwesp_mem_malloc(stack_size);
    if (stack_mem != NULL) {
        if (tx_thread_create(t, (CHAR*)name, (VOID (*)(ULONG))(thread_func), (ULONG)arg, stack_mem, stack_size, prio, prio, TX_NO_TIME_SLICE, TX_AUTO_START) == TX_SUCCESS) {
            rt = 1;
        } else {
            lwesp_mem_free(stack_mem);
        }
    }
    return rt;
}

uint8_t
lwesp_sys_thread_terminate(lwesp_sys_thread_t* t) {
    uint8_t rt = 0;

    /*  t == NULL means temrinate itself.
        Here termination of a thread requires deleting thread (free RCB) and releasing stack memory
        ThreadX does not support deleting itself, so I left this feature not supported (when t == NULL) */

    if ((t != NULL) && (t != tx_thread_identify())) {
        if (tx_thread_terminate(t) == TX_SUCCESS) {
            if (tx_thread_delete(t) == TX_SUCCESS) {
                lwesp_mem_free(t->tx_thread_stack_start);
                rt = 1;
            }
        }
    }
    return rt;
}

#endif /* !LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION */

uint8_t
lwesp_sys_thread_yield(void) {
    tx_thread_relinquish();
    return 0;
}

#endif /* !__DOXYGEN__ */
