/*****************************************************************************
*
* File Name : wm_osal_rtos.h
*
* Description: rtos include Module
*
* Copyright (c) 2014 Winner Microelectronics Co., Ltd.
* All rights reserved.
*
* Author : dave
*
* Date : 2014-8-27
*****************************************************************************/

#ifndef WM_OS_RTOS_H
#define WM_OS_RTOS_H

#include "wm_config.h"


//#include "include.h"
#include <stdio.h>
#include <stdlib.h>
#include <k_api.h>
#include <errno.h>
#include <aos/kernel.h>
#include "wm_mem.h"
#include "wm_osal.h"
#include "wm_ram_config.h"
/*
*********************************************************************************************************
*                                     CREATE A TASK (Extended Version)
*
* Description: This function is used to have uC/OS-II manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.
*
* Arguments  : task      is a pointer to the task'
*
*			name 	is the task's name
*
*			entry	is the task's entry function
*
*              param     is a pointer to an optional data area which can be used to pass parameters to
*                        the task when the task first executes.  Where the task is concerned it thinks
*                        it was invoked and passed the argument 'param' as follows:
*
*                            void Task (void *param)
*                            {
*                                for (;;) {
*                                    Task code;
*                                }
*                            }
*
*              stk_start      is a pointer to the task's bottom of stack.
*
*              stk_size  is the size of the stack in number of elements.  If OS_STK is set to u8,
*                        'stk_size' corresponds to the number of bytes available.  If OS_STK is set to
*                        INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                        OS_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                        available on the stack.
*
*              prio      is the task's priority.  A unique priority MUST be assigned to each task and the
*                        lower the number, the higher the priority.
*
*              flag       contains additional information about the behavior of the task.
*
* Returns    : TLS_OS_SUCCESS             if the function was successful.
*              TLS_OS_ERROR
*********************************************************************************************************
*/

ktask_t *task_malloc;

tls_os_status_t tls_os_task_create(tls_os_task_t *task,
      const char* name,
      void (*entry)(void* param),
      void* param,
      u8 *stk_start,
      u32 stk_size,
      u32 prio,
      u32 flag)
{
    u8 error;
    tls_os_status_t os_status;

    if (((u32)stk_start >= TASK_STACK_USING_MEM_UPPER_RANGE)
		||(((u32)stk_start + stk_size) >= TASK_STACK_USING_MEM_UPPER_RANGE))
    {
        printf("\nCurrent Stack [0x%8x, 0x%8x) is NOT in VALID STACK range [0x20000000,0x20028000)\n",
               (unsigned int)stk_start, (unsigned int)stk_start + stk_size);
        printf("Please refer to APIs' manul and modify task stack position!!!\n");
    	return TLS_OS_ERROR;
    }

	if(task == NULL)
	{
		task_malloc = tls_mem_alloc(sizeof(ktask_t));
		if(task_malloc == NULL)
		{
			return TLS_OS_ERROR;
		}
		error = krhino_task_create(task_malloc, name, param, prio, 0, stk_start, stk_size/4, entry, 1);
	}
	else
	{
		error = krhino_task_create(task, name, param, prio, 0, stk_start, stk_size/4, entry, 1);
	}

    if (error == RHINO_SUCCESS)
        os_status = TLS_OS_SUCCESS;
    else
        os_status = TLS_OS_ERROR;

    return os_status;
}


/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by
*              its own priority number.  The deleted task is returned to the dormant state and can be
*              re-activated by creating the deleted task again.
*
* Arguments  : prio: the task priority
*                    freefun: function to free resource
*
* Returns    : TLS_OS_SUCCESS             if the call is successful
*             	 TLS_OS_ERROR
*********************************************************************************************************
*/
#if ( INCLUDE_vTaskDelete == 1 )
tls_os_status_t tls_os_task_del(u8 prio,void (*freefun)(void))
{

	return TLS_OS_SUCCESS;
}
#endif

