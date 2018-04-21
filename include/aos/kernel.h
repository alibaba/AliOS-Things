/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_KERNEL_H
#define AOS_KERNEL_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_WAIT_FOREVER    0xffffffffu
#define AOS_NO_WAIT         0x0
#define AOS_DEFAULT_APP_PRI 32

typedef struct {
    void *hdl;
} aos_hdl_t;

typedef aos_hdl_t aos_task_t;
typedef aos_hdl_t aos_mutex_t;
typedef aos_hdl_t aos_sem_t;
typedef aos_hdl_t aos_queue_t;
typedef aos_hdl_t aos_timer_t;
typedef aos_hdl_t aos_work_t;
typedef aos_hdl_t aos_event_t;

typedef struct {
    void *hdl;
    void *stk;
} aos_workqueue_t;

typedef unsigned int aos_task_key_t;

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
 * @return  SYSINFO_KERNEL_VERSION.
 */
const char *aos_version_get(void);

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
int aos_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size);

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
 *
 * @return  0: success.
 */
int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio);

/**
 * Exit a task.
 *
 * @param[in]  code  not used now.
 */
void aos_task_exit(int code);

/**
 * Get task name.
 *
 * @return  the name of the task
 */
const char *aos_task_name(void);

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
 * Alloc a mutex.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in aos_mutex_t will refer a kernel obj internally.
 *
 * @return  0: success.
 */
int aos_mutex_new(aos_mutex_t *mutex);

/**
 * Free a mutex.
 *
 * @param[in]  mutex  mutex object, mem refered by hdl pointer in aos_mutex_t will
 *                    be freed internally.
 */
void aos_mutex_free(aos_mutex_t *mutex);

/**
 * Lock a mutex.
 *
 * @param[in]  mutex    mutex object, it contains kernel obj pointer which aos_mutex_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex  mutex object, it contains kernel obj pointer which oc_mutex_new alloced.
 *
 * @return  0: success.
 */
int aos_mutex_unlock(aos_mutex_t *mutex);

/**
 * This function will check if mutex is valid.
 *
 * @param[in]  mutex  pointer to the mutex.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_mutex_is_valid(aos_mutex_t *mutex);

/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be alloced,
 *                     hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0:success.
 */
int aos_sem_new(aos_sem_t *sem, int count);

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
 * @param[in]  sem      semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_sem_wait(aos_sem_t *sem, unsigned int timeout);

/**
 * Release a semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal(aos_sem_t *sem);

/**
 * This function will check if semaphore is valid.
 *
 * @param[in]  sem  pointer to the semaphore.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_sem_is_valid(aos_sem_t *sem);

/**
 * Release all semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal_all(aos_sem_t *sem);

/**
 * This function will create an event with an initialization flag set.
 * This function should not be called from interrupt context.
 *
 * @param[in]  event    event object pointer.
 * @param[in]  flags    initialization flag set(provided by caller).
 *
 * @return  0: success.
 */

int aos_event_new(aos_event_t *event, unsigned int flags);

/**
 * This function will free an event.
 * This function shoud not be called from interrupt context.
 *
 * @param[in]  event    memory refered by hdl pointer in event will be freed.
 *
 * @return  N/A.
 */

void aos_event_free(aos_event_t *event);

/**
 * This function will try to get flag set from given event, if the request flag
 * set is satisfied, it will return immediately, if the request flag set is not
 * satisfied with timeout(RHINO_WAIT_FOREVER,0xFFFFFFFF), the caller task will be
 * pended on event until the flag is satisfied, if the request flag is not 
 * satisfied with timeout(RHINO_NO_WAIT, 0x0), it will also return immediately.
 * Note, this function should not be called from interrupt context because it has
 * possible to lead context switch and an interrupt has no TCB to save context.
 *
 * @param[in]  event        event object pointer.
 * @param[in]  flags        request flag set.
 * @param[in]  opt          operation type, such as AND,OR,AND_CLEAR,OR_CLEAR.
 * @param[out] actl_flags   the internal flags value hold by event.
 * @param[in]  flags        request flag set.
 * @param[in]  timeout      max wait time in millisecond.
 *
 * @return  0: success.
 */

