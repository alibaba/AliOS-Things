/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

/*
 * !!!!!!Be careful! If you modify this file, please recompile the system compilation tool chain.!!!!!!
 */

#ifndef AOS_KERNEL_H
#define AOS_KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_WAIT_FOREVER 0xffffffffu
#define AOS_NO_WAIT      0x0
#define AOS_MUTEX_NESTED -101

#ifndef AOS_DEFAULT_APP_PRI
#define AOS_DEFAULT_APP_PRI 32
#endif

#ifndef AOS_CONFIG_PRI_MAX
#define AOS_CONFIG_PRI_MAX (62)
#endif

#ifndef AOS_IDLE_PRI
#define AOS_IDLE_PRI (AOS_CONFIG_PRI_MAX - 1)
#endif

typedef struct {
    void *hdl;
} aos_hdl_t;

typedef aos_hdl_t aos_task_t;
typedef aos_hdl_t aos_mutex_t;
typedef aos_hdl_t aos_sem_t;
typedef aos_hdl_t aos_queue_t;
typedef aos_hdl_t aos_timer_t;

typedef uint32_t aos_task_key_t;

#define AOS_EVENT_AND          0x02u
#define AOS_EVENT_AND_CLEAR    0x03u
#define AOS_EVENT_OR           0x00u
#define AOS_EVENT_OR_CLEAR     0x01u

void aos_init(void);

/**
 * Reboot AliOS.
 */
void aos_reboot(void);

/**
 * Get HZ(ticks per second).
 *
 * @return  RHINO_CONFIG_TICKS_PER_SECOND.
 */
int aos_get_hz(void);

/**
 * Get kernel version.
 *
 * @return  sysinfo_kernel_version.
 */
const char *aos_version_get(void);

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
char *aos_version_str_get(const char *buf, size_t len);

/**
 * Create a task.
 *
 * @param[in]  name       task name.
 * @param[in]  fn         function to run.
 * @param[in]  arg        argument of the function.
 * @param[in]  stacksize  stack-size in bytes.
 *
 * @return  0: success.
 */
int aos_task_new(const char *name, void (*fn)(void *), void *arg, size_t stack_size);

/**
 * Create a task.
 *
 * @param[in]  task        handle.
 * @param[in]  name        task name.
 * @param[in]  fn          task function.
 * @param[in]  arg         argument of the function..
 * @param[in]  stack_buf   stack-buf: if stack_buf==NULL, provided by kernel.
 * @param[in]  stack_size  stack-size in bytes.
 * @param[in]  prio        priority value, the max is RHINO_CONFIG_USER_PRI_MAX(default 60).
 * @param[in]  flags       Bit 0 represent the autorunning flag of task.
 *
 * @return  0: success.
 */
int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int prio, uint32_t flags);

/**
 * resume a task.
 *
 * @param[in]  task  task handle.
 */
int aos_task_resume(aos_task_t *task);

/**
 * Exit a task.
 *
 * @param[in]  code  not used now.
 */
void aos_task_exit(int code);

/**
 * Delete a task by task handle
 *
 * @param[in]  task  task handle.
 *
 * @return  0: success.
 */
int aos_task_delete(aos_task_t *task);

/**
 * Yield the current task
 * @return  0: success.
 */
int aos_task_yield(void);

/**
 * Get the current task name.
 *
 * @return  the name of the task
 */
const char *aos_task_name(void);

/**
 * Get a task name.
 * @param[in]   task    task handle
 * @param[out]  name    The returned task name
 * @return  0:  success     otherwise failed
 */
int aos_task_name_get(aos_task_t *task, const char **name);

/**
 * Create a task key.
 *
 * @param[in]  key  pointer of key object.
 *
 * @return  0: success, -EINVAL: error.
 */
int aos_task_key_create(aos_task_key_t *key);

/**
 * Delete a task key.
 *
 * @param[in]  key  key object.
 */
void aos_task_key_delete(aos_task_key_t key);

/**
 * Associate a task-specific value with a key.
 *
 * @param[in]  key  key object.
 * @param[in]  vp   pointer of a task-specific value.
 *
 * @return  the check status, 0 is OK, -1 indicates invalid.
 */
int aos_task_setspecific(aos_task_key_t key, void *vp);

/**
 * Get the value currently bound to the specified key.
 *
 * @param[in]  key  key object.
 */
void *aos_task_getspecific(aos_task_key_t key);

/**
 * Set a task's user info
 *
 * @param[in]  task  task handle.
 * @param[in]  idx   The id of the user info
 * @param[in]  info  The info.
 * @return     0: success.   otherwise, error.
 */
int aos_task_info_set(aos_task_t *task, size_t idx, void *info);

/**
 * Get a task's user info
 *
 * @param[in]  task  task handle.
 * @param[in]  idx   The id of the user info
 * @param[in]  info  The info.
 * @return     0: success.   otherwise, error.
 */
int aos_task_info_get(aos_task_t *task, size_t idx, void **info);

/**
 * Change a task's priority
 *
 * @param[in]  task  task handle.
 * @param[in]  pri   new priority to set
 * @param[out] old_pri  old priority
 * @return     0: success.   otherwise, error.
 */
int aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri);

/**
 * Get a task's base priority
 *
 * @param[in]  task  task handle.
 * @param[out] priority  task base priority
 * @return     0: success.   otherwise, error.
 */
int aos_task_pri_get(aos_task_t *task, uint8_t *priority);

/**
 * Change a task's scheduler policy and priority
 *
 * @param[in]  task  task handle.
 * @param[in]  policy    new policy to set
 * @param[in]  pri   new priority to set
 * @return     0: success.   otherwise, error.
 */
int aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri);

/**
 * Get a task's scheduler policy
 *
 * @param[in]  task  task handle.
 * @param[out] policy the returned policy
 * @return     0: success,   otherwise, error.
 */
int aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy);

/** Get the default sched policy of a new task created by aos api.
 * @return the default sched policy
 */
int aos_task_sched_policy_get_default();

/**
 * Set a task's scheduler time slice for RR policy
 *
 * @param[in]  task  task handle.
 * @param[in]  slice  time slice(ms)
 * @return     0: success.   otherwise, error.
 */
int aos_task_time_slice_set(aos_task_t *task, uint32_t slice);

/**
 * Get a task's scheduler time slice for RR policy
 *
 * @param[in]  task  task handle.
 * @param[out] slice  time slice(ms)
 * @return     0: success.   otherwise, error.
 */
int aos_task_time_slice_get(aos_task_t *task, uint32_t *slice);

/**
 * Set a task's posix thread's control block
 *
 * @param[in]  task  task handle
 * @param[in]  ptcb  the posix thread's control block
 * @return     0: success.   otherwise, error.
 */
int aos_task_ptcb_set(aos_task_t *task, void *ptcb);

/**
 * Get a task's posix thread's control block
 *
 * @param[in]  task  task handle
 * @param[out] ptcb  tthe posix thread's control block
 * @return     0: success.   otherwise, error.
 */
int aos_task_ptcb_get(aos_task_t *task, void **ptcb);

/**
 * Get cur task handle
 *
 * @return  NULL: error.
 */
void *aos_task_now(void);

/**
 * Get the maximum schedule priority
 *
 * @param[in]  policy  schedule policy
 * @return >=0: the returned priority. otherwise, error.
 */
int aos_sched_get_priority_max(int policy);

/**
 * Get the minimum schedule priority
 *
 * @param[in]  policy  schedule policy
 * @return >=0: the returned priority. otherwise, error.
 */
int aos_sched_get_priority_min(int policy);

/**
 * Alloc a mutex.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in aos_hdl_t will refer a kernel obj internally.
 *
 * @return  0: success.
 */
int aos_mutex_new(aos_hdl_t *mutex);

/**
 * Free a mutex.
 *
 * @param[in]  mutex  mutex object, mem refered by hdl pointer in
 *                    aos_hdl_t will be freed internally.
 */
void aos_mutex_free(aos_hdl_t *mutex);
/**
 * Lock a mutex.
 *
 * @param[in]  mutex    mutex object, it contains kernel obj pointer.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_mutex_lock(aos_hdl_t *mutex, uint32_t timeout);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex  mutex object, it contains kernel obj pointer.
 *
 * @return  0: success.
 */
int aos_mutex_unlock(aos_hdl_t *mutex);

/**
 * Open a named semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   name   initial semaphore name.
 * @param[in]   flag   initial semaphore flag, such as O_CREAT and O_EXCL.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0:success.
 */
int aos_sem_open(aos_hdl_t *sem, const char *name, int flag, int count);
/**
 * Close a named semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 *
 * @return  0:success.
 */
int aos_sem_close(aos_hdl_t *sem);
/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0:success.
 */
int aos_sem_new(aos_hdl_t *sem, int count);

/**
 * Destroy a semaphore.
 *
 * @param[in]  sem  pointer of semaphore object, mem refered by hdl pointer
 *                  in aos_hdl_t will be freed internally.
 */
void aos_sem_free(aos_hdl_t *sem);

/**
 * Acquire a semaphore.
 *
 * @param[in]  sem      semaphore object, it contains kernel obj pointer
 *                      which aos_sem_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_sem_wait(aos_hdl_t *sem, uint32_t timeout);

/**
 * Release a semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal(aos_hdl_t *sem);

/**
 * Release all semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal_all(aos_hdl_t *sem);

/**
 * Get semaphore value.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 * @param[out]  val  store the returned semaphore value.
 *
 * @return  0:success.
 */
int aos_sem_getvalue(aos_hdl_t *sem, int *val);

/**
 * Remove a named semaphore.
 *
 * @param[in]  name  pointer of semaphore name.
 *
 * @return  0:success.
 */
int aos_sem_unlink(const char *name);
/**
 * Alloc a event.
 *
 * @param[out]  event  pointer of event object, event object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   value  initial event value.
 *
 * @return  0:success.
 */
int aos_event_new(aos_hdl_t *event, uint32_t value);

/**
 * Destroy a event.
 *
 * @param[in]  event  pointer of event object, mem refered by hdl pointer
 *                    in aos_hdl_t will be freed internally.
 */
