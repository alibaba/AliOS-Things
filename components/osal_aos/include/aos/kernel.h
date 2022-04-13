/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_KERNEL_H
#define AOS_KERNEL_H

#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <aos/osal_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup aos_kernel kernel
 * kernel AOS API.
 *
 * @{
 */

/* Define the AliOS-Things' Version */
#define SYSINFO_KERNEL_VERSION "AOS-R-3.3.0"

/* Defined for API with delay time */
#define AOS_WAIT_FOREVER 0xffffffffu /**< Wait one event for ever */
#define AOS_NO_WAIT      0x0         /**< return immediately if no event */

/* Define default aos task priority*/
#ifndef AOS_DEFAULT_APP_PRI
#define AOS_DEFAULT_APP_PRI 32       /**< Default task priority */
#endif
#define AOS_EVENT_AND              0x02u
#define AOS_EVENT_AND_CLEAR        0x03u
#define AOS_EVENT_OR               0x00u
#define AOS_EVENT_OR_CLEAR         0x01u

#define AOS_TASK_NONE              0x0u
#define AOS_TASK_AUTORUN           0x01u

#define AOS_TIMER_NONE             0x0u
#define AOS_TIMER_AUTORUN          0x01u
#define AOS_TIMER_REPEAT           0x02u

/* Define the handle for all aos module     */
typedef void * aos_hdl_t;
/* Define the main handle for task module   */
typedef aos_hdl_t aos_task_t;
/* Define the main handle for mutex module  */
typedef aos_hdl_t aos_mutex_t;
/* Define the main handle for sem module    */
typedef aos_hdl_t aos_sem_t;
/* Define the main handle for queue module  */
typedef aos_hdl_t aos_event_t;
/* Define the main handle for event module  */
typedef aos_hdl_t aos_queue_t;
/* Define the main handle for timer module  */
typedef aos_hdl_t aos_timer_t;
/* Define the data type for task key index  */
typedef uint32_t aos_task_key_t;
/* Define the data type for function return */
typedef int32_t aos_status_t;

/**
 * Create a task.
 *
 * @param[in]  task        handle.
 * @param[in]  name        task name.
 * @param[in]  fn          task function.
 * @param[in]  arg         argument of the function..
 * @param[in]  stack_buf   stack-buf: if stack_buf == NULL, provided by kernel.
 * @param[in]  stack_size  stack-size in bytes.
 * @param[in]  prio        priority value, the max is RHINO_CONFIG_USER_PRI_MAX(default 60).
 * @param[in]  options     options,AOS_TASK_AUTORUN means autorun.
 *
 * @return  0: success.
 */
aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg,void *stack_buf, size_t stack_size, int32_t prio, uint32_t options);

/**
 * suspend a task.
 *
 * @param[in]  task  task handle.
 */
aos_status_t aos_task_suspend(aos_task_t *task);

/**
 * resume a task.
 *
 * @param[in]  task  task handle.
 */
aos_status_t aos_task_resume(aos_task_t *task);

/**
 * Exit a task.
 *
 * @param[in]  code  not used now.
 *
 * @return  none.
 */
void aos_task_exit(int32_t code);

/**
 * Delete a task by task handle
 *
 * @param[in]  task  task handle.
 *
 * @return  0: success.
 */
aos_status_t aos_task_delete(aos_task_t *task);

/**
 * Yield the current task
 * @return  0: success.
 */
aos_status_t aos_task_yield(void);


/**
 * Get a task name.
 * @param[in]   task        task handle
 * @param[out]  buf         the returned task name buf
 * @param[in]   buf_size    name buf size
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size);

/**
 * Get cur task handle
 *
 * @return  NULL: error.
 */
aos_task_t aos_task_self(void);

/**
 * Create a task key.
 *
 * @param[out]  key  pointer of key object.
 *
 * @return  0: success, -EINVAL: error.
 */
aos_status_t aos_task_key_create(aos_task_key_t *key);

/**
 * Delete a task key.
 *
 * @param[in]  key  key object.
 *
 * @return  none.
 */
void aos_task_key_delete(aos_task_key_t key);

/**
 * Associate a task-specific value with a key.
 *
 * @param[in]  key  key object.
 * @param[in]  vp   pointer of a task-specific value.
 *
 * @return  the check status, 0: OK, -1: indicates key invalid.
 */
