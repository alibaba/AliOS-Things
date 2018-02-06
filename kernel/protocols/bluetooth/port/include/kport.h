/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H
#include  <stdint.h>
#include  <stddef.h>
#include "config.h"
#include "aos/log.h"

#include <misc/dlist.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <k_config.h>
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_critical.h>
#include <k_sys.h>
#include <k_bitmap.h>
#include <k_list.h>
#include <k_obj.h>
#include <k_sched.h>
#include <k_task.h>
#include <k_ringbuf.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_sem.h>
#include <k_task_sem.h>
#include <k_mutex.h>
#include <k_timer.h>
#include <k_time.h>
#include <k_event.h>
#include <k_stats.h>
#include <k_mm_debug.h>
#include <k_mm_blk.h>
#include <k_mm_region.h>
#include <k_mm.h>
#include <k_workqueue.h>
#include <k_internal.h>
#include <k_trace.h>
#include <k_soc.h>
#include <k_hook.h>
#include <port.h>
#include <k_endian.h>

#if defined(__cplusplus)
extern "C"
{
#endif

typedef kqueue_t _queue_t;
typedef ksem_t  _sem_t;
typedef ktask_t _task_t;
typedef cpu_stack_t _stack_element_t;

#define _K_SEM_INITIALIZER(obj, initial_count, count_limit)  { }

#define K_SEM_INITIALIZER DEPRECATED_MACRO _K_SEM_INITIALIZER

#define K_SEM_DEFINE(name, initial_count, count_limit) \
        struct k_sem name \
                __in_section(_k_sem, static, name) = \
                _K_SEM_INITIALIZER(name, initial_count, count_limit)

/* Log define*/
enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX_BIT
};
#define BT_MOD  "AOSBT"

#ifdef CONFIG_OBJECT_TRACING
#define _OBJECT_TRACING_NEXT_PTR(type) struct type *__next
#define _OBJECT_TRACING_INIT .__next = NULL,
#else
#define _OBJECT_TRACING_INIT
#define _OBJECT_TRACING_NEXT_PTR(type)
#endif

#ifdef CONFIG_POLL
#define _POLL_EVENT_OBJ_INIT(obj) \
        .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events),
#define _POLL_EVENT sys_dlist_t poll_events
#else
#define _POLL_EVENT_OBJ_INIT(obj)
#define _POLL_EVENT
#endif

#define SYS_LOG_DBG(...) LOGD(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_INF(...) LOGI(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_WRN(...) LOGW(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_ERR(...) LOGE(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_FAT(...) LOGF(BT_MOD,##__VA_ARGS__)

#define LIFO_DEBUG 0
#define FIFO_DEBUG 0

#if 0
#define _K_QUEUE_INITIALIZER(obj) \
        { \
        .wait_q = SYS_DLIST_STATIC_INIT(&obj.wait_q), \
        .data_q = SYS_SLIST_STATIC_INIT(&obj.data_q), \
        _POLL_EVENT_OBJ_INIT(obj) \
        _OBJECT_TRACING_INIT \
        }

#define K_QUEUE_INITIALIZER DEPRECATED_MACRO _K_QUEUE_INITIALIZER
#endif

/* lifo define*/
struct k_lifo {
    _queue_t *_queue;
    sys_dlist_t poll_events;
#if LIFO_DEBUG
    uint32_t total_count;
    int32_t count;
#endif
};

struct k_queue {
    _queue_t *_queue;
    sys_dlist_t poll_events;
};

#define _K_LIFO_INITIALIZER(obj)  { 0 }

#define K_LIFO_INITIALIZER DEPRECATED_MACRO _K_LIFO_INITIALIZER

/**
 * @brief Initialize a lifo.
 *
 * This routine initializes a lifo object, prior to its first use.
 *
 * @param lifo Address of the lifo.
 *
 * @return N/A
 */
void k_lifo_init(struct k_lifo *lifo);

/**
 * @brief Add an element to a lifo.
 *
 * This routine adds a data item to @a lifo. A lifo data item must be
 * aligned on a 4-byte boundary
 *
 * @param lifo Address of the lifo.
 * @param data Address of the data item.
 *
 * @return N/A
 */
void k_lifo_put(struct k_lifo *lifo, void *data);

/**
 * @brief Get an element from a lifo.
 *
 * This routine removes a data item from @a lifo in a "last in, first out"
 * manner.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param lifo Address of the lifo.
 * @param timeout Waiting period to obtain a data item (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
void *k_lifo_get(struct k_lifo *lifo, tick_t timeout);

#if 0
#define _K_FIFO_INITIALIZER(obj) \
        { \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
        }
#endif
#define _K_FIFO_INITIALIZER(obj) { 0 }

#define K_FIFO_INITIALIZER DEPRECATED_MACRO _K_FIFO_INITIALIZER

#define K_FIFO_DEFINE(name) \
        struct k_fifo name \
                __in_section(_k_queue, static, name) = \
                _K_FIFO_INITIALIZER(name)

/* fifo define*/
struct k_fifo {
    _queue_t* _queue;
    sys_dlist_t poll_events;
#if FIFO_DEBUG
    uint32_t total_count;
    int32_t count;
#endif
};

/**
 * @brief Initialize a fifo.
 *
 * This routine initializes a fifo object, prior to its first use.
 *
 * @param fifo Address of the fifo.
 *
 * @return N/A
 */
void k_fifo_init(struct k_fifo *fifo);

/**
 * @brief Add an element to a fifo.
 *
 * This routine adds a data item to @a fifo. A fifo data item must be
 * aligned on a 4-byte boundary
 *
 * @param fifo Address of the fifo.
 * @param data Address of the data item.
 *
 * @return N/A
 */
void k_fifo_put(struct k_fifo *fifo, void *msg);

void k_fifo_put_list(struct k_fifo *fifo, void *head, void *tail);

/**
 * @brief Get an element from a fifo.
 *
 * This routine removes a data item from @a fifo in a "first in, first out"
 * manner. The first 32 bits of the data item are reserved for the kernel's use.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param fifo Address of the fifo.
 * @param timeout Waiting period to obtain a data item (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
void *k_fifo_get(struct k_fifo *fifo, tick_t timeout);

void k_fifo_cancel_wait(struct k_fifo *fifo);

/* sem define*/
struct k_sem {
    _sem_t sem;
    sys_dlist_t poll_events;
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
int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit);

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
typedef struct k_timer {
    ktimer_t timer;
    k_timer_handler_t handler;
    void *args;
    uint32_t timeout;
    uint32_t start_ms;
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
 * @return N/A
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
#define K_MSEC(ms)     (ms)
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)
#define K_MINUTES(m)   K_SECONDS((m) * 60)
#define K_HOURS(h)     K_MINUTES((h) * 60)

#define K_PRIO_COOP(x)  x

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get();
inline u32_t k_uptime_get_32(void)
{
    return (u32_t)krhino_sys_time_get();
}

struct k_thread {
    _task_t *task;
};

typedef _stack_element_t k_thread_stack_t;

inline void k_call_stacks_analyze(void) { }

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
                    size_t stack_size, k_thread_entry_t entry,
                    void *p1, void *p2, void *p3,
                    int prio, u32_t options, s32_t delay);

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

#define BIT(n)  (1UL << n)

#if defined(__cplusplus)
}
#endif

#endif /* PORT_H */

