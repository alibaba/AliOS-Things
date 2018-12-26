/**
 * @file    wm_osal.h
 *
 * @brief   WM OS adapter layer
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_OSAL_H
#define WM_OSAL_H

#include <stdio.h>
#include <stdlib.h>
#include "wm_config.h"
#include "wm_type_def.h"

#include <aos/kernel.h>
#include <k_api.h>

/**    OS TICK Frequency   */
#define HZ        (100)

/** Type definition of os_time_t */
typedef long os_time_t;
/** Structure definition of os_time */
struct os_time {
	os_time_t sec;
	os_time_t usec;
};
/** ENUMERATION of OS */
enum TLS_OS_TYPE{
	OS_UCOSII = 0,
	OS_FREERTOS = 1,
	OS_MAX_NUM
};

/** TYPE definition of OS_STK */
#ifdef OS_STK
#undef OS_STK
#endif
typedef unsigned int OS_STK;

/** TYPE definition of OS_CPU_SR */
#ifdef OS_CPU_SR
#undef OS_CPU_SR
#endif
#define TLS_USE_AOS_TIMER 1
#define TLS_USE_AOS_QUEUE 1
typedef unsigned int OS_CPU_SR;
/** TYPE definition of tls_os_task_t */
typedef void * 	tls_os_task_t;
/** TYPE definition of tls_os_timer_t */
#if TLS_USE_AOS_TIMER
typedef aos_timer_t      tls_os_timer_t;
#else
typedef ktimer_t      tls_os_timer_t;
#endif
/** TYPE definition of tls_os_sem_t */
typedef aos_sem_t	tls_os_sem_t;
/** TYPE definition of tls_os_queue_t */
#if TLS_USE_AOS_QUEUE
typedef aos_queue_t	tls_os_queue_t;
#else
typedef kqueue_t	tls_os_queue_t;
#endif
/** TYPE definition of tls_os_mailbox_t */
typedef aos_queue_t	tls_os_mailbox_t;
/** TYPE definition of tls_os_mutex_t */
typedef void    tls_os_mutex_t;
/** TYPE definition of TLS_OS_TIMER_CALLBACK */
typedef  void (*TLS_OS_TIMER_CALLBACK)(void *ptmr, void *parg);

/** MACRO definition of TIMER ONE times */
#define TLS_OS_TIMER_OPT_ONE_SHORT    1u
/** MACRO definition of TIMER PERIOD */
#define TLS_OS_TIMER_OPT_PERIOD       2u

/** ENUMERATION definition of OS STATUS */
typedef enum tls_os_status {
    TLS_OS_SUCCESS = 0,
    TLS_OS_ERROR,
    TLS_OS_ERR_TIMEOUT,
} tls_os_status_t;

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup OS_APIs OS APIs
 * @brief Operate system APIs
 */

/**
 * @addtogroup OS_APIs
 * @{
 */

/**
 * @brief          This function is used to register OS tick timer irq
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_os_timer_init(void);

/**
 * @brief          This function is  used to tick handler
 *
 * @param[in]      *p    argument
 *
 * @return         None
 *
 * @note           None
 */
void tls_os_time_tick(void *p);

/**
 * @brief          This function is used to initialize OS common resource
 *
 * @param[in]      *arg
 *
 * @return         None
 *
 * @note           None
 */
void tls_os_init(void *arg);

/**
 * @brief          This function is used to start task scheduler
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_os_start_scheduler(void);

/**
 * @brief          This function is used to get OS type
 *
 * @param[in]      None
 *
 * @retval         0     OS_UCOSII
 * @retval         1	 OS_FREERTOS
 *
 * @note           None
 */
int tls_os_get_type(void);