#if (INCLUDE_vTaskSuspend == 1)
/*
*********************************************************************************************************
*                                            SUSPEND A TASK
*
* Description: This function is called to suspend a task.
*
* Arguments  : task     is a pointer to the task
*
* Returns    : TLS_OS_SUCCESS               if the requested task is suspended
*
* Note       : You should use this function with great care.  If you suspend a task that is waiting for
*              an event (i.e. a message, a semaphore, a queue ...) you will prevent this task from
*              running when the event arrives.
*********************************************************************************************************
*/
 tls_os_status_t tls_os_task_suspend(tls_os_task_t *task)
{
	krhino_task_suspend(task);

	return TLS_OS_SUCCESS;
}

/*
*********************************************************************************************************
*                                        RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.
*
* Arguments  : task     is a pointer to the task
*
* Returns    : TLS_OS_SUCCESS                if the requested task is resumed
*
*********************************************************************************************************
*/
 tls_os_status_t tls_os_task_resume(tls_os_task_t *task)
{
	task_resume(task);

	return TLS_OS_SUCCESS;
}
#endif

/*
*********************************************************************************************************
*                                           CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  :sem		 is a pointer to the event control block (OS_EVENT) associated with the
*                            created semaphore
*			cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : TLS_OS_SUCCESS	The call was successful
*			TLS_OS_ERROR
*********************************************************************************************************
*/

#if 1//(1 == configUSE_COUNTING_SEMAPHORES)
 tls_os_status_t tls_os_sem_create(tls_os_sem_t **sem, u32 cnt)
{
	//if (*sem == NULL)
	{
		*sem = tls_mem_alloc(sizeof(tls_os_sem_t));
		if (*sem == NULL)
			return TLS_OS_ERROR;
	}
    return aos_sem_new(*sem, cnt);
}


/*
*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : TLS_OS_SUCCESS             The call was successful and the semaphore was deleted
*                            TLS_OS_ERROR
*
*********************************************************************************************************
*/
 tls_os_status_t tls_os_sem_delete(tls_os_sem_t *sem)
{
	if (sem){
		aos_sem_free(sem);
		tls_mem_free(sem);
	}
	return TLS_OS_SUCCESS;
}

/*
*********************************************************************************************************
*                                           PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/
//�ú������������жϷ��������
 tls_os_status_t tls_os_sem_acquire(tls_os_sem_t *sem,
        u32 wait_time)
{
	unsigned int time;

	if(0 == wait_time)
		time = AOS_WAIT_FOREVER;
	else
		time = wait_time*1000/HZ;
	return aos_sem_wait(sem, time);
}

/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/
 tls_os_status_t tls_os_sem_release(tls_os_sem_t *sem)
{
	aos_sem_signal(sem);
	return TLS_OS_SUCCESS;
}
#endif

// ========================================================================= //
// Message Passing							     //
// ========================================================================= //

/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : queue	is a pointer to the event control clock (OS_EVENT) associated with the
*                                created queue
*
*			queue_start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              	queue_size          is the number of elements in the storage area
*
*			msg_size
*
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/

 tls_os_status_t tls_os_queue_create(tls_os_queue_t **queue, u32 queue_size)
{
#if TLS_USE_AOS_QUEUE
	u8 *buf = NULL;

	//if (*queue == NULL)
	{
		*queue = tls_mem_alloc(sizeof(tls_os_queue_t *));
		if (*queue == NULL)
			return TLS_OS_ERROR;
	}

	buf = tls_mem_alloc(queue_size * sizeof(void *));
	if (buf == NULL)
	{
		tls_mem_free(*queue);
		return TLS_OS_ERROR;
	}
	//printf("que:%p,buf:%p\n",*queue, buf);
	return aos_queue_new(*queue, buf, queue_size*sizeof(void *), sizeof(void *));
#else
	return krhino_queue_dyn_create(queue, "UNDEF", queue_size);
#endif
}

/*
*********************************************************************************************************
*                                        DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : queue        is a pointer to the event control block associated with the desired
*                            queue.
*
*
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/
 tls_os_status_t tls_os_queue_delete(tls_os_queue_t *queue)
{
#if TLS_USE_AOS_QUEUE
	void * buf = NULL;
	if (queue){
		buf = aos_queue_buf_ptr(queue);
		if(buf)
			tls_mem_free(buf);
		aos_queue_free(queue);
		tls_mem_free(queue);
	}
	return TLS_OS_SUCCESS;
#else
	return krhino_queue_dyn_del(queue);
#endif
}

/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : queue        is a pointer to the event control block associated with the desired queue
*
*              	msg          is a pointer to the message to send.
*
*			msg_size
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/
 tls_os_status_t tls_os_queue_send(tls_os_queue_t *queue,
        void *msg,
        u32 msg_size)
{
#if TLS_USE_AOS_QUEUE
	int ret = aos_queue_send(queue, &msg, 4);
	return ret == 0 ? TLS_OS_SUCCESS : TLS_OS_ERROR;
#else

	return krhino_queue_back_send(queue, msg);
#endif
}
/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : queue        is a pointer to the event control block associated with the desired queue
*
*			msg		is a pointer to the message received
*
*			msg_size
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
* Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
*********************************************************************************************************
*/
 tls_os_status_t tls_os_queue_receive(tls_os_queue_t *queue,
        void **msg,
        u32 msg_size,
        u32 wait_time)
{
#if TLS_USE_AOS_QUEUE
	unsigned int xTicksToWait;
	if(0 == wait_time)
		xTicksToWait = AOS_WAIT_FOREVER;
	else
		xTicksToWait = wait_time*1000/HZ;

	return aos_queue_recv(queue, xTicksToWait, msg, &msg_size);
#else

	u8 error;
	tls_os_status_t os_status;
	unsigned int xTicksToWait;
//	portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
	u8 isrcount = 0;

	if(0 == wait_time)
		xTicksToWait = 0xFFFFFFFF;
	else
		xTicksToWait = wait_time;
	return krhino_queue_recv(queue,xTicksToWait, msg);
#endif
}

