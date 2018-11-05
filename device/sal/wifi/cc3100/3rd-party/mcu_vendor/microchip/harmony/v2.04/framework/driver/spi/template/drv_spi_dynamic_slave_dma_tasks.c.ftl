/*******************************************************************************
  SPI Driver Functions for Dynamic Standard Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_slave_dma_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.
    
  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_dynamic_slave_dma_tasks.c.ftl
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


<#macro DRV_SPI_SLAVE_DMA_TASKS_MACRO
            DRV_BIT_WIDTH 
            DRV_ISR>  
    <#if DRV_ISR>
        <#assign taskMode="ISR"/>
    <#else>
        <#assign taskMode="Polled"/>
    </#if>

    void DRV_SPI_SlaveDMASendData${DRV_BIT_WIDTH}Bit${taskMode}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    if (currentJob->dataLeftToTx != 0)
    {
            uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToTx), DRV_SPI_DMA_TXFER_SIZE);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToTx -= len;
            currentJob->dataTxed += len;
    }
    else
    {
        // Job is done
        currentJob->txDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        pDrvInstance->rxEnabled = true;
        SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
        SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);
    }
}


void DRV_SPI_${taskMode}DMASlaveSendEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->txDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI_SlaveDMASendData${DRV_BIT_WIDTH}Bit${taskMode}(event, handle, contextHandle);
            break;
        default:
            break;
    }
}
void DRV_SPI_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${taskMode}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    if (currentJob->dummyLeftToRx != 0)
    {
            uint8_t * ptr = sDrvSpiRxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dummyLeftToRx), DRV_SPI_DMA_TXFER_SIZE), DRV_SPI_DMA_DUMMY_BUFFER_SIZE);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToRx -= len;
    }
    else
    {
        // Job is done
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        pDrvInstance->txEnabled = true;
        SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);
    }
}

void DRV_SPI_SlaveDMAReceiveData${DRV_BIT_WIDTH}Bit${taskMode}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    if (currentJob->dataLeftToRx != 0)
    {
            uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToRx), DRV_SPI_DMA_TXFER_SIZE);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToRx -= len;
            currentJob->dataRxed += len;
    }
    else
    {
        DRV_SPI_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${taskMode}(event,  handle,  contextHandle);
    }
}

void DRV_SPI_${taskMode}DMASlaveReceiveEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->rxDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI_SlaveDMAReceiveData${DRV_BIT_WIDTH}Bit${taskMode}(event, handle, contextHandle);
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
            DRV_SPI_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${taskMode}(event, handle, contextHandle);
            break;
        default:
            break;
    }
}

</#macro>    


<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_SLAVE_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
     />
</#if>