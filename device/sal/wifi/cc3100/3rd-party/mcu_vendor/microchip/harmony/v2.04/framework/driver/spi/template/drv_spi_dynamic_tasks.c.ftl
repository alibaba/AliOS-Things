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

<#if CONFIG_DRV_SPI_USE_DMA>
// DMA Scratch Pad
uint8_t sDrvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
uint8_t sDrvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
</#if>

<#macro DRV_SPI_MASTER_TASKS_MACRO
            DRV_BIT_WIDTH 
            DRV_ISR
            DRV_EBM>
    <#if DRV_ISR>
        <#assign taskMode="ISR"/>
    <#else>
        <#assign taskMode="Polled"/>
    </#if>
    <#if DRV_EBM>
        <#assign bufferMode="EBM"/>
    <#else>
        <#assign bufferMode="RM"/>
    </#if>
int32_t DRV_SPI_${taskMode}Master${bufferMode}${DRV_BIT_WIDTH}BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )    
{
    volatile bool continueLoop;
    <#if DRV_EBM==false && DRV_ISR==false>
    uint8_t counter = 0;
    uint8_t numPolled = pDrvInstance->numTrfsSmPolled;
    uint8_t result = 0;
    </#if>
    
    <#if DRV_ISR==true>
    /* Disable the interrupts */
    SYS_INT_SourceDisable(pDrvInstance->rxInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->txInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->errInterruptSource);
    </#if>
    do {
        <#if DRV_ISR==true && CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">
        /* Clear the Interrupts */
        SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
        SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
        SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
        </#if>
        
        DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
        SPI_MODULE_ID spiId = pDrvInstance->spiId;
        /* Check for a new task */
        if (pDrvInstance->currentJob == NULL)
        {
    <#if DRV_ISR==true>
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_DequeueLock(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            if (pDrvInstance->currentJob == NULL)
            {
    <#if DRV_ISR==true>
                pDrvInstance->txEnabled = false;
    </#if>
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
    <#if DRV_ISR==true && DRV_EBM==true>
            if (currentJob->dataLeftToTx +currentJob->dummyLeftToTx > PLIB_SPI_RX_8BIT_FIFO_SIZE(spiId))
            {
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_1HALF_FULL_OR_MORE);
            }
    </#if>
            /* Flush out the Receive buffer */
            PLIB_SPI_BufferClear(spiId);
        }

    <#if CONFIG_DRV_SPI_USE_DMA>
        /* Set up DMA Receive job.  This is done here to ensure that the RX job is ready to receive when TXing starts*/
        if ((pDrvInstance->rxDmaThreshold != 0) && (currentJob->dataLeftToRx > pDrvInstance->rxDmaThreshold))
        {
    <#if DRV_EBM==true>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    </#if>
            uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(spiId);
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToRx -= len;
            currentJob->dataRxed += len;
    <#if DRV_ISR==true>
            pDrvInstance->rxEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        }
        else if ((currentJob->rxDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToRx == 0) && (pDrvInstance->rxDmaThreshold != 0) && (currentJob->dummyLeftToRx > pDrvInstance->rxDmaThreshold))
        {
    <#if DRV_EBM==true>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    </#if>
            uint8_t * ptr = sDrvSpiRxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(spiId);
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToRx -= len;
    <#if DRV_ISR==true>
            pDrvInstance->rxEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        }       
        /* Set up the DMA Transmit job here.  This is done after the RX job to help prevent buffer overruns.*/
        if ((pDrvInstance->txDmaThreshold != 0) && (currentJob->dataLeftToTx > pDrvInstance->txDmaThreshold))
        {
    <#if DRV_EBM==true>
        <#if CONFIG_PIC32MZ==true>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
        <#else>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL);
        </#if>
    </#if>
            uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToTx -= len;
            currentJob->dataTxed += len;
    <#if DRV_ISR==true>
            pDrvInstance->txEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>

        }
        else if ((currentJob->txDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToTx == 0) && (pDrvInstance->txDmaThreshold != 0) && (currentJob->dummyLeftToTx > pDrvInstance->txDmaThreshold))
        {
    <#if DRV_EBM==true>
        <#if CONFIG_PIC32MZ==true>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
        <#else>
            PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL);
        </#if>
    </#if>
            uint8_t * ptr = sDrvSpiTxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(pDrvInstance->spiId);
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToTx -= len;
    <#if DRV_ISR==true>
            pDrvInstance->txEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(pDrvInstance->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>
        }
        bool rxDMAInProgress = (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
        bool txDMAInProgress = (currentJob->txDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->txDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
    </#if>
                
        continueLoop = false;
        /* Execute the sub tasks */
    <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">   
                DRV_SPI_${taskMode}ErrorTasks(pDrvInstance);
        
        /* Figure out how many bytes are left to be received */
        volatile size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        // Check to see if we have any data left to receive and update the bytes left.

    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft != 0) && !rxDMAInProgress)
    <#else>
        if (bytesLeft != 0)
    </#if>
        {
            DRV_SPI_Master${bufferMode}Receive${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
            bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        }
        
        if 
    <#if CONFIG_DRV_SPI_USE_DMA>
            (!txDMAInProgress &&
    </#if>
            (currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
    <#if CONFIG_DRV_SPI_USE_DMA>
            )
    </#if>
        {
            DRV_SPI_Master${bufferMode}Send${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
        }
        
     <#else>   
             if 
    <#if CONFIG_DRV_SPI_USE_DMA>
            (!txDMAInProgress &&
    </#if>
            (currentJob->dataLeftToTx +currentJob->dummyLeftToTx != 0)
    <#if CONFIG_DRV_SPI_USE_DMA>
            )
    </#if>
        {
            DRV_SPI_Master${bufferMode}Send${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
        }
        
        DRV_SPI_${taskMode}ErrorTasks(pDrvInstance);
        
        /* Figure out how many bytes are left to be received */
        volatile size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        // Check to see if we have any data left to receive and update the bytes left.

    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft != 0) && !rxDMAInProgress)
    <#else>
        if (bytesLeft != 0)
    </#if>
        {
            DRV_SPI_Master${bufferMode}Receive${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
            bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        }
     </#if>
     
    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft == 0) && !rxDMAInProgress && !txDMAInProgress)
    <#else>
        if (bytesLeft == 0)
    </#if>
        {
        <#if DRV_ISR==true>
                    // Disable the interrupt, or more correctly don't re-enable it later*/
                    pDrvInstance->rxEnabled = false;
        </#if>
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
        <#if DRV_ISR==true>
                    if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        <#else>
                    if (DRV_SPI_SYS_QUEUE_FreeElementLock(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        </#if>
                    {
                        SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                        return 0;
                    }
                    /* Clean up */
                    pDrvInstance->currentJob = NULL;
        <#if DRV_ISR==true>
                    if (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue))
                    {
                        continueLoop = true;    
                        continue;
                    }
                    else
                    {
                        break;
                    }
        </#if>
                }
    <#if CONFIG_DRV_SPI_USE_DMA>
        else if (rxDMAInProgress)
        {
            // DMA is in progress
            // Wipe out the symbols in Progress
            pDrvInstance->rxEnabled = false;
            pDrvInstance->symbolsInProgress = 0;
        }
    </#if>

    
    <#if DRV_ISR==true>
        /* Check to see if the interrupts would fire again if so just go back into 
           the loop instead of suffering the interrupt latency of exiting and re-entering*/
        <#if CONFIG_DRV_SPI_USE_DMA>
        if ((pDrvInstance->currentJob != NULL) && (!rxDMAInProgress || !txDMAInProgress))
        <#else>
        if (pDrvInstance->currentJob != NULL)
        </#if>
        {   
            <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "persistent">
            /* Clear the Interrupts */
            SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
            </#if>
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
    <#elseif DRV_EBM==false>
        counter ++;
        if ((counter < numPolled) && (result != 0))
        {
            continueLoop = true;
        }
    </#if>
    
    } while(continueLoop);
    <#if DRV_ISR==true>
    /* if we're here it means that we have no more jobs in the queue, tx and rx interrupts will be re-enabled by the BufferAdd* functions*/
    SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
    SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
    </#if>
    return 0;
}
</#macro>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_MASTER_MODE==true>
    <@DRV_SPI_MASTER_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#macro DRV_SPI_SLAVE_TASKS_MACRO
            DRV_BIT_WIDTH 
            DRV_ISR
            DRV_EBM>
            
    <#if DRV_ISR>
        <#assign taskMode="ISR"/>
    <#else>
        <#assign taskMode="Polled"/>
    </#if>
    <#if DRV_EBM>
        <#assign bufferMode="EBM"/>
    <#else>
        <#assign bufferMode="RM"/>
    </#if>
            
int32_t DRV_SPI_${taskMode}Slave${bufferMode}${DRV_BIT_WIDTH}BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance )
{
    <#if DRV_ISR==true>
    volatile bool continueLoop;
    SYS_INT_SourceDisable(pDrvInstance->rxInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->txInterruptSource);
    SYS_INT_SourceDisable(pDrvInstance->errInterruptSource);
    do {
        <#if DRV_ISR==true && CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">
        /* Clear the Interrupts */
        SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
        SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
        SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
        </#if>
        
        continueLoop = false;
    </#if>
        DRV_SPI_JOB_OBJECT * currentJob = pDrvInstance->currentJob;
        SPI_MODULE_ID spiId = pDrvInstance->spiId;
    
        if (pDrvInstance->currentJob == NULL)
        {
    <#if DRV_ISR==true>
            if (DRV_SPI_SYS_QUEUE_Dequeue(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_DequeueLock(pDrvInstance->queue, (void *)&(pDrvInstance->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;       
            }
            if (pDrvInstance->currentJob == NULL)
            {
    <#if DRV_ISR==true>
                pDrvInstance->txEnabled = false;
    </#if>
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
            
            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
    <#if DRV_ISR==true && DRV_EBM==true>
            if (currentJob->dataLeftToTx > PLIB_SPI_RX_8BIT_FIFO_SIZE(spiId))
            {
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
                PLIB_SPI_FIFOInterruptModeSelect(spiId, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_1HALF_FULL_OR_MORE);
            }
    </#if>
            /* Flush out the Receive buffer */
            PLIB_SPI_BufferClear(spiId);
        }
        
        
        if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            DRV_SPI_Slave${bufferMode}Receive${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
        }
        if (currentJob->dataLeftToTx != 0)
        {
            DRV_SPI_Slave${bufferMode}Send${DRV_BIT_WIDTH}Bit${taskMode}(pDrvInstance);
        }
    <#if DRV_ISR==true>
        else
        {
            pDrvInstance->txEnabled = false;
            pDrvInstance->rxEnabled = true;
        }
    </#if>
        
        DRV_SPI_${taskMode}ErrorTasks(pDrvInstance);
    
        /* Check to see if the job is done */
        if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
        {
    <#if DRV_ISR==true>
            // Disable the interrupt, or more correctly don't re-enable it later*/
            pDrvInstance->rxEnabled = false;
    </#if>
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
    <#if DRV_ISR==true>
            if (DRV_SPI_SYS_QUEUE_FreeElement(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_FreeElementLock(pDrvInstance->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            /* Clean up */
            pDrvInstance->currentJob = NULL;
    <#if DRV_ISR==true>
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(pDrvInstance->queue))
            {
                continueLoop = true;    
                continue;
            }
    </#if>
        }

        <#if DRV_ISR==true>
        /* Check to see if the interrupts would fire again if so just go back into 
           the loop instead of suffering the interrupt latency of exiting and re-entering*/
        if (pDrvInstance->currentJob != NULL)
        {
            <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "persistent">
            /* Clear the Interrupts */
            SYS_INT_SourceStatusClear(pDrvInstance->rxInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->txInterruptSource);
            SYS_INT_SourceStatusClear(pDrvInstance->errInterruptSource);
            </#if>
            
            /* Interrupts should immediately become active again if they're in a fired condition */
            if ((SYS_INT_SourceStatusGet(pDrvInstance->rxInterruptSource)) ||
                (SYS_INT_SourceStatusGet(pDrvInstance->errInterruptSource)))
            {
                /* Interrupt would fire again anyway so we should just go back to the start*/
                continueLoop = true;
                continue;                            
            }
            /* If we're here then we know that the interrupt should not be firing immediately so enable it and exit */
            SYS_INT_SourceEnable(pDrvInstance->rxInterruptSource);
            if (pDrvInstance->txEnabled)
            {
                SYS_INT_SourceEnable(pDrvInstance->txInterruptSource);
            }

            return 0;            
        }

    } while(continueLoop);
    </#if>    
    return 0;
}            
</#macro>


<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_8BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="8"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_16BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="16"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_ISR_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=true
        DRV_EBM=false
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_ENHANCED_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
        DRV_EBM=true
     />
</#if>
<#if CONFIG_DRV_SPI_USE_POLLED_MODE==true && CONFIG_DRV_SPI_USE_STANDARD_BUFFER_MODE==true && CONFIG_DRV_SPI_USE_32BIT_MODE==true && CONFIG_DRV_SPI_USE_SLAVE_MODE==true>
    <@DRV_SPI_SLAVE_TASKS_MACRO
        DRV_BIT_WIDTH="32"
        DRV_ISR=false
        DRV_EBM=false
     />
</#if>

<#if CONFIG_DRV_SPI_USE_DMA>

void DRV_SPI_SetupDMA( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance)
{

        DMA_TRIGGER_SOURCE txSource = 0;
        DMA_TRIGGER_SOURCE rxSource = 0;

        switch(pDrvInstance->spiId)
        {
<#if (CONFIG_DS60001156 == true && CONFIG_HAVE_TRACE == false) || (CONFIG_DS60001143 == true && CONFIG_HAVE_TRACE == false && CONFIG_HAVE_USB == true)>
<#-- 64 pin devices of DS60001156 and 64 pin devices of  DS60001143 which have USB, doesn't have SPI 1 module, so switch case should start from SPI_ID_2-->
    <#list 1..CONFIG_SPI_NUMBER_OF_MODULES?number as idx>
            case SPI_ID_${idx+1}:
            {
                txSource = DMA_TRIGGER_SPI_${idx+1}_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_${idx+1}_RECEIVE;
            }
            break;
    </#list>
<#elseif CONFIG_PIC32WK == true>
<#-- PIC32WK has SPI instances 0,1 and 2 -->
    <#list 1..CONFIG_SPI_NUMBER_OF_MODULES?number as idx>
            case SPI_ID_${idx-1}:
            {
                txSource = DMA_TRIGGER_SPI_${idx-1}_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_${idx-1}_RECEIVE;
            }
            break;
    </#list>	
<#else>
    <#list 1..CONFIG_SPI_NUMBER_OF_MODULES?number as idx>
            case SPI_ID_${idx}:
            {
                txSource = DMA_TRIGGER_SPI_${idx}_TRANSMIT;
                rxSource = DMA_TRIGGER_SPI_${idx}_RECEIVE;
            }
            break;
    </#list>
</#if>
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
</#if>