/**
 * @brief          This function is used to have uC/OS-II manage the execut-
                   ion of a task.  Tasks can either be created prior to
                   the start of multitasking or by a running task.
                   A task cannot be created by an ISR.
 *
 * @param[in]      *task      pointer to the task
 * @param[in]      name       the task's name
 * @param[in]      entry      the task's entry function
 * @param[in]      param      pointer to an optional data area which can be
 							  used to pass parameters to the task when the
 							  task first executes.  Where the task is
 							  concerned it thinks it was invoked and passed
 							  the argument 'param' as follows:
                              void Task (void *param)
                              {
                                  for (;;) {
                                      Task code;
                                  }
                              }
 * @param[in]      *stk_start pointer to the task's bottom of stack.
 * @param[in]      stk_size   the size of the stack in number of elements.
 							  If OS_STK is set to INT8U,
 							  'stk_size' corresponds to the number of bytes
 							  available.
 							  If OS_STK is set to INT16U, 'stk_size' contains
 							  the number of 16-bit entries available.
 							  Finally, if OS_STK is set to INT32U,
 							  'stk_size' contains the number of 32-bit entries
 							  available on the stack.
 * @param[in]      prio       the task's priority.  A unique priority MUST be
 							  assigned to each task and the lower the number,
 							  the higher the priority.
 * @param[in]      flag       contains additional information about the behavior of the task
 *
 * @retval         TLS_OS_SUCCESS     the call was successful.
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
tls_os_status_t tls_os_task_create(tls_os_task_t *task,
      const char* name,
      void (*entry)(void* param),
      void* param,
      u8 *stk_start,
      u32 stk_size,
      u32 prio,
      u32 flag);

/**
 * @brief          This function allows you to delete a task.  The calling
                   task can delete itself by its own priority number.
                   The deleted task is returned to the dormant state
                   and can be re-activated by creating the deleted task
                   again.
 *
 * @param[in]      prio                task priority to delete
 * @param[in]      (*freefun)(void)    function to free resource
 *
 * @retval         TLS_OS_SUCCESS     the call is successful
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
tls_os_status_t tls_os_task_del(u8 prio, void (*freefun)(void));


/**
 * @brief          This function creates a mutual exclusion semaphore
 *
 * @param[in]      prio      the priority to use when accessing the mutual
 							 exclusion semaphore.  In other words, when the
 							 semaphore is acquired and a higher priority task
 							 attempts to obtain the semaphore then the
 							 priority of the task owning the semaphore is
 							 raised to this priority.  It is assumed that
 							 you will specify a priority that is LOWER in
 							 value than ANY of the tasks competing for the
 							 mutex.
 * @param[in]      **mutex   pointer to the event control clock (OS_EVENT)
 							 associated with the created mutex.
 *
 * @retval         TLS_OS_SUCCESS     the call was successful
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           1) The LEAST significant 8 bits of '.OSEventCnt' are used
 					  to hold the priority number of the task owning the mutex
 					  or 0xFF if no task owns the mutex.
	               2) The MOST  significant 8 bits of '.OSEventCnt' are used to
	               	  hold the priority number to use to reduce priority
	               	  inversion.
 */
tls_os_status_t tls_os_mutex_create(u8 prio, tls_os_mutex_t **mutex);

/**
 * @brief          This function deletes a mutual exclusion semaphore and
                   readies all tasks pending on the it
 *
 * @param[in]      *mutex    pointer to the event control block associated
 							 with the desired mutex
 *
 * @retval         TLS_OS_SUCCESS     The call was successful and the mutex
 									  was deleted
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           1) This function must be used with care.  Tasks that would
 					  normally expect the presence of the mutex MUST check the
 					  return code of OSMutexPend().
				   2) This call can potentially disable interrupts for a long
				      time.  The interrupt disable time is directly
				      proportional to the number of tasks waiting on the mutex.
                   3) Because ALL tasks pending on the mutex will be readied,
                      you MUST be careful because the resource(s) will no
                      longer be guarded by the mutex.
                   4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that
                      the owner of the Mutex (if there is one) is ready-to-run
                      and is thus NOT pending on another kernel object or has
                      delayed itself.In other words, if a task owns the mutex
                      being deleted,that task will be made ready-to-run at
                      its original priority.
 */
 tls_os_status_t tls_os_mutex_delete(tls_os_mutex_t *mutex);

/**
 * @brief          This function waits for a mutual exclusion semaphore
 *
 * @param[in]      *mutex     pointer to the event control block
 							  associated with the desired mutex
 * @param[in]      wait_time  an optional timeout period (in clock ticks).
 							  If non-zero, your task will wait for the resource
 							  up to the amount of time specified by
 							  this argument.
 							  If you specify 0, however, your task will wait
 							  forever at the specified mutex or,
 							  until the resource becomes available.
 *
 * @retval         TLS_OS_SUCCESS     The call was successful and your task
 									  owns the mutex
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           1) The task that owns the Mutex MUST NOT pend on
 					  any other event while it owns the mutex.
 				   2) You MUST NOT change the priority of the task
 				   	  that owns the mutex
 */
 tls_os_status_t tls_os_mutex_acquire(tls_os_mutex_t *mutex,
        u32 wait_time);

