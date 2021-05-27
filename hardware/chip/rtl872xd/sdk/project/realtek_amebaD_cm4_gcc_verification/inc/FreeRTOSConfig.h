/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
	See http://www.freertos.org/a00110.html for an explanation of the
	definitions contained in this file.
******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__) 
#include <stdint.h> 
extern uint32_t SystemCoreClock; 
#endif 
#ifndef __IASMARM__
#define __IASMARM__ 0 /* IAR */
#endif
#include "platform_autoconf.h"

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

#define configUSE_SAU					1
#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				( SystemCoreClock )
#define configTICK_RATE_HZ				( ( uint32_t ) 1000)
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 512 )
#ifdef CONFIG_WIFI_EN
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 150 * 1024 ) ) //default
#if (defined CONFIG_HIGH_TP_TEST)
	#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 100 * 1024 ) )		
#endif
#else
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 40 * 1024 ) )
#endif
#define configMAX_TASK_NAME_LEN			( 10 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0
#define configUSE_CO_ROUTINES 			1
#define configUSE_MUTEXES				1
#define configUSE_TIMERS                1

#define configMAX_PRIORITIES			( 11 )
#define PRIORITIE_OFFSET				( 4 )

#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define configUSE_COUNTING_SEMAPHORES 	1
#define configUSE_ALTERNATIVE_API 		0
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		1
#define configQUEUE_REGISTRY_SIZE		0
#define configGENERATE_RUN_TIME_STATS	0
#if configGENERATE_RUN_TIME_STATS
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() //( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE() RTIM_GetCount(0x48002000/*TIMM00*/) //ulHighFrequencyTimerTicks
#undef	configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY			1
#define portCONFIGURE_STATS_PEROID_VALUE	1000 //unit Ticks
#endif

#define configTIMER_TASK_PRIORITY       ( 1 )
#define configTIMER_QUEUE_LENGTH        ( 10 )
#define configTIMER_TASK_STACK_DEPTH    ( 512 )     //USE_MIN_STACK_SIZE modify from 512 to 256

#if (__IASMARM__ != 1)

extern void freertos_pre_sleep_processing(unsigned int *expected_idle_time);
extern void freertos_post_sleep_processing(unsigned int *expected_idle_time);
extern int  freertos_ready_to_sleep(void);

/* Enable tickless power saving. */
#define configUSE_TICKLESS_IDLE                 1

/* In wlan usage, this value is suggested to use value less than 80 milliseconds */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   2

/* It's magic trick that let us can use our own sleep function */
#define configPRE_SLEEP_PROCESSING( x )         ( freertos_pre_sleep_processing((unsigned int *)&x) )

#define configPOST_SLEEP_PROCESSING( x )        ( freertos_post_sleep_processing((unsigned int *)&x) )

/* It's magic trick that let us can enable/disable tickless dynamically */
#define traceLOW_POWER_IDLE_BEGIN();           {

                                                        // portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );

#define traceLOW_POWER_IDLE_END();              }

/* It's FreeRTOS related feature but it's not included in FreeRTOS design. */
#define configUSE_WAKELOCK_PMU                  1

#endif // #if (__IASMARM__ != 1)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_pcTaskGetTaskName       1
#define INCLUDE_xTimerPendFunctionCall 	1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif


/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0f

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5


/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

//#define RTK_MODE_TIMER


#endif /* FREERTOS_CONFIG_H */
