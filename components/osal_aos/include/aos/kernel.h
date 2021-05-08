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
 * 提供AliOS Things内核功能的基础API.
 *
 * @{
 */

/**
 * @addtogroup aos_kernel_sysctrl
 * 提供AliOS Things系统控制功能的基础API.
 *
 * @{
 */

/* Define the AliOS-Things' Version */
#define SYSINFO_KERNEL_VERSION "AOS-R-3.3.0" /**< AliOS Things内核版本信息 */

/* Defined for API with delay time */
#define AOS_WAIT_FOREVER 0xffffffffu /**< 阻塞性等待，即一直等待，直到事件发生或资源获得才返回 */
#define AOS_NO_WAIT      0x0         /**< 非阻塞性等待，即若无事件发生或无资源可获得，则返回 */

/* Define default aos task priority*/
#ifndef AOS_DEFAULT_APP_PRI
#define AOS_DEFAULT_APP_PRI 32       /**< 任务默认优先级，当调用aos_task_new()创建任务时被内核用来设置任务优先级 */
#endif

/** @} */

/**
 * @addtogroup aos_kernel_event
 * 提供AliOS Things系统内核事件功能的基础API.
 *
 * @{
 */
#define AOS_EVENT_AND              0x02u /**< 期望事件标志位均为1时，即等待所有事件均发生时，任务解除阻塞 */
#define AOS_EVENT_AND_CLEAR        0x03u /**< 期望事件标志位均为1时，即等待所有事件均发生时，任务解除阻塞且将事件标志位清零 */
#define AOS_EVENT_OR               0x00u /**< 期望任意事件标志位为1时，即等待的任意事件发生，任务解除阻塞 */
#define AOS_EVENT_OR_CLEAR         0x01u /**< 期望任意事件标志位为1时，即等待的任意事件发生，任务解除阻塞且将事件标志位清零 */
/** @} */

/**
 * @addtogroup aos_kernel_task
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */
#define AOS_TASK_NONE              0x0u  /**< 表示不指定任意选项，当调用aos_task_create()创建任务时，用来指定option参数 */
#define AOS_TASK_AUTORUN           0x01u /**< 表示任务创建后即可被调度执行，当调用aos_task_create()创建任务时，用来指定option参数*/
/** @} */

/**
 * @addtogroup aos_kernel_timer
 * 提供AliOS Things系统内核定时器功能的基础API.
 *
 * @{
 */
#define AOS_TIMER_NONE             0x0u  /**< 表示不指定任意选项，当调用aos_timer_create()创建定时器时，用来指定option参数 */
#define AOS_TIMER_AUTORUN          0x01u /**< 表示定时器创建后即启动，当调用aos_timer_create()创建定时器时，用来指定option参数*/
#define AOS_TIMER_REPEAT           0x02u /**< 表示定时器是周期性的，当调用aos_timer_create()创建定时器时，用来指定option参数 */
/** @} */

/**
 * @addtogroup aos_kernel_sysctrl
 * 提供AliOS Things系统控制功能的基础API.
 *
 * @{
 */

/* Define the handle for all aos module     */
typedef void * aos_hdl_t;   /**< AOS内核对象句柄通用类型 */
/** @} */

/**
 * @addtogroup aos_kernel_task
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */
/* Define the main handle for task module   */
typedef aos_hdl_t aos_task_t; /**< AOS任务对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_mutex
 * 提供AliOS Things系统内核互斥量功能的基础API.
 *
 * @{
 */
/* Define the main handle for mutex module  */
typedef aos_hdl_t aos_mutex_t;  /**< AOS互斥量对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_sem
 * 提供AliOS Things系统内核信号量功能的基础API.
 *
 * @{
 */
/* Define the main handle for sem module    */
typedef aos_hdl_t aos_sem_t;  /**< AOS信号量对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_event
 * 提供AliOS Things系统内核事件功能的基础API.
 *
 * @{
 */
/* Define the main handle for queue module  */
typedef aos_hdl_t aos_event_t; /**< AOS信号量对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_queue
 * 提供AliOS Things系统内核消息队列功能的基础API.
 *
 * @{
 */
/* Define the main handle for event module  */
typedef aos_hdl_t aos_queue_t; /**< AOS消息队列对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_timer
 * 提供AliOS Things系统内核定时器功能的基础API.
 *
 * @{
 */
/* Define the main handle for timer module  */
typedef aos_hdl_t aos_timer_t; /**< AOS定时器对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_task
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */
/* Define the data type for task key index  */
typedef uint32_t aos_task_key_t; /**< AOS任务私有数据对象句柄类型 */
/** @} */

/**
 * @addtogroup aos_kernel_sysctrl
 * 提供AliOS Things系统控制功能的基础API.
 *
 * @{
 */
/* Define the data type for function return */
typedef int32_t aos_status_t; /**< AOS返回值状态类型 */
/** @} */

/**
 * @addtogroup aos_kernel_spinlock
 * 提供AliOS Things系统内核自旋锁功能的基础API.
 *
 * @{
 */
/* Define the main handle for spinlock module  */
typedef struct {
    volatile uint32_t owner;  /* cpu index of owner */
} aos_spinlock_t;

/* Define the data type for spinlock irq save value */
typedef long  aos_irqsave_t;
/** @} */

/**
 * @defgroup aos_kernel_task 任务管理
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */

/**
 * 创建任务，该接口为创建任务分配TCB（任务控制块）并且根据指定的执行体、任务名称、栈大小来初始化对应成员.
 * 该接口任务栈是由内核分配的。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务执行体入口函数为NULL，或任务名为NULL，或任务句柄为NULL，则返回错误码-EINVAL \n
 * 如果栈大小为零，则返回错误 \n
 * 如果任务优先级超过配置上限或等于IDLE任务优先级，则返回错误码-EINVAL \n
 *
 * @param[in]  task        任务对象句柄.
 * @param[in]  name        任务名称.若任务名称为空，则使用默认任务名“default_name”。
 * @param[in]  fn          任务执行体入口函数。
 * @param[in]  arg         任务执行体入口函数的参数。
 * @param[in]  stack_buf   栈空间地址，如果地址为空则内核根据stack_size为任务分配栈空间.
 * @param[in]  stack_size  栈大小（字节为单位）。
 * @param[in]  prio        任务优先级，最大指由配置参数RHINO_CONFIG_USER_PRI_MAX(默认为60)决定.
 * @param[in]  options     任务创建选项,当前支持选项：\n
 *                         @ref AOS_TASK_AUTORUN 任务创建后自动加入就绪队列，可被调度器调度执行. \n
 *
 * @return  状态码
 * @retval 0 创建任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -ENOMEM 内存不足导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg,void *stack_buf, size_t stack_size, int32_t prio, uint32_t options);

/**
 * 挂起任务，该接口将已创建的任务挂起，暂时不执行，挂起的对象既可以是任务自身也可以是其他任务，\n
 * 但不允许挂起IDLE任务。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 * 如果挂起任务为IDLE，则返回错误码-EPERM \n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 挂起任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -EPERM 尝试挂起IDLE任务导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_suspend(aos_task_t *task);

/**
 * 恢复任务，该接口将挂起任务恢复，取消暂时不执行状态。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_resume(aos_task_t *task);

/**
 * 任务退出，该接口功能是任务删除自身，且IDLE任务不允许删除。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果挂起任务为IDLE，则直接返回 \n
 *
 * @param[in]  code        未使用.
 *
 * @return  无
 *
 */
void aos_task_exit(int32_t code);

/**
 * 删除任务，该接口删除一个任务并回收任务资源，不允许删除IDLE任务。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 *  如果删除的任务为IDLE，则返回错误码-EPERM \n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -EPERM 尝试删除IDLE任务导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_delete(aos_task_t *task);

/**
 * 当前任务让出CPU资源，该接口将当前任务唤出，放入就绪队列对尾，暂时放弃CPU的使用权。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 *
 * @param[in]  无.
 *
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_yield(void);

 /**
 * 获取任务名称，该接口将指定任务的任务名称拷贝到用户缓冲区。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 * 如果用户缓冲区地址参数为NULL，则返回错误码-EINVAL\n
 * 如果用户缓冲区大小为0，则返回错误码-EINVAL\n
 *
 * @param[in]   task        任务对象句柄
 * @param[out]  buf         输出任务名的用户缓冲区地址
 * @param[in]   buf_size    输出任务名的用户缓冲区大小
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size);

/**
 * 获取当前任务的任务对象句柄。
 *
 * @par 使用约束
 * 无。
 *
 * @par 错误处理
 * 无。
 *
 * @param[in]  无。
 *
 * @return  任务对象句柄
 *
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

/** @} */

/**
 * @defgroup aos_kernel_mutex 互斥量
 * 提供AliOS Things系统内核互斥量功能的基础API.
 *
 * @{
 */

/**
 * 创建互斥量，该接口在内核中创建一个互斥量对象，并返回该对象的句柄。
 *
 * @par 错误处理
 * 如果互斥量对象句柄为NULL，则返回-EINVAL。
 * 如果在创建的过程中内存不足，则返回-ENOMEM。
 *
 * @param[out]  mutex    互斥量对象句柄，该接口会创建一个互斥量对象，成功后把对象的地址通过此参数返回。
 * @param[in]   options  创建互斥量的选项，目前还没有支持的选项，待后续扩展。
 *
 * @return  状态码
 * @retval 0 创建互斥量成功
 * @retval -EINVAL 传入的参数非法
 * @retval -ENOMEM 内存不足
 */
aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options);

