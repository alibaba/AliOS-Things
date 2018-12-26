/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.

	FreeRTOS supports many tools and architectures. V7.0.0 is sponsored by:
	Atollic AB - Atollic provides professional embedded systems development
	tools for C/C++ development, code analysis and test automation.
	See http://www.atollic.com
    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************
*/
//  maintainer: Halley

#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/
//#define tskKERNEL_VERSION_NUMBER "V7.0.0"
/*
 * task. h
 *
 * Type by which tasks are referenced.  For example, a call to xTaskCreate
 * returns (via a pointer parameter) an xTaskHandle variable that can then
 * be used as a parameter to vTaskDelete to delete the task.
 *
 * \page xTaskHandle xTaskHandle
 * \ingroup Tasks
 */
typedef void* xTaskHandle;
/* Defines the prototype to which the application task hook function must conform. */
typedef long(*pdTASK_HOOK_CODE)(void*);
/* Defines the task function. */
typedef void (*pdTASK_CODE)(void*);

typedef struct xMEMORY_REGION
{
	void* pvBaseAddress;
	unsigned long ulLengthInBytes;
	unsigned long ulParameters;
} xMemoryRegion;

/*-----------------------------------------------------------
 * TASK CREATION API
 *----------------------------------------------------------*/

/*
 * Generic version of the task creation function which is in turn called by the
 * xTaskCreate() and xTaskCreateRestricted() macros.
 */
int xTaskGenericCreate(pdTASK_CODE pxTaskCode, const unsigned char* pcName, \
                       unsigned short usStackDepth, void* pvParameters, unsigned long uxPriority, \
                       xTaskHandle* pxCreatedTask, long* puxStackBuffer, \
                       const xMemoryRegion* const xRegions);

#define xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask) \
			xTaskGenericCreate((void(*)(void*))pvTaskCode, (const unsigned char*)pcName,\
				(unsigned short)usStackDepth, (void*)pvParameters, (unsigned int)uxPriority,\
						(xTaskHandle)pxCreatedTask, NULL, NULL);
/**
 * task. h
 * <pre>int vTaskDelete(xTaskHandle pxTask);</pre>
 *
 * CFG_TASK_DELETE must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Remove a task from the RTOS real time kernels management.  The task being
 * deleted will be removed from all ready, blocked, suspended and event lists.
 *
 * NOTE:  The idle task is responsible for freeing the kernel allocated
 * memory from tasks that have been deleted.  It is therefore important that
 * the idle task is not starved of microcontroller processing time if your
 * application makes any calls to vTaskDelete().  Memory allocated by the
 * task code is not automatically freed, and should be freed before the task
 * is deleted.
 *
 * See the demo application file death.c for sample code that utilises
 * vTaskDelete().
 *
 * @param pxTask The handle of the task to be deleted.  Passing NULL will
 * cause the calling task to be deleted.
 *
 * Example usage:
   <pre>
 void vOtherFunction(void)
 {
 xTaskHandle xHandle;

	 // Create the task, storing the handle.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, &xHandle);

	 // Use the handle to delete the task.
	 vTaskDelete(xHandle);
 }
   </pre>
 * \defgroup vTaskDelete vTaskDelete
 * \ingroup Tasks
 */
int vTaskDelete(xTaskHandle pxTaskToDelete);

/*-----------------------------------------------------------
 * TASK CONTROL API
 *----------------------------------------------------------*/

/**
 * task. h
 * <pre>void vTaskSleep(unsigned long xTicksToDelay);</pre>
 *
 * Delay a task for a given number of ticks.  The actual time that the
 * task remains blocked depends on the tick rate.  The constant
 * CFG_TICK_RATE can be used to calculate real time from the tick
 * rate - with the resolution of one tick period.
 *
 * CFG_TASK_DELAY must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 *
 * vTaskSleep() specifies a time at which the task wishes to unblock relative to
 * the time at which vTaskSleep() is called.  For example, specifying a block
 * period of 100 ticks will cause the task to unblock 100 ticks after
 * vTaskSleep() is called.  vTaskSleep() does not therefore provide a good method
 * of controlling the frequency of a cyclical task as the path taken through the
 * code, as well as other task and interrupt activity, will effect the frequency
 * at which vTaskSleep() gets called and therefore the time at which the task
 * next executes.  See vTaskSleepUntil() for an alternative API function designed
 * to facilitate fixed frequency execution.  It does this by specifying an
 * absolute time (rather than a relative time) at which the calling task should
 * unblock.
 *
 * @param xTicksToDelay The amount of time, in tick periods, that
 * the calling task should block.
 *
 * Example usage:

 void vTaskFunction(void * pvParameters)
 {
 void vTaskFunction(void * pvParameters)
 {
 // Block for 500ms.
 const unsigned long xDelay = 500 / CFG_TICK_RATE;

	 for( ;; )
	 {
		 // Simply toggle the LED every 500ms, blocking between each toggle.
		 vToggleLED();
		 vTaskSleep(xDelay);
	 }
 }

 * \defgroup vTaskSleep vTaskSleep
 * \ingroup TaskCtrl
 */