aos_status_t aos_task_setspecific(aos_task_key_t key, void *vp);

/**
 * Get the value currently bound to the specified key.
 *
 * @param[in]  key  key object.
 *
 * @return  NULL: get fail, otherwise: get succeed.
 */
void *aos_task_getspecific(aos_task_key_t key);

/**
 * Alloc a mutex.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in aos_mutex_t will refer a kernel obj internally.
 * @param[in]  options  reserved.
 * @return  0: success.
 */
aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options);
/**
 * Free a mutex.
 *
 * @param[in]  mutex  mutex object, mem refered by hdl pointer in
 *                    aos_mutex_t will be freed internally.
 */
void aos_mutex_free(aos_mutex_t *mutex);
/**
 * Lock a mutex.
 *
 * @param[in]  mutex    mutex object, it contains kernel obj pointer.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex  mutex object, it contains kernel obj pointer.
 *
 * @return  0: success.
 */
aos_status_t aos_mutex_unlock(aos_mutex_t *mutex);

/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @param[in]   options  reserved.
 * @return  0:success.
 */
aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options);
/**
 * Destroy a semaphore.
 *
 * @param[in]  sem  pointer of semaphore object, mem refered by hdl pointer
 *                  in aos_sem_t will be freed internally.
 */
void aos_sem_free(aos_sem_t *sem);

/**
 * Acquire a semaphore.
 *
 * @param[in]  sem      semaphore object, it contains kernel obj pointer
 *                      which aos_sem_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
aos_status_t aos_sem_wait(aos_sem_t *sem, uint32_t timeout);

/**
 * Release a semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal(aos_sem_t *sem);

/**
 * Release all semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal_all(aos_sem_t *sem);

/**
 * Alloc a event.
 *
 * @param[out]  sem    pointer of event object, event object must be
 *                     alloced, hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   value  initial event value.
 *
 * @param[in]   options  reserved.
 * @return  0:success.
 */
aos_status_t aos_event_create(aos_event_t *event, uint32_t value, uint32_t options);

/**
 * Destroy a event.
 *
 * @param[in]  event  pointer of event object, mem refered by hdl pointer
 *                    in aos_hdl_t will be freed internally.
 */
void aos_event_free(aos_event_t *event);


/**
 * Get a event.
 *
 * @param[in] event  event object.
 * @param[in] value  Expect value.
 * @param[in] opt    Mode of operation, one of AND/AND_CLEAR/OR/OR_CLEAR
 * @param[OUT] actl_value  Actual acquired value
 * @param[in] timeout waiting until timeout in milliseconds.
 *
 *  * @return  0:success.
 */
aos_status_t aos_event_get(aos_event_t *event, uint32_t value, uint32_t opt, uint32_t *actl_value, uint32_t timeout);

/**
 * Set a event.
 *
 * @param[in] event  event object.
 *
 * @param[in] value  The value to set.
 *
 * @param[in] opt    Mode of operation, one of AND/AND_CLEAR/OR/OR_CLEAR
 *
 * @param[in] timeout waiting until timeout in milliseconds.
 *
 * @return  0:success.
 */
aos_status_t aos_event_set(aos_event_t *event, uint32_t value, uint32_t opt);

/**
 * This function will create a queue.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of the msg.
 * @param[in]  options    reserved.
 * @return  0: success.
 */
aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msg, uint32_t options);
/**
 * This function will delete a queue.
 *
 * @param[in]  queue  pointer to the queue.
 */
void aos_queue_free(aos_queue_t *queue);

/**
 * This function will send a msg to the front of a queue.
 *
 * @param[in]  queue  pointer to the queue.
 * @param[in]  msg    msg to send.
 * @param[in]  size   size of the msg.
 *
 * @return  0: success.
 */
aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size);

/**
 * This function will receive msg from a queue.
 *
 * @param[in]   queue  pointer to the queue.
 * @param[in]   ms     ms to wait before receive.
 * @param[out]  msg    buf to save msg.
 * @param[out]  size   size of the msg.
 *
 * @return  0: success.
 */
aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg, size_t *size);

/**
 * Get number of queued messages in a message queue.
 *
 * @param[in]  queue  message queue handle to operate.
 *
 * @return  number of queued messages.negative indicates error code.
 */
