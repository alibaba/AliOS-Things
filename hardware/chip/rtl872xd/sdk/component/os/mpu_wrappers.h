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

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* This file redefines API functions to be called through a wrapper macro, but
only for ports that are using the MPU. */
#ifdef portUSING_MPU_WRAPPERS

	/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE will be defined when this file is
	included from queue.c or task.c to prevent it from having an effect within
	those files. */
	#ifndef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

		#define xTaskGenericCreate				MPU_xTaskGenericCreate
		#define vTaskAllocateMPURegions			MPU_vTaskAllocateMPURegions
		#define vTaskDelete						MPU_vTaskDelete
		#define vTaskDelayUntil					MPU_vTaskDelayUntil
		#define vTaskDelay						MPU_vTaskDelay
		#define uxTaskPriorityGet				MPU_uxTaskPriorityGet
		#define vTaskPrioritySet				MPU_vTaskPrioritySet
		#define eTaskGetState					MPU_eTaskGetState
		#define vTaskSuspend					MPU_vTaskSuspend
		#define vTaskResume						MPU_vTaskResume
		#define vTaskSuspendAll					MPU_vTaskSuspendAll
		#define xTaskResumeAll					MPU_xTaskResumeAll
		#define xTaskGetTickCount				MPU_xTaskGetTickCount
		#define uxTaskGetNumberOfTasks			MPU_uxTaskGetNumberOfTasks
		#define vTaskList						MPU_vTaskList
		#define vTaskGetRunTimeStats			MPU_vTaskGetRunTimeStats
		#define vTaskSetApplicationTaskTag		MPU_vTaskSetApplicationTaskTag
		#define xTaskGetApplicationTaskTag		MPU_xTaskGetApplicationTaskTag
		#define xTaskCallApplicationTaskHook	MPU_xTaskCallApplicationTaskHook
		#define uxTaskGetStackHighWaterMark		MPU_uxTaskGetStackHighWaterMark
		#define xTaskGetCurrentTaskHandle		MPU_xTaskGetCurrentTaskHandle
		#define xTaskGetSchedulerState			MPU_xTaskGetSchedulerState
		#define xTaskGetIdleTaskHandle			MPU_xTaskGetIdleTaskHandle
		#define uxTaskGetSystemState			MPU_uxTaskGetSystemState

		#define xQueueGenericCreate				MPU_xQueueGenericCreate
		#define xQueueCreateMutex				MPU_xQueueCreateMutex
		#define xQueueGiveMutexRecursive		MPU_xQueueGiveMutexRecursive
		#define xQueueTakeMutexRecursive		MPU_xQueueTakeMutexRecursive
		#define xQueueCreateCountingSemaphore	MPU_xQueueCreateCountingSemaphore
		#define xQueueGenericSend				MPU_xQueueGenericSend
		#define xQueueAltGenericSend			MPU_xQueueAltGenericSend
		#define xQueueAltGenericReceive			MPU_xQueueAltGenericReceive
		#define xQueueGenericReceive			MPU_xQueueGenericReceive
		#define uxQueueMessagesWaiting			MPU_uxQueueMessagesWaiting
		#define vQueueDelete					MPU_vQueueDelete
		#define xQueueGenericReset				MPU_xQueueGenericReset
		#define xQueueCreateSet					MPU_xQueueCreateSet
		#define xQueueSelectFromSet				MPU_xQueueSelectFromSet
		#define xQueueAddToSet					MPU_xQueueAddToSet
		#define xQueueRemoveFromSet				MPU_xQueueRemoveFromSet
		#define xQueuePeekFromISR				MPU_xQueuePeekFromISR
		#define xQueueGetMutexHolder			MPU_xQueueGetMutexHolder

		#define pvPortMalloc					MPU_pvPortMalloc
		#define vPortFree						MPU_vPortFree
		#define xPortGetFreeHeapSize			MPU_xPortGetFreeHeapSize
		#define vPortInitialiseBlocks			MPU_vPortInitialiseBlocks

		#if configQUEUE_REGISTRY_SIZE > 0
			#define vQueueAddToRegistry				MPU_vQueueAddToRegistry
			#define vQueueUnregisterQueue			MPU_vQueueUnregisterQueue
		#endif

		/* Remove the privileged function macro. */
		#define PRIVILEGED_FUNCTION

	#else /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

		/* Ensure API functions go in the privileged execution section. */
		#define PRIVILEGED_FUNCTION __attribute__((section("privileged_functions")))
		#define PRIVILEGED_DATA __attribute__((section("privileged_data")))

	#endif /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

#else /* portUSING_MPU_WRAPPERS */

	#define PRIVILEGED_FUNCTION
	#define PRIVILEGED_DATA
	#define portUSING_MPU_WRAPPERS 0

#endif /* portUSING_MPU_WRAPPERS */


#endif /* MPU_WRAPPERS_H */

