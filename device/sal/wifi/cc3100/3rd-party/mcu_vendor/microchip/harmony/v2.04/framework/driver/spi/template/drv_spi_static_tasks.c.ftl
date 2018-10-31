/*******************************************************************************
  SPI Driver Functions for Static Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.

  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_static_tasks.c.ftl
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

<#if CONFIG_DRV_SPI_USE_DMA>
// DMA Scratch Pad
uint8_t sDrvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
uint8_t sDrvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];

</#if>
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
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
    <#assign txIndex=.vars["CONFIG_DRV_SPI_TX_DMA_INST_IDX${DRV_INSTANCE}"]/>
    <#assign TX_DMA_CHANNEL=.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${txIndex}"]>
    <#assign rxIndex=.vars["CONFIG_DRV_SPI_RX_DMA_INST_IDX${DRV_INSTANCE}"]/>
    <#assign RX_DMA_CHANNEL=.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${rxIndex}"]>
</#if>
<#if DRV_MODE=="Master">
int32_t DRV_SPI${DRV_INSTANCE}_${DRV_ISR}Master${DRV_BUFFER}${DRV_BIT_WIDTH}BitTasks ( struct DRV_SPI_OBJ * dObj )
{
    volatile bool continueLoop;
    <#if DRV_BUFFER=="RM" && DRV_ISR=="Polled">
    uint8_t counter = 0;
    uint8_t numPolled = dObj->numTrfsSmPolled;
    uint8_t result = 0;
    </#if>
    <#if DRV_ISR=="ISR">
    /* Disable the interrupts */
    SYS_INT_SourceDisable(${RCV_INT});
    SYS_INT_SourceDisable(${XMIT_INT});
    SYS_INT_SourceDisable(${ERR_INT});
    </#if>
    do {
        <#if DRV_ISR=="ISR" && CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">
        /* Clear the Interrupts */
        SYS_INT_SourceStatusClear(${RCV_INT});
        SYS_INT_SourceStatusClear(${XMIT_INT});
        SYS_INT_SourceStatusClear(${ERR_INT});
        </#if>
        
        DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;

        /* Check for a new task */
        if (dObj->currentJob == NULL)
        {
    <#if DRV_ISR=="ISR">
            if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_DequeueLock(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;
            }
            if (dObj->currentJob == NULL)
            {
    <#if DRV_ISR=="ISR">
                dObj->txEnabled = false;
    </#if>
                return 0;
            }
            currentJob = dObj->currentJob;

            dObj->symbolsInProgress = 0;
    <#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>

            /* Call the operation starting function pointer.  This can be used to modify the slave select lines */
            if (dObj->operationStarting != NULL)
            {
                (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
    </#if>

            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
    <#if DRV_ISR=="ISR" && DRV_BUFFER=="EBM">
            if (currentJob->dataLeftToTx +currentJob->dummyLeftToTx > PLIB_SPI_RX_8BIT_FIFO_SIZE(${PLIB_INSTANCE}))
            {
                PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
                PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_1HALF_FULL_OR_MORE);
            }
    </#if>
            /* Flush out the Receive buffer */
            PLIB_SPI_BufferClear(${PLIB_INSTANCE});
        }

    <#if CONFIG_DRV_SPI_USE_DMA>
        /* Set up DMA Receive job.  This is done here to ensure that the RX job is ready to receive when TXing starts*/
        if ((dObj->rxDmaThreshold != 0) && (currentJob->dataLeftToRx > dObj->rxDmaThreshold))
        {
    <#if DRV_BUFFER=="EBM">
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    </#if>
            uint8_t * ptr = &(currentJob->rxBuffer[currentJob->dataRxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToRx -= len;
            currentJob->dataRxed += len;
    <#if DRV_ISR=="ISR">
            dObj->rxEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        }
        else if ((currentJob->rxDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToRx == 0) && (dObj->rxDmaThreshold != 0) && (currentJob->dummyLeftToRx > dObj->rxDmaThreshold))
        {
    <#if DRV_BUFFER=="EBM">
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    </#if>
            uint8_t * ptr = sDrvSpiRxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToRx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
            currentJob->rxDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToRx -= len;
    <#if DRV_ISR=="ISR">
            dObj->rxEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 1, ptr, len, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 2, ptr, len, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(dObj->rxDmaChannelHandle, spiPtr, 4, ptr, len, 4);
    </#if>
        }
        /* Set up the DMA Transmit job here.  This is done after the RX job to help prevent buffer overruns.*/
        if ((dObj->txDmaThreshold != 0) && (currentJob->dataLeftToTx > dObj->txDmaThreshold))
        {
    <#if DRV_BUFFER=="EBM">
        <#if CONFIG_PIC32MZ==true>
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
        <#else>
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL);
        </#if>
    </#if>
            uint8_t * ptr = &(currentJob->txBuffer[currentJob->dataTxed]);
            uint32_t len = MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_TXFER_SIZE), currentJob->dataLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DATA_INPROGRESS;
            currentJob->dataLeftToTx -= len;
            currentJob->dataTxed += len;
    <#if DRV_ISR=="ISR">
            dObj->txEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>

        }
        else if ((currentJob->txDMAProgressStage == DRV_SPI_DMA_NONE) && (currentJob->dataLeftToTx == 0) && (dObj->txDmaThreshold != 0) && (currentJob->dummyLeftToTx > dObj->txDmaThreshold))
        {
    <#if DRV_BUFFER=="EBM">
        <#if CONFIG_PIC32MZ==true>
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
        <#else>
            PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_NOT_FULL);
        </#if>
    </#if>
            uint8_t * ptr = sDrvSpiTxDummy;
            uint32_t len = MIN(MIN(MIN(PLIB_DMA_MAX_TRF_SIZE, DRV_SPI_DMA_DUMMY_BUFFER_SIZE), DRV_SPI_DMA_TXFER_SIZE), currentJob->dummyLeftToTx);
            void * spiPtr = PLIB_SPI_BufferAddressGet(${PLIB_INSTANCE});
            currentJob->txDMAProgressStage = DRV_SPI_DMA_DUMMY_INPROGRESS;
            currentJob->dummyLeftToTx -= len;
    <#if DRV_ISR=="ISR">
            dObj->txEnabled = false;
    </#if>
    <#if DRV_BIT_WIDTH=="8">
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 1, 1);
    <#elseif DRV_BIT_WIDTH=="16">
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 2, 2);
    <#else>
            SYS_DMA_ChannelTransferAdd(dObj->txDmaChannelHandle, ptr, len, spiPtr, 4, 4);
    </#if>
        }
        bool rxDMAInProgress = (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->rxDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
        bool txDMAInProgress = (currentJob->txDMAProgressStage == DRV_SPI_DMA_DATA_INPROGRESS) || (currentJob->txDMAProgressStage == DRV_SPI_DMA_DUMMY_INPROGRESS);
    </#if>

        continueLoop = false;
        
        /* Execute the sub tasks */
        <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">
        DRV_SPI${DRV_INSTANCE}_${DRV_ISR}ErrorTasks(dObj);
        
        /* Figure out how many bytes are left to be received */
        volatile size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        
        // Check to see if we have any data left to receive and update the bytes left.
    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft != 0) && !rxDMAInProgress)
    <#else>
        if (bytesLeft != 0)
    </#if>
        {
            DRV_SPI${DRV_INSTANCE}_Master${DRV_BUFFER}Receive${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
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
            DRV_SPI${DRV_INSTANCE}_Master${DRV_BUFFER}Send${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
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
            DRV_SPI${DRV_INSTANCE}_Master${DRV_BUFFER}Send${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
        }
        
        DRV_SPI${DRV_INSTANCE}_${DRV_ISR}ErrorTasks(dObj);
        
        /* Figure out how many bytes are left to be received */
        volatile size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        
        // Check to see if we have any data left to receive and update the bytes left.
    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft != 0) && !rxDMAInProgress)
    <#else>
        if (bytesLeft != 0)
    </#if>
        {
            DRV_SPI${DRV_INSTANCE}_Master${DRV_BUFFER}Receive${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
            bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;
        }
    </#if>    
    <#if CONFIG_DRV_SPI_USE_DMA>
        if ((bytesLeft == 0) && !rxDMAInProgress && !txDMAInProgress)
    <#else>
        if (bytesLeft == 0)
    </#if>
        {
        <#if DRV_ISR=="ISR">
                    // Disable the interrupt, or more correctly don't re-enable it later*/
                    dObj->rxEnabled = false;
        </#if>
                    /* Job is complete*/
                    currentJob->status = DRV_SPI_BUFFER_EVENT_COMPLETE;
                    /* Call the job complete call back*/
                    if (currentJob->completeCB != NULL)
                    {
                        (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
                    }
        <#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>

                    /* Call the operation complete call back.  This is different than the
                       job complete callback.  This can be used to modify the Slave Select line.*/

                    if (dObj->operationEnded != NULL)
                    {
                        (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
                    }
        </#if>

                    /* Return the job back to the free queue*/
        <#if DRV_ISR=="ISR">
                    if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        <#else>
                    if (DRV_SPI_SYS_QUEUE_FreeElementLock(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
        </#if>
                    {
                        SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                        return 0;
                    }
                    /* Clean up */
                    dObj->currentJob = NULL;
        <#if DRV_ISR=="ISR">
                    if (!DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue))
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
        if (rxDMAInProgress)
        {
            // DMA is in progress
            // Wipe out the symbols in Progress
            dObj->rxEnabled = false;
            dObj->symbolsInProgress = 0;
        }
    </#if>


    <#if DRV_ISR=="ISR">
        /* Check to see if the interrupts would fire again if so just go back into
           the loop instead of suffering the interrupt latency of exiting and re-entering*/
        <#if CONFIG_DRV_SPI_USE_DMA>
        if ((dObj->currentJob != NULL) && (!rxDMAInProgress || !txDMAInProgress))
        <#else>
        if (dObj->currentJob != NULL)
        </#if>
        {   
        <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "persistent">
            /* Clear the Interrupts */
            SYS_INT_SourceStatusClear(${RCV_INT});
            SYS_INT_SourceStatusClear(${XMIT_INT});
            SYS_INT_SourceStatusClear(${ERR_INT});
        </#if>
            /* Interrupts should immediately become active again if they're in a fired condition */
            if ((SYS_INT_SourceStatusGet(${RCV_INT})) ||
                (SYS_INT_SourceStatusGet(${XMIT_INT})) ||
                (SYS_INT_SourceStatusGet(${ERR_INT})))
            {
                /* Interrupt would fire again anyway so we should just go back to the start*/
                continueLoop = true;
                continue;
            }
             /* If we're here then we know that the interrupt should not be firing again immediately, so re-enable them and exit*/
                SYS_INT_SourceEnable(${RCV_INT});
                SYS_INT_SourceEnable(${XMIT_INT});
            return 0;
        }
    <#elseif DRV_BUFFER=="RM">
        counter ++;
        if ((counter < numPolled) && (result != 0))
        {
            continueLoop = true;
        }
    </#if>

    } while(continueLoop);
    <#if DRV_ISR=="ISR">
    /* if we're here it means that we have no more jobs in the queue, tx and rx interrupts will be re-enabled by the BufferAdd* functions*/
    SYS_INT_SourceStatusClear(${RCV_INT});
    SYS_INT_SourceStatusClear(${XMIT_INT});
    </#if>
    return 0;
}
</#if>

<#if DRV_MODE=="Slave">
int32_t DRV_SPI${DRV_INSTANCE}_${DRV_ISR}Slave${DRV_BUFFER}${DRV_BIT_WIDTH}BitTasks ( struct DRV_SPI_OBJ * dObj )
{
    <#if DRV_ISR=="ISR">
    volatile bool continueLoop;
    SYS_INT_SourceDisable(${RCV_INT});
    SYS_INT_SourceDisable(${XMIT_INT});
    SYS_INT_SourceDisable(${ERR_INT});
    do {
        <#if DRV_ISR=="ISR" && CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "non-persistent">
        /* Clear the Interrupts */
        SYS_INT_SourceStatusClear(${RCV_INT});
        SYS_INT_SourceStatusClear(${XMIT_INT});
        SYS_INT_SourceStatusClear(${ERR_INT});
        </#if>
        
        continueLoop = false;
    </#if>
        DRV_SPI_JOB_OBJECT * currentJob = dObj->currentJob;

        if (dObj->currentJob == NULL)
        {
    <#if DRV_ISR=="ISR">
            if (DRV_SPI_SYS_QUEUE_Dequeue(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_DequeueLock(dObj->queue, (void *)&(dObj->currentJob)) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Error in dequeing.");
                return 0;
            }
            if (dObj->currentJob == NULL)
            {
    <#if DRV_ISR=="ISR">
                dObj->txEnabled = false;
    </#if>
                return 0;
            }
            currentJob = dObj->currentJob;
            dObj->symbolsInProgress = 0;
    <#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>

            /* Call the operation starting function pointer.  This can be used to modify the slave select lines */
            if (dObj->operationStarting != NULL)
            {
                (*dObj->operationStarting)(DRV_SPI_BUFFER_EVENT_PROCESSING, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
    </#if>

            /* List the new job as processing*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_PROCESSING;
    <#if DRV_ISR=="ISR" && DRV_BUFFER=="EBM">
            if (currentJob->dataLeftToTx > PLIB_SPI_RX_8BIT_FIFO_SIZE(${PLIB_INSTANCE}))
            {
                PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_1HALF_EMPTY_OR_MORE);
                PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_1HALF_FULL_OR_MORE);
            }
    </#if>
            /* Flush out the Receive buffer */
            PLIB_SPI_BufferClear(${PLIB_INSTANCE});
        }


        if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx != 0)
        {
            DRV_SPI${DRV_INSTANCE}_Slave${DRV_BUFFER}Receive${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
        }
        if (currentJob->dataLeftToTx != 0)
        {
            DRV_SPI${DRV_INSTANCE}_Slave${DRV_BUFFER}Send${DRV_BIT_WIDTH}Bit${DRV_ISR}(dObj);
        }
    <#if DRV_ISR=="ISR">
        else
        {
            dObj->txEnabled = false;
            dObj->rxEnabled = true;
        }
    </#if>

        DRV_SPI${DRV_INSTANCE}_${DRV_ISR}ErrorTasks(dObj);

        /* Check to see if the job is done */
        if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
        {
    <#if DRV_ISR=="ISR">
            // Disable the interrupt, or more correctly don't re-enable it later*/
            dObj->rxEnabled = false;
    </#if>
            /* Job is complete*/
            currentJob->status = DRV_SPI_BUFFER_EVENT_COMPLETE;
            /* Call the job complete call back*/
            if (currentJob->completeCB != NULL)
            {
                (*currentJob->completeCB)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
    <#if CONFIG_DRV_SPI_USE_CLIENT_CONFIG_FUCNTION == true>

            /* Call the operation complete call back.  This is different than the
               job complete callback.  This can be used to modify the Slave Select line.*/

            if (dObj->operationEnded != NULL)
            {
                (*dObj->operationEnded)(DRV_SPI_BUFFER_EVENT_COMPLETE, (DRV_SPI_BUFFER_HANDLE)currentJob, currentJob->context);
            }
    </#if>

            /* Return the job back to the free queue*/
    <#if DRV_ISR=="ISR">
            if (DRV_SPI_SYS_QUEUE_FreeElement(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    <#else>
            if (DRV_SPI_SYS_QUEUE_FreeElementLock(dObj->queue, currentJob) != DRV_SPI_SYS_QUEUE_SUCCESS)
    </#if>
            {
                SYS_ASSERT(false, "\r\nSPI Driver: Queue free element error.");
                return 0;
            }
            /* Clean up */
            dObj->currentJob = NULL;
    <#if DRV_ISR=="ISR">
            if (!DRV_SPI_SYS_QUEUE_IsEmpty(dObj->queue))
            {
                continueLoop = true;
                continue;
            }
    </#if>
        }

        <#if DRV_ISR=="ISR">
        /* Check to see if the interrupts would fire again if so just go back into
           the loop instead of suffering the interrupt latency of exiting and re-entering*/
        if (dObj->currentJob != NULL)
        {
        <#if CONFIG_DRV_SPI_INTERRUPT_TYPE_SELECT == "persistent">
            /* Clear the Interrupts */
            SYS_INT_SourceStatusClear(${RCV_INT});
            SYS_INT_SourceStatusClear(${XMIT_INT});
            SYS_INT_SourceStatusClear(${ERR_INT});
        </#if>
            /* Interrupts should immediately become active again if they're in a fired condition */
            if ((SYS_INT_SourceStatusGet(${RCV_INT})) || (SYS_INT_SourceStatusGet(${ERR_INT})))
            {
                /* Interrupt would fire again anyway so we should just go back to the start*/
                continueLoop = true;
                continue;
            }
            /* If we're here then we know that the interrupt should not be firing immediately so enable it and exit */
                SYS_INT_SourceEnable(${RCV_INT});
	    if (dObj->txEnabled)
            {
                SYS_INT_SourceEnable(${XMIT_INT});
            }
                return 0;
        }

    } while(continueLoop);
    </#if>
    return 0;
}
</#if>

<#if CONFIG_DRV_SPI_USE_DMA>
void DRV_SPI${DRV_INSTANCE}_SetupDMA( struct DRV_SPI_OBJ * dObj)
{

        <#if .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_0">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_0_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_0_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_1">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_1_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_1_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_2">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_2_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_2_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_3">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_3_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_3_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_4">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_4_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_4_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_5">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_5_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_5_RECEIVE"
            />
        <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${DRV_INSTANCE}"]=="SPI_ID_6">
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_6_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_6_RECEIVE"
            />
        <#else>
            <@DMA_SETUP DMA_TX_SOURCE = "DMA_TRIGGER_SPI_7_TRANSMIT"
            DMA_RX_SOURCE = "DMA_TRIGGER_SPI_7_RECEIVE"
            />
        </#if>
}
</#if>
</#macro>

<#macro DMA_SETUP DMA_TX_SOURCE DMA_RX_SOURCE>
    if (dObj->txDmaThreshold != 0)
    {
        dObj->txDmaChannelHandle = SYS_DMA_ChannelAllocate(${TX_DMA_CHANNEL});
        SYS_DMA_ChannelSetup(dObj->txDmaChannelHandle, SYS_DMA_CHANNEL_OP_MODE_BASIC, ${DMA_TX_SOURCE});
        SYS_DMA_ChannelTransferEventHandlerSet(dObj->txDmaChannelHandle, dObj->sendDMAHander, (uintptr_t)dObj);
    }

    if (dObj->rxDmaThreshold != 0)
    {
        dObj->rxDmaChannelHandle = SYS_DMA_ChannelAllocate(${RX_DMA_CHANNEL});
        SYS_DMA_ChannelSetup(dObj->rxDmaChannelHandle, SYS_DMA_CHANNEL_OP_MODE_BASIC, ${DMA_RX_SOURCE});
        SYS_DMA_ChannelTransferEventHandlerSet(dObj->rxDmaChannelHandle, dObj->receiveDMAHander, (uintptr_t)dObj);
    }
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