/**
 * 销毁互斥量，该接口释放互斥量对象的资源，唤醒所有阻塞在该互斥量的任务。
 *
 * @par 错误处理
 * 如果互斥量对象句柄参数为NULL， 或者互斥量对象非法（没有成功创建或者对象类型不是互斥量），则返回-EINVAL。
 *
 * @param[in]  mutex  互斥量对象句柄
 *
 * @return 无
 */
void aos_mutex_free(aos_mutex_t *mutex);

/**
 * 锁定互斥量，该接口申请获得一把互斥量锁，常被用于任务之间保护共享资源。
 * 如果该互斥量锁当前没有其他任务持有，则当前任务能够立即获取这把锁并成功返回。
 * 如果该互斥量锁当前被其他任务持有，同时指定AOS_NO_WAIT，则不等待立即返回错误-1。
 * 如果该互斥量锁当前被其他任务持有，同时指定AOS_WAIT_FOREVER，则永远等待直到获得该互斥量锁。
 * 等待时当前任务处于阻塞状态，等待该互斥量锁。
 *
 * @note 注意内核中允许互斥量嵌套，如果任务再次获得自身持有的互斥量锁，则返回成功。
 *
 * @par 错误处理
 * 如果互斥量对象句柄参数为NULL， 或者互斥量对象非法（没有成功创建或者对象类型不是互斥量），则返回-EINVAL。
 *
 * @param[in]  mutex    互斥量对象句柄。
 * @param[in]  timeout  超时时间（单位：ms）@ref AOS_WAIT_FOREVER: 永远等待。 @ref AOS_NO_WAIT: 不等待。
 *
 * @return  状态码
 * @retval 0 返回成功，此时当前任务获得这把互斥量锁。
 * @retval -EINVAL 参数非法
 * @retval -ETIMEDOUT 等待超时
 * @retval -1 其他操作
 */
aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout);

/**
 * 解锁互斥量，该接口释放自身持有的互斥量锁。 如果此时有其他任务阻塞在该互斥量锁上，
 * 则从阻塞任务队列中挑选一个优先级最高的任务唤醒，使其继续。
 *
 * @note 任务只能释放自身持有的互斥量锁，否则返回错误-EPERM。
 * 内核中允许互斥量锁嵌套，如果进行过多次的互斥量锁定操作，注意需要进行相同次数的解锁操作，否则其他竞争的任务会一直阻塞。
 *
 * @par 错误处理
 * 如果互斥量对象句柄参数为NULL， 或者互斥量对象非法（没有成功创建或者对象类型不是互斥量），则返回-EINVAL。
 *
 * @param[in]  mutex  互斥量对象句柄。
 *
 * @return  状态码
 * @retval 0 返回成功，
 * @retval -EINVAL 参数非法
 * @retval -EPERM 权限不够（释放其他任务持有的互斥量锁）
 */
aos_status_t aos_mutex_unlock(aos_mutex_t *mutex);

/** @} */

/**
 * @defgroup aos_kernel_sem 信号量
 * 提供AliOS Things系统内核信号量功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @defgroup aos_kernel_event 事件
 * 提供AliOS Things系统内核事件功能的基础API.
 *
 * @{
 */

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

