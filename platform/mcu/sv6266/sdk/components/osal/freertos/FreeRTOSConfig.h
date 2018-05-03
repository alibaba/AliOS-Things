/*
    FreeRTOS V8.0.0 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
      FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

//Unit(ms)
#define OS_CONTEXT_SWITCHING_INTERVAL   1
#define OS_TICK_MAGIC_NUMBER            (1000/OS_CONTEXT_SWITCHING_INTERVAL)

#define configUSE_PREEMPTION                        1
// Freddie Todo: Add idle and tick hook
#define configUSE_IDLE_HOOK                         0
#define configUSE_TICK_HOOK                         0
#define configCPU_CLOCK_HZ                          ( ( unsigned long ) 40000000 )
#define configTICK_RATE_HZ                          ( ( portTickType ) OS_TICK_MAGIC_NUMBER )
//#define configMAX_PRIORITIES                        ( ( unsigned portBASE_TYPE ) 20 )
#define configMAX_PRIORITIES                      ( ( unsigned portBASE_TYPE ) 5 )

#define configMINIMAL_STACK_SIZE                    ( ( unsigned short ) MINIMAL_STACK_SIZE )
#define configTOTAL_HEAP_SIZE                       ( ( size_t ) 40960 )//( ( size_t ) 8192 )
#define configMAX_TASK_NAME_LEN                     ( 16 )
#define configUSE_TRACE_FACILITY                    1
#define configUSE_16_BIT_TICKS                      0
#define configIDLE_SHOULD_YIELD                     1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                       0
#define configMAX_CO_ROUTINE_PRIORITIES             ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                    1
#define INCLUDE_uxTaskPriorityGet                   1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskCleanUpResources               0
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_vTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_uxTaskGetStackHighWaterMark         1
#define INCLUDE_xTaskGetSchedulerState              1
#define INCLUDE_xTaskGetCurrentTaskHandle           1
#define INCLUDE_xTaskGetIdleTaskHandle              1
#define INCLUDE_pcTaskGetTaskName                   1

/* IPC Semaphore */
#define configUSE_COUNTING_SEMAPHORES               1

#define configUSE_MUTEXES                           1
#define configGENERATE_RUN_TIME_STATS               0
#define configUSE_RECURSIVE_MUTEXES                 1
#define configUSE_APPLICATION_TASK_TAG              1
#define configUSE_ALTERNATIVE_API                   1

#define configLIST_VOLATILE                         volatile

/* For debug */
#define configCHECK_FOR_STACK_OVERFLOW              0
#define configUSE_MALLOC_FAILED_HOOK                0

/* For OS aware debugging */
// iComm #define configQUEUE_REGISTRY_SIZE                   10
#define configQUEUE_REGISTRY_SIZE                   10

/* For EventGroups */
#define configUSE_TIMERS                            1    /* EventGroup need SW Timer daemon Task */
#define configTIMER_TASK_PRIORITY                   4    /* Daemon task priority */
#define configTIMER_QUEUE_LENGTH                    5    /* Command Queue of daemon task */
#define configTIMER_TASK_STACK_DEPTH                ( configMINIMAL_STACK_SIZE * 4 )  /* Daemon stack size */

#define INCLUDE_eTaskGetState                       0    /* EventGroup demo program need */
#define INCLUDE_xTimerPendFunctionCall              1    /* EventGroup "FromISR" API need */
#define INCLUDE_xTimerGetTimerDaemonTaskHandle      0

#if 1

extern void vAssertCalled(const char *func, int line);

#define configASSERT( x ) \
    do { \
      if ( /*lint -save -e731 */ ( x ) == 0 /*lint -restore */) \
        vAssertCalled(__func__, __LINE__); \
    } while (0)

#endif // 0

/* Arch Optimization Option */
//#define configUSE_PORT_OPTIMISED_TASK_SELECTION        1

/* Tickless idle/low power functionality support. */
#define configUSE_TICKLESS_IDLE                0

#endif /* FREERTOS_CONFIG_H */
