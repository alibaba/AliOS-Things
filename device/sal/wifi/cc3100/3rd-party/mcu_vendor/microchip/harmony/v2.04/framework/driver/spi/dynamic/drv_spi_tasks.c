/*******************************************************************************
  SPI Driver Functions for Dynamic Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_dynamic_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.
    
  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_dynamic_tasks.c.ftl
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

// DMA Scratch Pad
uint8_t sDrvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
uint8_t sDrvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];


int32_t DRV_SPI_ISRMasterRM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )    
{
    volatile bool continueLoop;
    
    /* Disable the interrupts */
    SYS_INT_SourceDisable(pDrvInstance->rxInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->txInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->errInterruptSource);
    do {
        
        DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
        SPI_MODULE_ID spiId = pDrvInstance->spiId;
        /* Check for a new task */
        if (pDrvInstance->currentJob == NULL)
        {
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            if (pDrvInstance->currentJob == NULL)
            {
                pDrvInstance->txEnabled = false;
                return 0;
            }
            currentJob = pDrvInstance->currentJob;

            pDrvInstance->symbolsInProgress = 0;

            /* Call the operation starting function pointer.  This can be used to modify the slave select lines */
            DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
            if (pClient->operationStarting != NULL)
            {
                (*pClient->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
            /* Check the baud rate.  If its different set the new baud rate*/
            if (pClient->baudRate != pDrvInstance->currentBaudRate)
            {
                #if defined (PLIB_SPI_ExistsBaudRateClock)
                    if (pDrvInstance->baudClockSource == SPI_BAUD_RATE_PBCLK_CLOCK)
                    {
                        PLIB_SPI_BaudRateSet( spiId , SYS_CLK_PeripheralFrequencyGet(pDrvInstance->spiClk), pClient->baudRate );
                    }
                    else // if baud clock source is reference clock
                    {
                        PLIB_SPI_BaudRateSet( spiId , SYS_CLK_ReferenceFrequencyGet(CLK_BUS_REFERENCE_1), pClient->baudRate );
                    }
                #else
                    PLIB_SPI_BaudRateSet( spiId , SYS_CLK_PeripheralFrequencyGet(pDrvInstance->spiClk), pClient->baudRate );
                #endif

                pDrvInstance->currentBaudRate = pClient->baudRate;
            }
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
            /* Flush out the Receive buffer */
            PLIB_SPI_BufferClear(spiId);
        }

        /* Set up DMA Receive job.  This is done here to ensure that the RX job is ready to receive when TXing starts*/
        if ((pDrvInstance->rxDmaThreshold != 0) && (currentJob->dataLeftToRx > pDrvInstance->rxDmaThreshold))
        {
            uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(spiId);
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToRx -= len;
            currentJob->dataRxed += len;
            pDrvInstance->rxEnabled = false;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
        }
        else if ((currentJob->rxDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToRx == 0) && (pDrvInstance->rxDmaThreshold != 0) && (currentJob->dummyLeftToRx > pDrvInstance->rxDmaThreshold))
        {
            uint8_t * ptr = sDrvSpiRxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(spiId);
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToRx -= len;
            pDrvInstance->rxEnabled = false;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
        }       
        /* Set up the DMA Transmit job here.  This is done after the RX job to help prevent buffer overruns.*/
        if ((pDrvInstance->txDmaThreshold != 0) && (currentJob->dataLeftToTx > pDrvInstance->txDmaThreshold))
        {
            uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToTx -= len;
            currentJob->dataTxed += len;
            pDrvInstance->txEnabled = false;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);

        }
        else if ((currentJob->txDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToTx == 0) && (pDrvInstance->txDmaThreshold != 0) && (currentJob->dummyLeftToTx > pDrvInstance->txDmaThreshold))
        {
            uint8_t * ptr = sDrvSpiTxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToTx -= len;
            pDrvInstance->txEnabled = false;
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
        }
        bool rxDMAInProgress = (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
        bool txDMAInProgress = (currentJob->txDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->txDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
                
        continueLoop = false;
        /* Execute the sub tasks */
             if 
            (!txDMAInProgress &&
            (currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
            )
        {
            DRV_SPI_MasterRMSend8BitISR(pDrvInstance);
        }
        
        DRV_SPI_ISRErrorTasks(pDrvInstance);
        
        /* Figure out how many bytes are left to be received */
        volatile size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        // Check to see if we have any data left to receive and update the bytes left.

        if ((bytesLeft != 0) && !rxDMAInProgress)
        {
            DRV_SPI_MasterRMReceive8BitISR(pDrvInstance);
            bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        }
     
        if ((bytesLeft == 0) && !rxDMAInProgress && !txDMAInProgress)
        {
                    // Disable the interrupt, or more correctly don't re-enable it later*/
                    pDrvInstance->rxEnabled = false;
                    /* Job is complete*/
                    currentJob->status = DRV_SPI_BUFFER_EVENT_COMPLETE;
                    /* Call the job complete call back*/
                    if (currentJob->completeCB != NULL)
                    {
                        (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
                    }
                    /* Call the operation complete call back.  This is different than the
                       job complete callback.  This can be used to modify the Slave Select line.
                       The job complete callback can be used to free a client that is blocked 
                       waiting for complete*/
                    DRV_SPI_CLIENT_OBJECT * pClient = (DRV_SPI_CLIENT_OBJECT*)currentJob->pClient;
                    if (pClient->operationEnded != NULL)
                    {
                        (*pClient->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
                    }
                    /* Return the job back to the free queue*/
                    if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
                    {
                        SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                        return 0;
                    }
                    /* Clean up */
                    pDrvInstance->currentJob = NULL;
                    if (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue))
                    {
                        continueLoop = true;    
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
        else if (rxDMAInProgress)
        {
            // DMA is in progress
            // Wipe out the symbols in Progress
            pDrvInstance->rxEnabled = false;
            pDrvInstance->symbolsInProgress = 0;
        }

    
        /* Check to see if the interrupts would fire again if so just go back into 
           the loop instead of suffering the interrupt latency of exiting and re-entering*/
        if ((pDrvInstance->currentJob != NULL) && (!rxDMAInProgress || !txDMAInProgress))
        {   
            /* Clear the Interrupts */
            SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
            /* Interrupts should immediately become active again if they're in a fired condition */
            if ((SYS_INT_SourceStatusGet(pDrvInstance->rxInterruptSource)) ||
                (SYS_INT_SourceStatusGet(pDrvInstance->txInterruptSource)) ||
                (SYS_INT_SourceStatusGet(pDrvInstance->errInterruptSource)))
            {
                /* Interrupt would fire again anyway so we should just go back to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be firing again immediately, so re-enable them and exit*/
            SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);

            return 0;            
        }
    
    } while(continueLoop);
    /* if we're here it means that we have no more jobs in the queue, tx and rx interrupts will be re-enabled by the BufferAdd* functions*/
    SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
    SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
    return 0;
}









void DRV_SPI_SetupDMA( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance)
{

        DMA_TRIGGER_SOURCE txSource = 0;
        DMA_TRIGGER_SOURCE rxSource = 0;

        switch(pDrvInstance->spiId)
        {
            case SPI_ID_1:
            {
                txSource = DMA_TRIGGER_SPI_1_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_1_RECEIVE;
            }
            break;
            case SPI_ID_2:
            {
                txSource = DMA_TRIGGER_SPI_2_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_2_RECEIVE;
            }
            break;
            case SPI_ID_3:
            {
                txSource = DMA_TRIGGER_SPI_3_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_3_RECEIVE;
            }
            break;
            case SPI_ID_4:
            {
                txSource = DMA_TRIGGER_SPI_4_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_4_RECEIVE;
            }
            break;
            default:
               break;
        }
        if (pDrvInstance->txDmaThreshold != 0)
        {
            pDrvInstance->txDmaChannelHandle = SYS_DMA_ChannelAllocate(pDrvInstance->txDmaChannel);
            SYS_DMA_ChannelSetup(pDrvInstance->txDmaChannelHandle, SYS_DMA_CHANNEL_OP_MODE_BASIC, txSource);
            SYS_DMA_ChannelTransferEventHandlerSet(pDrvInstance->txDmaChannelHandle, pDrvInstance->sendDMAHander, (uintptr_t)pDrvInstance);
        }
            
        if (pDrvInstance->rxDmaThreshold != 0)
        {
            pDrvInstance->rxDmaChannelHandle = SYS_DMA_ChannelAllocate(pDrvInstance->rxDmaChannel);
            SYS_DMA_ChannelSetup(pDrvInstance->rxDmaChannelHandle, SYS_DMA_CHANNEL_OP_MODE_BASIC, rxSource);
            SYS_DMA_ChannelTransferEventHandlerSet(pDrvInstance->rxDmaChannelHandle, pDrvInstance->receiveDMAHander, (uintptr_t)pDrvInstance);                
        }
}
