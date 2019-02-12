/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _MESH_HAL_OS_H_
#define _MESH_HAL_OS_H_

#include <misc/dlist.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * Attetion: os_port_def.h file is needed as well as 
 * mesh_hal_os.c when porting to new platform.
 * 
 *   - os_port_def.h: provides platform-dependent defines of
 *                    queue_t, sem_t, log primitives, etc.
 *   - mesh_hal_os.c: provide implementation of util APIs.
 */
#include "os_port_def.h"

#define QUEUE_DEF_SIZE 7

struct k_queue
{
    _queue_t    _queue;
    uint8_t     msg_start[QUEUE_DEF_SIZE * (sizeof(void *) + 1)];
};

/*attention: this is intialied as zero,the queue variable shoule use
 * k_queue_init\k_lifo_init\k_fifo_init again*/
#define _K_QUEUE_INITIALIZER(obj) \
    {                             \
        {                         \
            {                     \
                {                 \
                    0             \
                }                 \
            }                     \
        }                         \
    }
#define K_QUEUE_INITIALIZER DEPRECATED_MACRO _K_QUEUE_INITIALIZER

extern void  k_queue_init(struct k_queue *queue);
extern void  k_queue_uninit(struct k_queue *queue);
extern void  k_queue_cancel_wait(struct k_queue *queue);
extern void  k_queue_append(struct k_queue *queue, void *data);
extern void  k_queue_prepend(struct k_queue *queue, void *data);
extern void  k_queue_insert(struct k_queue *queue, void *prev, void *data);
extern void  k_queue_append_list(struct k_queue *queue, void *head, void *tail);
extern void *k_queue_get(struct k_queue *queue, s32_t timeout);
extern int   k_queue_is_empty(struct k_queue *queue);

/* lifo define*/
struct k_lifo
{
    struct k_queue _queue;
};

#define _K_LIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }

#define K_LIFO_INITIALIZER DEPRECATED_MACRO _K_LIFO_INITIALIZER

#define k_lifo_init(lifo) k_queue_init((struct k_queue *)lifo)

#define k_lifo_put(lifo, data) k_queue_prepend((struct k_queue *)lifo, data)

#define k_lifo_get(lifo, timeout) k_queue_get((struct k_queue *)lifo, timeout)

#define K_LIFO_DEFINE(name)                                   \
    struct k_lifo name = \
      _K_LIFO_INITIALIZER(name)

struct k_fifo
{
    struct k_queue _queue;
};

#define _K_FIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }
#define K_FIFO_INITIALIZER DEPRECATED_MACRO _K_FIFO_INITIALIZER

#define k_fifo_init(fifo) k_queue_init((struct k_queue *)fifo)

#define k_fifo_cancel_wait(fifo) k_queue_cancel_wait((struct k_queue *)fifo)

#define k_fifo_put(fifo, data) k_queue_append((struct k_queue *)fifo, data)

#define k_fifo_put_list(fifo, head, tail) \
    k_queue_append_list((struct k_queue *)fifo, head, tail)

#define k_fifo_get(fifo, timeout) k_queue_get((struct k_queue *)fifo, timeout)

#define K_FIFO_DEFINE(name)                                   \
    struct k_fifo name = \
      _K_FIFO_INITIALIZER(name)

/* sem define*/
struct k_sem
{
    _sem_t      sem;
};

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @return 0 Creat a semaphore succcess
 */
int k_sem_init(struct k_sem *sem, unsigned int initial_count,
               unsigned int limit);

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @note When porting code from the nanokernel legacy API to the new API, be
 * careful with the return value of this function. The return value is the
 * reverse of the one of nano_sem_take family of APIs: 0 means success, and
 * non-zero means failure, while the nano_sem_take family returns 1 for success
 * and 0 for failure.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int k_sem_take(struct k_sem *sem, uint32_t timeout);

