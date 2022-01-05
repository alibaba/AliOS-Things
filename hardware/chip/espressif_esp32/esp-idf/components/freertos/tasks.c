/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE
#include "esp_newlib.h"
#include "esp_compiler.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "StackMacros.h"
#include "portmacro.h"
#include "portmacro_priv.h"
#include "semphr.h"

/* Lint e961 and e750 are suppressed as a MISRA exception justified because the
MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be defined for the
header files above, but not in this file, in order to generate the correct
privileged Vs unprivileged linkage and placement. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750. */

/* Set configUSE_STATS_FORMATTING_FUNCTIONS to 2 to include the stats formatting
functions but without including stdio.h here. */
#if ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 )
	/* At the bottom of this file are two optional functions that can be used
	to generate human readable text from the raw data generated by the
	uxTaskGetSystemState() function.  Note the formatting functions are provided
	for convenience only, and are NOT considered part of the kernel. */
	#include <stdio.h>
#endif /* configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) */

/* Sanity check the configuration. */
#if configUSE_TICKLESS_IDLE != 0
	#if INCLUDE_vTaskSuspend != 1
		#error INCLUDE_vTaskSuspend must be set to 1 if configUSE_TICKLESS_IDLE is not set to 0
	#endif /* INCLUDE_vTaskSuspend */
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Defines the size, in bytes, of the stack allocated to the idle task.
 */
#define tskIDLE_STACK_SIZE	configIDLE_TASK_STACK_SIZE

#if( configUSE_PREEMPTION == 0 )
	/* If the cooperative scheduler is being used then a yield should not be
	performed just because a higher priority task has been woken. */
	#define taskYIELD_IF_USING_PREEMPTION()
#else
	#define taskYIELD_IF_USING_PREEMPTION() portYIELD_WITHIN_API()
#endif




/* Value that can be assigned to the eNotifyState member of the TCB. */
typedef enum
{
	eNotWaitingNotification = 0,
	eWaitingNotification,
	eNotified
} eNotifyValue;

/* Sometimes the FreeRTOSConfig.h settings only allow a task to be created using
dynamically allocated RAM, in which case when any task is deleted it is known
that both the task's stack and TCB need to be freed.  Sometimes the
FreeRTOSConfig.h settings only allow a task to be created using statically
allocated RAM, in which case when any task is deleted it is known that neither
the task's stack or TCB should be freed.  Sometimes the FreeRTOSConfig.h
settings allow a task to be created using either statically or dynamically
allocated RAM, in which case a member of the TCB is used to record whether the
stack and/or TCB were allocated statically or dynamically, so when a task is
deleted the RAM that was allocated dynamically is freed again and no attempt is
made to free the RAM that was allocated statically.
tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE is only true if it is possible for a
task to be created using either statically or dynamically allocated RAM.  Note
that if portUSING_MPU_WRAPPERS is 1 then a protected task can be created with
a statically allocated stack and a dynamically allocated TCB. */
#define tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE ( ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) || ( portUSING_MPU_WRAPPERS == 1 ) )
#define tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB 		( ( uint8_t ) 0 )
#define tskSTATICALLY_ALLOCATED_STACK_ONLY 			( ( uint8_t ) 1 )
#define tskSTATICALLY_ALLOCATED_STACK_AND_TCB		( ( uint8_t ) 2 )

/*
 * Task control block.  A task control block (TCB) is allocated for each task,
 * and stores task state information, including a pointer to the task's context
 * (the task's run time environment, including register values)
 */
typedef struct tskTaskControlBlock
{
	volatile StackType_t	*pxTopOfStack;	/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

	#if ( portUSING_MPU_WRAPPERS == 1 )
		xMPU_SETTINGS	xMPUSettings;		/*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
	#endif

	ListItem_t			xGenericListItem;	/*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
	ListItem_t			xEventListItem;		/*< Used to reference a task from an event list. */
	UBaseType_t			uxPriority;			/*< The priority of the task.  0 is the lowest priority. */
	StackType_t			*pxStack;			/*< Points to the start of the stack. */
	char				pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	BaseType_t			xCoreID;			/*< Core this task is pinned to */
											/* If this moves around (other than pcTaskName size changes), please change the define in xtensa_vectors.S as well. */
	#if ( portSTACK_GROWTH > 0 || configENABLE_TASK_SNAPSHOT == 1 )
		StackType_t		*pxEndOfStack;		/*< Points to the end of the stack on architectures where the stack grows up from low memory. */
	#endif

	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
		UBaseType_t 	uxCriticalNesting; 	/*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
		uint32_t		uxOldInterruptState; /*< Interrupt state before the outer taskEnterCritical was called */
	#endif

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t		uxTCBNumber;		/*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
		UBaseType_t  	uxTaskNumber;		/*< Stores a number specifically for use by third party trace code. */
	#endif

	#if ( configUSE_MUTEXES == 1 )
		UBaseType_t 	uxBasePriority;		/*< The priority last assigned to the task - used by the priority inheritance mechanism. */
		UBaseType_t 	uxMutexesHeld;
	#endif

	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
		TaskHookFunction_t pxTaskTag;
	#endif

	#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
		void *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
	#if ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )
		TlsDeleteCallbackFunction_t pvThreadLocalStoragePointersDelCallback[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
	#endif
	#endif

	#if ( configGENERATE_RUN_TIME_STATS == 1 )
		uint32_t		ulRunTimeCounter;	/*< Stores the amount of time the task has spent in the Running state. */
	#endif

	#if ( configUSE_NEWLIB_REENTRANT == 1 )
		/* Allocate a Newlib reent structure that is specific to this task.
		Note Newlib support has been included by popular demand, but is not
		used by the FreeRTOS maintainers themselves.  FreeRTOS is not
		responsible for resulting newlib operation.  User must be familiar with
		newlib and must provide system-wide implementations of the necessary
		stubs. Be warned that (at the time of writing) the current newlib design
		implements a system-wide malloc() that must be provided with locks. */
		struct 	_reent xNewLib_reent;
	#endif

	#if ( configUSE_TASK_NOTIFICATIONS == 1 )
		volatile uint32_t ulNotifiedValue;
		volatile eNotifyValue eNotifyState;
	#endif

	/* See the comments above the definition of
	tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
	#if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
		uint8_t	ucStaticallyAllocated; 		/*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
	#endif

} tskTCB;

/* The old tskTCB name is maintained above then typedefed to the new TCB_t name
below to enable the use of older kernel aware debuggers. */
typedef tskTCB TCB_t;

#if __GNUC_PREREQ(4, 6)
_Static_assert(sizeof(StaticTask_t) == sizeof(TCB_t), "StaticTask_t != TCB_t");
#endif

/*
 * Some kernel aware debuggers require the data the debugger needs access to to
 * be global, rather than file scope.
 */
#ifdef portREMOVE_STATIC_QUALIFIER
	#define static
#endif

/*lint -e956 A manual analysis and inspection has been used to determine which
static variables must be declared volatile. */

PRIVILEGED_DATA TCB_t * volatile pxCurrentTCB[ portNUM_PROCESSORS ] = { NULL };

/* Lists for ready and blocked tasks. --------------------*/
PRIVILEGED_DATA static List_t pxReadyTasksLists[ configMAX_PRIORITIES ];/*< Prioritised ready tasks. */
PRIVILEGED_DATA static List_t xDelayedTaskList1;						/*< Delayed tasks. */
PRIVILEGED_DATA static List_t xDelayedTaskList2;						/*< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
PRIVILEGED_DATA static List_t * volatile pxDelayedTaskList;				/*< Points to the delayed task list currently being used. */
PRIVILEGED_DATA static List_t * volatile pxOverflowDelayedTaskList;		/*< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
PRIVILEGED_DATA static List_t xPendingReadyList[ portNUM_PROCESSORS ];						/*< Tasks that have been readied while the scheduler was suspended.  They will be moved to the ready list when the scheduler is resumed. */

#if ( INCLUDE_vTaskDelete == 1 )

	PRIVILEGED_DATA static List_t xTasksWaitingTermination;				/*< Tasks that have been deleted - but their memory not yet freed. Protected by xTaskQueueMutex.*/
	PRIVILEGED_DATA static volatile UBaseType_t uxTasksDeleted = ( UBaseType_t ) 0U;

#endif

#if ( INCLUDE_vTaskSuspend == 1 )

	PRIVILEGED_DATA static List_t xSuspendedTaskList;					/*< Tasks that are currently suspended. */

#endif

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )

	PRIVILEGED_DATA static TaskHandle_t xIdleTaskHandle[portNUM_PROCESSORS] = {NULL};			/*< Holds the handle of the idle task.  The idle task is created automatically when the scheduler is started. */

#endif

/* Other file private variables. --------------------------------*/
PRIVILEGED_DATA static volatile UBaseType_t uxCurrentNumberOfTasks 	= ( UBaseType_t ) 0U;
PRIVILEGED_DATA static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;
PRIVILEGED_DATA static volatile UBaseType_t uxTopReadyPriority 		= tskIDLE_PRIORITY;
PRIVILEGED_DATA static volatile BaseType_t xSchedulerRunning 		= pdFALSE;
PRIVILEGED_DATA static volatile UBaseType_t uxPendedTicks 			= ( UBaseType_t ) 0U;
PRIVILEGED_DATA static volatile BaseType_t xYieldPending[portNUM_PROCESSORS] 		= {pdFALSE};
PRIVILEGED_DATA static volatile BaseType_t xNumOfOverflows 			= ( BaseType_t ) 0;
PRIVILEGED_DATA static UBaseType_t uxTaskNumber 					= ( UBaseType_t ) 0U;
PRIVILEGED_DATA static volatile TickType_t xNextTaskUnblockTime		= portMAX_DELAY;

/* Context switches are held pending while the scheduler is suspended.  Also,
interrupts must not manipulate the xGenericListItem of a TCB, or any of the
lists the xGenericListItem can be referenced from, if the scheduler is suspended.
If an interrupt needs to unblock a task while the scheduler is suspended then it
moves the task's event list item into the xPendingReadyList, ready for the
kernel to move the task from the pending ready list into the real ready list
when the scheduler is unsuspended.  The pending ready list itself can only be
accessed from a critical section. */
PRIVILEGED_DATA static volatile UBaseType_t uxSchedulerSuspended[ portNUM_PROCESSORS ]	= { ( UBaseType_t ) pdFALSE };

/* We use just one spinlock for all the critical sections. */
PRIVILEGED_DATA static portMUX_TYPE xTaskQueueMutex = portMUX_INITIALIZER_UNLOCKED;

#if ( configGENERATE_RUN_TIME_STATS == 1 )

	PRIVILEGED_DATA static uint32_t ulTaskSwitchedInTime[portNUM_PROCESSORS] = {0U};	/*< Holds the value of a timer/counter the last time a task was switched in on a particular core. */
	PRIVILEGED_DATA static uint32_t ulTotalRunTime = 0UL;		/*< Holds the total amount of execution time as defined by the run time counter clock. */

#endif


// per-CPU flags indicating that we are doing context switch, it is used by apptrace and sysview modules
// in order to avoid calls of vPortYield from traceTASK_SWITCHED_IN/OUT when waiting
// for locks to be free or for host to read full trace buffer
PRIVILEGED_DATA static volatile BaseType_t xSwitchingContext[ portNUM_PROCESSORS ]  = { pdFALSE };

/*lint +e956 */

/* Debugging and trace facilities private variables and macros. ------------*/

/*
 * The value used to fill the stack of a task when the task is created.  This
 * is used purely for checking the high water mark for tasks.
 */
#define tskSTACK_FILL_BYTE	( 0xa5U )

/*
 * Macros used by vListTask to indicate which state a task is in.
 */
#define tskBLOCKED_CHAR		( 'B' )
#define tskREADY_CHAR		( 'R' )
#define tskDELETED_CHAR		( 'D' )
#define tskSUSPENDED_CHAR	( 'S' )

/*-----------------------------------------------------------*/


#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )

	/* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 0 then task selection is
	performed in a generic way that is not optimised to any particular
	microcontroller architecture. */

	/* uxTopReadyPriority holds the priority of the highest priority ready
	state task. */
	#define taskRECORD_READY_PRIORITY( uxPriority )														\
	{																									\
		if( ( uxPriority ) > uxTopReadyPriority )														\
		{																								\
			uxTopReadyPriority = ( uxPriority );														\
		}																								\
	} /* taskRECORD_READY_PRIORITY */

	/*-----------------------------------------------------------*/

	#define taskSELECT_HIGHEST_PRIORITY_TASK()															\
	{																									\
		/* Find the highest priority queue that contains ready tasks. */								\
		while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopReadyPriority ] ) ) )						\
		{																								\
			configASSERT( uxTopReadyPriority );															\
			--uxTopReadyPriority;																		\
		}																								\
																										\
		/* listGET_OWNER_OF_NEXT_ENTRY indexes through the list, so the tasks of						\
		the	same priority get an equal share of the processor time. */									\
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB[xPortGetCoreID()], &( pxReadyTasksLists[ uxTopReadyPriority ] ) );		\
	} /* taskSELECT_HIGHEST_PRIORITY_TASK */

	/*-----------------------------------------------------------*/

	/* Define away taskRESET_READY_PRIORITY() and portRESET_READY_PRIORITY() as
	they are only required when a port optimised method of task selection is
	being used. */
	#define taskRESET_READY_PRIORITY( uxPriority )
	#define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#else /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

	/* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 1 then task selection is
	performed in a way that is tailored to the particular microcontroller
	architecture being used. */

	/* A port optimised version is provided.  Call the port defined macros. */
	#define taskRECORD_READY_PRIORITY( uxPriority )	portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

	/*-----------------------------------------------------------*/

	#define taskSELECT_HIGHEST_PRIORITY_TASK()														\
	{																								\
	UBaseType_t uxTopPriority;																		\
																									\
		/* Find the highest priority queue that contains ready tasks. */							\
		portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );								\
		configASSERT( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ uxTopPriority ] ) ) > 0 );		\
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB[xPortGetCoreID()], &( pxReadyTasksLists[ uxTopPriority ] ) );		\
	} /* taskSELECT_HIGHEST_PRIORITY_TASK() */

	/*-----------------------------------------------------------*/

	/* A port optimised version is provided, call it only if the TCB being reset
	is being referenced from a ready list.  If it is referenced from a delayed
	or suspended list then it won't be in a ready list. */
	#define taskRESET_READY_PRIORITY( uxPriority )														\
	{																									\
		if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == ( UBaseType_t ) 0 )	\
		{																								\
			portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );							\
		}																								\
	}

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*-----------------------------------------------------------*/

/* pxDelayedTaskList and pxOverflowDelayedTaskList are switched when the tick
count overflows. */
#define taskSWITCH_DELAYED_LISTS()																	\
{																									\
	List_t *pxTemp;																					\
																									\
	/* The delayed tasks list should be empty when the lists are switched. */						\
	configASSERT( ( listLIST_IS_EMPTY( pxDelayedTaskList ) ) );										\
																									\
	pxTemp = pxDelayedTaskList;																		\
	pxDelayedTaskList = pxOverflowDelayedTaskList;													\
	pxOverflowDelayedTaskList = pxTemp;																\
	xNumOfOverflows++;																				\
	prvResetNextTaskUnblockTime();																	\
}

/*-----------------------------------------------------------*/

/*
 * Place the task represented by pxTCB into the appropriate ready list for
 * the task.  It is inserted at the end of the list.
 */
#define prvAddTaskToReadyList( pxTCB )																\
	traceMOVED_TASK_TO_READY_STATE( pxTCB );														\
	taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );												\
	vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xGenericListItem ) )
/*
 * Place the task represented by pxTCB which has been in a ready list before
 * into the appropriate ready list for the task.
 * It is inserted at the end of the list.
 */
#define prvReaddTaskToReadyList( pxTCB )                                                               \
   traceREADDED_TASK_TO_READY_STATE( pxTCB );                                                      \
   taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );                                             \
   vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xGenericListItem ) )
/*-----------------------------------------------------------*/

#define tskCAN_RUN_HERE( cpuid ) ( cpuid==xPortGetCoreID() || cpuid==tskNO_AFFINITY )

/*
 * Several functions take an TaskHandle_t parameter that can optionally be NULL,
 * where NULL is used to indicate that the handle of the currently executing
 * task should be used in place of the parameter.  This macro simply checks to
 * see if the parameter is NULL and returns a pointer to the appropriate TCB.
 */
/* ToDo: See if this still works for multicore. */
#define prvGetTCBFromHandle( pxHandle ) ( ( ( pxHandle ) == NULL ) ? ( TCB_t * ) xTaskGetCurrentTaskHandle() : ( TCB_t * ) ( pxHandle ) )

/* The item value of the event list item is normally used to hold the priority
of the task to which it belongs (coded to allow it to be held in reverse
priority order).  However, it is occasionally borrowed for other purposes.  It
is important its value is not updated due to a task priority change while it is
being used for another purpose.  The following bit definition is used to inform
the scheduler that the value should not be changed - in which case it is the
responsibility of whichever module is using the value to ensure it gets set back
to its original value when it is released. */
#if configUSE_16_BIT_TICKS == 1
	#define taskEVENT_LIST_ITEM_VALUE_IN_USE	0x8000U
#else
	#define taskEVENT_LIST_ITEM_VALUE_IN_USE	0x80000000UL
#endif

/* Callback function prototypes. --------------------------*/
#if configCHECK_FOR_STACK_OVERFLOW > 0
	extern void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName );
#endif

#if configUSE_TICK_HOOK > 0
	extern void vApplicationTickHook( void );
#endif

#if  portFIRST_TASK_HOOK
	extern void vPortFirstTaskHook(TaskFunction_t taskfn);
#endif


/* File private functions. --------------------------------*/

/**
 * Utility task that simply returns pdTRUE if the task referenced by xTask is
 * currently in the Suspended state, or pdFALSE if the task referenced by xTask
 * is in any other state.
 *
 * Caller must hold xTaskQueueMutex before calling this function.
 */
