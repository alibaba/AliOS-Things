/*******************************************************************************
  SPI Driver Functions for Dynamic Enhanced Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_slave_ebm_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.
    
  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_dynamic_slave_ebm_tasks.c.ftl
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

<#macro DRV_SPI_SLAVE_EBM_TASKS_MACRO
            DRV_BIT_WIDTH 
            DRV_ISR>
    <#if DRV_ISR>
        <#assign taskMode="ISR"/>
    <#else>
        <#assign taskMode="Polled"/>
    </#if>
            
int32_t DRV_SPI_SlaveEBMSend${DRV_BIT_WIDTH}Bit${taskMode}( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    if (currentJob == NULL)
    {
        return 0;
    }
    /* determine the maximum number of bytes that can be transmitted */
    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_TX_8BIT_FIFO_SIZE(spiId) - PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_TRANSMIT);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = (PLIB_SPI_TX_16BIT_FIFO_SIZE(spiId) - PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_TRANSMIT)) << 1;
    <#else>
    uint8_t bufferBytes = (PLIB_SPI_TX_32BIT_FIFO_SIZE(spiId) - PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_TRANSMIT)) << 2;
    </#if>
    size_t dataUnits = MIN(currentJob->dataLeftToTx, bufferBytes);
    currentJob->dataLeftToTx -= dataUnits;


    <#if DRV_ISR==true>
    /* Check to see if we'll run out of data*/
    if (currentJob->dataLeftToTx < bufferBytes)
    {
        PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
        SYS_INT_SourceDisable(pDrvInstance->txInterruptSource);
        SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
    }
    </#if>

    /* Set the buffer location of where to start transmitting from*/
    <#if DRV_BIT_WIDTH=="8">
    uint8_t *bufferLoc = &(currentJob->txBuffer[currentJob->dataTxed]);
    <#elseif DRV_BIT_WIDTH=="16">
    uint16_t *bufferLoc = (uint16_t*)&(currentJob->txBuffer[currentJob->dataTxed]);
    /* Convert the data units to 16 bit units */
    dataUnits >>=1;
    <#else>
    uint32_t *bufferLoc = (uint32_t*)&(currentJob->txBuffer[currentJob->dataTxed]);
    /* Convert the data units to 32 bit units */
    dataUnits >>=2;
    </#if>
    size_t counter;
    for (counter = 0; counter < dataUnits; counter++)
    {
        /* Add the data to the buffer */
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferWrite(spiId, bufferLoc[counter]);
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(spiId, bufferLoc[counter]);
    <#else>
        PLIB_SPI_BufferWrite32bit(spiId, bufferLoc[counter]);
    </#if>
    }

    <#if DRV_BIT_WIDTH=="8">
    currentJob->dataTxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
    currentJob->dataTxed += dataUnits<<1;
    <#else>
    currentJob->dataTxed += dataUnits<<2;
    </#if>
    
    <#if DRV_ISR==true>
    SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
    </#if>
    return 0;
    
}

int32_t DRV_SPI_SlaveEBMReceive${DRV_BIT_WIDTH}Bit${taskMode}( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    register SPI_MODULE_ID spiId = pDrvInstance->spiId;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
    {
        return 0;
    }

    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE) << 1;
    <#else>
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(spiId, SPI_FIFO_TYPE_RECEIVE) << 2;
    </#if>
    size_t dataUnits = MIN(currentJob->dataLeftToRx, bufferBytes);
    bufferBytes -= dataUnits;
    currentJob->dataLeftToRx -= dataUnits;
    size_t dummyUnits = MIN(currentJob->dummyLeftToRx, bufferBytes);
    currentJob->dummyLeftToRx -= dummyUnits;

    <#if DRV_BIT_WIDTH=="8">
    uint8_t *bufferLoc = &(currentJob->rxBuffer[currentJob->dataRxed]);
    <#elseif DRV_BIT_WIDTH=="16">
    uint16_t *bufferLoc = (uint16_t*)&(currentJob->rxBuffer[currentJob->dataRxed]);
    dataUnits >>=1;
    <#else>
    uint32_t *bufferLoc = (uint32_t*)&(currentJob->rxBuffer[currentJob->dataRxed]);
    dataUnits >>=2;
    </#if>
    size_t counter;
    for (counter = 0; counter < dataUnits; counter++)
    {
    <#if DRV_BIT_WIDTH=="8">
        bufferLoc[counter] = PLIB_SPI_BufferRead(spiId);
    <#elseif DRV_BIT_WIDTH=="16">
        bufferLoc[counter] = PLIB_SPI_BufferRead16bit(spiId);
    <#else>
        bufferLoc[counter] = PLIB_SPI_BufferRead32bit(spiId);
    </#if>
    }

    for (counter = 0; counter < dummyUnits; counter++)
    {
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferRead(spiId);
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferRead16bit(spiId);
    <#else>
        PLIB_SPI_BufferRead32bit(spiId);
    </#if>
    }
    <#if DRV_BIT_WIDTH=="8">
    currentJob->dataRxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
    currentJob->dataRxed += dataUnits<<1;
    <#else>
    currentJob->dataRxed += dataUnits<<2;
    </#if>

    <#if DRV_ISR==true>
    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx < PLIB_SPI_RX_8BIT_HW_MARK(spiId))
    {
        PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    }
    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
    {
        SYS_INT_SourceDisable(pDrvInstance->rxInterruptSource);
        SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);
    }
    SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
    </#if>
    return 0;    
}
</#macro>


<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_SLAVE_EBM_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
     />
</#if>