/**
 * @brief          This function signals a mutual exclusion semaphore
 *
 * @param[in]      *mutex    pointer to the event control block
 							 associated with the desired mutex
 *
 * @retval         TLS_OS_SUCCESS     The call was successful and the mutex was signaled.
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_mutex_release(tls_os_mutex_t *mutex);

/**
 * @brief          This function creates a semaphore
 *
 * @param[in]      **sem    pointer to the event control block (OS_EVENT)
 							associated with the created semaphore
 * @param[in]      cnt      the initial value for the semaphore.
 							If the value is 0, no resource is available
 							(or no event has occurred).
 							You initialize the semaphore to a non-zero value
 							to specify how many resources are available
 							(e.g. if you have 10 resources, you would
 							initialize the semaphore to 10).
 *
 * @retval         TLS_OS_SUCCESS     success,The call was successful
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
tls_os_status_t tls_os_sem_create(tls_os_sem_t **sem, u32 cnt);


/**
 * @brief          This function deletes a semaphore and readies all tasks
                   pending on the semaphore.
 *
 * @param[in]      *sem    pointer to the event control block associated
 						   with the desired semaphore
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
tls_os_status_t tls_os_sem_delete(tls_os_sem_t *sem);

/**
 * @brief          This function waits for a semaphore
 *
 * @param[in]      *sem         pointer to the event control block
 								associated with the desired semaphore
 * @param[in]      wait_time    an optional timeout period (in clock ticks).
 								If non-zero, your task will wait for the
 								resource up to the amount of time specified
 								by this argument.If you specify 0, however,
 								your task will wait forever at the specified
 								semaphore or, until the resource becomes
 								available (or the event occurs).
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR	      failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_sem_acquire(tls_os_sem_t *sem,
        u32 wait_time);

/**
 * @brief          This function signals a semaphore
 *
 * @param[in]      *sem    pointer to the event control block associated
 						   with the desired semaphore
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_sem_release(tls_os_sem_t *sem);

/**
 * @brief          This function sets the semaphore count to the value spec-
                   ified as an argument.Typically,this value would be 0.You
                   would typically use this function when a semaphore is
                   used as a signaling mechanism and, you want to reset
                   the count value.
 *
 * @param[in]      *sem    pointer to the event control block
 * @param[in]      cnt     the new value for the semaphore count.  You would
 						   pass 0 to reset the semaphore count.
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_sem_set(tls_os_sem_t *sem, u16 cnt);


/**
 * @brief          This function creates a message queue if free event cont-
                   rol blocks are available
 *
 * @param[in]      **queue       pointer to the event control clock (OS_EVENT)
 								 associated with the created queue
 * @param[in]      queue_size    the number of elements in the storage area
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_queue_create(tls_os_queue_t **queue, u32 queue_size);

/**
 * @brief          This function deletes a message queue and readies all
                   tasks pending on the queue
 *
 * @param[in]      *queue    pointer to the event control block associated
 							 with the desired queue
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_queue_delete(tls_os_queue_t *queue);

/**
 * @brief          This function sends a message to a queue
 *
 * @param[in]      *queue     pointer to the event control block
 							  associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.
 * @param[in]      msg_size   message size
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_queue_send(tls_os_queue_t *queue,
        void *msg,
        u32 msg_size);

/**
 * @brief          This function is used to flush the contents of the message
                   queue.
 *
 * @param[in]      *queue
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
tls_os_status_t tls_os_queue_flush(tls_os_queue_t *queue);


/**
 * @brief          This function waits for a message to be sent to a queue
 *
 * @param[in]      *queue       pointer to the event control block associated
 								with the desired queue
 * @param[in]      **msg        pointer to the message received
 * @param[in]      msg_size     message size
 * @param[in]      wait_time    an optional timeout period (in clock ticks).
 								If non-zero, your task will wait for a message
 								to arrive at the queue up to the amount of time
 								specified by this argument.  If you specify 0,
 								however, your task will wait forever at the
 								specified queue or, until a message arrives.
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR		  failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_queue_receive(tls_os_queue_t *queue,void **msg,
        u32 msg_size,
        u32 wait_time);

/**
 * @brief          This function creates a message mailbox if free event
                   control blocks are available.
 *
 * @param[in]      **mailbox     pointer to the event control clock (OS_EVENT)
 							     associated with the created mailbox
 * @param[in]      mailbox_size  size
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_mailbox_create(tls_os_mailbox_t **mailbox, u32 mailbox_size);

/**
 * @brief          This function deletes a mailbox and readies all tasks
                   pending on the mailbox.
 *
 * @param[in]      *mailbox    pointer to the event control block
 							   associated with the desired mailbox.
 *
 * @retval         TLS_OS_SUCCESS    success
 * @retval         TLS_OS_ERROR      failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_mailbox_delete(tls_os_mailbox_t *mailbox);

/**
 * @brief          This function sends a message to a mailbox
 *
 * @param[in]      *mailbox    pointer to the event control block associated
 							   with the desired mailbox
 * @param[in]      *msg        pointer to the message to send.
 							   You MUST NOT send a NULL pointer
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR		  failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_mailbox_send(tls_os_mailbox_t *mailbox,void *msg);

/**
 * @brief          This function waits for a message to be sent to a mailbox

 *
 * @param[in]      *mailbox    pointer to the event control block associated
 							   with the desired mailbox
 * @param[in]      **msg       pointer to the message received
 * @param[in]      wait_time   an optional timeout period (in clock ticks).
 							   If non-zero, your task will wait for a message
 							   to arrive at the mailbox up to the amount of
 							   time specified by this argument.
 							   If you specify 0, however, your task will wait
 							   forever at the specified mailbox or,
 							   until a message arrives.
*
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR		  failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_mailbox_receive(tls_os_mailbox_t *mailbox,
        void **msg,
        u32 wait_time);

/**
 * @brief          This function is used by your application to obtain the
                   current value of the 32-bit counter which keeps track
                   of the number of clock ticks
 *
 * @param[in]      None
 *
 * @retval         current value of OSTime
 *
 * @note           None
 */
 u32 tls_os_get_time(void);