#if ( INCLUDE_vTaskSuspend == 1 )
	static BaseType_t prvTaskIsTaskSuspended( const TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
#endif /* INCLUDE_vTaskSuspend */

/*
 * Utility to ready all the lists used by the scheduler.  This is called
 * automatically upon the creation of the first task.
 */
static void prvInitialiseTaskLists( void ) PRIVILEGED_FUNCTION;

/*
 * The idle task, which as all tasks is implemented as a never ending loop.
 * The idle task is automatically created and added to the ready lists upon
 * creation of the first user task.
 *
 * The portTASK_FUNCTION_PROTO() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
static portTASK_FUNCTION_PROTO( prvIdleTask, pvParameters );

/*
 * Utility to free all memory allocated by the scheduler to hold a TCB,
 * including the stack pointed to by the TCB.
 *
 * This does not free memory allocated by the task itself (i.e. memory
 * allocated by calls to pvPortMalloc from within the tasks application code).
 */
#if ( INCLUDE_vTaskDelete == 1 )

	static void prvDeleteTCB( TCB_t *pxTCB ) PRIVILEGED_FUNCTION;

#endif

//Function to call the Thread Local Storage Pointer Deletion Callbacks. Will be
//called during task deletion before prvDeleteTCB is called.
#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) && ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )
	static void prvDeleteTLS( TCB_t *pxTCB );
#endif

/*
 * Used only by the idle task.  This checks to see if anything has been placed
 * in the list of tasks waiting to be deleted.  If so the task is cleaned up
 * and its TCB deleted.
 */
static void prvCheckTasksWaitingTermination( void ) PRIVILEGED_FUNCTION;

/*
 * The currently executing task is entering the Blocked state.  Add the task to
 * either the current or the overflow delayed task list.
 */
static void prvAddCurrentTaskToDelayedList( const portBASE_TYPE xCoreID, const TickType_t xTimeToWake ) PRIVILEGED_FUNCTION;

/*
 * Fills an TaskStatus_t structure with information on each task that is
 * referenced from the pxList list (which may be a ready list, a delayed list,
 * a suspended list, etc.).
 *
 * THIS FUNCTION IS INTENDED FOR DEBUGGING ONLY, AND SHOULD NOT BE CALLED FROM
 * NORMAL APPLICATION CODE.
 */
#if ( configUSE_TRACE_FACILITY == 1 )

	static UBaseType_t prvListTaskWithinSingleList( TaskStatus_t *pxTaskStatusArray, List_t *pxList, eTaskState eState ) PRIVILEGED_FUNCTION;

#endif

/*
 * When a task is created, the stack of the task is filled with a known value.
 * This function determines the 'high water mark' of the task stack by
 * determining how much of the stack remains at the original preset value.
 */
#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )

	static uint32_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte ) PRIVILEGED_FUNCTION;

#endif

/*
 * Return the amount of time, in ticks, that will pass before the kernel will
 * next move a task from the Blocked state to the Running state.
 *
 * This conditional compilation should use inequality to 0, not equality to 1.
 * This is to ensure portSUPPRESS_TICKS_AND_SLEEP() can be called when user
 * defined low power mode implementations require configUSE_TICKLESS_IDLE to be
 * set to a value other than 1.
 */
#if ( configUSE_TICKLESS_IDLE != 0 )

	static TickType_t prvGetExpectedIdleTime( void ) PRIVILEGED_FUNCTION;

#endif

/*
 * Set xNextTaskUnblockTime to the time at which the next Blocked state task
 * will exit the Blocked state.
 */
static void prvResetNextTaskUnblockTime( void );

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

	/*
	 * Helper function used to pad task names with spaces when printing out
	 * human readable tables of task information.
	 */
	static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName );

#endif

/*
 * Called after a Task_t structure has been allocated either statically or
 * dynamically to fill in the structure's members.
 */
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,
									const char * const pcName,
									const uint32_t ulStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									TaskHandle_t * const pxCreatedTask,
									TCB_t *pxNewTCB,
									const MemoryRegion_t * const xRegions, const BaseType_t xCoreID) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/*
 * Called after a new task has been created and initialised to place the task
 * under the control of the scheduler.
 */
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB, TaskFunction_t pxTaskCode, const BaseType_t xCoreID ) PRIVILEGED_FUNCTION;



/*-----------------------------------------------------------*/

/*
 * This routine tries to send an interrupt to another core if needed to make it execute a task
 * of higher priority. We try to figure out if needed first by inspecting the pxTCB of the
 * other CPU first. Specifically for Xtensa, we can do this because pxTCB is an atomic pointer. It
 * is possible that it is inaccurate because the other CPU just did a task switch, but in that case
 * at most a superfluous interrupt is generated.
*/
void taskYIELD_OTHER_CORE( BaseType_t xCoreID, UBaseType_t uxPriority )
{
	TCB_t *curTCB;
	BaseType_t i;

	if (xCoreID != tskNO_AFFINITY) {
		curTCB = pxCurrentTCB[xCoreID];
		if ( curTCB->uxPriority < uxPriority ) {	// NOLINT(clang-analyzer-core.NullDereference) IDF-685
			vPortYieldOtherCore( xCoreID );
		}
	}
	else
	{
		/* The task has no affinity. See if we can find a CPU to put it on.*/
		for (i=0; i<portNUM_PROCESSORS; i++) {
			if (i != xPortGetCoreID() && pxCurrentTCB[ i ]->uxPriority < uxPriority)
			{
				vPortYieldOtherCore( i );
				break;
			}
		}
	}
}

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

	TaskHandle_t xTaskCreateStaticPinnedToCore(	TaskFunction_t pxTaskCode,
									const char * const pcName,
									const uint32_t ulStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									StackType_t * const puxStackBuffer,
									StaticTask_t * const pxTaskBuffer,
                                    const BaseType_t xCoreID )
	{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;

		configASSERT( portVALID_TCB_MEM(pxTaskBuffer) );
		configASSERT( portVALID_STACK_MEM(puxStackBuffer) );
		configASSERT( (xCoreID>=0 && xCoreID<portNUM_PROCESSORS) || (xCoreID==tskNO_AFFINITY) );

		if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
		{
			/* The memory used for the task's TCB and stack are passed into this
			function - use them. */
			pxNewTCB = ( TCB_t * ) pxTaskBuffer; /*lint !e740 Unusual cast is ok as the structures are designed to have the same alignment, and the size is checked by an assert. */
			pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;

			#if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
			{
				/* Tasks can be created statically or dynamically, so note this
				task was created statically in case the task is later deleted. */
				pxNewTCB->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_AND_TCB;
			}
			#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

			prvInitialiseNewTask( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, &xReturn, pxNewTCB, NULL, xCoreID );
			prvAddNewTaskToReadyList( pxNewTCB, pxTaskCode, xCoreID );
		}
		else
		{
			xReturn = NULL;
		}

		return xReturn;
	}

#endif /* SUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( portUSING_MPU_WRAPPERS == 1 )

	BaseType_t xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask )
	{
	TCB_t *pxNewTCB;
	BaseType_t xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

		configASSERT( pxTaskDefinition->puxStackBuffer );

		if( pxTaskDefinition->puxStackBuffer != NULL )
		{
			/* Allocate space for the TCB.  Where the memory comes from depends
			on the implementation of the port malloc function and whether or
			not static allocation is being used. */
			pxNewTCB = ( TCB_t * ) pvPortMallocTcbMem( sizeof( TCB_t ) );

			if( pxNewTCB != NULL )
			{
				/* Store the stack location in the TCB. */
				pxNewTCB->pxStack = pxTaskDefinition->puxStackBuffer;

				/* Tasks can be created statically or dynamically, so note
				this task had a statically allocated stack in case it is
				later deleted.  The TCB was allocated dynamically. */
				pxNewTCB->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_ONLY;

				prvInitialiseNewTask(	pxTaskDefinition->pvTaskCode,
										pxTaskDefinition->pcName,
										pxTaskDefinition->usStackDepth,
										pxTaskDefinition->pvParameters,
										pxTaskDefinition->uxPriority,
										pxCreatedTask, pxNewTCB,
										pxTaskDefinition->xRegions,
										tskNO_AFFINITY );

				prvAddNewTaskToReadyList( pxNewTCB, pxTaskDefinition->pvTaskCode, tskNO_AFFINITY );
				xReturn = pdPASS;
			}
		}

		return xReturn;
	}

    BaseType_t xTaskCreateRestrictedPinnedToCore(const TaskParameters_t *const pxTaskDefinition, TaskHandle_t *pxCreatedTask, const BaseType_t xCoreID) 
    {
        TCB_t *pxNewTCB;
        BaseType_t xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

        configASSERT(pxTaskDefinition->puxStackBuffer);

        if (pxTaskDefinition->puxStackBuffer != NULL)
        {
            /* Allocate space for the TCB.  Where the memory comes from depends
 			on the implementation of the port malloc function and whether or
 			not static allocation is being used. */
            pxNewTCB = (TCB_t *)pvPortMallocTcbMem(sizeof(TCB_t));
            if (pxNewTCB != NULL)
            {
                /* Store the stack location in the TCB. */
                pxNewTCB->pxStack = pxTaskDefinition->puxStackBuffer;
                /* Tasks can be created statically or dynamically, so note
   				this task had a statically allocated stack in case it is
   				later deleted.  The TCB was allocated dynamically. */
                pxNewTCB->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_ONLY;
                prvInitialiseNewTask(	pxTaskDefinition->pvTaskCode,
										pxTaskDefinition->pcName,
                                        pxTaskDefinition->usStackDepth,
										pxTaskDefinition->pvParameters,
                                        pxTaskDefinition->uxPriority,
										pxCreatedTask, pxNewTCB,
                                        pxTaskDefinition->xRegions,
										xCoreID);

                prvAddNewTaskToReadyList(pxNewTCB, pxTaskDefinition->pvTaskCode, xCoreID);
                xReturn = pdPASS;
            }
        }
        return xReturn;
    }

#endif /* portUSING_MPU_WRAPPERS */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

        BaseType_t
        xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char *const pcName, const uint32_t usStackDepth,
                                void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask,
                                const BaseType_t xCoreID)
	{
	TCB_t *pxNewTCB;
	BaseType_t xReturn;

		/* If the stack grows down then allocate the stack then the TCB so the stack
		does not grow into the TCB.  Likewise if the stack grows up then allocate
		the TCB then the stack. */
		#if( portSTACK_GROWTH > 0 )
		{
			/* Allocate space for the TCB.  Where the memory comes from depends on
			the implementation of the port malloc function and whether or not static
			allocation is being used. */
			pxNewTCB = ( TCB_t * ) pvPortMallocTcbMem( sizeof( TCB_t ) );

			if( pxNewTCB != NULL )
			{
				/* Allocate space for the stack used by the task being created.
				The base of the stack memory stored in the TCB so the task can
				be deleted later if required. */
				pxNewTCB->pxStack = ( StackType_t * ) pvPortMallocStackMem( ( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

				if( pxNewTCB->pxStack == NULL )
				{
					/* Could not allocate the stack.  Delete the allocated TCB. */
					vPortFree( pxNewTCB );
					pxNewTCB = NULL;
				}
			}
		}
		#else /* portSTACK_GROWTH */
		{
		StackType_t *pxStack;

			/* Allocate space for the stack used by the task being created. */
			pxStack = ( StackType_t * ) pvPortMallocStackMem( ( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

			if( pxStack != NULL )
			{
				/* Allocate space for the TCB. */
				pxNewTCB = ( TCB_t * ) pvPortMallocTcbMem( sizeof( TCB_t ) ); /*lint !e961 MISRA exception as the casts are only redundant for some paths. */

				if( pxNewTCB != NULL )
				{
					/* Store the stack location in the TCB. */
					pxNewTCB->pxStack = pxStack;
				}
				else
				{
					/* The stack cannot be used as the TCB was not created.  Free
					it again. */
					vPortFree( pxStack );
				}
			}
			else
			{
				pxNewTCB = NULL;
			}
		}
		#endif /* portSTACK_GROWTH */

		if( pxNewTCB != NULL )
		{
			#if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
			{
				/* Tasks can be created statically or dynamically, so note this
				task was created dynamically in case it is later deleted. */
				pxNewTCB->ucStaticallyAllocated = tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB;
			}
			#endif /* configSUPPORT_STATIC_ALLOCATION */

			prvInitialiseNewTask( pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask, pxNewTCB, NULL, xCoreID );
			prvAddNewTaskToReadyList( pxNewTCB, pxTaskCode, xCoreID );
			xReturn = pdPASS;
		}
		else
		{
			xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
		}

		return xReturn;
	}

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,
									const char * const pcName,
									const uint32_t ulStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									TaskHandle_t * const pxCreatedTask,
									TCB_t *pxNewTCB,
									const MemoryRegion_t * const xRegions, const BaseType_t xCoreID ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
{
StackType_t *pxTopOfStack;
UBaseType_t x;

	#if( portUSING_MPU_WRAPPERS == 1 )
		/* Should the task be created in privileged mode? */
		BaseType_t xRunPrivileged;
		if( ( uxPriority & portPRIVILEGE_BIT ) != 0U )
		{
			xRunPrivileged = pdTRUE;
		}
		else
		{
			xRunPrivileged = pdFALSE;
		}
		uxPriority &= ~portPRIVILEGE_BIT;
	#endif /* portUSING_MPU_WRAPPERS == 1 */

	/* Avoid dependency on memset() if it is not required. */
	#if( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) || ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )
	{
		/* Fill the stack with a known value to assist debugging. */
		( void ) memset( pxNewTCB->pxStack, ( int ) tskSTACK_FILL_BYTE, ( size_t ) ulStackDepth * sizeof( StackType_t ) );
	}
	#endif /* ( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) || ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) ) ) */

	/* Calculate the top of stack address.  This depends on whether the stack
	grows from high memory to low (as per the 80x86) or vice versa.
	portSTACK_GROWTH is used to make the result positive or negative as required
	by the port. */
	#if( portSTACK_GROWTH < 0 )
	{
		pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
		pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) ); /*lint !e923 MISRA exception.  Avoiding casts between pointers and integers is not practical.  Size differences accounted for using portPOINTER_SIZE_TYPE type. */

		/* Check the alignment of the calculated top of stack is correct. */
		configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );
		#if ( configENABLE_TASK_SNAPSHOT == 1 )
		{
			/* need stack end for core dumps */
			pxNewTCB->pxEndOfStack = pxTopOfStack;
		}
#endif
	}
	#else /* portSTACK_GROWTH */
	{
		pxTopOfStack = pxNewTCB->pxStack;

		/* Check the alignment of the stack buffer is correct. */
		configASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxNewTCB->pxStack & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );

		/* The other extreme of the stack space is required if stack checking is
		performed. */
		pxNewTCB->pxEndOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	}
	#endif /* portSTACK_GROWTH */

	/* Store the task name in the TCB. */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		/* Don't copy all configMAX_TASK_NAME_LEN if the string is shorter than
		configMAX_TASK_NAME_LEN characters just in case the memory after the
		string is not accessible (extremely unlikely). */
		if( pcName[ x ] == 0x00 )
		{
			break;
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

	/* Ensure the name string is terminated in the case that the string length
	was greater or equal to configMAX_TASK_NAME_LEN. */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

	/* This is used as an array index so must ensure it's not too large.  First
	remove the privilege bit if one is present. */
	if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
	{
		uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	pxNewTCB->uxPriority = uxPriority;
	pxNewTCB->xCoreID = xCoreID;
	#if ( configUSE_MUTEXES == 1 )
	{
		pxNewTCB->uxBasePriority = uxPriority;
		pxNewTCB->uxMutexesHeld = 0;
	}
	#endif /* configUSE_MUTEXES */

	vListInitialiseItem( &( pxNewTCB->xGenericListItem ) );
	vListInitialiseItem( &( pxNewTCB->xEventListItem ) );

	/* Set the pxNewTCB as a link back from the ListItem_t.  This is so we can get
	back to	the containing TCB from a generic item in a list. */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xGenericListItem ), pxNewTCB );

	/* Event lists are always in priority order. */
	listSET_LIST_ITEM_VALUE( &( pxNewTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxPriority ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xEventListItem ), pxNewTCB );

	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
	{
		pxNewTCB->uxCriticalNesting = ( UBaseType_t ) 0U;
	}
	#endif /* portCRITICAL_NESTING_IN_TCB */

	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	{
		pxNewTCB->pxTaskTag = NULL;
	}
	#endif /* configUSE_APPLICATION_TASK_TAG */

	#if ( configGENERATE_RUN_TIME_STATS == 1 )
	{
		pxNewTCB->ulRunTimeCounter = 0UL;
	}
	#endif /* configGENERATE_RUN_TIME_STATS */

	#if ( portUSING_MPU_WRAPPERS == 1 )
	{
		vPortStoreTaskMPUSettings( &( pxNewTCB->xMPUSettings ), xRegions, pxNewTCB->pxStack, ulStackDepth );
	}
	#else
	{
		/* Avoid compiler warning about unreferenced parameter. */
		( void ) xRegions;
	}
	#endif

	#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
	{
		for( x = 0; x < ( UBaseType_t ) configNUM_THREAD_LOCAL_STORAGE_POINTERS; x++ )
		{
			pxNewTCB->pvThreadLocalStoragePointers[ x ] = NULL;
			#if ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS == 1)
			pxNewTCB->pvThreadLocalStoragePointersDelCallback[ x ] = NULL;
			#endif
		}
	}
	#endif

	#if ( configUSE_TASK_NOTIFICATIONS == 1 )
	{
		pxNewTCB->ulNotifiedValue = 0;
		pxNewTCB->eNotifyState = eNotWaitingNotification;
	}
	#endif

	#if ( configUSE_NEWLIB_REENTRANT == 1 )
	{
		/* Initialise this task's Newlib reent structure. */
		esp_reent_init(&pxNewTCB->xNewLib_reent);
	}
	#endif

	#if( INCLUDE_xTaskAbortDelay == 1 )
	{
		pxNewTCB->ucDelayAborted = pdFALSE;
	}
	#endif

	/* Initialize the TCB stack to look as if the task was already running,
	but had been interrupted by the scheduler.  The return address is set
	to the start of the task function. Once the stack has been initialised
	the	top of stack variable is updated. */
	#if( portUSING_MPU_WRAPPERS == 1 )
	{
		pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters, xRunPrivileged );
	}
	#else /* portUSING_MPU_WRAPPERS */
	{
		pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );
	}
	#endif /* portUSING_MPU_WRAPPERS */

	if( ( void * ) pxCreatedTask != NULL )
	{
		/* Pass the handle out in an anonymous way.  The handle can be used to
		change the created task's priority, delete the created task, etc.*/
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}
/*-----------------------------------------------------------*/

