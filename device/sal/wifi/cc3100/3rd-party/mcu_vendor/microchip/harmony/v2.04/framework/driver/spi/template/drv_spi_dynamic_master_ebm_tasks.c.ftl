/*******************************************************************************
  SPI Driver Functions for Dynamic Enhanced Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_ebm_tasks.c

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

<#macro DRV_SPI_MASTER_EBM_TASKS_MACRO
            DRV_BIT_WIDTH
            DRV_ISR>
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#else>
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    </#if>
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    /* Determine the maximum number of bytes we can send to the FIFO*/
        uint8_t symbolsInTransit = MAX(pDrvInstance->symbolsInProgress, PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_TRANSMIT));
    <#if DRV_BIT_WIDTH=="8">
        uint8_t bufferBytes = PLIB_SPI_TX_8BIT_FIFO_SIZE(spiId) - symbolsInTransit;
    <#elseif DRV_BIT_WIDTH=="16">
        uint8_t bufferBytes = (PLIB_SPI_TX_16BIT_FIFO_SIZE(spiId) - symbolsInTransit) << 1;
    <#else>
        uint8_t bufferBytes = (PLIB_SPI_TX_32BIT_FIFO_SIZE(spiId) - symbolsInTransit) << 2;
    </#if>
    /* Figure out how much data we can send*/
    size_t dataUnits = MIN(currentJob->dataLeftToTx, bufferBytes);

    size_t counter;

    if (dataUnits != 0)
    {
        /* Adjust the maximum buffer size downwards based on how much data we'll be sending*/
        bufferBytes -= dataUnits;
        currentJob->dataLeftToTx -= dataUnits;
        /* Set the location in the buffer of where to start sending from*/
    <#if DRV_BIT_WIDTH=="8">
        uint8_t *bufferLoc = &(currentJob->txBuffer[currentJob->dataTxed]);
    <#elseif DRV_BIT_WIDTH=="16">
        uint16_t *bufferLoc = (uint16_t*)&(currentJob->txBuffer[currentJob->dataTxed]);
        /* change the number of data units to be in units of 16 bits*/
        dataUnits >>=1;
    <#else>
        uint32_t *bufferLoc = (uint32_t*)&(currentJob->txBuffer[currentJob->dataTxed]);
        /* change the number of data units to be in units of 32 bits*/
        dataUnits >>=2;
    </#if>
        for (counter = 0; counter < dataUnits; counter++)
        {
            /* Send a unit/symbol of data*/
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferWrite(spiId, bufferLoc[counter]);
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferWrite16bit(spiId, bufferLoc[counter]);
    <#else>
            PLIB_SPI_BufferWrite32bit(spiId, bufferLoc[counter]);
    </#if>
        }
        /* Update the number of bytes transmitted*/
    <#if DRV_BIT_WIDTH=="8">
        currentJob->dataTxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
        currentJob->dataTxed += dataUnits<<1;
    <#else>
        currentJob->dataTxed += dataUnits<<2;
    </#if>
        /* Adjust the symbols in progress */
        pDrvInstance->symbolsInProgress += dataUnits;
    }
    size_t dummyUnits = MIN(currentJob->dummyLeftToTx, bufferBytes);
    if (dummyUnits != 0)
    {
        currentJob->dummyLeftToTx -= dummyUnits;
    <#if DRV_BIT_WIDTH=="16">
        /* change the number of dummy units to be in units of 16 bits*/
        dummyUnits >>=1;
    <#elseif DRV_BIT_WIDTH=="32">
        /* change the number of dummy units to be in units of 32 bits*/
        dummyUnits >>=2;
    </#if>
        /* Adjust the symbols in progress */
        pDrvInstance->symbolsInProgress += dummyUnits;

        for (counter = 0; counter < dummyUnits; counter++)
        {
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferWrite(spiId, (uint8_t)pDrvInstance->dummyByteValue);
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferWrite16bit(spiId, (uint16_t)pDrvInstance->dummyByteValue);
    <#else>
            PLIB_SPI_BufferWrite32bit(spiId, (uint32_t)pDrvInstance->dummyByteValue);
    </#if>
        }
    }
