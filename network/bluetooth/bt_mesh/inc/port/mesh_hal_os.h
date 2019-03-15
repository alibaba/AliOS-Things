/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _MESH_HAL_OS_H_
#define _MESH_HAL_OS_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef CONFIG_MESH_STACK_ALONE
/**
 * Attetion: os_port_def.h file is needed as well as 
 * mesh_hal_os.c when porting to new platform.
 * 
 *   - os_port_def.h: provides platform-dependent defines of
 *                    queue_t, sem_t, log primitives, etc.
 *   - mesh_hal_os.c: provide implementation of util APIs.
 */
#include "os_port_def.h"

/* sem define*/
struct k_sem
{
    _sem_t      sem;
};

typedef void (*k_timer_handler_t)(void *timer, void *args);
typedef struct k_timer
{
    ktimer_t          timer;
    k_timer_handler_t handler;
    void *            args;
    uint32_t          timeout;
    uint32_t          start_ms;
} k_timer_t;

/*time define*/
#define MSEC_PER_SEC 1000
#define K_MSEC(ms) (ms)
#define K_SECONDS(s) K_MSEC((s)*MSEC_PER_SEC)
#define K_MINUTES(m) K_SECONDS((m)*60)
#define K_HOURS(h) K_MINUTES((h)*60)

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

#endif /* #ifdef CONFIG_MESH_STACK_ALONE */

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

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get(void);
u32_t k_uptime_get_32(void);
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