static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB, TaskFunction_t pxTaskCode, BaseType_t xCoreID )
{
	TCB_t *curTCB, *tcb0, *tcb1;

	/* Assure that xCoreID is valid or we'll have an out-of-bounds on pxCurrentTCB
	   You will assert here if e.g. you only have one CPU enabled in menuconfig and
	   are trying to start a task on core 1. */
	configASSERT( xCoreID == tskNO_AFFINITY || xCoreID < portNUM_PROCESSORS);

    /* Ensure interrupts don't access the task lists while the lists are being
	updated. */
	taskENTER_CRITICAL(&xTaskQueueMutex);
	{
		uxCurrentNumberOfTasks++;

		// Determine which core this task starts on
		if ( xCoreID == tskNO_AFFINITY )
		{
			if ( portNUM_PROCESSORS == 1 )
			{
				xCoreID = 0;
			}
			else
			{
				// if the task has no affinity, put it on either core if nothing is currently scheduled there. Failing that,
				// put it on the core where it will preempt the lowest priority running task. If neither of these are true,
				// queue it on the currently running core.
				tcb0 = pxCurrentTCB[0];
				tcb1 = pxCurrentTCB[1];
				if ( tcb0 == NULL )
				{
					xCoreID = 0;
				}
				else if ( tcb1 == NULL )
				{
					xCoreID = 1;
				}
				else if ( tcb0->uxPriority < pxNewTCB->uxPriority && tcb0->uxPriority < tcb1->uxPriority )
				{
					xCoreID = 0;
				}
				else if ( tcb1->uxPriority < pxNewTCB->uxPriority )
				{
					xCoreID = 1;
				}
				else
				{
					xCoreID = xPortGetCoreID(); // Both CPU have higher priority tasks running on them, so this won't run yet
				}
			}
		}

        // If nothing is running on this core, put the new task there now
		if( pxCurrentTCB[ xCoreID ] == NULL )
		{
			/* There are no other tasks, or all the other tasks are in
			the suspended state - make this the current task. */
			pxCurrentTCB[ xCoreID ] = pxNewTCB;

			if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
			{
#if portFIRST_TASK_HOOK
				if ( xPortGetCoreID() == 0 ) {
					vPortFirstTaskHook(pxTaskCode);
				}
#endif /* configFIRST_TASK_HOOK */
				/* This is the first task to be created so do the preliminary
				initialisation required.  We will not recover if this call
				fails, but we will report the failure. */
				prvInitialiseTaskLists();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			/* If the scheduler is not already running, make this task the
			current task if it is the highest priority task to be created
			so far. */
			if( xSchedulerRunning == pdFALSE )
			{
				/* Scheduler isn't running yet. We need to determine on which CPU to run this task.
				   Schedule now if either nothing is scheduled yet or we can replace a task of lower prio. */
				if ( pxCurrentTCB[xCoreID] == NULL || pxCurrentTCB[xCoreID]->uxPriority <= pxNewTCB->uxPriority )
				{
					pxCurrentTCB[xCoreID] = pxNewTCB;
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}

		uxTaskNumber++;

		#if ( configUSE_TRACE_FACILITY == 1 )
		{
			/* Add a counter into the TCB for tracing only. */
			pxNewTCB->uxTCBNumber = uxTaskNumber;
		}
		#endif /* configUSE_TRACE_FACILITY */
		traceTASK_CREATE( pxNewTCB );

		prvAddTaskToReadyList( pxNewTCB );

		portSETUP_TCB( pxNewTCB );
	}

	taskEXIT_CRITICAL(&xTaskQueueMutex);

	if( xSchedulerRunning != pdFALSE )
	{
		taskENTER_CRITICAL(&xTaskQueueMutex);

		curTCB = pxCurrentTCB[ xCoreID ];
		/* Scheduler is running. If the created task is of a higher priority than an executing task
		   then it should run now.
		*/
		if( curTCB == NULL || curTCB->uxPriority < pxNewTCB->uxPriority )
		{
			if( xCoreID == xPortGetCoreID() )
			{
				taskYIELD_IF_USING_PREEMPTION();
			}
			else {
				taskYIELD_OTHER_CORE(xCoreID, pxNewTCB->uxPriority);
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )

	void vTaskDelete( TaskHandle_t xTaskToDelete )
	{
	//The following vTaskDelete() is backported from FreeRTOS v9.0.0 and modified for SMP.
	//v9.0.0 vTaskDelete() will immediately free task memory if the task being deleted is
	//NOT currently running and not pinned to the other core. Otherwise, freeing of task memory
	//will still be delegated to the Idle Task.

	TCB_t *pxTCB;
	int core = xPortGetCoreID();	//Current core
	UBaseType_t free_now;	//Flag to indicate if task memory can be freed immediately

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* If null is passed in here then it is the calling task that is
			being deleted. */
			pxTCB = prvGetTCBFromHandle( xTaskToDelete );

			/* Remove task from the ready list. */
			if( uxListRemove( &( pxTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
			{
				taskRESET_READY_PRIORITY( pxTCB->uxPriority );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* Is the task waiting on an event also? */
			if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
			{
				( void ) uxListRemove( &( pxTCB->xEventListItem ) );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* Increment the uxTaskNumber also so kernel aware debuggers can
			detect that the task lists need re-generating.  This is done before
			portPRE_TASK_DELETE_HOOK() as in the Windows port that macro will
			not return. */
			uxTaskNumber++;

			//If task to be deleted is currently running on either core or is pinned to the other core. Let Idle free memory
			if( pxTCB == pxCurrentTCB[ core ] ||
				(portNUM_PROCESSORS > 1 && pxTCB == pxCurrentTCB[ !core ]) ||
				(portNUM_PROCESSORS > 1 && pxTCB->xCoreID == (!core)) )
			{
				/* Deleting a currently running task. This cannot complete
				within the task itself, as a context switch to another task is
				required. Place the task in the termination list.  The idle task
				will check the termination list and free up any memory allocated
				by the scheduler for the TCB and stack of the deleted task. */
				vListInsertEnd( &xTasksWaitingTermination, &( pxTCB->xGenericListItem ) );

				/* Increment the ucTasksDeleted variable so the idle task knows
				there is a task that has been deleted and that it should therefore
				check the xTasksWaitingTermination list. */
				++uxTasksDeleted;

				/* The pre-delete hook is primarily for the Windows simulator,
				in which Windows specific clean up operations are performed,
				after which it is not possible to yield away from this task -
				hence xYieldPending is used to latch that a context switch is
				required. */
				portPRE_TASK_DELETE_HOOK( pxTCB, &xYieldPending );

				free_now = pdFALSE;		//Let Idle Task free task memory
			}
			else	//Task is not currently running and not pinned to the other core
			{
				--uxCurrentNumberOfTasks;

				/* Reset the next expected unblock time in case it referred to
				the task that has just been deleted. */
				prvResetNextTaskUnblockTime();
				free_now = pdTRUE;		//Set flag to free task memory immediately
			}

			traceTASK_DELETE( pxTCB );
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		if(free_now == pdTRUE){		//Free task memory. Outside critical section due to deletion callbacks
			#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) && ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )
				prvDeleteTLS( pxTCB );	//Run deletion callbacks before deleting TCB
			#endif
			prvDeleteTCB( pxTCB );	//Must only be called after del cb
		}

		/* Force a reschedule if it is the currently running task that has just
		been deleted. */
		if( xSchedulerRunning != pdFALSE )
		{
			//No mux; no harm done if this misfires. The deleted task won't get scheduled anyway.
			if( pxTCB == pxCurrentTCB[ core ] )	//If task was currently running on this core
			{
				configASSERT( xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED )

				/* The pre-delete hook is primarily for the Windows simulator,
				in which Windows specific clean up operations are performed,
				after which it is not possible to yield away from this task -
				hence xYieldPending is used to latch that a context switch is
				required. */
				portPRE_TASK_DELETE_HOOK( pxTCB, &xYieldPending[xPortGetCoreID()] );
				portYIELD_WITHIN_API();
			}
			else if ( portNUM_PROCESSORS > 1 && pxTCB == pxCurrentTCB[ !core] )	//If task was currently running on the other core
			{
				/* if task is running on the other CPU, force a yield on that CPU to take it off */
				vPortYieldOtherCore( !core );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
	}

#endif /* INCLUDE_vTaskDelete */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelayUntil == 1 )

/* ToDo: Make this multicore-compatible. */
	void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement )
	{
	TickType_t xTimeToWake;
	BaseType_t xAlreadyYielded=pdFALSE, xShouldDelay = pdFALSE;

		configASSERT( pxPreviousWakeTime );
		configASSERT( ( xTimeIncrement > 0U ) );
		configASSERT( xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED );

		taskENTER_CRITICAL(&xTaskQueueMutex);
//		vTaskSuspendAll();
		{
			/* Minor optimisation.  The tick count cannot change in this
			block. */
			const TickType_t xConstTickCount = xTickCount;

			/* Generate the tick time at which the task wants to wake. */
			xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;

			if( xConstTickCount < *pxPreviousWakeTime )
			{
				/* The tick count has overflowed since this function was
				lasted called.  In this case the only time we should ever
				actually delay is if the wake time has also	overflowed,
				and the wake time is greater than the tick time.  When this
				is the case it is as if neither time had overflowed. */
				if( ( xTimeToWake < *pxPreviousWakeTime ) && ( xTimeToWake > xConstTickCount ) )
				{
					xShouldDelay = pdTRUE;
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				/* The tick time has not overflowed.  In this case we will
				delay if either the wake time has overflowed, and/or the
				tick time is less than the wake time. */
				if( ( xTimeToWake < *pxPreviousWakeTime ) || ( xTimeToWake > xConstTickCount ) )
				{
					xShouldDelay = pdTRUE;
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}

			/* Update the wake time ready for the next call. */
			*pxPreviousWakeTime = xTimeToWake;

			if( xShouldDelay != pdFALSE )
			{
				traceTASK_DELAY_UNTIL();

				/* Remove the task from the ready list before adding it to the
				blocked list as the same list item is used for both lists. */
				if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
				{
					/* The current task must be in a ready list, so there is
					no need to check, and the port reset macro can be called
					directly. */
					portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
//		xAlreadyYielded = xTaskResumeAll();
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		/* Force a reschedule if xTaskResumeAll has not already done so, we may
		have put ourselves to sleep. */
		if( xAlreadyYielded == pdFALSE )
		{
			portYIELD_WITHIN_API();
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* INCLUDE_vTaskDelayUntil */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
	void vTaskDelay( const TickType_t xTicksToDelay )
	{
	TickType_t xTimeToWake;
	BaseType_t xAlreadyYielded = pdFALSE;

		/* A delay time of zero just forces a reschedule. */
		if( xTicksToDelay > ( TickType_t ) 0U )
		{
			configASSERT( xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED );
			taskENTER_CRITICAL(&xTaskQueueMutex);
//			vTaskSuspendAll();
			{
				traceTASK_DELAY();

				/* A task that is removed from the event list while the
				scheduler is suspended will not get placed in the ready
				list or removed from the blocked list until the scheduler
				is resumed.

				This task cannot be in an event list as it is the currently
				executing task. */

				/* Calculate the time to wake - this may overflow but this is
				not a problem. */
				xTimeToWake = xTickCount + xTicksToDelay;

				/* We must remove ourselves from the ready list before adding
				ourselves to the blocked list as the same list item is used for
				both lists. */
				if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
				{
					/* The current task must be in a ready list, so there is
					no need to check, and the port reset macro can be called
					directly. */
					portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
				prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
			}
//			xAlreadyYielded = xTaskResumeAll();
			taskEXIT_CRITICAL(&xTaskQueueMutex);
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		/* Force a reschedule if xTaskResumeAll has not already done so, we may
		have put ourselves to sleep. */
		if( xAlreadyYielded == pdFALSE )
		{
			portYIELD_WITHIN_API();
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* INCLUDE_vTaskDelay */
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
	eTaskState eTaskGetState( TaskHandle_t xTask )
	{
	eTaskState eReturn;
	List_t *pxStateList;
	const TCB_t * const pxTCB = ( TCB_t * ) xTask;
		TCB_t * curTCBcurCore = xTaskGetCurrentTaskHandle();
		TCB_t * curTCBothrCore = xTaskGetCurrentTaskHandleForCPU(!xPortGetCoreID());    //Returns NULL if Unicore

		configASSERT( pxTCB );

		if( pxTCB == curTCBcurCore || pxTCB == curTCBothrCore )
		{
			/* The task calling this function is querying its own state. */
			eReturn = eRunning;
		}
		else
		{
			taskENTER_CRITICAL(&xTaskQueueMutex);
			{
				pxStateList = ( List_t * ) listLIST_ITEM_CONTAINER( &( pxTCB->xGenericListItem ) );
			}
			taskEXIT_CRITICAL(&xTaskQueueMutex);

			if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )
			{
				/* The task being queried is referenced from one of the Blocked
				lists. */
				eReturn = eBlocked;
			}

			#if ( INCLUDE_vTaskSuspend == 1 )
				else if( pxStateList == &xSuspendedTaskList )
				{
					/* The task being queried is referenced from the suspended
					list.  Is it genuinely suspended or is it block
					indefinitely? */
					if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL )
					{
						eReturn = eSuspended;
					}
					else
					{
						eReturn = eBlocked;
					}
				}
			#endif

			#if ( INCLUDE_vTaskDelete == 1 )
				else if( pxStateList == &xTasksWaitingTermination )
				{
					/* The task being queried is referenced from the deleted
					tasks list. */
					eReturn = eDeleted;
				}
			#endif

			else /*lint !e525 Negative indentation is intended to make use of pre-processor clearer. */
			{
				/* If the task is not in any other state, it must be in the
				Ready (including pending ready) state. */
				eReturn = eReady;
			}
		}

		return eReturn;
	} /*lint !e818 xTask cannot be a pointer to const because it is a typedef. */

#endif /* INCLUDE_eTaskGetState */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
	UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask )
	{
	TCB_t *pxTCB;
	UBaseType_t uxReturn;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* If null is passed in here then we are changing the
			priority of the calling function. */
			pxTCB = prvGetTCBFromHandle( xTask );
			uxReturn = pxTCB->uxPriority;
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return uxReturn;
	}

#endif /* INCLUDE_uxTaskPriorityGet */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
	UBaseType_t uxTaskPriorityGetFromISR( TaskHandle_t xTask )
	{
	TCB_t *pxTCB;
	UBaseType_t uxReturn;

		taskENTER_CRITICAL_ISR(&xTaskQueueMutex);
		{
			/* If null is passed in here then it is the priority of the calling
			task that is being queried. */
			pxTCB = prvGetTCBFromHandle( xTask );
			uxReturn = pxTCB->uxPriority;
		}
		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);

		return uxReturn;
	}

#endif /* INCLUDE_uxTaskPriorityGet */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )

	void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority )
	{
	TCB_t *pxTCB;
	UBaseType_t uxCurrentBasePriority, uxPriorityUsedOnEntry;
	BaseType_t xYieldRequired = pdFALSE;

		configASSERT( ( uxNewPriority < configMAX_PRIORITIES ) );

		/* Ensure the new priority is valid. */
		if( uxNewPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
		{
			uxNewPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* If null is passed in here then it is the priority of the calling
			task that is being changed. */
			pxTCB = prvGetTCBFromHandle( xTask );

			traceTASK_PRIORITY_SET( pxTCB, uxNewPriority );

			#if ( configUSE_MUTEXES == 1 )
			{
				uxCurrentBasePriority = pxTCB->uxBasePriority;
			}
			#else
			{
				uxCurrentBasePriority = pxTCB->uxPriority;
			}
			#endif

			if( uxCurrentBasePriority != uxNewPriority )
			{
				/* The priority change may have readied a task of higher
				priority than the calling task. */
				if( uxNewPriority > uxCurrentBasePriority )
				{
					if( pxTCB != pxCurrentTCB[ xPortGetCoreID() ] )
					{
						/* The priority of a task other than the currently
						running task is being raised.  Is the priority being
						raised above that of the running task? */
						if ( tskCAN_RUN_HERE(pxTCB->xCoreID) && uxNewPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
						{
							xYieldRequired = pdTRUE;
						}
						else if ( pxTCB->xCoreID != xPortGetCoreID() )
						{
							taskYIELD_OTHER_CORE( pxTCB->xCoreID, uxNewPriority );
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}
					}
					else
					{
						/* The priority of the running task is being raised,
						but the running task must already be the highest
						priority task able to run so no yield is required. */
					}
				}
				else if( pxTCB == pxCurrentTCB[ xPortGetCoreID() ] )
				{
					/* Setting the priority of the running task down means
					there may now be another task of higher priority that
					is ready to execute. */
					xYieldRequired = pdTRUE;
				}
				else
				{
					/* Setting the priority of any other task down does not
					require a yield as the running task must be above the
					new priority of the task being modified. */
				}

				/* Remember the ready list the task might be referenced from
				before its uxPriority member is changed so the
				taskRESET_READY_PRIORITY() macro can function correctly. */
				uxPriorityUsedOnEntry = pxTCB->uxPriority;

				#if ( configUSE_MUTEXES == 1 )
				{
					/* Only change the priority being used if the task is not
					currently using an inherited priority. */
					if( pxTCB->uxBasePriority == pxTCB->uxPriority )
					{
						pxTCB->uxPriority = uxNewPriority;
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/* The base priority gets set whatever. */
					pxTCB->uxBasePriority = uxNewPriority;
				}
				#else
				{
					pxTCB->uxPriority = uxNewPriority;
				}
				#endif

				/* Only reset the event list item value if the value is not
				being used for anything else. */
				if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
				{
					listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxNewPriority ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				/* If the task is in the blocked or suspended list we need do
				nothing more than change it's priority variable. However, if
				the task is in a ready list it needs to be removed and placed
				in the list appropriate to its new priority. */
				if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxPriorityUsedOnEntry ] ), &( pxTCB->xGenericListItem ) ) != pdFALSE )
				{
					/* The task is currently in its ready list - remove before adding
					it to it's new ready list.  As we are in a critical section we
					can do this even if the scheduler is suspended. */
					if( uxListRemove( &( pxTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
					{
						/* It is known that the task is in its ready list so
						there is no need to check again and the port level
						reset macro can be called directly. */
						portRESET_READY_PRIORITY( uxPriorityUsedOnEntry, uxTopReadyPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
                    prvReaddTaskToReadyList( pxTCB );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				if( xYieldRequired == pdTRUE )
				{
					taskYIELD_IF_USING_PREEMPTION();
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				/* Remove compiler warning about unused variables when the port
				optimised task selection is not being used. */
				( void ) uxPriorityUsedOnEntry;
			}
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);
	}

#endif /* INCLUDE_vTaskPrioritySet */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
	void vTaskSuspend( TaskHandle_t xTaskToSuspend )
	{
	TCB_t *pxTCB;
        TCB_t *curTCB;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* If null is passed in here then it is the running task that is
			being suspended. */
			pxTCB = prvGetTCBFromHandle( xTaskToSuspend );

			traceTASK_SUSPEND( pxTCB );

			/* Remove task from the ready/delayed list and place in the
			suspended list. */
			if( uxListRemove( &( pxTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
			{
				taskRESET_READY_PRIORITY( pxTCB->uxPriority );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* Is the task waiting on an event also? */
			if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
			{
				( void ) uxListRemove( &( pxTCB->xEventListItem ) );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
            traceMOVED_TASK_TO_SUSPENDED_LIST(pxTCB);
			vListInsertEnd( &xSuspendedTaskList, &( pxTCB->xGenericListItem ) );
			curTCB = pxCurrentTCB[ xPortGetCoreID() ];
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		if( pxTCB == curTCB )
		{
			if( xSchedulerRunning != pdFALSE )
			{
				/* The current task has just been suspended. */
				configASSERT( xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED );
				portYIELD_WITHIN_API();
			}
			else
			{
				/* The scheduler is not running, but the task that was pointed
				to by pxCurrentTCB has just been suspended and pxCurrentTCB
				must be adjusted to point to a different task. */
				if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == uxCurrentNumberOfTasks )
				{
					/* No other tasks are ready, so set pxCurrentTCB back to
					NULL so when the next task is created pxCurrentTCB will
					be set to point to it no matter what its relative priority
					is. */
					taskENTER_CRITICAL(&xTaskQueueMutex);
					pxCurrentTCB[ xPortGetCoreID() ] = NULL;
					taskEXIT_CRITICAL(&xTaskQueueMutex);
				}
				else
				{
					vTaskSwitchContext();
				}
			}
		}
		else
		{
			if( xSchedulerRunning != pdFALSE )
			{
				/* A task other than the currently running task was suspended,
				reset the next expected unblock time in case it referred to the
				task that is now in the Suspended state. */
				taskENTER_CRITICAL(&xTaskQueueMutex);
				{
					prvResetNextTaskUnblockTime();
				}
				taskEXIT_CRITICAL(&xTaskQueueMutex);
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
	}

#endif /* INCLUDE_vTaskSuspend */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
	static BaseType_t prvTaskIsTaskSuspended( const TaskHandle_t xTask )
	{
	BaseType_t xReturn = pdFALSE;
	const TCB_t * const pxTCB = ( TCB_t * ) xTask;

		/* Accesses xPendingReadyList so must be called from a critical
		   section (caller is required to hold xTaskQueueMutex). */

		/* It does not make sense to check if the calling task is suspended. */
		configASSERT( xTask );

		/* Is the task being resumed actually in the suspended list? */
		if( listIS_CONTAINED_WITHIN( &xSuspendedTaskList, &( pxTCB->xGenericListItem ) ) != pdFALSE )
		{
			/* Has the task already been resumed from within an ISR? */
			if( listIS_CONTAINED_WITHIN( &xPendingReadyList[ xPortGetCoreID() ], &( pxTCB->xEventListItem ) ) == pdFALSE )
			{
				/* Is it in the suspended list because it is in the	Suspended
				state, or because is is blocked with no timeout? */
				if( listIS_CONTAINED_WITHIN( NULL, &( pxTCB->xEventListItem ) ) != pdFALSE )
				{
					xReturn = pdTRUE;
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		return xReturn;
	} /*lint !e818 xTask cannot be a pointer to const because it is a typedef. */

#endif /* INCLUDE_vTaskSuspend */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )

	void vTaskResume( TaskHandle_t xTaskToResume )
	{
	TCB_t * const pxTCB = ( TCB_t * ) xTaskToResume;

		/* It does not make sense to resume the calling task. */
		configASSERT( xTaskToResume );

		taskENTER_CRITICAL(&xTaskQueueMutex);
		/* The parameter cannot be NULL as it is impossible to resume the
		currently executing task. */
		if( ( pxTCB != NULL ) && ( pxTCB != pxCurrentTCB[ xPortGetCoreID() ] ) )
		{
			{
				if( prvTaskIsTaskSuspended( pxTCB ) == pdTRUE )
				{
					traceTASK_RESUME( pxTCB );

					/* As we are in a critical section we can access the ready
					lists even if the scheduler is suspended. */
					( void ) uxListRemove(  &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );

					/* We may have just resumed a higher priority task. */
					if( tskCAN_RUN_HERE(pxTCB->xCoreID) && pxTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
					{
						/* This yield may not cause the task just resumed to run,
						but will leave the lists in the correct state for the
						next yield. */
						taskYIELD_IF_USING_PREEMPTION();
					}
					else if( pxTCB->xCoreID != xPortGetCoreID() )
					{
						taskYIELD_OTHER_CORE( pxTCB->xCoreID, pxTCB->uxPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);
	}

#endif /* INCLUDE_vTaskSuspend */

/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskResumeFromISR == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) )

	BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume )
	{
	BaseType_t xYieldRequired = pdFALSE;
	TCB_t * const pxTCB = ( TCB_t * ) xTaskToResume;

		configASSERT( xTaskToResume );

		taskENTER_CRITICAL_ISR(&xTaskQueueMutex);

		{
			if( prvTaskIsTaskSuspended( pxTCB ) == pdTRUE )
			{
				traceTASK_RESUME_FROM_ISR( pxTCB );

				/* Check the ready lists can be accessed. */
				if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
				{
					/* Ready lists can be accessed so move the task from the
					suspended list to the ready list directly. */
					( void ) uxListRemove(  &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );

					if( tskCAN_RUN_HERE( pxTCB->xCoreID ) && pxTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
					{
						xYieldRequired = pdTRUE;
					}
					else if ( pxTCB->xCoreID != xPortGetCoreID() )
					{
						taskYIELD_OTHER_CORE( pxTCB->xCoreID, pxTCB->uxPriority);
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}
				else
				{
					/* The delayed or ready lists cannot be accessed so the task
					is held in the pending ready list until the scheduler is
					unsuspended. */
					vListInsertEnd( &( xPendingReadyList[ xPortGetCoreID() ] ), &( pxTCB->xEventListItem ) );
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);

		return xYieldRequired;
	}

#endif /* ( ( INCLUDE_xTaskResumeFromISR == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) ) */
/*-----------------------------------------------------------*/

void vTaskStartScheduler( void )
{
BaseType_t xReturn;
BaseType_t i;

	/* Add the per-core idle tasks at the lowest priority. */
	for ( i=0; i<portNUM_PROCESSORS; i++) {
		//Generate idle task name
		char cIdleName[configMAX_TASK_NAME_LEN];
		snprintf(cIdleName, configMAX_TASK_NAME_LEN, "IDLE%d", i);
		#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
		{
			/* Create the idle task, storing its handle in xIdleTaskHandle so it can
			be returned by the xTaskGetIdleTaskHandle() function. */
			xReturn = xTaskCreatePinnedToCore( prvIdleTask, cIdleName, tskIDLE_STACK_SIZE, ( void * ) NULL, ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), &xIdleTaskHandle[i], i ); /*lint !e961 MISRA exception, justified as it is not a redundant explicit cast to all supported compilers. */
		}
		#else
		{
			/* Create the idle task without storing its handle. */
			xReturn = xTaskCreatePinnedToCore( prvIdleTask, cIdleName, tskIDLE_STACK_SIZE, ( void * ) NULL, ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), NULL, i);  /*lint !e961 MISRA exception, justified as it is not a redundant explicit cast to all supported compilers. */
		}
		#endif /* INCLUDE_xTaskGetIdleTaskHandle */
	}

	#if ( configUSE_TIMERS == 1 )
	{
		if( xReturn == pdPASS )
		{
			xReturn = xTimerCreateTimerTask();
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
	#endif /* configUSE_TIMERS */

	if( xReturn == pdPASS )
	{
		/* Interrupts are turned off here, to ensure a tick does not occur
		before or during the call to xPortStartScheduler().  The stacks of
		the created tasks contain a status word with interrupts switched on
		so interrupts will automatically get re-enabled when the first task
		starts to run. */
		portDISABLE_INTERRUPTS();


		xTickCount = ( TickType_t ) 0U;

		/* If configGENERATE_RUN_TIME_STATS is defined then the following
		macro must be defined to configure the timer/counter used to generate
		the run time counter time base. */
		portCONFIGURE_TIMER_FOR_RUN_TIME_STATS();
		xSchedulerRunning = pdTRUE;

		/* Setting up the timer tick is hardware specific and thus in the
		portable interface. */
		if( xPortStartScheduler() != pdFALSE )
		{
			/* Should not reach here as if the scheduler is running the
			function will not return. */
		}
		else
		{
			/* Should only reach here if a task calls xTaskEndScheduler(). */
		}
	}
	else
	{
		/* This line will only be reached if the kernel could not be started,
		because there was not enough FreeRTOS heap to create the idle task
		or the timer task. */
		configASSERT( xReturn );
	}
}
/*-----------------------------------------------------------*/

void vTaskEndScheduler( void )
{
	/* Stop the scheduler interrupts and call the portable scheduler end
	routine so the original ISRs can be restored if necessary.  The port
	layer must ensure interrupts enable	bit is left in the correct state. */
	portDISABLE_INTERRUPTS();
	xSchedulerRunning = pdFALSE;
	vPortEndScheduler();
}
/*----------------------------------------------------------*/


#if ( configUSE_NEWLIB_REENTRANT == 1 )
//Return global reent struct if FreeRTOS isn't running,
struct _reent* __getreent(void) {
	//No lock needed because if this changes, we won't be running anymore.
	TCB_t *currTask=xTaskGetCurrentTaskHandle();
	if (currTask==NULL) {
		//No task running. Return global struct.
		return _GLOBAL_REENT;
	} else {
		//We have a task; return its reentrant struct.
		return &currTask->xNewLib_reent;
	}
}
#endif


void vTaskSuspendAll( void )
{
	/* A critical section is not required as the variable is of type
	BaseType_t.  Please read Richard Barry's reply in the following link to a
	post in the FreeRTOS support forum before reporting this as a bug! -
	http://goo.gl/wu4acr */
	unsigned state;

	state = portENTER_CRITICAL_NESTED();
	++uxSchedulerSuspended[ xPortGetCoreID() ];
	portEXIT_CRITICAL_NESTED(state);
}
/*----------------------------------------------------------*/

#if ( configUSE_TICKLESS_IDLE != 0 )

#if ( portNUM_PROCESSORS > 1 )

	static BaseType_t xHaveReadyTasks( void )
	{
		for (int i = tskIDLE_PRIORITY + 1; i < configMAX_PRIORITIES; ++i)
		{
			if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ i ] ) ) > 0 )
			{
				return pdTRUE;
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		return pdFALSE;
	}

#endif // portNUM_PROCESSORS > 1

	static TickType_t prvGetExpectedIdleTime( void )
	{
	TickType_t xReturn;


		taskENTER_CRITICAL(&xTaskQueueMutex);
		if( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority > tskIDLE_PRIORITY )
		{
			xReturn = 0;
		}
#if portNUM_PROCESSORS > 1
		/* This function is called from Idle task; in single core case this
		 * means that no higher priority tasks are ready to run, and we can
		 * enter sleep. In SMP case, there might be ready tasks waiting for
		 * the other CPU, so need to check all ready lists.
		 */
		else if( xHaveReadyTasks() )
		{
			xReturn = 0;
		}
#endif // portNUM_PROCESSORS > 1
		else if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > portNUM_PROCESSORS )
		{
			/* There are other idle priority tasks in the ready state.  If
			time slicing is used then the very next tick interrupt must be
			processed. */
			xReturn = 0;
		}
		else
		{
			xReturn = xNextTaskUnblockTime - xTickCount;
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return xReturn;
	}

#endif /* configUSE_TICKLESS_IDLE */
/*----------------------------------------------------------*/

BaseType_t xTaskResumeAll( void )
{
TCB_t *pxTCB;
BaseType_t xAlreadyYielded = pdFALSE;

	/* If scheduler state is `taskSCHEDULER_RUNNING` then this function does not match a
	previous call to vTaskSuspendAll(). */
	configASSERT( xTaskGetSchedulerState() != taskSCHEDULER_RUNNING );
	/* It is possible that an ISR caused a task to be removed from an event
	list while the scheduler was suspended.  If this was the case then the
	removed task will have been added to the xPendingReadyList.  Once the
	scheduler has been resumed it is safe to move all the pending ready
	tasks from this list into their appropriate ready list. */

	taskENTER_CRITICAL(&xTaskQueueMutex);
	{
		--uxSchedulerSuspended[ xPortGetCoreID() ];

		if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
		{
			if( uxCurrentNumberOfTasks > ( UBaseType_t ) 0U )
			{
				/* Move any readied tasks from the pending list into the
				appropriate ready list. */
				while( listLIST_IS_EMPTY( &xPendingReadyList[ xPortGetCoreID() ] ) == pdFALSE )
				{
					pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &xPendingReadyList[ xPortGetCoreID() ] ) );
					( void ) uxListRemove( &( pxTCB->xEventListItem ) );
					( void ) uxListRemove( &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );

					/* If the moved task has a priority higher than the current
					task then a yield must be performed. */
					if ( tskCAN_RUN_HERE(pxTCB->xCoreID) && pxTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
					{
						/* We can schedule the awoken task on this CPU. */
						xYieldPending[xPortGetCoreID()] = pdTRUE;
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}

				/* If any ticks occurred while the scheduler was suspended then
				they should be processed now.  This ensures the tick count does
				not	slip, and that any delayed tasks are resumed at the correct
				time. */
				if( uxPendedTicks > ( UBaseType_t ) 0U )
				{
					while( uxPendedTicks > ( UBaseType_t ) 0U )
					{
						if( xTaskIncrementTick() != pdFALSE )
						{
							xYieldPending[ xPortGetCoreID() ] = pdTRUE;
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}
						--uxPendedTicks;
					}
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				if( xYieldPending[ xPortGetCoreID() ] == pdTRUE )
				{
					#if( configUSE_PREEMPTION != 0 )
					{
						xAlreadyYielded = pdTRUE;
					}
					#endif
					taskYIELD_IF_USING_PREEMPTION();
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
	taskEXIT_CRITICAL(&xTaskQueueMutex);

	return xAlreadyYielded;
}
/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCount( void )
{
	return xTickCount;
}
/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCountFromISR( void )
{
	return xTickCount;
}
/*-----------------------------------------------------------*/

UBaseType_t uxTaskGetNumberOfTasks( void )
{
	/* A critical section is not required because the variables are of type
	BaseType_t. */
	return uxCurrentNumberOfTasks;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_pcTaskGetTaskName == 1 )
	char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	{
	TCB_t *pxTCB;

		/* If null is passed in here then the name of the calling task is being queried. */
		pxTCB = prvGetTCBFromHandle( xTaskToQuery );
		configASSERT( pxTCB );
		return &( pxTCB->pcTaskName[ 0 ] );
	}

#endif /* INCLUDE_pcTaskGetTaskName */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime )
	{
	UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* Is there a space in the array for each task in the system? */
			if( uxArraySize >= uxCurrentNumberOfTasks )
			{
				/* Fill in an TaskStatus_t structure with information on each
				task in the Ready state. */
				do
				{
					uxQueue--;
					uxTask += prvListTaskWithinSingleList( &( pxTaskStatusArray[ uxTask ] ), &( pxReadyTasksLists[ uxQueue ] ), eReady );

				} while( uxQueue > ( UBaseType_t ) tskIDLE_PRIORITY ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

				/* Fill in an TaskStatus_t structure with information on each
				task in the Blocked state. */
				uxTask += prvListTaskWithinSingleList( &( pxTaskStatusArray[ uxTask ] ), ( List_t * ) pxDelayedTaskList, eBlocked );
				uxTask += prvListTaskWithinSingleList( &( pxTaskStatusArray[ uxTask ] ), ( List_t * ) pxOverflowDelayedTaskList, eBlocked );

				#if( INCLUDE_vTaskDelete == 1 )
				{
					/* Fill in an TaskStatus_t structure with information on
					each task that has been deleted but not yet cleaned up. */
					uxTask += prvListTaskWithinSingleList( &( pxTaskStatusArray[ uxTask ] ), &xTasksWaitingTermination, eDeleted );
				}
				#endif

				#if ( INCLUDE_vTaskSuspend == 1 )
				{
					/* Fill in an TaskStatus_t structure with information on
					each task in the Suspended state. */
					uxTask += prvListTaskWithinSingleList( &( pxTaskStatusArray[ uxTask ] ), &xSuspendedTaskList, eSuspended );
				}
				#endif

				#if ( configGENERATE_RUN_TIME_STATS == 1)
				{
					if( pulTotalRunTime != NULL )
					{
						#ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
							portALT_GET_RUN_TIME_COUNTER_VALUE( ( *pulTotalRunTime ) );
						#else
							*pulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
						#endif
					}
				}
				#else
				{
					if( pulTotalRunTime != NULL )
					{
						*pulTotalRunTime = 0;
					}
				}
				#endif
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);
		return uxTask;
	}

#endif /* configUSE_TRACE_FACILITY */
/*----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )

	TaskHandle_t xTaskGetIdleTaskHandle( void )
	{
		/* If xTaskGetIdleTaskHandle() is called before the scheduler has been
		started, then xIdleTaskHandle will be NULL. */
		configASSERT( ( xIdleTaskHandle[ xPortGetCoreID() ] != NULL ) );
		return xIdleTaskHandle[ xPortGetCoreID() ];
	}

	TaskHandle_t xTaskGetIdleTaskHandleForCPU( UBaseType_t cpuid )
	{
	    TaskHandle_t xReturn = NULL;
	    /* If xTaskGetIdleTaskHandleForCPU() is called before the scheduler has been
        started, then xIdleTaskHandle will be NULL. */
	    if (cpuid < portNUM_PROCESSORS) {
	        configASSERT( ( xIdleTaskHandle[ cpuid ] != NULL ) );
	        xReturn = xIdleTaskHandle[ cpuid ];
	    }
	    return xReturn;
	}

#endif /* INCLUDE_xTaskGetIdleTaskHandle */
/*----------------------------------------------------------*/

/* This conditional compilation should use inequality to 0, not equality to 1.
This is to ensure vTaskStepTick() is available when user defined low power mode
implementations require configUSE_TICKLESS_IDLE to be set to a value other than
1. */
#if ( configUSE_TICKLESS_IDLE != 0 )

	void vTaskStepTick( const TickType_t xTicksToJump )
	{
		/* Correct the tick count value after a period during which the tick
		was suppressed.  Note this does *not* call the tick hook function for
		each stepped tick. */
		portENTER_CRITICAL( &xTaskQueueMutex );
		configASSERT( ( xTickCount + xTicksToJump ) <= xNextTaskUnblockTime );
		xTickCount += xTicksToJump;
		portEXIT_CRITICAL( &xTaskQueueMutex );
		traceINCREASE_TICK_COUNT( xTicksToJump );
	}

#endif /* configUSE_TICKLESS_IDLE */
/*----------------------------------------------------------*/

BaseType_t xTaskIncrementTick( void )
{
TCB_t * pxTCB;
TickType_t xItemValue;
BaseType_t xSwitchRequired = pdFALSE;

	/* Called by the portable layer each time a tick interrupt occurs.
	Increments the tick then checks to see if the new tick value will cause any
	tasks to be unblocked. */

	/* Only allow core 0 increase the tick count in the case of xPortSysTickHandler processing. */
	/* And allow core 0 and core 1 to unwind uxPendedTicks during xTaskResumeAll. */

	if (xPortInIsrContext())
	{
		#if ( configUSE_TICK_HOOK == 1 )
		vApplicationTickHook();
		#endif /* configUSE_TICK_HOOK */
		#if ( CONFIG_FREERTOS_LEGACY_HOOKS == 1 )
		esp_vApplicationTickHook();
		#endif /* CONFIG_FREERTOS_LEGACY_HOOKS */

		if (xPortGetCoreID() == 1 )
		{
			return pdTRUE;
		}
	}


	traceTASK_INCREMENT_TICK( xTickCount );

	if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
	{
		taskENTER_CRITICAL_ISR( &xTaskQueueMutex );
		/* Increment the RTOS tick, switching the delayed and overflowed
		delayed lists if it wraps to 0. */
		++xTickCount;
		{
			/* Minor optimisation.  The tick count cannot change in this
			block. */
			const TickType_t xConstTickCount = xTickCount;

			if( xConstTickCount == ( TickType_t ) 0U )
			{
				taskSWITCH_DELAYED_LISTS();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* See if this tick has made a timeout expire.  Tasks are stored in
			the	queue in the order of their wake time - meaning once one task
			has been found whose block time has not expired there is no need to
			look any further down the list. */
			if( xConstTickCount >= xNextTaskUnblockTime )
			{
				for( ;; )
				{
					if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
					{
						/* The delayed list is empty.  Set xNextTaskUnblockTime
						to the maximum possible value so it is extremely
						unlikely that the
						if( xTickCount >= xNextTaskUnblockTime ) test will pass
						next time through. */
						xNextTaskUnblockTime = portMAX_DELAY;
						break;
					}
					else
					{
						/* The delayed list is not empty, get the value of the
						item at the head of the delayed list.  This is the time
						at which the task at the head of the delayed list must
						be removed from the Blocked state. */
						pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
						xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xGenericListItem ) );

						if( xConstTickCount < xItemValue )
						{
							/* It is not time to unblock this item yet, but the
							item value is the time at which the task at the head
							of the blocked list must be removed from the Blocked
							state -	so record the item value in
							xNextTaskUnblockTime. */
							xNextTaskUnblockTime = xItemValue;
							break;
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}

						/* It is time to remove the item from the Blocked state. */
						( void ) uxListRemove( &( pxTCB->xGenericListItem ) );

						/* Is the task waiting on an event also?  If so remove
						it from the event list. */
						if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
						{
							( void ) uxListRemove( &( pxTCB->xEventListItem ) );
						}
						else
						{
							mtCOVERAGE_TEST_MARKER();
						}

						/* Place the unblocked task into the appropriate ready
						list. */
						prvAddTaskToReadyList( pxTCB );

						/* A task being unblocked cannot cause an immediate
						context switch if preemption is turned off. */
						#if (  configUSE_PREEMPTION == 1 )
						{
							/* Preemption is on, but a context switch should
							only be performed if the unblocked task has a
							priority that is equal to or higher than the
							currently executing task. */
							if( pxTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
							{
								xSwitchRequired = pdTRUE;
							}
							else
							{
								mtCOVERAGE_TEST_MARKER();
							}
						}
						#endif /* configUSE_PREEMPTION */
					}
				}
			}
		}

		/* Tasks of equal priority to the currently running task will share
		processing time (time slice) if preemption is on, and the application
		writer has not explicitly turned time slicing off. */
		#if ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) )
		{
			if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ pxCurrentTCB[ xPortGetCoreID() ]->uxPriority ] ) ) > ( UBaseType_t ) 1 )
			{
				xSwitchRequired = pdTRUE;
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		#endif /* ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) ) */

		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);
	}
	else
	{
		++uxPendedTicks;
	}

	#if ( configUSE_PREEMPTION == 1 )
	{
		if( xYieldPending [ xPortGetCoreID() ] != pdFALSE )
		{
			xSwitchRequired = pdTRUE;
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
	#endif /* configUSE_PREEMPTION */

	return xSwitchRequired;
}
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

	void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction )
	{
	TCB_t *xTCB;

		/* If xTask is NULL then it is the task hook of the calling task that is
		getting set. */
		if( xTask == NULL )
		{
			xTCB = ( TCB_t * ) pxCurrentTCB[ xPortGetCoreID() ];
		}
		else
		{
			xTCB = ( TCB_t * ) xTask;
		}

		/* Save the hook function in the TCB.  A critical section is required as
		the value can be accessed from an interrupt. */
		taskENTER_CRITICAL(&xTaskQueueMutex);
			xTCB->pxTaskTag = pxHookFunction;
		taskEXIT_CRITICAL(&xTaskQueueMutex);
	}

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

	TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask )
	{
	TCB_t *xTCB;
	TaskHookFunction_t xReturn;

		/* If xTask is NULL then we are setting our own task hook. */
		if( xTask == NULL )
		{
			xTCB = ( TCB_t * ) xTaskGetCurrentTaskHandle();
		}
		else
		{
			xTCB = ( TCB_t * ) xTask;
		}

		/* Save the hook function in the TCB.  A critical section is required as
		the value can be accessed from an interrupt. */
		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			xReturn = xTCB->pxTaskTag;
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return xReturn;
	}

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

	BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter )
	{
	TCB_t *xTCB;
	BaseType_t xReturn;

		/* If xTask is NULL then we are calling our own task hook. */
		if( xTask == NULL )
		{
			xTCB = ( TCB_t * ) xTaskGetCurrentTaskHandle();
		}
		else
		{
			xTCB = ( TCB_t * ) xTask;
		}

		if( xTCB->pxTaskTag != NULL )
		{
			xReturn = xTCB->pxTaskTag( pvParameter );
		}
		else
		{
			xReturn = pdFAIL;
		}

		return xReturn;
	}

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

void vTaskSwitchContext( void )
{
	//Theoretically, this is only called from either the tick interrupt or the crosscore interrupt, so disabling
	//interrupts shouldn't be necessary anymore. Still, for safety we'll leave it in for now.
	int irqstate=portENTER_CRITICAL_NESTED();

	if( uxSchedulerSuspended[ xPortGetCoreID() ] != ( UBaseType_t ) pdFALSE )
	{
		/* The scheduler is currently suspended - do not allow a context
		switch. */
		xYieldPending[ xPortGetCoreID() ] = pdTRUE;
	}
	else
	{
		xYieldPending[ xPortGetCoreID() ] = pdFALSE;
        xSwitchingContext[ xPortGetCoreID() ] = pdTRUE;
		traceTASK_SWITCHED_OUT();

		#if ( configGENERATE_RUN_TIME_STATS == 1 )
		{
				#ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
					portALT_GET_RUN_TIME_COUNTER_VALUE( ulTotalRunTime );
				#else
					ulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
				#endif

				/* Add the amount of time the task has been running to the
				accumulated time so far.  The time the task started running was
				stored in ulTaskSwitchedInTime.  Note that there is no overflow
				protection here so count values are only valid until the timer
				overflows.  The guard against negative values is to protect
				against suspect run time stat counter implementations - which
				are provided by the application, not the kernel. */
				taskENTER_CRITICAL_ISR(&xTaskQueueMutex);
				if( ulTotalRunTime > ulTaskSwitchedInTime[ xPortGetCoreID() ] )
				{
					pxCurrentTCB[ xPortGetCoreID() ]->ulRunTimeCounter += ( ulTotalRunTime - ulTaskSwitchedInTime[ xPortGetCoreID() ] );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
				taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);
				ulTaskSwitchedInTime[ xPortGetCoreID() ] = ulTotalRunTime;
		}
		#endif /* configGENERATE_RUN_TIME_STATS */

		/* Check for stack overflow, if configured. */
		taskFIRST_CHECK_FOR_STACK_OVERFLOW();
		taskSECOND_CHECK_FOR_STACK_OVERFLOW();

		/* Select a new task to run */

		/*
		 We cannot do taskENTER_CRITICAL_ISR(&xTaskQueueMutex); here because it saves the interrupt context to the task tcb, and we're
		 swapping that out here. Instead, we're going to do the work here ourselves. Because interrupts are already disabled, we only
		 need to acquire the mutex.
		*/
		vPortCPUAcquireMutex( &xTaskQueueMutex );

#if !configUSE_PORT_OPTIMISED_TASK_SELECTION
		unsigned portBASE_TYPE foundNonExecutingWaiter = pdFALSE, ableToSchedule = pdFALSE, resetListHead;
		unsigned portBASE_TYPE holdTop=pdFALSE;
		tskTCB * pxTCB;

		portBASE_TYPE uxDynamicTopReady = uxTopReadyPriority;
		/*
		 *  ToDo: This scheduler doesn't correctly implement the round-robin scheduling as done in the single-core
		 *  FreeRTOS stack when multiple tasks have the same priority and are all ready; it just keeps grabbing the
		 *  first one. ToDo: fix this.
		 *  (Is this still true? if any, there's the issue with one core skipping over the processes for the other
		 *  core, potentially not giving the skipped-over processes any time.)
		 */

		while ( ableToSchedule == pdFALSE && uxDynamicTopReady >= 0 )
		{
			resetListHead = pdFALSE;
			// Nothing to do for empty lists
			if (!listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxDynamicTopReady ] ) )) {

				ableToSchedule = pdFALSE;
				tskTCB * pxRefTCB;

				/* Remember the current list item so that we
				can detect if all items have been inspected.
				Once this happens, we move on to a lower
				priority list (assuming nothing is suitable
				for scheduling). Note: This can return NULL if
				the list index is at the listItem */
				pxRefTCB = pxReadyTasksLists[ uxDynamicTopReady ].pxIndex->pvOwner;

				if ((void*)pxReadyTasksLists[ uxDynamicTopReady ].pxIndex==(void*)&pxReadyTasksLists[ uxDynamicTopReady ].xListEnd) {
					//pxIndex points to the list end marker. Skip that and just get the next item.
					listGET_OWNER_OF_NEXT_ENTRY( pxRefTCB, &( pxReadyTasksLists[ uxDynamicTopReady ] ) );
				}

				do {
					listGET_OWNER_OF_NEXT_ENTRY( pxTCB, &( pxReadyTasksLists[ uxDynamicTopReady ] ) );
					/* Find out if the next task in the list is
					already being executed by another core */
					foundNonExecutingWaiter = pdTRUE;
					portBASE_TYPE i = 0;
					for ( i=0; i<portNUM_PROCESSORS; i++ ) {
						if (i == xPortGetCoreID()) {
							continue;
						} else if (pxCurrentTCB[i] == pxTCB) {
							holdTop=pdTRUE; //keep this as the top prio, for the other CPU
							foundNonExecutingWaiter = pdFALSE;
							break;
						}
					}

					if (foundNonExecutingWaiter == pdTRUE) {
						/* If the task is not being executed
						by another core and its affinity is
						compatible with the current one,
						prepare it to be swapped in */
						if (pxTCB->xCoreID == tskNO_AFFINITY) {
							pxCurrentTCB[xPortGetCoreID()] = pxTCB;
							ableToSchedule = pdTRUE;
						} else if (pxTCB->xCoreID == xPortGetCoreID()) {
							pxCurrentTCB[xPortGetCoreID()] = pxTCB;
							ableToSchedule = pdTRUE;
						} else {
							ableToSchedule = pdFALSE;
							holdTop=pdTRUE; //keep this as the top prio, for the other CPU
						}
					} else {
						ableToSchedule = pdFALSE;
					}

					if (ableToSchedule == pdFALSE) {
						resetListHead = pdTRUE;
					} else if ((ableToSchedule == pdTRUE) && (resetListHead == pdTRUE)) {
						tskTCB * pxResetTCB;
						do {
							listGET_OWNER_OF_NEXT_ENTRY( pxResetTCB, &( pxReadyTasksLists[ uxDynamicTopReady ] ) );
						} while(pxResetTCB != pxRefTCB);
					}
				} while ((ableToSchedule == pdFALSE) && (pxTCB != pxRefTCB));
			} else {
				if (!holdTop) --uxTopReadyPriority;
			}
			--uxDynamicTopReady;
		}

#else 
		//For Unicore targets we can keep the current FreeRTOS O(1)
		//Scheduler. I hope to optimize better the scheduler for 
		//Multicore settings -- This will involve to create a per
		//affinity ready task list which will impact hugely on 
		//tasks module
		taskSELECT_HIGHEST_PRIORITY_TASK();
#endif
		traceTASK_SWITCHED_IN();
        xSwitchingContext[ xPortGetCoreID() ] = pdFALSE;

		//Exit critical region manually as well: release the mux now, interrupts will be re-enabled when we
		//exit the function.
		vPortCPUReleaseMutex( &xTaskQueueMutex );


#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
		vPortSetStackWatchpoint(pxCurrentTCB[xPortGetCoreID()]->pxStack);
#endif

	}
	portEXIT_CRITICAL_NESTED(irqstate);
}
/*-----------------------------------------------------------*/

void vTaskPlaceOnEventList( List_t * const pxEventList, const TickType_t xTicksToWait )
{
TickType_t xTimeToWake;

	configASSERT( pxEventList );

	taskENTER_CRITICAL(&xTaskQueueMutex);

	/* Place the event list item of the TCB in the appropriate event list.
	This is placed in the list in priority order so the highest priority task
	is the first to be woken by the event.  The queue that contains the event
	list is locked, preventing simultaneous access from interrupts. */
	vListInsert( pxEventList, &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ) );

	/* The task must be removed from from the ready list before it is added to
	the blocked list as the same list item is used for both lists.  Exclusive
	access to the ready lists guaranteed because the scheduler is locked. */
	if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
	{
		/* The current task must be in a ready list, so there is no need to
		check, and the port reset macro can be called directly. */
		portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	#if ( INCLUDE_vTaskSuspend == 1 )
	{
		if( xTicksToWait == portMAX_DELAY )
		{
			/* Add the task to the suspended task list instead of a delayed task
			list to ensure the task is not woken by a timing event.  It will
			block indefinitely. */
            traceMOVED_TASK_TO_SUSPENDED_LIST(pxCurrentTCB[ xPortGetCoreID() ]);
			vListInsertEnd( &xSuspendedTaskList, &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) );
		}
		else
		{
			/* Calculate the time at which the task should be woken if the event
			does not occur.  This may overflow but this doesn't matter, the
			scheduler will handle it. */
			xTimeToWake = xTickCount + xTicksToWait;
			prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
		}
	}
	#else /* INCLUDE_vTaskSuspend */
	{
			/* Calculate the time at which the task should be woken if the event does
			not occur.  This may overflow but this doesn't matter, the scheduler
			will handle it. */
			xTimeToWake = xTickCount + xTicksToWait;
			prvAddCurrentTaskToDelayedList( xTimeToWake );
	}
	#endif /* INCLUDE_vTaskSuspend */

	taskEXIT_CRITICAL(&xTaskQueueMutex);

}
/*-----------------------------------------------------------*/

void vTaskPlaceOnUnorderedEventList( List_t * pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait )
{
TickType_t xTimeToWake;

	configASSERT( pxEventList );

	taskENTER_CRITICAL(&xTaskQueueMutex);

	/* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED.  It is used by
	the event groups implementation. */
	configASSERT( uxSchedulerSuspended[ xPortGetCoreID() ] != 0 );

	/* Store the item value in the event list item.  It is safe to access the
	event list item here as interrupts won't access the event list item of a
	task that is not in the Blocked state. */
	listSET_LIST_ITEM_VALUE( &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ), xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

	/* Place the event list item of the TCB at the end of the appropriate event
	list.  It is safe to access the event list here because it is part of an
	event group implementation - and interrupts don't access event groups
	directly (instead they access them indirectly by pending function calls to
	the task level). */
	vListInsertEnd( pxEventList, &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ) );

	/* The task must be removed from the ready list before it is added to the
	blocked list.  Exclusive access can be assured to the ready list as the
	scheduler is locked. */
	if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
	{
		/* The current task must be in a ready list, so there is no need to
		check, and the port reset macro can be called directly. */
		portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	#if ( INCLUDE_vTaskSuspend == 1 )
	{
		if( xTicksToWait == portMAX_DELAY )
		{
			/* Add the task to the suspended task list instead of a delayed task
			list to ensure it is not woken by a timing event.  It will block
			indefinitely. */
			vListInsertEnd( &xSuspendedTaskList, &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) );
		}
		else
		{
			/* Calculate the time at which the task should be woken if the event
			does not occur.  This may overflow but this doesn't matter, the
			kernel will manage it correctly. */
			xTimeToWake = xTickCount + xTicksToWait;
			prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
		}
	}
	#else /* INCLUDE_vTaskSuspend */
	{
			/* Calculate the time at which the task should be woken if the event does
			not occur.  This may overflow but this doesn't matter, the kernel
			will manage it correctly. */
			xTimeToWake = xTickCount + xTicksToWait;
			prvAddCurrentTaskToDelayedList( xTimeToWake );
	}
	#endif /* INCLUDE_vTaskSuspend */

	taskEXIT_CRITICAL(&xTaskQueueMutex);
}
/*-----------------------------------------------------------*/

#if configUSE_TIMERS == 1

	void vTaskPlaceOnEventListRestricted( List_t * const pxEventList, const TickType_t xTicksToWait )
	{
	TickType_t xTimeToWake;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		configASSERT( pxEventList );

		/* This function should not be called by application code hence the
		'Restricted' in its name.  It is not part of the public API.  It is
		designed for use by kernel code, and has special calling requirements -
		it should be called from a critical section. */


		/* Place the event list item of the TCB in the appropriate event list.
		In this case it is assume that this is the only task that is going to
		be waiting on this event list, so the faster vListInsertEnd() function
		can be used in place of vListInsert. */
		vListInsertEnd( pxEventList, &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ) );

		/* We must remove this task from the ready list before adding it to the
		blocked list as the same list item is used for both lists.  This
		function is called form a critical section. */
		if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
		{
			/* The current task must be in a ready list, so there is no need to
			check, and the port reset macro can be called directly. */
			portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		/* Calculate the time at which the task should be woken if the event does
		not occur.  This may overflow but this doesn't matter. */
		xTimeToWake = xTickCount + xTicksToWait;

		traceTASK_DELAY_UNTIL();
		prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
		taskEXIT_CRITICAL(&xTaskQueueMutex);

	}

#endif /* configUSE_TIMERS */
/*-----------------------------------------------------------*/

BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList )
{
TCB_t *pxUnblockedTCB;
BaseType_t xReturn;
BaseType_t xTaskCanBeReady;
UBaseType_t i, uxTargetCPU;

	/* THIS FUNCTION MUST BE CALLED FROM A CRITICAL SECTION.  It can also be
	called from a critical section within an ISR. */
	taskENTER_CRITICAL_ISR(&xTaskQueueMutex);
	/* The event list is sorted in priority order, so the first in the list can
	be removed as it is known to be the highest priority.  Remove the TCB from
	the delayed list, and add it to the ready list.

	If an event is for a queue that is locked then this function will never
	get called - the lock count on the queue will get modified instead.  This
	means exclusive access to the event list is guaranteed here.

	This function assumes that a check has already been made to ensure that
	pxEventList is not empty. */
	if ( ( listLIST_IS_EMPTY( pxEventList ) ) == pdFALSE ) {
		pxUnblockedTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxEventList );
		configASSERT( pxUnblockedTCB );
		( void ) uxListRemove( &( pxUnblockedTCB->xEventListItem ) );
	} else {
		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);
		return pdFALSE;
	}

	/* Determine if the task can possibly be run on either CPU now, either because the scheduler
	   the task is pinned to is running or because a scheduler is running on any CPU. */
	xTaskCanBeReady = pdFALSE;
	if ( pxUnblockedTCB->xCoreID == tskNO_AFFINITY ) {
		uxTargetCPU = xPortGetCoreID();
		for (i = 0; i < portNUM_PROCESSORS; i++) {
			if ( uxSchedulerSuspended[ i ] == ( UBaseType_t ) pdFALSE ) {
				xTaskCanBeReady = pdTRUE;
				break;
			}
		}
	} else {
		uxTargetCPU = pxUnblockedTCB->xCoreID;
		xTaskCanBeReady = uxSchedulerSuspended[ uxTargetCPU ] == ( UBaseType_t ) pdFALSE;

	}

	if( xTaskCanBeReady )
	{
		( void ) uxListRemove( &( pxUnblockedTCB->xGenericListItem ) );
		prvAddTaskToReadyList( pxUnblockedTCB );
	}
	else
	{
		/* The delayed and ready lists cannot be accessed, so hold this task
		pending until the scheduler is resumed on this CPU. */
		vListInsertEnd( &( xPendingReadyList[ uxTargetCPU ] ), &( pxUnblockedTCB->xEventListItem ) );
	}

	if ( tskCAN_RUN_HERE(pxUnblockedTCB->xCoreID) && pxUnblockedTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
	{
		/* Return true if the task removed from the event list has a higher
		priority than the calling task.  This allows the calling task to know if
		it should force a context switch now. */
		xReturn = pdTRUE;

		/* Mark that a yield is pending in case the user is not using the
		"xHigherPriorityTaskWoken" parameter to an ISR safe FreeRTOS function. */
		xYieldPending[ xPortGetCoreID() ] = pdTRUE;
	}
	else if ( pxUnblockedTCB->xCoreID != xPortGetCoreID() )
	{
		taskYIELD_OTHER_CORE( pxUnblockedTCB->xCoreID, pxUnblockedTCB->uxPriority );
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdFALSE;
	}

	#if( configUSE_TICKLESS_IDLE == 1 )
	{
		/* If a task is blocked on a kernel object then xNextTaskUnblockTime
		might be set to the blocked task's time out time.  If the task is
		unblocked for a reason other than a timeout xNextTaskUnblockTime is
		normally left unchanged, because it is automatically get reset to a new
		value when the tick count equals xNextTaskUnblockTime.  However if
		tickless idling is used it might be more important to enter sleep mode
		at the earliest possible time - so reset xNextTaskUnblockTime here to
		ensure it is updated at the earliest possible time. */
		prvResetNextTaskUnblockTime();
	}
	#endif
	taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t xTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, const TickType_t xItemValue )
{
TCB_t *pxUnblockedTCB;
BaseType_t xReturn;

	taskENTER_CRITICAL(&xTaskQueueMutex);
	/* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED.  It is used by
	the event flags implementation. */
	configASSERT( uxSchedulerSuspended[ xPortGetCoreID() ] != pdFALSE );

	/* Store the new item value in the event list. */
	listSET_LIST_ITEM_VALUE( pxEventListItem, xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

	/* Remove the event list form the event flag.  Interrupts do not access
	event flags. */
	pxUnblockedTCB = ( TCB_t * ) listGET_LIST_ITEM_OWNER( pxEventListItem );
	configASSERT( pxUnblockedTCB );
	( void ) uxListRemove( pxEventListItem );

	/* Remove the task from the delayed list and add it to the ready list.  The
	scheduler is suspended so interrupts will not be accessing the ready
	lists. */
	( void ) uxListRemove( &( pxUnblockedTCB->xGenericListItem ) );
	prvAddTaskToReadyList( pxUnblockedTCB );

	if ( tskCAN_RUN_HERE(pxUnblockedTCB->xCoreID) && pxUnblockedTCB->uxPriority >= pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
	{
		/* Return true if the task removed from the event list has
		a higher priority than the calling task.  This allows
		the calling task to know if it should force a context
		switch now. */
		xReturn = pdTRUE;

		/* Mark that a yield is pending in case the user is not using the
		"xHigherPriorityTaskWoken" parameter to an ISR safe FreeRTOS function. */
		xYieldPending[ xPortGetCoreID() ] = pdTRUE;
	}
	else if ( pxUnblockedTCB->xCoreID != xPortGetCoreID() )
	{
		taskYIELD_OTHER_CORE( pxUnblockedTCB->xCoreID, pxUnblockedTCB->uxPriority );
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdFALSE;
	}

	taskEXIT_CRITICAL(&xTaskQueueMutex);
	return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
	configASSERT( pxTimeOut );
	pxTimeOut->xOverflowCount = xNumOfOverflows;
	pxTimeOut->xTimeOnEntering = xTickCount;
}
/*-----------------------------------------------------------*/

BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait )
{
BaseType_t xReturn;

	configASSERT( pxTimeOut );
	configASSERT( pxTicksToWait );

	taskENTER_CRITICAL(&xTaskQueueMutex);
	{
		/* Minor optimisation.  The tick count cannot change in this block. */
		const TickType_t xConstTickCount = xTickCount;

		#if ( INCLUDE_vTaskSuspend == 1 )
			/* If INCLUDE_vTaskSuspend is set to 1 and the block time specified is
			the maximum block time then the task should block indefinitely, and
			therefore never time out. */
			if( *pxTicksToWait == portMAX_DELAY )
			{
				xReturn = pdFALSE;
			}
			else /* We are not blocking indefinitely, perform the checks below. */
		#endif

		if( ( xNumOfOverflows != pxTimeOut->xOverflowCount ) && ( xConstTickCount >= pxTimeOut->xTimeOnEntering ) ) /*lint !e525 Indentation preferred as is to make code within pre-processor directives clearer. */
		{
			/* The tick count is greater than the time at which vTaskSetTimeout()
			was called, but has also overflowed since vTaskSetTimeOut() was called.
			It must have wrapped all the way around and gone past us again. This
			passed since vTaskSetTimeout() was called. */
			xReturn = pdTRUE;
		}
		else if( ( xConstTickCount - pxTimeOut->xTimeOnEntering ) < *pxTicksToWait )
		{
			/* Not a genuine timeout. Adjust parameters for time remaining. */
			*pxTicksToWait -= ( xConstTickCount -  pxTimeOut->xTimeOnEntering );
			vTaskSetTimeOutState( pxTimeOut );
			xReturn = pdFALSE;
		}
		else
		{
			xReturn = pdTRUE;
		}
	}
	taskEXIT_CRITICAL(&xTaskQueueMutex);

	return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskMissedYield( void )
{
	xYieldPending[ xPortGetCoreID() ] = pdTRUE;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask )
	{
	UBaseType_t uxReturn;
	TCB_t *pxTCB;

		if( xTask != NULL )
		{
			pxTCB = ( TCB_t * ) xTask;
			uxReturn = pxTCB->uxTaskNumber;
		}
		else
		{
			uxReturn = 0U;
		}

		return uxReturn;
	}

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

	void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle )
	{
	TCB_t *pxTCB;

		if( xTask != NULL )
		{
			pxTCB = ( TCB_t * ) xTask;
			pxTCB->uxTaskNumber = uxHandle;
		}
	}

#endif /* configUSE_TRACE_FACILITY */

/*
 * -----------------------------------------------------------
 * The Idle task.
 * ----------------------------------------------------------
 *
 * The portTASK_FUNCTION() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
	/* Stop warnings. */
	( void ) pvParameters;

	for( ;; )
	{
		/* See if any tasks have been deleted. */
		prvCheckTasksWaitingTermination();

		#if ( configUSE_PREEMPTION == 0 )
		{
			/* If we are not using preemption we keep forcing a task switch to
			see if any other task has become available.  If we are using
			preemption we don't need to do this as any task becoming available
			will automatically get the processor anyway. */
			taskYIELD();
		}
		#endif /* configUSE_PREEMPTION */

		#if ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) )
		{
			/* When using preemption tasks of equal priority will be
			timesliced.  If a task that is sharing the idle priority is ready
			to run then the idle task should yield before the end of the
			timeslice.

			A critical region is not required here as we are just reading from
			the list, and an occasional incorrect value will not matter.  If
			the ready list at the idle priority contains more than one task
			then a task other than the idle task is ready to execute. */
			if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > ( UBaseType_t ) 1 )
			{
				taskYIELD();
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		#endif /* ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) ) */

		#if ( configUSE_IDLE_HOOK == 1 )
		{
			extern void vApplicationIdleHook( void );

			/* Call the user defined function from within the idle task.  This
			allows the application designer to add background functionality
			without the overhead of a separate task.
			NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES,
			CALL A FUNCTION THAT MIGHT BLOCK. */
			vApplicationIdleHook();
		}
		#endif /* configUSE_IDLE_HOOK */
		#if ( CONFIG_FREERTOS_LEGACY_HOOKS == 1 )
		{
			/* Call the esp-idf hook system */
			esp_vApplicationIdleHook();
		}
		#endif /* CONFIG_FREERTOS_LEGACY_HOOKS */


		/* This conditional compilation should use inequality to 0, not equality
		to 1.  This is to ensure portSUPPRESS_TICKS_AND_SLEEP() is called when
		user defined low power mode	implementations require
		configUSE_TICKLESS_IDLE to be set to a value other than 1. */
		#if ( configUSE_TICKLESS_IDLE != 0 )
		{
		TickType_t xExpectedIdleTime;

			/* It is not desirable to suspend then resume the scheduler on
			each iteration of the idle task.  Therefore, a preliminary
			test of the expected idle time is performed without the
			scheduler suspended.  The result here is not necessarily
			valid. */
			xExpectedIdleTime = prvGetExpectedIdleTime();

			if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
			{
				taskENTER_CRITICAL(&xTaskQueueMutex);
				{
					/* Now the scheduler is suspended, the expected idle
					time can be sampled again, and this time its value can
					be used. */
					configASSERT( xNextTaskUnblockTime >= xTickCount );
					xExpectedIdleTime = prvGetExpectedIdleTime();

					if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
					{
						traceLOW_POWER_IDLE_BEGIN();
						portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );
						traceLOW_POWER_IDLE_END();
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}
				}
				taskEXIT_CRITICAL(&xTaskQueueMutex);
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		#endif /* configUSE_TICKLESS_IDLE */
	}
}
/*-----------------------------------------------------------*/

#if configUSE_TICKLESS_IDLE != 0

	eSleepModeStatus eTaskConfirmSleepModeStatus( void )
	{
	eSleepModeStatus eReturn = eStandardSleep;
		taskENTER_CRITICAL(&xTaskQueueMutex);

		if( listCURRENT_LIST_LENGTH( &xPendingReadyList[ xPortGetCoreID() ] ) != 0 )
		{
			/* A task was made ready while the scheduler was suspended. */
			eReturn = eAbortSleep;
		}
		else if( xYieldPending[ xPortGetCoreID() ] != pdFALSE )
		{
			/* A yield was pended while the scheduler was suspended. */
			eReturn = eAbortSleep;
		}
		else
		{
			#if configUSE_TIMERS == 0
			{
				/* The idle task exists in addition to the application tasks. */
				const UBaseType_t uxNonApplicationTasks = 1;

				/* If timers are not being used and all the tasks are in the
				suspended list (which might mean they have an infinite block
				time rather than actually being suspended) then it is safe to
				turn all clocks off and just wait for external interrupts. */
				if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == ( uxCurrentNumberOfTasks - uxNonApplicationTasks ) )
				{
					eReturn = eNoTasksWaitingTimeout;
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			#endif /* configUSE_TIMERS */
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return eReturn;
	}
#endif /* configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )

#if ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )

	void vTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue , TlsDeleteCallbackFunction_t xDelCallback)
	{
	TCB_t *pxTCB;

		if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
		{
			taskENTER_CRITICAL(&xTaskQueueMutex);
			pxTCB = prvGetTCBFromHandle( xTaskToSet );
			pxTCB->pvThreadLocalStoragePointers[ xIndex ] = pvValue;
			pxTCB->pvThreadLocalStoragePointersDelCallback[ xIndex ] = xDelCallback;
			taskEXIT_CRITICAL(&xTaskQueueMutex);
		}
	}

	void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue )
	{
		vTaskSetThreadLocalStoragePointerAndDelCallback( xTaskToSet, xIndex, pvValue, (TlsDeleteCallbackFunction_t)NULL );
	}


#else
	void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue )
	{
	TCB_t *pxTCB;

		if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
		{
			taskENTER_CRITICAL(&xTaskQueueMutex);
			pxTCB = prvGetTCBFromHandle( xTaskToSet );
			pxTCB->pvThreadLocalStoragePointers[ xIndex ] = pvValue;
			taskEXIT_CRITICAL(&xTaskQueueMutex);
		}
	}
#endif /* configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS */

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )

	void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex )
	{
	void *pvReturn = NULL;
	TCB_t *pxTCB;

		if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
		{
			pxTCB = prvGetTCBFromHandle( xTaskToQuery );
			pvReturn = pxTCB->pvThreadLocalStoragePointers[ xIndex ];
		}
		else
		{
			pvReturn = NULL;
		}

		return pvReturn;
	}

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */


#if ( portUSING_MPU_WRAPPERS == 1 )
/* ToDo: Check for multicore */
	void vTaskAllocateMPURegions( TaskHandle_t xTaskToModify, const MemoryRegion_t * const xRegions )
	{
	TCB_t *pxTCB;

		UNTESTED_FUNCTION();
		/* If null is passed in here then we are deleting ourselves. */
		pxTCB = prvGetTCBFromHandle( xTaskToModify );

        vPortStoreTaskMPUSettings( &( pxTCB->xMPUSettings ), xRegions, NULL, 0 );
	}

#endif /* portUSING_MPU_WRAPPERS */
/*-----------------------------------------------------------*/

static void prvInitialiseTaskLists( void )
{
UBaseType_t uxPriority;

	for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}

	vListInitialise( &xDelayedTaskList1 );
	vListInitialise( &xDelayedTaskList2 );
	vListInitialise( &xPendingReadyList[ 0 ] );
	if (portNUM_PROCESSORS == 2) {
		vListInitialise( &xPendingReadyList[ 1 ] );
	}

	#if ( INCLUDE_vTaskDelete == 1 )
	{
		vListInitialise( &xTasksWaitingTermination );
	}
	#endif /* INCLUDE_vTaskDelete */

	#if ( INCLUDE_vTaskSuspend == 1 )
	{
		vListInitialise( &xSuspendedTaskList );
	}
	#endif /* INCLUDE_vTaskSuspend */

	/* Start with pxDelayedTaskList using list1 and the pxOverflowDelayedTaskList
	using list2. */
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}
/*-----------------------------------------------------------*/

static void prvCheckTasksWaitingTermination( void )
{
	#if ( INCLUDE_vTaskDelete == 1 )
	{
		BaseType_t xListIsEmpty;
		int core = xPortGetCoreID();

		/* ucTasksDeleted is used to prevent vTaskSuspendAll() being called
		too often in the idle task. */
		while(uxTasksDeleted > ( UBaseType_t ) 0U )
		{
			TCB_t *pxTCB = NULL;

			taskENTER_CRITICAL(&xTaskQueueMutex);
			{
				xListIsEmpty = listLIST_IS_EMPTY( &xTasksWaitingTermination );
				if( xListIsEmpty == pdFALSE )
				{
					/* We only want to kill tasks that ran on this core because e.g. _xt_coproc_release needs to
					be called on the core the process is pinned on, if any */
					ListItem_t *target = listGET_HEAD_ENTRY(&xTasksWaitingTermination);
					for( ; target != listGET_END_MARKER(&xTasksWaitingTermination); target = listGET_NEXT(target) ){	//Walk the list
						TCB_t *tgt_tcb = ( TCB_t * )listGET_LIST_ITEM_OWNER(target);
						int affinity = tgt_tcb->xCoreID;
						//Self deleting tasks are added to Termination List before they switch context. Ensure they aren't still currently running
						if( pxCurrentTCB[core] == tgt_tcb || (portNUM_PROCESSORS > 1 && pxCurrentTCB[!core] == tgt_tcb) ){
							continue;	//Can't free memory of task that is still running
						}
						if(affinity == core || affinity == tskNO_AFFINITY){		//Find first item not pinned to other core
							pxTCB = tgt_tcb;
							break;
						}
					}
					if(pxTCB != NULL){
						( void ) uxListRemove( target );	//Remove list item from list
						--uxCurrentNumberOfTasks;
						--uxTasksDeleted;
					}
				}
			}
			taskEXIT_CRITICAL(&xTaskQueueMutex);	//Need to call deletion callbacks outside critical section

			if (pxTCB != NULL) {	//Call deletion callbacks and free TCB memory
				#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) && ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )
					prvDeleteTLS( pxTCB );
				#endif
				prvDeleteTCB( pxTCB );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
				break;	//No TCB found that could be freed by this core, break out of loop
			}
		}
	}
	#endif /* vTaskDelete */
}
/*-----------------------------------------------------------*/

//This should be called with the taskqueuemutex grabbed. -JD
static void prvAddCurrentTaskToDelayedList( const BaseType_t xCoreID, const TickType_t xTimeToWake )
{
	/* The list item will be inserted in wake time order. */
	listSET_LIST_ITEM_VALUE( &( pxCurrentTCB[ xCoreID ]->xGenericListItem ), xTimeToWake );

	if( xTimeToWake < xTickCount )
	{
        traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST();
		/* Wake time has overflowed.  Place this item in the overflow list. */
		vListInsert( pxOverflowDelayedTaskList, &( pxCurrentTCB[ xCoreID ]->xGenericListItem ) );
	}
	else
	{
        traceMOVED_TASK_TO_DELAYED_LIST();
		/* The wake time has not overflowed, so the current block list is used. */
		vListInsert( pxDelayedTaskList, &( pxCurrentTCB[ xCoreID ]->xGenericListItem ) );

		/* If the task entering the blocked state was placed at the head of the
		list of blocked tasks then xNextTaskUnblockTime needs to be updated
		too. */
		if( xTimeToWake < xNextTaskUnblockTime )
		{
			xNextTaskUnblockTime = xTimeToWake;
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
}
/*-----------------------------------------------------------*/

BaseType_t xTaskGetAffinity( TaskHandle_t xTask )
{
	TCB_t *pxTCB;

	pxTCB = prvGetTCBFromHandle( xTask );

	return pxTCB->xCoreID;
}
/*-----------------------------------------------------------*/


#if ( configUSE_TRACE_FACILITY == 1 )

	static UBaseType_t prvListTaskWithinSingleList( TaskStatus_t *pxTaskStatusArray, List_t *pxList, eTaskState eState )
	{
	volatile TCB_t *pxNextTCB, *pxFirstTCB;
	UBaseType_t uxTask = 0;

		if( listCURRENT_LIST_LENGTH( pxList ) > ( UBaseType_t ) 0 )
		{
			listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );

			/* Populate an TaskStatus_t structure within the
			pxTaskStatusArray array for each task that is referenced from
			pxList.  See the definition of TaskStatus_t in task.h for the
			meaning of each TaskStatus_t structure member. */
			do
			{
				listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );

				pxTaskStatusArray[ uxTask ].xHandle = ( TaskHandle_t ) pxNextTCB;
				pxTaskStatusArray[ uxTask ].pcTaskName = ( const char * ) &( pxNextTCB->pcTaskName [ 0 ] );
				pxTaskStatusArray[ uxTask ].xTaskNumber = pxNextTCB->uxTCBNumber;
				pxTaskStatusArray[ uxTask ].eCurrentState = eState;
				pxTaskStatusArray[ uxTask ].uxCurrentPriority = pxNextTCB->uxPriority;

				#if ( configTASKLIST_INCLUDE_COREID == 1 )
				pxTaskStatusArray[ uxTask ].xCoreID = pxNextTCB->xCoreID;
				#endif /* configTASKLIST_INCLUDE_COREID */

				#if ( INCLUDE_vTaskSuspend == 1 )
				{
					/* If the task is in the suspended list then there is a chance
					it is actually just blocked indefinitely - so really it should
					be reported as being in the Blocked state. */
					if( eState == eSuspended )
					{
						if( listLIST_ITEM_CONTAINER( &( pxNextTCB->xEventListItem ) ) != NULL )
						{
							pxTaskStatusArray[ uxTask ].eCurrentState = eBlocked;
						}
					}
				}
				#endif /* INCLUDE_vTaskSuspend */

				#if ( configUSE_MUTEXES == 1 )
				{
					pxTaskStatusArray[ uxTask ].uxBasePriority = pxNextTCB->uxBasePriority;
				}
				#else
				{
					pxTaskStatusArray[ uxTask ].uxBasePriority = 0;
				}
				#endif

				#if ( configGENERATE_RUN_TIME_STATS == 1 )
				{
					pxTaskStatusArray[ uxTask ].ulRunTimeCounter = pxNextTCB->ulRunTimeCounter;
				}
				#else
				{
					pxTaskStatusArray[ uxTask ].ulRunTimeCounter = 0;
				}
				#endif

				#if ( portSTACK_GROWTH > 0 )
				{
					pxTaskStatusArray[ uxTask ].usStackHighWaterMark = prvTaskCheckFreeStackSpace( ( uint8_t * ) pxNextTCB->pxEndOfStack );
				}
				#else
				{
					pxTaskStatusArray[ uxTask ].usStackHighWaterMark = prvTaskCheckFreeStackSpace( ( uint8_t * ) pxNextTCB->pxStack );
				}
				#endif

				uxTask++;

			} while( pxNextTCB != pxFirstTCB );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		return uxTask;
	}

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )

	static uint32_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte )
	{
	uint32_t ulCount = 0U;

		while( *pucStackByte == ( uint8_t ) tskSTACK_FILL_BYTE )
		{
			pucStackByte -= portSTACK_GROWTH;
			ulCount++;
		}

		ulCount /= ( uint32_t ) sizeof( StackType_t ); /*lint !e961 Casting is not redundant on smaller architectures. */

		return ( uint32_t ) ulCount;
	}

#endif /* ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )

	UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
	{
	TCB_t *pxTCB;
	uint8_t *pucEndOfStack;
	UBaseType_t uxReturn;

		pxTCB = prvGetTCBFromHandle( xTask );

		#if portSTACK_GROWTH < 0
		{
			pucEndOfStack = ( uint8_t * ) pxTCB->pxStack;
		}
		#else
		{
			pucEndOfStack = ( uint8_t * ) pxTCB->pxEndOfStack;
		}
		#endif

		uxReturn = ( UBaseType_t ) prvTaskCheckFreeStackSpace( pucEndOfStack );

		return uxReturn;
	}

#endif /* INCLUDE_uxTaskGetStackHighWaterMark */
/*-----------------------------------------------------------*/

#if (INCLUDE_pxTaskGetStackStart == 1)

	uint8_t* pxTaskGetStackStart( TaskHandle_t xTask)
	{
		TCB_t *pxTCB;
		uint8_t* uxReturn;

		pxTCB = prvGetTCBFromHandle( xTask );
		uxReturn = (uint8_t*)pxTCB->pxStack;

		return uxReturn;
	}

#endif /* INCLUDE_pxTaskGetStackStart */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )

	static void prvDeleteTCB( TCB_t *pxTCB )
	{
		/* This call is required for any port specific cleanup related to task.
		It must be above the vPortFree() calls. */
		portCLEAN_UP_TCB( pxTCB );

		/* Free up the memory allocated by the scheduler for the task.  It is up
		to the task to free any memory allocated at the application level. */
		#if ( configUSE_NEWLIB_REENTRANT == 1 )
		{
			_reclaim_reent( &( pxTCB->xNewLib_reent ) );
		}
		#endif /* configUSE_NEWLIB_REENTRANT */

		#if ( portUSING_MPU_WRAPPERS == 1 )
			vPortReleaseTaskMPUSettings( &( pxTCB->xMPUSettings) );
		#endif

		#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) && ( portUSING_MPU_WRAPPERS == 0 ) )
		{
			/* The task can only have been allocated dynamically - free both
			the stack and TCB. */
			vPortFreeAligned( pxTCB->pxStack );
			vPortFree( pxTCB );
		}
		#elif( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE == 1 )
		{
			/* The task could have been allocated statically or dynamically, so
			check what was statically allocated before trying to free the
			memory. */
			if( pxTCB->ucStaticallyAllocated == tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB )
			{
				/* Both the stack and TCB were allocated dynamically, so both
				must be freed. */
				vPortFreeAligned( pxTCB->pxStack );
				vPortFree( pxTCB );
			}
			else if( pxTCB->ucStaticallyAllocated == tskSTATICALLY_ALLOCATED_STACK_ONLY )
			{
				/* Only the stack was statically allocated, so the TCB is the
				only memory that must be freed. */
                vPortFreeAligned(pxTCB->pxStack);
                vPortFree( pxTCB );
			}
			else
			{
				/* Neither the stack nor the TCB were allocated dynamically, so
				nothing needs to be freed. */
				configASSERT( pxTCB->ucStaticallyAllocated == tskSTATICALLY_ALLOCATED_STACK_AND_TCB	)
				mtCOVERAGE_TEST_MARKER();
			}
		}
		#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
	}

#endif /* INCLUDE_vTaskDelete */
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) && ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )

	static void prvDeleteTLS( TCB_t *pxTCB )
	{
		configASSERT( pxTCB );
		for( int x = 0; x < ( UBaseType_t ) configNUM_THREAD_LOCAL_STORAGE_POINTERS; x++ )
		{
			if (pxTCB->pvThreadLocalStoragePointersDelCallback[ x ] != NULL)	//If del cb is set
			{
				pxTCB->pvThreadLocalStoragePointersDelCallback[ x ](x, pxTCB->pvThreadLocalStoragePointers[ x ]);	//Call del cb
			}
		}
	}

#endif /* ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 ) && ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS ) */
/*-----------------------------------------------------------*/

static void prvResetNextTaskUnblockTime( void )
{
TCB_t *pxTCB;

	if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
	{
		/* The new current delayed list is empty.  Set
		xNextTaskUnblockTime to the maximum possible value so it is
		extremely unlikely that the
		if( xTickCount >= xNextTaskUnblockTime ) test will pass until
		there is an item in the delayed list. */
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/* The new current delayed list is not empty, get the value of
		the item at the head of the delayed list.  This is the time at
		which the task at the head of the delayed list should be removed
		from the Blocked state. */
		( pxTCB ) = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
		xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE( &( ( pxTCB )->xGenericListItem ) );
	}
}
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )

	TaskHandle_t xTaskGetCurrentTaskHandle( void )
	{
	TaskHandle_t xReturn;
	unsigned state;

		state = portENTER_CRITICAL_NESTED();
		xReturn = pxCurrentTCB[ xPortGetCoreID() ];
		portEXIT_CRITICAL_NESTED(state);

		return xReturn;
	}

	TaskHandle_t xTaskGetCurrentTaskHandleForCPU( BaseType_t cpuid )
	{
	TaskHandle_t xReturn=NULL;

		//Xtensa-specific: the pxCurrentPCB pointer is atomic so we shouldn't need a lock.
		if (cpuid < portNUM_PROCESSORS) {
			xReturn = pxCurrentTCB[ cpuid ];
		}

		return xReturn;
	}


#endif /* ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )

	BaseType_t xTaskGetSchedulerState( void )
	{
	BaseType_t xReturn;
	unsigned state;

		state = portENTER_CRITICAL_NESTED();
		if( xSchedulerRunning == pdFALSE )
		{
			xReturn = taskSCHEDULER_NOT_STARTED;
		}
		else
		{
			if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
			{
				xReturn = taskSCHEDULER_RUNNING;
			}
			else
			{
				xReturn = taskSCHEDULER_SUSPENDED;
			}
		}
		portEXIT_CRITICAL_NESTED(state);

		return xReturn;
	}

#endif /* ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

	void vTaskPriorityInherit( TaskHandle_t const pxMutexHolder )
	{
	TCB_t * const pxTCB = ( TCB_t * ) pxMutexHolder;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		/* If the mutex was given back by an interrupt while the queue was
		locked then the mutex holder might now be NULL. */
		if( pxMutexHolder != NULL )
		{
			if( pxTCB->uxPriority < pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
			{
				taskENTER_CRITICAL(&xTaskQueueMutex);
				/* Adjust the mutex holder state to account for its new
				priority.  Only reset the event list item value if the value is
				not	being used for anything else. */
				if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
				{
					listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) pxCurrentTCB[ xPortGetCoreID() ]->uxPriority ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}

				/* If the task being modified is in the ready state it will need to
				be moved into a new list. */
				if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ pxTCB->uxPriority ] ), &( pxTCB->xGenericListItem ) ) != pdFALSE )
				{
					if( uxListRemove( &( pxTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
					{
						taskRESET_READY_PRIORITY( pxTCB->uxPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/* Inherit the priority before being moved into the new list. */
					pxTCB->uxPriority = pxCurrentTCB[ xPortGetCoreID() ]->uxPriority;
                    prvReaddTaskToReadyList( pxTCB );
				}
				else
				{
					/* Just inherit the priority. */
					pxTCB->uxPriority = pxCurrentTCB[ xPortGetCoreID() ]->uxPriority;
				}

				taskEXIT_CRITICAL(&xTaskQueueMutex);

				traceTASK_PRIORITY_INHERIT( pxTCB, pxCurrentTCB[ xPortGetCoreID() ]->uxPriority );
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		taskEXIT_CRITICAL(&xTaskQueueMutex);

	}

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

	BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder )
	{
	TCB_t * const pxTCB = ( TCB_t * ) pxMutexHolder;
	BaseType_t xReturn = pdFALSE;
		taskENTER_CRITICAL(&xTaskQueueMutex);

		if( pxMutexHolder != NULL )
		{
			configASSERT( pxTCB->uxMutexesHeld );
			( pxTCB->uxMutexesHeld )--;

			if( pxTCB->uxPriority != pxTCB->uxBasePriority )
			{
				/* Only disinherit if no other mutexes are held. */
				if( pxTCB->uxMutexesHeld == ( UBaseType_t ) 0 )
				{
					taskENTER_CRITICAL(&xTaskQueueMutex);
					/* A task can only have an inhertied priority if it holds
					the mutex.  If the mutex is held by a task then it cannot be
					given from an interrupt, and if a mutex is given by the
					holding	task then it must be the running state task.  Remove
					the	holding task from the ready	list. */
					if( uxListRemove( &( pxTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
					{
						taskRESET_READY_PRIORITY( pxTCB->uxPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					/* Disinherit the priority before adding the task into the
					new	ready list. */
					traceTASK_PRIORITY_DISINHERIT( pxTCB, pxTCB->uxBasePriority );
					pxTCB->uxPriority = pxTCB->uxBasePriority;

					/* Reset the event list item value.  It cannot be in use for
					any other purpose if this task is running, and it must be
					running to give back the mutex. */
					listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) pxTCB->uxPriority ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
                    prvReaddTaskToReadyList( pxTCB );

					/* Return true to indicate that a context switch is required.
					This is only actually required in the corner case whereby
					multiple mutexes were held and the mutexes were given back
					in an order different to that in which they were taken.
					If a context switch did not occur when the first mutex was
					returned, even if a task was waiting on it, then a context
					switch should occur when the last mutex is returned whether
					a task is waiting on it or not. */
					xReturn = pdTRUE;
					taskEXIT_CRITICAL(&xTaskQueueMutex);
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		taskEXIT_CRITICAL(&xTaskQueueMutex);
		return xReturn;
	}

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

/* For multicore, this assumes the vPortCPUAquireMutex is recursive, that is, it can be called multiple
   times and the release call will have to be called as many times for the mux to unlock. */

/* Gotcha (which seems to be deliberate in FreeRTOS, according to
http://www.freertos.org/FreeRTOS_Support_Forum_Archive/December_2012/freertos_PIC32_Bug_-_vTaskEnterCritical_6400806.html
) is that calling vTaskEnterCritical followed by vTaskExitCritical will leave the interrupts DISABLED when the scheduler
is not running.  Re-enabling the scheduler will re-enable the interrupts instead.

For ESP32 FreeRTOS, vTaskEnterCritical implements both portENTER_CRITICAL and portENTER_CRITICAL_ISR.
*/

#if ( portCRITICAL_NESTING_IN_TCB == 1 )

	void vTaskEnterCritical( portMUX_TYPE *mux )
	{
		BaseType_t oldInterruptLevel=0;
		BaseType_t schedulerRunning = xSchedulerRunning;
		if(schedulerRunning != pdFALSE)
		{
			//Interrupts may already be disabled (because we're doing this recursively) but we can't get the interrupt level after
			//vPortCPUAquireMutex, because it also may mess with interrupts. Get it here first, then later figure out if we're nesting
			//and save for real there.
			oldInterruptLevel=portENTER_CRITICAL_NESTED();
		}

		vPortCPUAcquireMutex( mux );
		if( schedulerRunning != pdFALSE )
		{
			TCB_t *tcb = pxCurrentTCB[xPortGetCoreID()];
			BaseType_t newNesting = tcb->uxCriticalNesting + 1;
 			tcb->uxCriticalNesting = newNesting;
			if( newNesting == 1 )
			{
				//This is the first time we get called. Save original interrupt level.
				tcb->uxOldInterruptState = oldInterruptLevel;
			}

			/* Original FreeRTOS comment, saved for reference:
			This is not the interrupt safe version of the enter critical
			function so	assert() if it is being called from an interrupt
			context.  Only API functions that end in "FromISR" can be used in an
			interrupt. Only assert if the critical nesting count is 1 to
			protect against recursive calls if the assert function also uses a
			critical section. */

			/* DISABLED in the esp32 port - because of SMP, For ESP32
			FreeRTOS, vTaskEnterCritical implements both
			portENTER_CRITICAL and portENTER_CRITICAL_ISR. vTaskEnterCritical
			has to be used in way more places than before, and some are called
			both from ISR as well as non-ISR code, thus we re-organized
			vTaskEnterCritical to also work in ISRs. */
#if 0
			if( newNesting	== 1 )
			{
				portASSERT_IF_IN_ISR();
			}
#endif

		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/
/*
For ESP32 FreeRTOS, vTaskExitCritical implements both portEXIT_CRITICAL and portEXIT_CRITICAL_ISR.
*/
#if ( portCRITICAL_NESTING_IN_TCB == 1 )

	void vTaskExitCritical( portMUX_TYPE *mux )
	{

		vPortCPUReleaseMutex( mux );
		if( xSchedulerRunning != pdFALSE )
		{
			TCB_t *tcb = pxCurrentTCB[xPortGetCoreID()];
			BaseType_t nesting = tcb->uxCriticalNesting;
			if(nesting > 0U)
			{
				nesting--;
				tcb->uxCriticalNesting = nesting;

				if( nesting == 0U )
				{
					portEXIT_CRITICAL_NESTED(tcb->uxOldInterruptState);
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

	static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName )
	{
	BaseType_t x;

		/* Start by copying the entire string. */
		strcpy( pcBuffer, pcTaskName );

		/* Pad the end of the string with spaces to ensure columns line up when
		printed out. */
		for( x = strlen( pcBuffer ); x < ( configMAX_TASK_NAME_LEN - 1 ); x++ )
		{
			pcBuffer[ x ] = ' ';
		}

		/* Terminate. */
		pcBuffer[ x ] = 0x00;

		/* Return the new end of string. */
		return &( pcBuffer[ x ] );
	}

#endif /* ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

	void vTaskList( char * pcWriteBuffer )
	{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	char cStatus;

		/*
		 * PLEASE NOTE:
		 *
		 * This function is provided for convenience only, and is used by many
		 * of the demo applications.  Do not consider it to be part of the
		 * scheduler.
		 *
		 * vTaskList() calls uxTaskGetSystemState(), then formats part of the
		 * uxTaskGetSystemState() output into a human readable table that
		 * displays task names, states and stack usage.
		 *
		 * vTaskList() has a dependency on the sprintf() C library function that
		 * might bloat the code size, use a lot of stack, and provide different
		 * results on different platforms.  An alternative, tiny, third party,
		 * and limited functionality implementation of sprintf() is provided in
		 * many of the FreeRTOS/Demo sub-directories in a file called
		 * printf-stdarg.c (note printf-stdarg.c does not provide a full
		 * snprintf() implementation!).
		 *
		 * It is recommended that production systems call uxTaskGetSystemState()
		 * directly to get access to raw stats data, rather than indirectly
		 * through a call to vTaskList().
		 */


		/* Make sure the write buffer does not contain a string. */
		*pcWriteBuffer = 0x00;

		/* Take a snapshot of the number of tasks in case it changes while this
		function is executing. */
		uxArraySize = uxCurrentNumberOfTasks;

		/* Allocate an array index for each task.  NOTE!  if
		configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
		equate to NULL. */
		pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof( TaskStatus_t ) );

		if( pxTaskStatusArray != NULL )
		{
			/* Generate the (binary) data. */
			uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

			/* Create a human readable table from the binary data. */
			for( x = 0; x < uxArraySize; x++ )
			{
				switch( pxTaskStatusArray[ x ].eCurrentState )
				{
					case eReady:		cStatus = tskREADY_CHAR;
										break;

					case eBlocked:		cStatus = tskBLOCKED_CHAR;
										break;

					case eSuspended:	cStatus = tskSUSPENDED_CHAR;
										break;

					case eDeleted:		cStatus = tskDELETED_CHAR;
										break;

					default:			/* Should not get here, but it is included
										to prevent static checking errors. */
										cStatus = 0x00;
										break;
				}

				/* Write the task name to the string, padding with spaces so it
				can be printed in tabular form more easily. */
				pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName );

				/* Write the rest of the string. */
#if configTASKLIST_INCLUDE_COREID
				sprintf( pcWriteBuffer, "\t%c\t%u\t%u\t%u\t%hd\r\n", cStatus, ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber, ( int ) pxTaskStatusArray[ x ].xCoreID );
#else
				sprintf( pcWriteBuffer, "\t%c\t%u\t%u\t%u\r\n", cStatus, ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber );
#endif
				pcWriteBuffer += strlen( pcWriteBuffer );
			}

			/* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
			is 0 then vPortFree() will be #defined to nothing. */
			vPortFree( pxTaskStatusArray );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) ) */
/*----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )

	void vTaskGetRunTimeStats( char *pcWriteBuffer )
	{
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	uint32_t ulTotalTime, ulStatsAsPercentage;

		#if( configUSE_TRACE_FACILITY != 1 )
		{
			#error configUSE_TRACE_FACILITY must also be set to 1 in FreeRTOSConfig.h to use vTaskGetRunTimeStats().
		}
		#endif

		/*
		 * PLEASE NOTE:
		 *
		 * This function is provided for convenience only, and is used by many
		 * of the demo applications.  Do not consider it to be part of the
		 * scheduler.
		 *
		 * vTaskGetRunTimeStats() calls uxTaskGetSystemState(), then formats part
		 * of the uxTaskGetSystemState() output into a human readable table that
		 * displays the amount of time each task has spent in the Running state
		 * in both absolute and percentage terms.
		 *
		 * vTaskGetRunTimeStats() has a dependency on the sprintf() C library
		 * function that might bloat the code size, use a lot of stack, and
		 * provide different results on different platforms.  An alternative,
		 * tiny, third party, and limited functionality implementation of
		 * sprintf() is provided in many of the FreeRTOS/Demo sub-directories in
		 * a file called printf-stdarg.c (note printf-stdarg.c does not provide
		 * a full snprintf() implementation!).
		 *
		 * It is recommended that production systems call uxTaskGetSystemState()
		 * directly to get access to raw stats data, rather than indirectly
		 * through a call to vTaskGetRunTimeStats().
		 */

		/* Make sure the write buffer does not contain a string. */
		*pcWriteBuffer = 0x00;

		/* Take a snapshot of the number of tasks in case it changes while this
		function is executing. */
		uxArraySize = uxCurrentNumberOfTasks;

		/* Allocate an array index for each task.  NOTE!  If
		configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
		equate to NULL. */
		pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof( TaskStatus_t ) );

		if( pxTaskStatusArray != NULL )
		{
			/* Generate the (binary) data. */
			uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalTime );

			/* For percentage calculations. */
			ulTotalTime /= 100UL;

			/* Avoid divide by zero errors. */
			if( ulTotalTime > 0 )
			{
				/* Create a human readable table from the binary data. */
				for( x = 0; x < uxArraySize; x++ )
				{
					/* What percentage of the total run time has the task used?
					This will always be rounded down to the nearest integer.
					ulTotalRunTimeDiv100 has already been divided by 100. */
				    /* Also need to consider total run time of all */
					ulStatsAsPercentage = (pxTaskStatusArray[ x ].ulRunTimeCounter/portNUM_PROCESSORS)/ ulTotalTime;

					/* Write the task name to the string, padding with
					spaces so it can be printed in tabular form more
					easily. */
					pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName );

					if( ulStatsAsPercentage > 0UL )
					{
						#ifdef portLU_PRINTF_SPECIFIER_REQUIRED
						{
							sprintf( pcWriteBuffer, "\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
						}
						#else
						{
							/* sizeof( int ) == sizeof( long ) so a smaller
							printf() library can be used. */
							sprintf( pcWriteBuffer, "\t%u\t\t%u%%\r\n", ( unsigned int ) pxTaskStatusArray[ x ].ulRunTimeCounter, ( unsigned int ) ulStatsAsPercentage );
						}
						#endif
					}
					else
					{
						/* If the percentage is zero here then the task has
						consumed less than 1% of the total run time. */
						#ifdef portLU_PRINTF_SPECIFIER_REQUIRED
						{
							sprintf( pcWriteBuffer, "\t%lu\t\t<1%%\r\n", pxTaskStatusArray[ x ].ulRunTimeCounter );
						}
						#else
						{
							/* sizeof( int ) == sizeof( long ) so a smaller
							printf() library can be used. */
							sprintf( pcWriteBuffer, "\t%u\t\t<1%%\r\n", ( unsigned int ) pxTaskStatusArray[ x ].ulRunTimeCounter );
						}
						#endif
					}

					pcWriteBuffer += strlen( pcWriteBuffer );
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			/* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
			is 0 then vPortFree() will be #defined to nothing. */
			vPortFree( pxTaskStatusArray );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

#endif /* ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) ) */
/*-----------------------------------------------------------*/

TickType_t uxTaskResetEventItemValue( void )
{
TickType_t uxReturn;
	taskENTER_CRITICAL(&xTaskQueueMutex);
	uxReturn = listGET_LIST_ITEM_VALUE( &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ) );

	/* Reset the event list item to its normal value - so it can be used with
	queues and semaphores. */
	listSET_LIST_ITEM_VALUE( &( pxCurrentTCB[ xPortGetCoreID() ]->xEventListItem ), ( ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) pxCurrentTCB[ xPortGetCoreID() ]->uxPriority ) ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
	taskEXIT_CRITICAL(&xTaskQueueMutex);

	return uxReturn;
}
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

	void *pvTaskIncrementMutexHeldCount( void )
	{
	TCB_t *curTCB;

		/* If xSemaphoreCreateMutex() is called before any tasks have been created
		then pxCurrentTCB will be NULL. */
		taskENTER_CRITICAL(&xTaskQueueMutex);
		if( pxCurrentTCB[ xPortGetCoreID() ] != NULL )
		{
			( pxCurrentTCB[ xPortGetCoreID() ]->uxMutexesHeld )++;
		}
		curTCB = pxCurrentTCB[ xPortGetCoreID() ];
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return curTCB;
	}

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

	uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
	{
	TickType_t xTimeToWake;
	uint32_t ulReturn;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* Only block if the notification count is not already non-zero. */
			if( pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue == 0UL )
			{
				/* Mark this task as waiting for a notification. */
				pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState = eWaitingNotification;

				if( xTicksToWait > ( TickType_t ) 0 )
				{
					/* The task is going to block.  First it must be removed
					from the ready list. */
					if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
					{
						/* The current task must be in a ready list, so there is
						no need to check, and the port reset macro can be called
						directly. */
						portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					#if ( INCLUDE_vTaskSuspend == 1 )
					{
						if( xTicksToWait == portMAX_DELAY )
						{
							/* Add the task to the suspended task list instead
							of a delayed task list to ensure the task is not
							woken by a timing event.  It will block
							indefinitely. */
                            traceMOVED_TASK_TO_SUSPENDED_LIST(pxCurrentTCB[ xPortGetCoreID() ]);
							vListInsertEnd( &xSuspendedTaskList, &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) );
						}
						else
						{
							/* Calculate the time at which the task should be
							woken if no notification events occur.  This may
							overflow but this doesn't matter, the scheduler will
							handle it. */
							xTimeToWake = xTickCount + xTicksToWait;
							prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
						}
					}
					#else /* INCLUDE_vTaskSuspend */
					{
							/* Calculate the time at which the task should be
							woken if the event does not occur.  This may
							overflow but this doesn't matter, the scheduler will
							handle it. */
							xTimeToWake = xTickCount + xTicksToWait;
							prvAddCurrentTaskToDelayedList( xTimeToWake );
					}
					#endif /* INCLUDE_vTaskSuspend */

					/* All ports are written to allow a yield in a critical
					section (some will yield immediately, others wait until the
					critical section exits) - but it is not something that
					application code should ever do. */
					portYIELD_WITHIN_API();
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			ulReturn = pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue;

			if( ulReturn != 0UL )
			{
				if( xClearCountOnExit != pdFALSE )
				{
					pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue = 0UL;
				}
				else
				{
					( pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue )--;
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}

			pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState = eNotWaitingNotification;
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return ulReturn;
	}

#endif /* configUSE_TASK_NOTIFICATIONS */
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

	BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait )
	{
	TickType_t xTimeToWake;
	BaseType_t xReturn;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			/* Only block if a notification is not already pending. */
			if( pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState != eNotified )
			{
				/* Clear bits in the task's notification value as bits may get
				set	by the notifying task or interrupt.  This can be used to
				clear the value to zero. */
				pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue &= ~ulBitsToClearOnEntry;

				/* Mark this task as waiting for a notification. */
				pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState = eWaitingNotification;

				if( xTicksToWait > ( TickType_t ) 0 )
				{
					/* The task is going to block.  First it must be removed
					from the	ready list. */
					if( uxListRemove( &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) ) == ( UBaseType_t ) 0 )
					{
						/* The current task must be in a ready list, so there is
						no need to check, and the port reset macro can be called
						directly. */
						portRESET_READY_PRIORITY( pxCurrentTCB[ xPortGetCoreID() ]->uxPriority, uxTopReadyPriority );
					}
					else
					{
						mtCOVERAGE_TEST_MARKER();
					}

					#if ( INCLUDE_vTaskSuspend == 1 )
					{
						if( xTicksToWait == portMAX_DELAY )
						{
							/* Add the task to the suspended task list instead
							of a delayed task list to ensure the task is not
							woken by a timing event.  It will block
							indefinitely. */
                            traceMOVED_TASK_TO_SUSPENDED_LIST(pxCurrentTCB[ xPortGetCoreID() ]);
							vListInsertEnd( &xSuspendedTaskList, &( pxCurrentTCB[ xPortGetCoreID() ]->xGenericListItem ) );
						}
						else
						{
							/* Calculate the time at which the task should be
							woken if no notification events occur.  This may
							overflow but this doesn't matter, the scheduler will
							handle it. */
							xTimeToWake = xTickCount + xTicksToWait;
							prvAddCurrentTaskToDelayedList( xPortGetCoreID(), xTimeToWake );
						}
					}
					#else /* INCLUDE_vTaskSuspend */
					{
							/* Calculate the time at which the task should be
							woken if the event does not occur.  This may
							overflow but this doesn't matter, the scheduler will
							handle it. */
							xTimeToWake = xTickCount + xTicksToWait;
							prvAddCurrentTaskToDelayedList( xTimeToWake );
					}
					#endif /* INCLUDE_vTaskSuspend */

					/* All ports are written to allow a yield in a critical
					section (some will yield immediately, others wait until the
					critical section exits) - but it is not something that
					application code should ever do. */
					portYIELD_WITHIN_API();
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			if( pulNotificationValue != NULL )
			{
				/* Output the current notification value, which may or may not
				have changed. */
				*pulNotificationValue = pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue;
			}

			/* If eNotifyValue is set then either the task never entered the
			blocked state (because a notification was already pending) or the
			task unblocked because of a notification.  Otherwise the task
			unblocked because of a timeout. */
			if( pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState == eWaitingNotification )
			{
				/* A notification was not received. */
				xReturn = pdFALSE;
			}
			else
			{
				/* A notification was already pending or a notification was
				received while the task was waiting. */
				pxCurrentTCB[ xPortGetCoreID() ]->ulNotifiedValue &= ~ulBitsToClearOnExit;
				xReturn = pdTRUE;
			}

			pxCurrentTCB[ xPortGetCoreID() ]->eNotifyState = eNotWaitingNotification;
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return xReturn;
	}

#endif /* configUSE_TASK_NOTIFICATIONS */
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

	BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction )
	{
	TCB_t * pxTCB;
	eNotifyValue eOriginalNotifyState;
	BaseType_t xReturn = pdPASS;

		configASSERT( xTaskToNotify );
		pxTCB = ( TCB_t * ) xTaskToNotify;

		taskENTER_CRITICAL(&xTaskQueueMutex);
		{
			eOriginalNotifyState = pxTCB->eNotifyState;

			pxTCB->eNotifyState = eNotified;

			switch( eAction )
			{
				case eSetBits	:
					pxTCB->ulNotifiedValue |= ulValue;
					break;

				case eIncrement	:
					( pxTCB->ulNotifiedValue )++;
					break;

				case eSetValueWithOverwrite	:
					pxTCB->ulNotifiedValue = ulValue;
					break;

				case eSetValueWithoutOverwrite :
					if( eOriginalNotifyState != eNotified )
					{
						pxTCB->ulNotifiedValue = ulValue;
					}
					else
					{
						/* The value could not be written to the task. */
						xReturn = pdFAIL;
					}
					break;

				case eNoAction:
					/* The task is being notified without its notify value being
					updated. */
					break;
			}


			/* If the task is in the blocked state specifically to wait for a
			notification then unblock it now. */
			if( eOriginalNotifyState == eWaitingNotification )
			{
				( void ) uxListRemove( &( pxTCB->xGenericListItem ) );
				prvAddTaskToReadyList( pxTCB );

				/* The task should not have been on an event list. */
				configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

				if( tskCAN_RUN_HERE(pxTCB->xCoreID) && pxTCB->uxPriority > pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
				{
					/* The notified task has a priority above the currently
					executing task so a yield is required. */
					portYIELD_WITHIN_API();
				}
				else if ( pxTCB->xCoreID != xPortGetCoreID() )
				{
					taskYIELD_OTHER_CORE(pxTCB->xCoreID, pxTCB->uxPriority);
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
			else
			{
				mtCOVERAGE_TEST_MARKER();
			}
		}
		taskEXIT_CRITICAL(&xTaskQueueMutex);

		return xReturn;
	}

#endif /* configUSE_TASK_NOTIFICATIONS */
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

	BaseType_t xTaskNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken )
	{
	TCB_t * pxTCB;
	eNotifyValue eOriginalNotifyState;
	BaseType_t xReturn = pdPASS;

		configASSERT( xTaskToNotify );

		pxTCB = ( TCB_t * ) xTaskToNotify;

		taskENTER_CRITICAL_ISR(&xTaskQueueMutex);

		{
			eOriginalNotifyState = pxTCB->eNotifyState;

			pxTCB->eNotifyState = eNotified;

			switch( eAction )
			{
				case eSetBits	:
					pxTCB->ulNotifiedValue |= ulValue;
					break;

				case eIncrement	:
					( pxTCB->ulNotifiedValue )++;
					break;

				case eSetValueWithOverwrite	:
					pxTCB->ulNotifiedValue = ulValue;
					break;

				case eSetValueWithoutOverwrite :
					if( eOriginalNotifyState != eNotified )
					{
						pxTCB->ulNotifiedValue = ulValue;
					}
					else
					{
						/* The value could not be written to the task. */
						xReturn = pdFAIL;
					}
					break;

				case eNoAction :
					/* The task is being notified without its notify value being
					updated. */
					break;
			}


			/* If the task is in the blocked state specifically to wait for a
			notification then unblock it now. */
			if( eOriginalNotifyState == eWaitingNotification )
			{
				/* The task should not have been on an event list. */
				configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

				if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
				{
					( void ) uxListRemove( &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );
				}
				else
				{
					/* The delayed and ready lists cannot be accessed, so hold
					this task pending until the scheduler is resumed. */
					vListInsertEnd( &( xPendingReadyList[ xPortGetCoreID() ] ), &( pxTCB->xEventListItem ) );
				}

				if( tskCAN_RUN_HERE(pxTCB->xCoreID) && pxTCB->uxPriority > pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
				{
					/* The notified task has a priority above the currently
					executing task so a yield is required. */
					if( pxHigherPriorityTaskWoken != NULL )
					{
						*pxHigherPriorityTaskWoken = pdTRUE;
					}
				}
				else if ( pxTCB->xCoreID != xPortGetCoreID() )
				{
					taskYIELD_OTHER_CORE( pxTCB->xCoreID, pxTCB->uxPriority );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
		}
		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);

		return xReturn;
	}

#endif /* configUSE_TASK_NOTIFICATIONS */
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

	void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken )
	{
	TCB_t * pxTCB;
	eNotifyValue eOriginalNotifyState;

		configASSERT( xTaskToNotify );


		pxTCB = ( TCB_t * ) xTaskToNotify;

		taskENTER_CRITICAL_ISR(&xTaskQueueMutex);
		{
			eOriginalNotifyState = pxTCB->eNotifyState;
			pxTCB->eNotifyState = eNotified;

			/* 'Giving' is equivalent to incrementing a count in a counting
			semaphore. */
			( pxTCB->ulNotifiedValue )++;

			/* If the task is in the blocked state specifically to wait for a
			notification then unblock it now. */
			if( eOriginalNotifyState == eWaitingNotification )
			{
				/* The task should not have been on an event list. */
				configASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

				if( uxSchedulerSuspended[ xPortGetCoreID() ] == ( UBaseType_t ) pdFALSE )
				{
					( void ) uxListRemove( &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );
				}
				else
				{
					/* The delayed and ready lists cannot be accessed, so hold
					this task pending until the scheduler is resumed. */
					vListInsertEnd( &( xPendingReadyList[ xPortGetCoreID() ] ), &( pxTCB->xEventListItem ) );
				}

				if( tskCAN_RUN_HERE(pxTCB->xCoreID) && pxTCB->uxPriority > pxCurrentTCB[ xPortGetCoreID() ]->uxPriority )
				{
					/* The notified task has a priority above the currently
					executing task so a yield is required. */
					if( pxHigherPriorityTaskWoken != NULL )
					{
						*pxHigherPriorityTaskWoken = pdTRUE;
					}
				}
				else if ( pxTCB->xCoreID != xPortGetCoreID() )
				{
					taskYIELD_OTHER_CORE( pxTCB->xCoreID, pxTCB->uxPriority );
				}
				else
				{
					mtCOVERAGE_TEST_MARKER();
				}
			}
		}
		taskEXIT_CRITICAL_ISR(&xTaskQueueMutex);
	}

#endif /* configUSE_TASK_NOTIFICATIONS */

#if ( configENABLE_TASK_SNAPSHOT == 1 )
	static void prvTaskGetSnapshot( TaskSnapshot_t *pxTaskSnapshotArray, UBaseType_t *uxTask, TCB_t *pxTCB )
	{
		if (pxTCB == NULL) {
			return;
		}
		pxTaskSnapshotArray[ *uxTask ].pxTCB = pxTCB;
		pxTaskSnapshotArray[ *uxTask ].pxTopOfStack = (StackType_t *)pxTCB->pxTopOfStack;
		#if( portSTACK_GROWTH < 0 )
		{
			pxTaskSnapshotArray[ *uxTask ].pxEndOfStack = pxTCB->pxEndOfStack;
		}
		#else
		{
			pxTaskSnapshotArray[ *uxTask ].pxEndOfStack = pxTCB->pxStack;
		}
		#endif
		(*uxTask)++;
	}

	static void prvTaskGetSnapshotsFromList( TaskSnapshot_t *pxTaskSnapshotArray, UBaseType_t *uxTask, const UBaseType_t uxArraySize, List_t *pxList )
	{
		TCB_t *pxNextTCB, *pxFirstTCB;

		if( listCURRENT_LIST_LENGTH( pxList ) > ( UBaseType_t ) 0 )
		{
			listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
			do
			{
				if( *uxTask >= uxArraySize )
					break;

				listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
				prvTaskGetSnapshot( pxTaskSnapshotArray, uxTask, pxNextTCB );
			} while( pxNextTCB != pxFirstTCB );
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}

	UBaseType_t uxTaskGetSnapshotAll( TaskSnapshot_t * const pxTaskSnapshotArray, const UBaseType_t uxArraySize, UBaseType_t * const pxTcbSz )
	{
		UBaseType_t uxTask = 0, i = 0;


		*pxTcbSz = sizeof(TCB_t);
		/* Fill in an TaskStatus_t structure with information on each
		task in the Ready state. */
		i = configMAX_PRIORITIES;
		do
		{
			i--;
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, &( pxReadyTasksLists[ i ] ) );
		} while( i > ( UBaseType_t ) tskIDLE_PRIORITY ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

		/* Fill in an TaskStatus_t structure with information on each
		task in the Blocked state. */
		prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, ( List_t * ) pxDelayedTaskList );
		prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, ( List_t * ) pxOverflowDelayedTaskList );
		for (i = 0; i < portNUM_PROCESSORS; i++) {
			if( uxTask >= uxArraySize )
				break;
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, &( xPendingReadyList[ i ] ) );
		}

		#if( INCLUDE_vTaskDelete == 1 )
		{
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, &xTasksWaitingTermination );
		}
		#endif

		#if ( INCLUDE_vTaskSuspend == 1 )
		{
			prvTaskGetSnapshotsFromList( pxTaskSnapshotArray, &uxTask, uxArraySize, &xSuspendedTaskList );
		}
		#endif
		return uxTask;
	}

#endif

#ifdef FREERTOS_MODULE_TEST
	#include "tasks_test_access_functions.h"
#endif

