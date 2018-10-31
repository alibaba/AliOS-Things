/*******************************************************************************
  SPI Driver Functions for Static Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static_dma_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.

  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_static_dma_tasks.c.ftl
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

#include "system_config.h"
#include "system_definitions.h"
#include "driver/spi/static/src/drv_spi_static_local.h"
<#macro DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE PLIB_INSTANCE XMIT_INT RCV_INT ERR_INT>
<#assign DRV_MODE="Master">
<#assign DRV_ISR="ISR">
<#assign DRV_BUFFER="RM">
<#assign DRV_BIT_WIDTH="8">
<#if .vars["CONFIG_DRV_SPI_SPI_MODE_SLAVE_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_MODE="Slave">
</#if>
<#if .vars["CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_ISR="Polled">
</#if>
<#if .vars["CONFIG_DRV_SPI_BUFFER_ENHANCED_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BUFFER="EBM">
</#if>
<#if .vars["CONFIG_DRV_SPI_COMM_WIDTH_16_BIT_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BIT_WIDTH="16">
</#if>
<#if .vars["CONFIG_DRV_SPI_COMM_WIDTH_32_BIT_IDX${DRV_INSTANCE}"] == true>
    <#assign DRV_BIT_WIDTH="32">
</#if>
<#if DRV_MODE=="Master">
void DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dummyLeftToTx != 0)
    {
        uint8_t * ptr = sDrvSpiTxDummy;
        uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dummyLeftToTx), DRV_SPI_DMA_TXFER_SIZE), DRV_SPI_DMA_DUMMY_BUFFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
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


void DRV_SPI${DRV_INSTANCE}_MasterDMASendData${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dataLeftToTx != 0)
    {
        uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
        uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToTx), DRV_SPI_DMA_TXFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
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
            SYS_INT_SourceEnable(${XMIT_INT});
        }
    }
}


void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMAMasterSendEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    switch(currentJob->txDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData8BitPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendData32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy8BitPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMASendDummy32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        default:
            break;
    }
}


void DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dummyLeftToRx != 0)
    {
        uint8_t * ptr = sDrvSpiRxDummy;
        uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dummyLeftToRx), DRV_SPI_DMA_TXFER_SIZE), DRV_SPI_DMA_DUMMY_BUFFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
        currentJob->dummyLeftToRx -= len;
    }
    else
    {
        // Job is done
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        SYS_INT_SourceEnable(${RCV_INT});
    }
}


void DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dataLeftToRx != 0)
    {
        uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
        uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToRx), DRV_SPI_DMA_TXFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
        currentJob->dataLeftToRx -= len;
        currentJob->dataRxed += len;
    }
    else
    {
        // Job is done
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        SYS_INT_SourceEnable(${RCV_INT});
    }
}


void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMAMasterReceiveEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    switch(currentJob->rxDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData8BitPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveData32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
    <#if DRV_BIT_WIDTH=="8">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy8BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy8BitPolled(event, handle, contextHandle);
        </#if>
    <#elseif DRV_BIT_WIDTH=="16">
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy16BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy16BitPolled(event, handle, contextHandle);
        </#if>
    <#else>
        <#if DRV_ISR=="ISR">
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy32BitISR(event, handle, contextHandle);
        <#else>
            DRV_SPI${DRV_INSTANCE}_MasterDMAReceiveDummy32BitPolled(event, handle, contextHandle);
        </#if>
    </#if>
            break;
        default:
            break;
    }
}
</#if>

<#if DRV_MODE=="Slave">
void DRV_SPI${DRV_INSTANCE}_SlaveDMASendData${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dataLeftToTx != 0)
    {
        uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
        uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToTx), DRV_SPI_DMA_TXFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>
        currentJob->txDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
        currentJob->dataLeftToTx -= len;
        currentJob->dataTxed += len;
    }
    else
    {
        // Job is done
        currentJob->txDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        SYS_INT_SourceEnable(${RCV_INT});
        SYS_INT_SourceEnable(${XMIT_INT});
    }
}


void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMASlaveSendEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    switch(currentJob->txDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI${DRV_INSTANCE}_SlaveDMASendData${DRV_BIT_WIDTH}Bit${DRV_ISR}(event, handle, contextHandle);
            break;
        default:
            break;
    }
}
void DRV_SPI${DRV_INSTANCE}_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dummyLeftToRx != 0)
    {
        uint8_t * ptr = sDrvSpiRxDummy;
        uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dummyLeftToRx), DRV_SPI_DMA_TXFER_SIZE), DRV_SPI_DMA_DUMMY_BUFFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
        currentJob->dummyLeftToRx -= len;
    }
    else
    {
        // Job is done
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_COMPLETE;
        SYS_INT_SourceEnable(${XMIT_INT});
    }
}

void DRV_SPI${DRV_INSTANCE}_SlaveDMAReceiveData${DRV_BIT_WIDTH}Bit${DRV_ISR}(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    if (event != SYS_DMA_TRANSFER_EVENT_COMPLETE)
    {
        // Ignore for now
        return;
    }
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ *)contextHandle;
    DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob->dataLeftToRx != 0)
    {
        uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
        uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, currentJob->dataLeftToRx), DRV_SPI_DMA_TXFER_SIZE);
        void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
    <#if DRV_BIT_WIDTH=="8">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
        SYS_DMA_ChannelTransferAdd(pDrvObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
        currentJob->dataLeftToRx -= len;
        currentJob->dataRxed += len;
    }
    else
    {
        DRV_SPI${DRV_INSTANCE}_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${DRV_ISR}(event,  handle,  contextHandle);
    }
}

void DRV_SPI${DRV_INSTANCE}_${DRV_ISR}DMASlaveReceiveEventHandler${DRV_BIT_WIDTH}bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_OBJ * pDrvObj = (struct DRV_SPI_OBJ * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    switch(currentJob->rxDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI${DRV_INSTANCE}_SlaveDMAReceiveData${DRV_BIT_WIDTH}Bit${DRV_ISR}(event, handle, contextHandle);
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
            DRV_SPI${DRV_INSTANCE}_SlaveDMAReceiveDummy${DRV_BIT_WIDTH}Bit${DRV_ISR}(event, handle, contextHandle);
            break;
        default:
            break;
    }
}
</#if>
</#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="0"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX0
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX0
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX0
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX0
/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="1"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX1
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX1
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX1
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX1
/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="2"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX2
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX2
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX2
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX2
/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="3"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX3
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX3
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX3
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX3
/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="4"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX4
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX4
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX4
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX4
/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
<@DRV_SPI_STATIC_FUNCTIONS DRV_INSTANCE="5"
PLIB_INSTANCE=CONFIG_DRV_SPI_SPI_ID_IDX5
XMIT_INT=CONFIG_DRV_SPI_TX_INT_SOURCE_IDX5
RCV_INT=CONFIG_DRV_SPI_RX_INT_SOURCE_IDX5
ERR_INT=CONFIG_DRV_SPI_ERROR_INT_SOURCE_IDX5
/>
</#if>