int vTaskSleep(int xTicksToDelay);

/**
 * task. h
 * <pre>unsigned long uxTaskPriorityGet(xTaskHandle pxTask);</pre>
 *
 * INCLUDE_xTaskPriorityGet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Obtain the priority of any task.
 *
 * @param pxTask Handle of the task to be queried.  Passing a NULL
 * handle results in the priority of the calling task being returned.
 *
 * @return The priority of pxTask.
 *
 * Example usage:
   <pre>
 void vAFunction(void)
 {
 xTaskHandle xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, &xHandle);

	 // ...

	 // Use the handle to obtain the priority of the created task.
	 // It was created with TASK_IDLE_PRIORITY, but may have changed
	 // it itself.
	 if(uxTaskPriorityGet(xHandle) != TASK_IDLE_PRIORITY)
	 {
		 // The task has changed it's priority.
	 }

	 // ...

	 // Is our priority higher than the created task?
	 if(uxTaskPriorityGet(xHandle) < uxTaskPriorityGet(NULL))
	 {
		 // Our priority (obtained using NULL handle) is higher.
	 }
 }
   </pre>
 * \defgroup uxTaskPriorityGet uxTaskPriorityGet
 * \ingroup TaskCtrl
 */
unsigned long uxTaskPriorityGet(xTaskHandle pxTask);

/**
 * task. h
 * <pre>void vTaskPrioritySet(xTaskHandle pxTask, unsigned long uxNewPriority);</pre>
 *
 * CFG_TASK_PRIORITY_SET must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Set the priority of any task.
 *
 * A context switch will occur before the function returns if the priority
 * being set is higher than the currently executing task.
 *
 * @param pxTask Handle to the task for which the priority is being set.
 * Passing a NULL handle results in the priority of the calling task being set.
 *
 * @param uxNewPriority The priority to which the task will be set.
 *
 * Example usage:
   <pre>
 void vAFunction(void)
 {
 xTaskHandle xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, &xHandle);

	 // ...

	 // Use the handle to raise the priority of the created task.
	 vTaskPrioritySet(xHandle, TASK_IDLE_PRIORITY + 1);

	 // ...

	 // Use a NULL handle to raise our priority to the same value.
	 vTaskPrioritySet(NULL, TASK_IDLE_PRIORITY + 1);
 }
   </pre>
 * \defgroup vTaskPrioritySet vTaskPrioritySet
 * \ingroup TaskCtrl
 */
void vTaskPrioritySet(xTaskHandle pxTask, unsigned long uxNewPriority);

/**
 * task. h
 * <pre>void vTaskSuspend(xTaskHandle pxTaskToSuspend);</pre>
 *
 * CFG_TASK_SUSPEND must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Suspend any task.  When suspended a task will never get any microcontroller
 * processing time, no matter what its priority.
 *
 * Calls to vTaskSuspend are not accumulative -
 * i.e. calling vTaskSuspend() twice on the same task still only requires one
 * call to vTaskResume() to ready the suspended task.
 *
 * @param pxTaskToSuspend Handle to the task being suspended.  Passing a NULL
 * handle will cause the calling task to be suspended.
 *
 * Example usage:
   <pre>
 void vAFunction(void)
 {
 xTaskHandle xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, &xHandle);

	 // ...

	 // Use the handle to suspend the created task.
	 vTaskSuspend(xHandle);

	 // ...

	 // The created task will not run during this period, unless
	 // another task calls vTaskResume(xHandle).

	 //...


	 // Suspend ourselves.
	 vTaskSuspend(NULL);

	 // We cannot get here unless another task calls vTaskResume
	 // with our handle as the parameter.
 }
   </pre>
 * \defgroup vTaskSuspend vTaskSuspend
 * \ingroup TaskCtrl
 */