/*
*********************************************************************************************************
*                                             FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns    : TLS_OS_SUCCESS
*			 TLS_OS_ERROR
*At present, no use for freeRTOS
*********************************************************************************************************
*/
tls_os_status_t tls_os_queue_flush(tls_os_queue_t *queue)
{
#if TLS_USE_AOS_QUEUE
	return TLS_OS_SUCCESS;
#else
	return krhino_queue_flush(queue);
#endif
}

/*
*********************************************************************************************************
*                                         GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the 32-bit
*              counter which keeps track of the number of clock ticks.
*
* Arguments  : none
*
* Returns    : The current value of OSTime
*********************************************************************************************************
*/
 u32 tls_os_get_time(void)
{
    return krhino_sys_tick_get();
}
u32 os_cnter = 0;
/**********************************************************************************************************
* Description: Disable interrupts by preserving the state of interrupts.
*
* Arguments  : none
*
* Returns    : cpu_sr
***********************************************************************************************************/
 u32 tls_os_set_critical(void)
{
	os_cnter++;
	return cpu_intrpt_save();
}

/**********************************************************************************************************
* Description: Enable interrupts by preserving the state of interrupts.
*
* Arguments  : cpu_sr
*
* Returns    : none
***********************************************************************************************************/
 void tls_os_release_critical(u32 cpu_sr)
{
	cpu_intrpt_restore(cpu_sr);
}