int aos_event_get(aos_event_t *event, unsigned int flags, unsigned char opt,
                       unsigned int *actl_flags, unsigned int timeout);

/**
* This function will set flag set to given event, and it will check if any task
* which is pending on the event should be waken up. 
*
* @param[in]  event    event object pointer.
* @param[in]  flags    flag set to be set into event.
* @param[in]  opt      operation type, such as AND,OR.
*
* @return  0: success.
*/

int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt);

/**
 * This function will create a queue.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  buf      buf of the queue(provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of the msg.
 *
 * @return  0: success.
 */

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg);

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
int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size);

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
int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, unsigned int *size);

/**
 * This function will check if queue is valid.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_queue_is_valid(aos_queue_t *queue);

/**
 * This function will return buf ptr if queue is inited.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  NULL: error.
 */
void *aos_queue_buf_ptr(aos_queue_t *queue);

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
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat);

/**
 * This function will create a timer.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 * @param[in]  auto_run  run auto or not when the timer is created.
 *
 * @return  0: success.
 */
int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat, unsigned char auto_run);

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
int aos_timer_start(aos_timer_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_stop(aos_timer_t *timer);

/**
 * This function will change attributes of a timer.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[in]  ms     ms of the timer triger.
 *
 * @return  0: success.
 */
int aos_timer_change(aos_timer_t *timer, int ms);

/**
 * This function will creat a workqueue.
 *
 * @param[in]  workqueue   the workqueue to be created.
 * @param[in]  pri         the priority of the worker.
 * @param[in]  stack_size  the size of the worker-stack.
 *
 * @return  0: success.
 */
int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size);

/**
 * This function will initialize a work.
 *
 * @param[in]  work  the work to be initialized.
 * @param[in]  fn    the call back function to run.
 * @param[in]  arg   the paraments of the function.
 * @param[in]  dly   ms to delay before run.
 *
 * @return  0: success.
 */
int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly);

/**
 * This function will destroy a work.
 *
 * @param[in]  work  the work to be destroied.
 */
void aos_work_destroy(aos_work_t *work);

/**
 * This function will run a work on a workqueue.
 *
 * @param[in]  workqueue  the workqueue to run work.
 * @param[in]  work       the work to run.
 *
 * @return  0: success.
 */
int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work);

/**
 * This function will run a work on the default workqueue.
 *
 * @param[in]  work  the work to run.
 *
 * @return  0: success.
 */
int aos_work_sched(aos_work_t *work);

/**
 * This function will cancel a work on the default workqueue.
 *
 * @param[in]  work  the work to cancel.
 *
 * @return  0: success.
 */
int aos_work_cancel(aos_work_t *work);

/**
 * Realloc memory.
 *
 * @param[in]  mem   current memory address point.
 * @param[in]  size  new size of the mem to remalloc.
 *
 * @return  NULL: error.
 */
void *aos_realloc(void *mem, unsigned int size);

/**
 * Alloc memory.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_malloc(unsigned int size);

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_zalloc(unsigned int size);

/**
 * Trace alloced mems.
 *
 * @param[in]  addr       pointer of the mem alloced by malloc.
 * @param[in]  allocator  buildin_address.
 */
void aos_alloc_trace(void *addr, size_t allocator);

/**
 * Free memory.
 *
 * @param[in]  ptr  address point of the mem.
 */
void aos_free(void *mem);

/**
 * Get current time in nano seconds.
 *
 * @return  elapsed time in nano seconds from system starting.
 */
long long aos_now(void);

/**
 * Get current time in mini seconds.
 *
 * @return  elapsed time in mini seconds from system starting.
 */
long long aos_now_ms(void);

/**
 * Msleep.
 *
 * @param[in]  ms  sleep time in milliseconds.
 */
void aos_msleep(int ms);

/**
 * Initialize system
 */
void aos_init(void);

/**
 * Start system
 */
void aos_start(void);

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_H */

