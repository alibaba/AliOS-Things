/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PORT_H
#define PORT_H
#include  <stdint.h>
#include  <stddef.h>
#include "config.h"
#include "aos/log.h"

#include "esp_log.h"

#ifdef CONFIG_AOS_RHINO
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
#include <k_obj_set.h>
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
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#ifdef CONFIG_AOS_RHINO
typedef kqueue_t _queue_t;
typedef ksem_t  _sem_t;
#else
typedef void* _queue_t;
typedef void* _sem_t
#endif

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
#define BT_MOD  "ZEPHYRBT"

#define SYS_LOG_DBG(...) LOGD(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_INF(...) LOGI(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_WRN(...) LOGW(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_ERR(...) LOGE(BT_MOD,##__VA_ARGS__)
#define SYS_LOG_FAT(...) LOGF(BT_MOD,##__VA_ARGS__)

#define LIFO_DEBUG 0
#define FIFO_DEBUG 0

/* lifo define*/
struct k_lifo {
    _queue_t *_queue;
#if LIFO_DEBUG
    uint32_t total_count;
    int32_t count;
#endif
};

/**
 * @brief Initialize a lifo.
 *
 * This routine initializes a lifo object, prior to its first use.
 *
 * @param lifo Address of the lifo.
 * @param name lifo name.
 * @param start Address of static assgin memery.if NULL ,alloc memery dynamically
 * @param msg_num message numbers of the lifo.
 *
 * @return N/A
 */
void k_lifo_init(struct k_lifo *lifo, const char *name,void **start,size_t msg_num);

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

/* fifo define*/
struct k_fifo {
    _queue_t* _queue;
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
 * @param name lifo name.
 * @param start Address of static assgin memery.if NULL ,alloc memery dynamically
 * @param msg_num message numbers of the lifo.
 *
 * @return N/A
 */
void k_fifo_init(struct k_fifo *fifo, const char *name, void **start, size_t msg_len);

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

/* sem define*/
struct k_sem {
    _sem_t *sem;
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

typedef void (*k_timer_handler_t)(void *arg);
typedef struct k_timer {
    void *timer;
    k_timer_handler_t handler;
    void *args;
	uint32_t timeout;
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

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get();

typedef void (*k_thread_entry_t)(void *args);
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
int k_thread_spawn(const char *name, uint32_t *stack, uint32_t stack_size, \
                   k_thread_entry_t fn, void *arg, int prio);

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

#if defined(__cplusplus)
}
#endif

#endif /* PORT_H */

