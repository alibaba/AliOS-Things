/*******************************************************************************
  SPI Driver Functions for Dynamic Standard Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_rm_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.

  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_dynamic_ebm_tasks.c.ftl
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
//DOM-IGNORE-END


#include "driver/spi/src/dynamic/drv_spi_internal.h"
#include <stdbool.h>

<#macro DRV_SPI_MASTER_RM_TASKS_MACRO
            DRV_BIT_WIDTH
            DRV_ISR>
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#else>
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    </#if>
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    /* Check to see if we have any more bytes to transmit */
    if (currentJob->dataLeftToTx + currentJob->dummyLeftToTx == 0)
    {
        <#if DRV_ISR==true>
        /* We don't have any more data to send make sure the transmit interrupt is disabled */
        pDrvInstance->txEnabled = false;
        </#if>
        return 0;
    }
    /* Check to see if the transmit buffer is empty*/
    if (!PLIB_SPI_TransmitBufferIsEmpty(spiId))
    {
        return 0;
    }
    /* Make sure that we don't have something in progress and overrun the RX buffer */
    if (pDrvInstance->symbolsInProgress != 0)
    {
        return 0;
    }
    if (currentJob->dataLeftToTx != 0)
    {
    /* Transmit the data & update the counts */
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferWrite(spiId, currentJob->txBuffer[currentJob->dataTxed]);
        currentJob->dataTxed++;
        currentJob->dataLeftToTx--;
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(spiId, ((uint16_t*)currentJob->txBuffer)[currentJob->dataTxed>>1]);
        currentJob->dataTxed+=2;
        currentJob->dataLeftToTx-=2;
    <#else>
        PLIB_SPI_BufferWrite32bit(spiId, ((uint32_t*)currentJob->txBuffer)[currentJob->dataTxed>>2]);
        currentJob->dataTxed+=4;
        currentJob->dataLeftToTx-=4;
    </#if>
    }
    else
    {
        /* Transmit the dummy data & update the counts */
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferWrite(spiId, (uint8_t)pDrvInstance->dummyByteValue);
        currentJob->dummyLeftToTx--;
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(spiId, (uint16_t)pDrvInstance->dummyByteValue);
        currentJob->dummyLeftToTx-=2;
    <#else>
        PLIB_SPI_BufferWrite32bit(spiId, (uint32_t)pDrvInstance->dummyByteValue);
        currentJob->dummyLeftToTx-=4;
    </#if>
    }
    /* We now have a symbol in progress*/
    pDrvInstance->symbolsInProgress = 1;

    return 0;
}

    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#else>
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterRMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterRMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    </#if>
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    if (currentJob == NULL)
    {
        return 0;
    }
    if (PLIB_SPI_ReceiverBufferIsFull(spiId))
    {
        /* We have data waiting in the SPI buffer */
        if (currentJob->dataLeftToRx != 0)
        {
            /* Receive the data and updates the count */
    <#if DRV_BIT_WIDTH=="8">
            currentJob->rxBuffer[currentJob->dataRxed] = PLIB_SPI_BufferRead(spiId);
            currentJob->dataRxed++;
            currentJob->dataLeftToRx --;
    <#elseif DRV_BIT_WIDTH=="16">
            ((uint16_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>1] = PLIB_SPI_BufferRead16bit(spiId);
            currentJob->dataRxed+=2;
            currentJob->dataLeftToRx -=2;
    <#else>
            ((uint32_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>2] = PLIB_SPI_BufferRead32bit(spiId);
            currentJob->dataRxed+=4;
            currentJob->dataLeftToRx -=4;
    </#if>
        }
        else
        {
            /* No Data but dummy data: Note: We cannot just clear the
               buffer because we have to keep track of how many symbols/units we
               have received, and the number may have increased since we checked
               how full the buffer is.*/
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferRead(spiId);
            //SYS_CONSOLE_MESSAGE("Rd ");
            currentJob->dummyLeftToRx--;
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferRead16bit(spiId);
            currentJob->dummyLeftToRx-=2;
    <#else>
            PLIB_SPI_BufferRead32bit(spiId);
            currentJob->dummyLeftToRx-=4;
    </#if>
        }
        /* No longer have a symbol in progress */
        pDrvInstance->symbolsInProgress = 0;
    }

    return 0;
}

</#macro>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_RM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
     />
</#if>