void vTaskSuspend(xTaskHandle pxTaskToSuspend);

/**
 * task. h
 * <pre>void vTaskResume(xTaskHandle pxTaskToResume);</pre>
 *
 * CFG_TASK_SUSPEND must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * Resumes a suspended task.
 *
 * A task that has been suspended by one of more calls to vTaskSuspend()
 * will be made available for running again by a single call to
 * vTaskResume().
 *
 * @param pxTaskToResume Handle to the task being readied.
 *
 * Example usage:
   <pre>
 void vAFunction(void)
 {
 xTaskHandle xHandle;

	 // Create a task, storing the handle.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, &xHandle);

	 // ...

	 // Use the handle to suspend the created task.
	 vTaskSuspend(xHandle);

	 // ...

	 // The created task will not run during this period, unless
	 // another task calls vTaskResume(xHandle).

	 //...


	 // Resume the suspended task ourselves.
	 vTaskResume(xHandle);

	 // The created task will once again get microcontroller processing
	 // time in accordance with it priority within the system.
 }
   </pre>
 * \defgroup vTaskResume vTaskResume
 * \ingroup TaskCtrl
 */
int vTaskResume(xTaskHandle pxTaskToResume);


/*-----------------------------------------------------------
 * SCHEDULER CONTROL
 *----------------------------------------------------------*/

/**
 * task. h
 * <pre>void vTaskStartScheduler(void);</pre>
 *
 * Starts the real time kernel tick processing.  After calling the kernel
 * has control over which tasks are executed and when.  This function
 * does not return until an executing task calls vTaskEndScheduler().
 *
 * At least one task should be created via a call to xTaskCreate()
 * before calling vTaskStartScheduler().  The idle task is created
 * automatically when the first application task is created.
 *
 * See the demo application file main.c for an example of creating
 * tasks and starting the kernel.
 *
 * Example usage:
   <pre>
 void vAFunction(void)
 {
	 // Create at least one task before starting the kernel.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, NULL);

	 // Start the real time kernel with preemption.
	 vTaskStartScheduler();

	 // Will not get here unless a task calls vTaskEndScheduler()
 }
   </pre>
 *
 * \defgroup vTaskStartScheduler vTaskStartScheduler
 * \ingroup SchedulerControl
 */
void vTaskStartScheduler(void);

/**
 * task. h
 * <pre>void vTaskEndScheduler(void);</pre>
 *
 * Stops the real time kernel tick.  All created tasks will be automatically
 * deleted and multitasking (either preemptive or cooperative) will
 * stop.  Execution then resumes from the point where vTaskStartScheduler()
 * was called, as if vTaskStartScheduler() had just returned.
 *
 * See the demo application file main. c in the demo/PC directory for an
 * example that uses vTaskEndScheduler().
 *
 * vTaskEndScheduler() requires an exit function to be defined within the
 * portable layer (see vPortEndScheduler() in port. c for the PC port).  This
 * performs hardware specific operations such as stopping the kernel tick.
 *
 * vTaskEndScheduler() will cause all of the resources allocated by the
 * kernel to be freed - but will not free resources allocated by application
 * tasks.
 *
 * Example usage:
   <pre>
 void vTaskCode(void * pvParameters)
 {
	 for( ;; )
	 {
		 // Task code goes here.

		 // At some point we want to end the real time kernel processing
		 // so call ...
		 vTaskEndScheduler();
	 }
 }

 void vAFunction(void)
 {
	 // Create at least one task before starting the kernel.
	 xTaskCreate(vTaskCode, "NAME", STACK_SIZE, NULL, TASK_IDLE_PRIORITY, NULL);

	 // Start the real time kernel with preemption.
	 vTaskStartScheduler();

	 // Will only get here when the vTaskCode() task has called
	 // vTaskEndScheduler().  When we get here we are back to single task
	 // execution.
 }
   </pre>
 *
 * \defgroup vTaskEndScheduler vTaskEndScheduler
 * \ingroup SchedulerControl
 */
void vTaskEndScheduler(void);