aos_status_t aos_queue_get_count(aos_queue_t *queue);

/**
 * This function will create a timer.
 *
 * @param[in]  timer     pointer to the timer.
 * @param[in]  fn        callbak of the timer.
 * @param[in]  arg       the argument of the callback.
 * @param[in]  ms        ms of the normal timer triger.
 * @param[in]  options     option settings, can be set to AOS_TIMER_AUTORUN|AOS_TIMER_REPEAT
 *
 * @return  0: success.
 */
aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint32_t options);

/**
 * This function will delete a timer.
 *
 * @param[in]  timer  pointer to a timer.
 */
void aos_timer_free(aos_timer_t *timer);

/**
 * This function will start a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
aos_status_t aos_timer_start(aos_timer_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
aos_status_t aos_timer_stop(aos_timer_t *timer);

/**
 * This function will change attributes of a timer.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[in]  ms     ms of the timer triger.
 *
 * @note change the timer attributes should follow
 *       the sequence as timer_stop->timer_change->timer_start
 *
 * @return  0: success.
 */
aos_status_t aos_timer_change(aos_timer_t *timer, uint32_t ms);

/**
 * This function will change attributes of a timer once.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[in]  ms     ms of the timer triger.
 *
 * @note change the timer attributes should follow
 *       the sequence as timer_stop->timer_change->timer_start
 *
 * @return  0: success.
 */
aos_status_t aos_timer_change_once(aos_timer_t *timer, uint32_t ms);

/**
 * Realloc memory.
 *
 * @param[in]  mem   current memory address point.
 * @param[in]  size  new size of the mem to remalloc.
 *
 * @return  NULL: error.
 */
void *aos_realloc(void *mem, size_t size);

/**
 * Alloc memory.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_malloc(size_t size);

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  nitems  number of items to malloc.
 * @param[in]  size    size of one item to malloc.
 *
 * @return  NULL: error.
 */
void *aos_calloc(size_t nitems, size_t size);

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_zalloc(size_t size);

/**
 * Trace alloced mems.
 *
 * @param[in]  addr       pointer of the mem alloced by malloc.
 * @param[in]  allocator  buildin_address.
 */
void aos_alloc_trace(void *addr, uintptr_t allocator);

/**
 * Free memory.
 *
 * @param[in]  ptr  address point of the mem.
 *
 * @return  none.
 */
void aos_free(void *mem);

/**
 * Set calendar time.
 * @param[in]  now_ms the calender time (unit:ms) to set, for example,
 *                    the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 */
void aos_calendar_time_set(uint64_t now_ms);

/**
 * Get calendar time.
 *
 * @return current calendar time (unit:ms), for example,
 *         the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 */
uint64_t aos_calendar_time_get(void);

/**
 * Get calendar time with utc offset.
 *
 * @return current calendar time with utc offset (unit:ms).
 */
uint64_t aos_calendar_localtime_get(void);

/**
 * Get current time in nanoseconds.
 *
 * @return  elapsed time in nanoseconds from system starting.
 */
uint64_t aos_now(void);

/**
 * Get current time in milliseconds.
 *
 * @return  elapsed time in milliseconds from system starting.
 */
uint64_t aos_now_ms(void);

/**
 * @brief Retrieves the timer string. Under RTOS w/o RTC, this fucntion will
 *        return the UTC time string that consider boot-up as 01-01 00:00:00.000.
 *        Under Linuxhost and compile option "vall=posix", this function will
 *        get the local time which considering time zone.
 *
 * @param[out]  buf  give buffer to save timer string
 * @param[in]   len  the length of buffer, recommand 19, right a terminating
 *                   null-character is appended in last.
 *
 * @return  0: success.
 *
 */
aos_status_t aos_now_time_str(char *buffer, size_t len);

/**
 * Msleep.
 *
 * @param[in]  ms  sleep time in milliseconds.
 */
void aos_msleep(uint32_t ms);

/**
 * srand function.
 *
 * @param[in]  seed  The seed number to use to generate the random sequence.
 *
 * @return  none.
 */
void aos_srand(uint32_t seed);

/**
 * rand function.
 *
 * @return  random value.
 */
int32_t aos_rand(void);

/**
 * Reboot AliOS.
 *
 * @return none.
 */
void aos_reboot(void);

/**
 * Get HZ(ticks per second).
 *
 * @return RHINO_CONFIG_TICKS_PER_SECOND.
 */