/**
 * @brief          This function is used to disable interrupts by preserving
                   the state of interrupts
 *
 * @param[in]      None
 *
 * @retval         cpu_sr
 *
 * @note           None
 */
 u32 tls_os_set_critical(void);

/**
 * @brief          This function is used to enable interrupts by preserving
                   the state of interrupts
 *
 * @param[in]      cpu_sr
 *
 * @return         None
 *
 * @note           None
 */
 void tls_os_release_critical(u32 cpu_sr);


/**
 * @brief          This function is called by your application code to create
                   a timer
 *
 * @param[in]      **timer     pointer to an OS_TMR data structure.
 							   This is the 'handle' that your application will
 							   use to reference the timer created
 * @param[in]      callback    pointer to a callback function that will
 							   be called when the timer expires.  The callback
 							   function must be declared as follows
 							   void MyCallback (OS_TMR *ptmr, void *p_arg);
 * @param[in]      *callback_arg   argument (a pointer) that is passed to
 								   the callback function when it is called
 * @param[in]      period      The 'period' being repeated for the timer.
                               If you specified 'OS_TMR_OPT_PERIODIC' as
                               an option, when the timer expires, it will
                               automatically restart with the same period.
 * @param[in]      repeat      if repeat
 * @param[in]      *name       pointer to an ASCII string that is used to
 							   name the timer.  Names are useful for
	                           debugging.
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR		  failed
 *
 * @note           None
 */
 tls_os_status_t tls_os_timer_create(tls_os_timer_t **timer,
        TLS_OS_TIMER_CALLBACK callback,
        void *callback_arg,
        u32 period,
        bool repeat,
        u8 *name);

/**
 * @brief          This function is called by your application code to start
                   a timer.
 *
 * @param[in]      *timer    pointer to an OS_TMR
 *
 * @return         None
 *
 * @note           None
 */
 void tls_os_timer_start(tls_os_timer_t *timer);

/*
************************************************************************************************************************
*                                                   CHANGE A TIMER WAIT TIME
*
* Description: This function is called by your application code to change a timer wait time.
*
* Arguments  : timer          Is a pointer to an OS_TMR
*
*			ticks			is the wait time
************************************************************************************************************************
*/
 void tls_os_timer_change(tls_os_timer_t *timer, u32 ticks);

/**
 * @brief          This function is called by your application code to stop
                   a timer
 *
 * @param[in]      *timer   pointer to the timer to stop.
 *
 * @return         None
 *
 * @note           None
 */
 void tls_os_timer_stop(tls_os_timer_t *timer);

/**
 * @brief          This function is called by your application code to delete
                   a timer
 *
 * @param[in]      *timer    pointer to the timer to delete
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR		  failed
 *
 * @note           None
 */
int tls_os_timer_delete(tls_os_timer_t *timer);

/**
 * @brief          This function is called to delay execution of the curren-
                   tly running task until the specified number of system
                   ticks expires. This, of course, directly equates to
                   delaying the current task for some time to expire.
                   No delay will result If the specified delay is 0.
                   If the specified delay is greater than 0 then,
                   a context switch will result.
 *
 * @param[in]      ticks   the time delay that the task will be suspended
 				   in number of clock 'ticks'.Note that by specifying 0,
 				   the task will not be delayed.
 *
 * @return         None
 *
 * @note           None
 */
 void tls_os_time_delay(u32 ticks);


/**
 * @brief          This function is used to display stat info
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_os_disp_task_stat_info(void);

/**
 * @}
 */

/**
 * @}
 */

#endif /* end of WM_OSAL_H */

