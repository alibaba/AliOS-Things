/**
 * \file
 * \brief FreeRTOS Hardware/OS Abstration Layer.
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "atca_hal.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#ifndef ATCA_MUTEX_TIMEOUT
#define ATCA_MUTEX_TIMEOUT  portMAX_DELAY
#endif

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

#ifdef ATCA_USE_RTOS_TIMER
/**
 * \brief This function delays for a number of milliseconds.
 *
 *        You can override this function if you like to do
 *        something else in your system while delaying.
 *
 * \param[in] delay  Number of milliseconds to delay
 */
void atca_delay_ms(uint32_t delay)
{
    /* If the freeRTOS scheduler is running yield to other tasks */
    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        vTaskDelay(pdMS_TO_TICKS(delay) + 1);
    }
    else
    {
        atca_delay_ms_internal(delay);
    }
}
#endif

ATCA_STATUS hal_create_mutex(void ** ppMutex, char* pName)
{
    (void)pName;

    if (!ppMutex)
    {
        return ATCA_BAD_PARAM;
    }

    (*ppMutex) = xSemaphoreCreateMutex();

    if (!*ppMutex)
    {
        return ATCA_FUNC_FAIL;
    }

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_destroy_mutex(void * pMutex)
{
    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    vSemaphoreDelete(pMutex);

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_lock_mutex(void * pMutex)
{
    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    if (!xSemaphoreTake((SemaphoreHandle_t)pMutex, ( TickType_t )ATCA_MUTEX_TIMEOUT))
    {
        return ATCA_GEN_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

ATCA_STATUS hal_unlock_mutex(void * pMutex)
{
    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    if (!xSemaphoreGive((SemaphoreHandle_t)pMutex))
    {
        return ATCA_GEN_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}


/** @} */