/**
 * task. h
 * <pre>void vTaskSuspendAll(void);</pre>
 *
 * Suspends all real time kernel activity while keeping interrupts (including the
 * kernel tick) enabled.
 *
 * After calling vTaskSuspendAll() the calling task will continue to execute
 * without risk of being swapped out until a call to xTaskResumeAll() has been
 * made.
 *
 * API functions that have the potential to cause a context switch (for example,
 * vTaskSleepUntil(), xQueueSend(), etc.) must not be called while the scheduler
 * is suspended.
 *
 * Example usage:
   <pre>
 void vTask1(void * pvParameters)
 {
	 for( ;; )
	 {
		 // Task code goes here.

		 // ...

		 // At some point the task wants to perform a long operation during
		 // which it does not want to get swapped out.  It cannot use
		 // TASK_CRITICAL_ENTER()/TASK_CRITICAL_EXIT() as the length of the
		 // operation may cause interrupts to be missed - including the
		 // ticks.

		 // Prevent the real time kernel swapping out the task.
		 vTaskSuspendAll();

		 // Perform the operation here.  There is no need to use critical
		 // sections as we have all the microcontroller processing time.
		 // During this time interrupts will still operate and the kernel
		 // tick count will be maintained.

		 // ...

		 // The operation is complete.  Restart the kernel.
		 xTaskResumeAll();
	 }
 }
   </pre>
 * \defgroup vTaskSuspendAll vTaskSuspendAll
 * \ingroup SchedulerControl
 */
void vTaskSuspendAll(void);

/**
 * task. h
 * <pre>char xTaskResumeAll(void);</pre>
 *
 * Resumes real time kernel activity following a call to vTaskSuspendAll().
 * After a call to vTaskSuspendAll() the kernel will take control of which
 * task is executing at any time.
 *
 * @return If resuming the scheduler caused a context switch then TRUE is
 *		  returned, otherwise FALSE is returned.
 *
 * Example usage:
   <pre>
 void vTask1(void * pvParameters)
 {
	 for( ;; )
	 {
		 // Task code goes here.

		 // ...

		 // At some point the task wants to perform a long operation during
		 // which it does not want to get swapped out.  It cannot use
		 // TASK_CRITICAL_ENTER()/TASK_CRITICAL_EXIT() as the length of the
		 // operation may cause interrupts to be missed - including the
		 // ticks.

		 // Prevent the real time kernel swapping out the task.
		 vTaskSuspendAll();

		 // Perform the operation here.  There is no need to use critical
		 // sections as we have all the microcontroller processing time.
		 // During this time interrupts will still operate and the real
		 // time kernel tick count will be maintained.

		 // ...

		 // The operation is complete.  Restart the kernel.  We want to force
		 // a context switch - but there is no point if resuming the scheduler
		 // caused a context switch already.
		 if(!xTaskResumeAll())
		 {
			  schedule_yield();
		 }
	 }
 }
   </pre>
 * \defgroup xTaskResumeAll xTaskResumeAll
 * \ingroup SchedulerControl
 */
signed long xTaskResumeAll(void);

/**
 * task. h
 * <pre>signed long xTaskIsTaskSuspended(xTaskHandle xTask);</pre>
 *
 * Utility task that simply returns TRUE if the task referenced by xTask is
 * currently in the Suspended state, or FALSE if the task referenced by xTask
 * is in any other state.
 *
 */
signed long xTaskIsTaskSuspended(xTaskHandle xTask);

/*-----------------------------------------------------------
 * TASK UTILITIES
 *----------------------------------------------------------*/

/**
 * task. h
 * <PRE>unsigned long xTaskGetTickCount(void);</PRE>
 *
 * @return The count of ticks since vTaskStartScheduler was called.
 *
 * \page xTaskGetTickCount xTaskGetTickCount
 * \ingroup TaskUtils
 */
unsigned int xTaskGetTickCount(void);

/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 *----------------------------------------------------------*/
/*
 * Return the handle of the calling task.
 */
xTaskHandle xTaskGetCurrentTaskHandle(void);

/*
 * Raises the priority of the mutex holder to that of the calling task should
 * the mutex holder have a priority less than the calling task.
 */
void vTaskPriorityInherit(xTaskHandle * const pxMutexHolder);

/*
 * Set the priority of a task back to its proper priority in the case that it
 * inherited a higher priority while it was holding a semaphore.
 */
void vTaskPriorityDisinherit(xTaskHandle * const pxMutexHolder);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif /* __TASK_H__ */