/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : timer	A pointer to an OS_TMR data structure.This is the 'handle' that your application
*						will use to reference the timer created.
*
*		        callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                               callback function must be declared as follows:
*
*                               void MyCallback (OS_TMR *ptmr, void *p_arg);
*
* 	             callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*          	   	 period        The 'period' being repeated for the timer.
*                               If you specified 'OS_TMR_OPT_PERIODIC' as an option, when the timer expires, it will
*                               automatically restart with the same period.
*
*			repeat	if repeat
*
*             	pname         Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                               debugging.
*
*Returns    : TLS_OS_SUCCESS
*			TLS_OS_ERROR
************************************************************************************************************************
*/
 tls_os_status_t tls_os_timer_create(tls_os_timer_t **timer,
        TLS_OS_TIMER_CALLBACK callback,
        void *callback_arg,
        u32 period,
        bool repeat,
        u8 *name)
{
    tick_t repeat_period = 0;
#if TLS_USE_AOS_TIMER
	period = period * 1000 / HZ;
	repeat_period  = repeat ? period : 0;
    //if (*timer == NULL)
	{
		*timer = tls_mem_alloc(sizeof(tls_os_timer_t*));
		if (*timer == NULL)
			return TLS_OS_ERROR;
	}
	//printf("timer:%p\n", *timer);
    return aos_timer_new_ext(*timer, callback, callback_arg, period, repeat_period, 0);
#else
	repeat_period  = repeat?period:0;

	return krhino_timer_dyn_create(timer, name, callback, period, repeat_period, callback_arg, 0);
#endif

}

/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : timer          Is a pointer to an OS_TMR
*
************************************************************************************************************************
*/
 void tls_os_timer_start(tls_os_timer_t *timer)
{
#if TLS_USE_AOS_TIMER
	//printf("stimer:%p\n", timer);
	return aos_timer_start(timer);
#else
	u8 isrcount = 0;

	return krhino_timer_start(timer);
#endif
}
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
 void tls_os_timer_change(tls_os_timer_t *timer, u32 ticks)
{
#if TLS_USE_AOS_TIMER
	//printf("timer chg:%p,tick:%d\n", timer, ticks);
	aos_timer_stop(timer);
	aos_timer_change(timer, ticks*1000/HZ);
	aos_timer_start(timer);
#else
	tick_t repeat_ticks = ((ktimer_t *)timer)->round_ticks ? ticks : 0;

	krhino_timer_change(timer, ticks, repeat_ticks);
	tls_os_timer_start(timer);
#endif
}
/*
************************************************************************************************************************
*                                                   STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : timer          Is a pointer to the timer to stop.
*
************************************************************************************************************************
*/
 void tls_os_timer_stop(tls_os_timer_t *timer)
{
#if TLS_USE_AOS_TIMER
	//printf("stoptimer:%p\n", timer);
	aos_timer_stop(timer);
#else
	krhino_timer_stop(timer);
#endif
}


/*
************************************************************************************************************************
*                                                   Delete A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : timer          Is a pointer to the timer to delete.
*
************************************************************************************************************************
*/
 int tls_os_timer_delete(tls_os_timer_t *timer)
{
#if TLS_USE_AOS_TIMER
	//printf("dtimer:%p\n", timer);
	if (timer){
		aos_timer_free(timer);
		tls_mem_free(timer);
	}
	return TLS_OS_SUCCESS;
#else
	return krhino_timer_dyn_del(timer);
#endif
}


/*
*********************************************************************************************************
*                                       DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/
 void tls_os_time_delay(u32 ticks)
{
	aos_msleep(ticks*1000/HZ);
}

/*
*********************************************************************************************************
*                                       task stat info
*
* Description: This function is used to display stat info
* Arguments  :
*
* Returns    : none
*********************************************************************************************************
*/
void tls_os_disp_task_stat_info(void)
{

}
/*
*********************************************************************************************************
*                                     OS INIT function
*
* Description: This function is used to init os common resource
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/

void tls_os_init(void *arg)
{
}
/*
*********************************************************************************************************
*                                     OS scheduler start function
*
* Description: This function is used to start task schedule
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/

void tls_os_start_scheduler(void)
{
}
/*
*********************************************************************************************************
*                                     Get OS TYPE
*
* Description: This function is used to get OS type
*
* Arguments  : None;
*
* Returns    : TLS_OS_TYPE
*                	 OS_UCOSII = 0,
*	             OS_FREERTOS = 1,
*********************************************************************************************************
*/

int tls_os_get_type(void)
{
	return (int)OS_FREERTOS;
}
/*
*********************************************************************************************************
*                                     OS tick handler
*
* Description: This function is  tick handler.
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/

void tls_os_time_tick(void *p){

}


#endif /* end of WM_OSAL_RTOS_H */
