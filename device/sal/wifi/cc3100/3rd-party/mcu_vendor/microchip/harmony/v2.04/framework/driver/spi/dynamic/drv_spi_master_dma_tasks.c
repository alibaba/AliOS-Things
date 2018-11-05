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
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
//DOM-IGNORE-END

#include "driver/spi/src/dynamic/drv_spi_internal.h"
#include <stdbool.h>





void DRV_SPI_MasterDMASendDummy8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
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
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToTx -= len;
    }
    else
    {
        // Job is done
        currentJob->txDMAProgressStage = DRV_SPI_DMA_COMPLETE;
    }

}


void DRV_SPI_MasterDMASendData8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
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
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
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


void DRV_SPI_ISRDMAMasterSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->txDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI_MasterDMASendData8BitISR(event, handle, contextHandle);
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
            DRV_SPI_MasterDMASendDummy8BitISR(event, handle, contextHandle);
            break;
        default:
            break;
    }
}
void DRV_SPI_MasterDMAReceiveDummy8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
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
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
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

void DRV_SPI_MasterDMAReceiveData8BitISR(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
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
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
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

void DRV_SPI_ISRDMAMasterReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle)
{
    struct DRV_SPI_DRIVER_OBJECT * pDrvInstance = (struct DRV_SPI_DRIVER_OBJECT * )contextHandle;
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;

    switch(currentJob->rxDMAProgressStage)
    {
        case DRV_SPI_DMA_DATA_INPROGRESS:
            DRV_SPI_MasterDMAReceiveData8BitISR(event, handle, contextHandle);
            break;
        case DRV_SPI_DMA_DUMMY_INPROGRESS:
            DRV_SPI_MasterDMAReceiveDummy8BitISR(event, handle, contextHandle);
            break;
        default:
            break;
    }
}