/** @} */


/**
 * @defgroup aos_kernel_queue 消息队列
 * 提供AliOS Things系统内核消息队列功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @defgroup aos_kernel_timer 定时器
 * 提供AliOS Things系统内核定时器功能的基础API.
 *
 * @{
 */

/**
 * This function will create a timer.
 *
 * @param[in]  timer     pointer to the timer.
 * @param[in]  fn        callbak of the timer.
 * @param[in]  arg       the argument of the callback.
 * @param[in]  ms        ms of the normal timer triger.
 * @param[in]  options     option settings, can be set to AOS_TIMER_AUTORUN|AOS_TIMER_REPEAT
 * @note fn first arg: timer->hdl, not aos_timer_t *timer; second arg: user param arg.
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

/** @} */

/**
 * @defgroup aos_kernel_spinlock 自旋锁
 * 提供AliOS Things系统内核自旋锁功能的基础API.
 *
 * @{
 */

/**
 * Init a spinlock.
 *
 * @param[in]  spinlock  pointer of spinlock object,spinlock object must be
 *                       alloced.
 */
void aos_spin_lock_init(aos_spinlock_t *spinlock);

/**
 * Lock a spinlock.
 *
 * @param[in]  spinlock  spinlock object.
 */
void aos_spin_lock(aos_spinlock_t *spinlock);

/**
 * Unlock a spinlock.
 *
 * @param[in]  spinlock  spinlock object.
 */
void aos_spin_unlock(aos_spinlock_t *spinlock);

/**
 * Lock a spinlock in ISR.
 *
 * @param[in]  spinlock  spinlock object.
 *
 * @return    CPSR value.
 */
aos_irqsave_t aos_spin_lock_irqsave(aos_spinlock_t *spinlock);

/**
 * Unlock a spinlock in ISR.
 *
 * @param[in]  spinlock  spinlock object.
 * @param[in]  flag     CPSR value.
 *
 */
void aos_spin_unlock_irqrestore(aos_spinlock_t *spinlock, aos_irqsave_t flag);

/** @} */

/**
 * @defgroup aos_kernel_memory 内存管理
 * 提供AliOS Things系统内核内存管理功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @defgroup aos_kernel_time 时间管理
 * 提供AliOS Things系统内核时间管理功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @defgroup aos_kernel_sysctrl 系统管理
 * 提供AliOS Things系统控制功能的基础API.
 *
 * @{
 */

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

/** @} */


/* The following APIs is marked deprecated and is not recommended */

/**
 * @addtogroup aos_kernel_task
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @addtogroup aos_kernel_mutex
 * 提供AliOS Things系统内核互斥量功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @addtogroup aos_kernel_sem
 * 提供AliOS Things系统内核信号量功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @addtogroup aos_kernel_event
 * 提供AliOS Things系统内核事件功能的基础API.
 *
 * @{
 */

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

/** @} */

/**
 * @addtogroup aos_kernel_timer
 * 提供AliOS Things系统内核定时器功能的基础API.
 *
 * @{
 */

/**
 * This function will create a timer and run auto.
 * Deprecated, not Recommended.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 * @note fn first arg: timer->hdl, not aos_timer_t *timer; second arg: user param arg.
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
 * @note fn first arg: timer->hdl, not aos_timer_t *timer; second arg: user param arg.
 * @return  0: success.
 */
aos_status_t aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, bool repeat, bool  autorun);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_H */
