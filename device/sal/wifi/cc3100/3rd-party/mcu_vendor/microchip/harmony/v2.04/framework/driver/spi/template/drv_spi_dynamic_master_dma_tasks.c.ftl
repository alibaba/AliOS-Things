/*******************************************************************************
  SPI Driver Functions for Dynamic Standard Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_master_dma_tasks.c

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


<#macro DRV_SPI_MASTER_DMA_TASKS_MACRO
            DRV_BIT_WIDTH 
            DRV_ISR>  

    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendDummy8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendDummyPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendDummy16BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendDummy16BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendDummy32BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendDummy32BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
    if (currentJob->dummyLeftToTx != 0)
    {
            uint8_t * ptr = sDrvSpiTxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dummyLeftToTx), DRV_SPI_DMA_TXFER_SIZE), DRV_SPI_DMA_DUMMY_BUFFER_SIZE);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToTx -= len;
    }
    else
    {
        // Job is done
        currentJob->txDMAProgressStage = DRV_SPI_DMA_COMPLETE;
    }

}


    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendData8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendDataPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendData16BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendData16BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMASendData32BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMASendData32BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
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
        if (currentJob->dummyLeftToTx != 0)
        {
            SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);
        }
    }
}


    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->txDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendData8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendDataPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendData16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendData16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendData32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendData32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendDummy8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendDummyPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendDummy16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendDummy16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMASendDummy32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMASendDummy32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        default:
            break;
    }
}
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveDummy8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveDummyPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveDummy16BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveDummy16BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveDummy32BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveDummy32BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
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
        SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
    }
}

    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveData8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveDataPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveData16BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveData16BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_MasterDMAReceiveData32BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_MasterDMAReceiveData32BitPolled(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
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
        // Job is done
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
    }
}

    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    <#else>
        <#if DRV_ISR==true>
void DRV_SPI_ISRDMAMasterReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        <#else>
void DRV_SPI_PolledDMAMasterReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
        </#if>
    </#if>
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->rxDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveData8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveDataPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveData16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveData16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveData32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveData32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveDummy8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveDummyPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveDummy16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveDummy16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR==true>
            DRV_SPI_MasterDMAReceiveDummy32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI_MasterDMAReceiveDummy32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        default:
            break;
    }
}

</#macro>    


<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_8BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_16BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true &&  CONFIG_DRV_SPI_USE_32BIT_MODE==true>
    <@DRV_SPI_MASTER_DMA_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
     />
</#if>