/**
 * @brief Give a semaphore.
 *
 * This routine gives @a sem, unless the semaphore is already at its maximum
 * permitted count.
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 Give semaphore success
 */
int k_sem_give(struct k_sem *sem);

/**
 * @brief Delete a semaphore.
 *
 * This routine delete @a sem,
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 delete semaphore success
 */
int k_sem_delete(struct k_sem *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
unsigned int k_sem_count_get(struct k_sem *sem);

typedef void (*k_timer_handler_t)(void *timer, void *args);
typedef struct k_timer
{
    ktimer_t          timer;
    k_timer_handler_t handler;
    void *            args;
    uint32_t          timeout;
    uint32_t          start_ms;
} k_timer_t;

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param handle Function to invoke each time the timer expires.
 * @param args   Arguments sent to handle.
 *
e* @return N/A
 */
void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args);

/**
 * @brief Start a timer.
 *
 * @param timer    Address of timer.
 * @param timeout  time before timeout happen(in milliseconds).
 *
 * @return N/A
 */
void k_timer_start(k_timer_t *timer, uint32_t timeout);

/**
 * @brief Stop a timer.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
void k_timer_stop(k_timer_t *timer);

/*time define*/
#define MSEC_PER_SEC 1000
#define K_MSEC(ms) (ms)
#define K_SECONDS(s) K_MSEC((s)*MSEC_PER_SEC)
#define K_MINUTES(m) K_SECONDS((m)*60)
#define K_HOURS(h) K_MINUTES((h)*60)

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get(void);
u32_t k_uptime_get_32(void);

struct k_thread
{
    _task_t task;
};

typedef _stack_element_t k_thread_stack_t;

inline void k_call_stacks_analyze(void) {}

#define K_THREAD_STACK_DEFINE(sym, size) _stack_element_t sym[size]
#define K_THREAD_STACK_SIZEOF(sym) sizeof(sym)

static inline char *K_THREAD_STACK_BUFFER(k_thread_stack_t *sym)
{
    return (char *)sym;
}

typedef void (*k_thread_entry_t)(void *p1, void *p2, void *p3);
/**
 * @brief Spawn a thread.
 *
 * This routine initializes a thread, then schedules it for execution.
 *
 * @param name  Thread name
 * @param stack Pointer to the stack space.
 * @param stack_size Stack size in bytes.
 * @param fn Thread entry function.
 * @param arg entry point parameter.
 * @param prio Thread priority.
 *
 * @return 0 success.
 */
int k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack,
                    size_t stack_size, k_thread_entry_t entry, void *p1,
                    void *p2, void *p3, int prio, u32_t options, s32_t delay);

/**
 * @brief Yield the current thread.
 *
 * This routine causes the current thread to yield execution to another
 * thread of the same or higher priority. If there are no other ready threads
 * of the same or higher priority, the routine returns immediately.
 *
 * @return N/A
 */
int k_yield();

/**
 * @brief Lock interrupts.
 *
 * This routine disables all interrupts on the CPU. It returns an unsigned
 * integer "lock-out key", which is an architecture-dependent indicator of
 * whether interrupts were locked prior to the call. The lock-out key must be
 * passed to irq_unlock() to re-enable interrupts.
 *
 * @return Lock-out key.
 */
unsigned int irq_lock();

/**
 * @brief Unlock interrupts.
 *
 * This routine reverses the effect of a previous call to irq_lock() using
 * the associated lock-out key. The caller must call the routine once for
 * each time it called irq_lock(), supplying the keys in the reverse order
 * they were acquired, before interrupts are enabled.
 *
 * @param key Lock-out key generated by irq_lock().
 *
 * @return N/A
 */
void irq_unlock(unsigned int key);

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args);
void k_timer_start(k_timer_t *timer, uint32_t timeout);
void k_timer_stop(k_timer_t *timer);

void k_sleep(s32_t duration);
long long k_now_ms();

#endif /* _MESH_HAL_OS_H_ */
