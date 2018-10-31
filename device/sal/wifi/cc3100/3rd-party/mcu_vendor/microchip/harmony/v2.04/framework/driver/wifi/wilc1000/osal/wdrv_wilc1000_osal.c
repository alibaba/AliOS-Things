/*******************************************************************************
  WILC1000 Wireless Driver OS Abstraction Layer

  File Name:
    wilc1000_osal.c

  Summary:
    OS abstraction layer for WILC1000 wireless driver.

  Description:
    OS abstraction layer for WILC1000 wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "wdrv_wilc1000_api.h"

uint32_t WDRV_MutexInit(OSAL_MUTEX_HANDLE_TYPE *mutex_ptr)
{
	OSAL_MUTEX_Create(mutex_ptr) ;
    return 0;
}

uint32_t WDRV_MutexDestroy(OSAL_MUTEX_HANDLE_TYPE *mutex_ptr)
{
    OSAL_MUTEX_Delete(mutex_ptr);
    return 0;
}

uint32_t WDRV_MutexLock(OSAL_MUTEX_HANDLE_TYPE *mutex_ptr, uint32_t tick_count)
{
    OSAL_MUTEX_Lock(mutex_ptr, OSAL_WAIT_FOREVER);
    return 0;
}

uint32_t WDRV_MutexUnlock(OSAL_MUTEX_HANDLE_TYPE *mutex_ptr)
{
    OSAL_MUTEX_Unlock(mutex_ptr);
    return 0;
}

bool WDRV_SemInit(OSAL_SEM_HANDLE_TYPE *SemID)
{
    if (OSAL_SEM_Create(SemID, OSAL_SEM_TYPE_COUNTING, 10, 0) == OSAL_RESULT_TRUE)
        return true;
    else
        return false;
}

void WDRV_SemTake(OSAL_SEM_HANDLE_TYPE *SemID, uint16_t timeout)
{
    OSAL_SEM_Pend(SemID, timeout);
}

void WDRV_SemGive(OSAL_SEM_HANDLE_TYPE *SemID)
{
    OSAL_SEM_Post(SemID);
}

void WDRV_SemGiveFromISR(OSAL_SEM_HANDLE_TYPE *SemID)
{
    OSAL_SEM_PostISR(SemID);
}

void WDRV_SemDeInit(OSAL_SEM_HANDLE_TYPE *SemID)
{
    OSAL_SEM_Delete(SemID);
}

/*
 * OSAL_USE_RTOS == 1 means FreeRTOS version 8.x.x is used.
 * OSAL_USE_RTOS == 9 means the latest FreeRTOS version that comes with Harmony
 *  is used.
 * These are the only two cases supported by current WILC1000 driver.
 * Following functions are implemented specifically for these two cases.
 */
#if (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9)

static WDRV_OSAL_STATUS ErrorCodeGet(uint32_t os_err)
{
    WDRV_OSAL_STATUS osal_ret;

    switch(os_err)
    {
    case pdPASS:
        osal_ret = WDRV_SUCCESS;
        break;
    case pdFAIL:
        osal_ret = WDRV_ERROR;
        break;
    default:
        osal_ret = WDRV_ERROR;
    }

    return osal_ret;
}

WDRV_OSAL_STATUS WDRV_TaskCreate(void Task(void *), const char *task_name, int stack_size, void *param,
    unsigned long task_priority, TaskHandle_t *task_handle, bool auto_start)
{
    WDRV_OSAL_STATUS ret = WDRV_SUCCESS;
    uint32_t os_ret;

    os_ret = xTaskCreate((TaskFunction_t)Task,  /* pointer to the task entry code */
            task_name,                          /* task name */
            stack_size,                         /* task stack size */
            param,                              /* parameters to pass */
            task_priority,                      /* task priority - lower number indicates lower priority */
            task_handle);                       /* handle by which task can be referenced */

    ret = ErrorCodeGet(os_ret);
    return ret;
}

WDRV_OSAL_STATUS WDRV_TaskDestroy(TaskHandle_t task_handle)
{
    WDRV_OSAL_STATUS ret = WDRV_SUCCESS;

    vTaskDelete(task_handle);
    return ret;
}

void WDRV_UsecDelay(uint32_t uSec)
{
    unsigned long xUnblockPeriod = uSec/1000;

    if(!xUnblockPeriod)
    {
        xUnblockPeriod = 1;
    }

    vTaskDelay(xUnblockPeriod * portTICK_PERIOD_MS);

    return;
}

#endif /* (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9) */

//DOM-IGNORE-END