<#if DRV_ISR==true>
    if (currentJob->dataLeftToTx + currentJob->dummyLeftToTx == 0)
    {
        /* We have no more data to send, turn off the TX interrupt*/
        PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
        pDrvInstance->txEnabled = false;

        /* Turn on the RX Interrupt*/
        pDrvInstance->rxEnabled = true;
    }
</#if>
    return 0;
}

    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    <#else>
        <#if DRV_ISR==true>
int32_t DRV_SPI_MasterEBMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        <#else>
int32_t DRV_SPI_MasterEBMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
        </#if>
    </#if>
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    /* Figure out how many bytes are waiting to be received."*/
    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE) << 1;
    <#else>
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE) << 2;
    </#if>
    /* Calculate the maximum number of data bytes that can be received*/
    size_t dataUnits = MIN(currentJob->dataLeftToRx, bufferBytes);
    size_t counter;

    if (dataUnits != 0)
    {
        bufferBytes -= dataUnits;
        currentJob->dataLeftToRx -= dataUnits;
        /* Set the buffer location to receive bytes from the SPI to*/
    <#if DRV_BIT_WIDTH=="8">
        uint8_t *bufferLoc = &(currentJob->rxBuffer[currentJob->dataRxed]);
    <#elseif DRV_BIT_WIDTH=="16">
        uint16_t *bufferLoc = (uint16_t*)&(currentJob->rxBuffer[currentJob->dataRxed]);
        /* Adjust the number of data units to be in units of 16 bits */
        dataUnits >>=1;
    <#else>
        uint32_t *bufferLoc = (uint32_t*)&(currentJob->rxBuffer[currentJob->dataRxed]);
        /* Adjust the number of data units to be in units of 32 bits */
        dataUnits >>=2;
    </#if>
        for (counter = 0; counter < dataUnits; counter++)
        {
            /* Receive the data from the SPI */
    <#if DRV_BIT_WIDTH=="8">
            bufferLoc[counter] = PLIB_SPI_BufferRead(spiId);
    <#elseif DRV_BIT_WIDTH=="16">
            bufferLoc[counter] = PLIB_SPI_BufferRead16bit(spiId);
    <#else>
            bufferLoc[counter] = PLIB_SPI_BufferRead32bit(spiId);
    </#if>
        }
        /* Adjust the amount of data that has been received */
    <#if DRV_BIT_WIDTH=="8">
        currentJob->dataRxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
        currentJob->dataRxed += dataUnits<<1;
    <#else>
        currentJob->dataRxed += dataUnits<<2;
    </#if>
        /* Update the symbols in progress so we can send more units later */
        pDrvInstance->symbolsInProgress -= dataUnits;
    }

    /* Figure out the maximum number of dummy data to be received */
    size_t dummyUnits = MIN(currentJob->dummyLeftToRx, bufferBytes);
    if (dummyUnits != 0)
    {
        /* Lower the number of dummy bytes to be received */
        currentJob->dummyLeftToRx -= dummyUnits;
    <#if DRV_BIT_WIDTH=="16">
        /* Adjust the dummy units into units of 16 bites */
        dummyUnits >>=1;
    <#elseif DRV_BIT_WIDTH=="32">
        /* Adjust the dummy units into units of 32 bites */
        dummyUnits >>=2;
    </#if>
        pDrvInstance->symbolsInProgress -= dummyUnits;
        for (counter = 0; counter < dummyUnits; counter++)
        {
            /* Receive and throw away the byte.  Note: We cannot just clear the
               buffer because we have to keep track of how many symbols/units we
               have received, and the number may have increased since we checked
               how full the buffer is.*/
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferRead(spiId);
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferRead16bit(spiId);
    <#else>
            PLIB_SPI_BufferRead32bit(spiId);
    </#if>
        }
    }

    <#if DRV_ISR==true>
    /* Figure out how many bytes are left to be received */
    size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;

    /* If the bytes left are smaller than the HW mark we have to change the interrupt mode */
    if (bytesLeft < PLIB_SPI_RX_8BIT_HW_MARK(spiId))
    {
        PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    }
    </#if>
    return 0;
}

</#macro>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
     />
</#if>