int32_t aos_get_hz(void);

/**
 * Get kernel version string.
 *
 * @param[in]   len   the size of the @buf in bytes
 * @param[out]  buf   buffer pointer to copy kernel version string
 *
 * @return  On succuess, return the buf pointer, else return NULL.
 * @note  If the actual kernel version string length is larger than
 *        the buffer len-1, than only len-1 bytes are copied into
 *        the buffer, and set the last byte of the buf to zero.
 *        Else, copy the whole kernel version string into the buf.
 */
aos_status_t aos_version_str_get(char *buf, size_t len);

/**
 * Initialize system
 */
void aos_init(void);

/**
 * Start system.
 */
void aos_start(void);

/* The following APIs is marked deprecated and is not recommended */

/**
 * Create a task.
 * Deprecated, not Recommended.
 *
 * @param[in]  name       task name.
 * @param[in]  fn         function to run.
 * @param[in]  arg        argument of the function.
 * @param[in]  stacksize  stack-size in bytes.
 *
 * @return  0: success, otherwise: fail.
 */
aos_status_t aos_task_new(const char *name, void (*fn)(void *), void *arg, size_t stack_size);

/**
 * Create a task.
 * Deprecated, not Recommended.
 *
 * @param[in]  task        handle.
 * @param[in]  name        task name.
 * @param[in]  fn          task function.
 * @param[in]  arg         argument of the function..
 * @param[in]  stack_size  stack-size in bytes.
 * @param[in]  prio        priority value, the max is RHINO_CONFIG_USER_PRI_MAX(default 60).
 *
 * @return  0: success.
 */
aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int32_t prio);

/**
 * Alloc a mutex.
 * Deprecated, not Recommended.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in aos_mutex_t will refer a kernel obj internally.
 *
 * @return  0: success.
 */
aos_status_t aos_mutex_new(aos_mutex_t *mutex);

/**
 * This function will check if mutex is valid.
 * Deprecated, not Recommended.
 *
 * @param[in]  mutex  pointer to the mutex.
 *
 * @return  false: invalid, true: valid.
 */
bool aos_mutex_is_valid(aos_mutex_t *mutex);


/**
 * Alloc a semaphore.
 * Deprecated, not Recommended.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0:success.
 */
aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count);

/**
 * This function will check if semaphore is valid.
 * Deprecated, not Recommended.
 *
 * @param[in]  sem  pointer to the sem.
 *
 * @return  false: invalid, true: valid.
 */
bool aos_sem_is_valid(aos_sem_t *sem);

/**
 * Alloc a event.
 * Deprecated, not Recommended.
 *
 * @param[out]  event  pointer of event object, event object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   value  initial event value.
 *
 * @return  0:success.
 */
aos_status_t aos_event_new(aos_event_t *event, uint32_t value);

/**
 * This function will check if event is valid.
 * Deprecated, not Recommended.
 *
 * @param[in]  event  pointer to the event.
 *
 * @return  false: invalid, true: valid.
 */
bool aos_event_is_valid(aos_event_t *event);

/**
 * This function will create a queue.
 * Deprecated, not Recommended.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  buf      buf of the queue(provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of the msg.
 *
 * @return  0: success.
 */
aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg);

/**
 * This function will check if queue is valid.
 * Deprecated, not Recommended.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  false: invalid, true: valid.
 */
bool aos_queue_is_valid(aos_queue_t *queue);

/**
 * This function will return buf start ptr if queue is inited.
 * Deprecated, not Recommended.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  NULL: error.
 */
void *aos_queue_buf_ptr(aos_queue_t *queue);

/**
 * This function will create a timer and run auto.
 * Deprecated, not Recommended.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 *
 * @return  0: success.
 */
aos_status_t aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, bool repeat);

/**
 * This function will create a timer.
 * Deprecated, not Recommended.
 *
 * @param[in]  timer     pointer to the timer.
 * @param[in]  fn        callbak of the timer.
 * @param[in]  arg       the argument of the callback.
 * @param[in]  ms        ms of the normal timer triger.
 * @param[in]  repeat    repeat or not when the timer is created.
 * @param[in]  autorun   auto run.
 *
 * @return  0: success.
 */
aos_status_t aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, bool repeat, bool  autorun);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_H */