void aos_event_free(aos_hdl_t *event);

/**
 * Open a named event.
 *
 * @param[out]  event  pointer of event object, event object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   name   initial event name.
 * @param[in]   o_flag initial event flag, such as O_CREAT and O_EXCL.
 * @param[in]   value  initial event value.
 *
 * @return  0:success.
 */
int aos_event_open(aos_hdl_t *event, const char *name, int o_flag, uint32_t value);

/**
 * Close a named event.
 *
 * @param[out]  event  pointer of semaphore object, event object must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 *
 * @return  0:success.
 */
int aos_event_close(aos_hdl_t *event);

/**
 * Remove a named event.
 *
 * @param[in]  name  pointer of event name.
 *
 * @return  0:success.
 */
int aos_event_unlink(const char *name);
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
int aos_event_get(aos_hdl_t *event, uint32_t value, uint8_t opt, uint32_t *actl_value, uint32_t timeout);

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
int aos_event_set(aos_hdl_t *event, uint32_t value, uint8_t opt);

/**
 * Open a named queue.
 *
 * @param[out]  queue   pointer of queue object, queue object must be
 *                      alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 * @param[in]   name    initial queue name.
 * @param[in]   o_flag  initial queue flag, such as O_CREAT and O_EXCL.
 * @param[in]   size    the bytes of the buf.
 * @param[in]   max_msg the max size of the msg.
 *
 * @return  0:success.
 */
int aos_queue_open(aos_hdl_t *queue, const char *name, int flag, size_t size, size_t max_msg);
/**
 * This function will create a queue.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of the msg.
 *
 * @return  0: success.
 */
int aos_queue_new(aos_hdl_t *queue, size_t size, size_t max_msg);

/**
 * This function will delete a queue.
 *
 * @param[in]  queue  pointer to the queue.
 */
void aos_queue_free(aos_hdl_t *queue);

/**
 * Close a named queue.
 *
 * @param[out]  queue  pointer of queue object or it`s handle, event queue must be
 *                     alloced, hdl pointer in aos_hdl_t will refer a kernel obj internally.
 *
 * @return  0:success.
 */
int aos_queue_close(aos_hdl_t *queue);

/**
 * This function will send a msg to the front of a queue.
 *
 * @param[in]  queue  pointer to the queue.
 * @param[in]  msg    msg to send.
 * @param[in]  size   size of the msg.
 *
 * @return  0: success.
 */
int aos_queue_send(aos_hdl_t *queue, void *msg, size_t size);

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
int aos_queue_recv(aos_hdl_t *queue, uint32_t ms, void *msg, size_t *size);

/**
 * Remove a named queue.
 *
 * @param[in]  name  pointer of queue name.
 *
 * @return  0:success.
 */
int aos_queue_unlink(const char *name);

/**
 * Get number of queued messages in a message queue.
 *
 * @param[in]  queue  message queue handle to operate.
 *
 * @return  number of queued messages.negative indicates error code.
 */
int aos_queue_get_count(aos_queue_t *queue);

/**
 * This function will create a timer and run auto.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 *
 * @return  0: success.
 */
int aos_timer_new(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, uint8_t repeat);

/**
 * This function will create a timer.
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
int aos_timer_new_ext(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint8_t repeat, uint8_t  autorun);

/**
 * This function will delete a timer.
 *
 * @param[in]  timer  pointer to a timer.
 */
void aos_timer_free(aos_hdl_t *timer);

/**
 * This function will start a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_start(aos_hdl_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_stop(aos_hdl_t *timer);

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
int aos_timer_change(aos_hdl_t *timer, uint32_t ms);

/**
 * Get the timer's time vaule.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[out] value  store the returned time.
 *
 * @return  0: success, otherwise error.
 */
int aos_timer_gettime(aos_timer_t *timer, uint64_t value[]);

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
 * Get utc offsest time.
 * @return current utc offset time (unit:ms).
 */
uint64_t aos_utc_offset_get(void);

/**
 * Set utc offsest time.
 * @param[in] current utc offset time (unit:ms).
 */
void aos_utc_offset_set(uint64_t offset_ms);

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
 * @return the string of timer. Format is MM-dd hh:mm:ss.ms. e.g. 01-01 00:00:10.000.
 *
 */
char *aos_now_time_str(char *buffer, size_t len);

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
 */
void aos_srand(uint32_t seed);

/**
 * rand function.
 *
 * @return  random value.
 */
int aos_rand(void);

/**
 * Start system.
 */
void aos_start(void);

/**
 * system abort.
 */
void aos_abort(void);

/**
 * Exit system.
 */
void aos_exit(int status);

/**
 * Control pthread.
 */
int aos_prctl(int option, ...);

/**
 * get aos name.
 */
int aos_sysname(char *name, size_t length);

/**
 * Get process ID.
 *
 * @return The process ID
 */
uint32_t aos_getpid(void);

/**
 * Get the current task ID.
 *
 * @return The current task ID
 */
uint32_t aos_gettid(void);

/**
 * do futex operation.
 */
int aos_do_futex(uint32_t *uaddr, int futex_op, uint32_t val, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_H */

