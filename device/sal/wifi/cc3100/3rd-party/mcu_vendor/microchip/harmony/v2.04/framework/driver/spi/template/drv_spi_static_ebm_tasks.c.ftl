/*******************************************************************************
  SPI Driver Functions for Static Enhanced Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static_ebm_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.

  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_static_ebm_tasks.c.ftl
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
<#if DRV_MODE == "Master" && DRV_BUFFER=="EBM">
int32_t DRV_SPI${DRV_INSTANCE}_MasterEBMSend${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    /* Determine the maximum number of bytes we can send to the FIFO*/
        uint8_t symbolsInTransit = MAX(pDrvObj->symbolsInProgress, PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_TRANSMIT));
    <#if DRV_BIT_WIDTH=="8">
        uint8_t bufferBytes = PLIB_SPI_TX_8BIT_FIFO_SIZE(${PLIB_INSTANCE}) - symbolsInTransit;
    <#elseif DRV_BIT_WIDTH=="16">
        uint8_t bufferBytes = (PLIB_SPI_TX_16BIT_FIFO_SIZE(${PLIB_INSTANCE}) - symbolsInTransit) << 1;
    <#else>
        uint8_t bufferBytes = (PLIB_SPI_TX_32BIT_FIFO_SIZE(${PLIB_INSTANCE}) - symbolsInTransit) << 2;
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
            PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, bufferLoc[counter]);
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, bufferLoc[counter]);
    <#else>
            PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, bufferLoc[counter]);
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
        pDrvObj->symbolsInProgress += dataUnits;
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
        pDrvObj->symbolsInProgress += dummyUnits;
        
        for (counter = 0; counter < dummyUnits; counter++)
        {
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, (uint8_t)pDrvObj->dummyByteValue);
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, (uint16_t)pDrvObj->dummyByteValue);
    <#else>
            PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, (uint32_t)pDrvObj->dummyByteValue);
    </#if>
        }
    }
<#if DRV_ISR=="ISR">
    if (currentJob->dataLeftToTx + currentJob->dummyLeftToTx == 0)
    {
        /* We have no more data to send, turn off the TX interrupt*/
        PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
        pDrvObj->txEnabled = false;

        /* Turn on the RX Interrupt*/
        pDrvObj->rxEnabled = true;
    }
</#if>
    return 0;
}

int32_t DRV_SPI${DRV_INSTANCE}_MasterEBMReceive${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    /* Figure out how many bytes are waiting to be received."*/
    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE) << 1;
    <#else>
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE) << 2;
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
            bufferLoc[counter] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH=="16">
            bufferLoc[counter] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
            bufferLoc[counter] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
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
        pDrvObj->symbolsInProgress -= dataUnits;
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
        pDrvObj->symbolsInProgress -= dummyUnits;
        for (counter = 0; counter < dummyUnits; counter++)
        {
            /* Receive and throw away the byte.  Note: We cannot just clear the
               buffer because we have to keep track of how many symbols/units we
               have received, and the number may have increased since we checked
               how full the buffer is.*/
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
            PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
    </#if>
        }
    }

    <#if DRV_ISR=="ISR">
    /* Figure out how many bytes are left to be received */
    size_t bytesLeft = currentJob->dataLeftToRx + currentJob->dummyLeftToRx;

    /* If the bytes left are smaller than the HW mark we have to change the interrupt mode */
    if (bytesLeft < PLIB_SPI_RX_8BIT_HW_MARK(${PLIB_INSTANCE}))
    {
        PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    }
    </#if>
    return 0;
}
</#if>

<#if DRV_MODE == "Slave" && DRV_BUFFER=="EBM">
int32_t DRV_SPI${DRV_INSTANCE}_SlaveEBMSend${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob == NULL)
    {
        return 0;
    }
    /* determine the maximum number of bytes that can be transmitted */
    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_TX_8BIT_FIFO_SIZE(${PLIB_INSTANCE}) - PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_TRANSMIT);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = (PLIB_SPI_TX_16BIT_FIFO_SIZE(${PLIB_INSTANCE}) - PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_TRANSMIT)) << 1;
    <#else>
    uint8_t bufferBytes = (PLIB_SPI_TX_32BIT_FIFO_SIZE(${PLIB_INSTANCE}) - PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_TRANSMIT)) << 2;
    </#if>
    size_t dataUnits = MIN(currentJob->dataLeftToTx, bufferBytes);
    currentJob->dataLeftToTx -= dataUnits;


    <#if DRV_ISR=="ISR">
    /* Check to see if we'll run out of data*/
    if (currentJob->dataLeftToTx < bufferBytes)
    {
        PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_TRANSMIT_BUFFER_IS_COMPLETELY_EMPTY);
        SYS_INT_SourceDisable(${XMIT_INT});
        SYS_INT_SourceEnable(${RCV_INT});
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
        PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, bufferLoc[counter]);
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, bufferLoc[counter]);
    <#else>
        PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, bufferLoc[counter]);
    </#if>
    }

    <#if DRV_BIT_WIDTH=="8">
    currentJob->dataTxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
    currentJob->dataTxed += dataUnits<<1;
    <#else>
    currentJob->dataTxed += dataUnits<<2;
    </#if>

    <#if DRV_ISR=="ISR">
    SYS_INT_SourceStatusClear(${XMIT_INT});
    </#if>
    return 0;

}

int32_t DRV_SPI${DRV_INSTANCE}_SlaveEBMReceive${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
    {
        return 0;
    }

    <#if DRV_BIT_WIDTH=="8">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE);
    <#elseif DRV_BIT_WIDTH=="16">
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE) << 1;
    <#else>
    uint8_t bufferBytes = PLIB_SPI_FIFOCountGet(${PLIB_INSTANCE}, SPI_FIFO_TYPE_RECEIVE) << 2;
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
        bufferLoc[counter] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH=="16">
        bufferLoc[counter] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
        bufferLoc[counter] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
    </#if>
    }

    for (counter = 0; counter < dummyUnits; counter++)
    {
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferRead(${PLIB_INSTANCE});
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
    <#else>
        PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
    </#if>
    }
    <#if DRV_BIT_WIDTH=="8">
    currentJob->dataRxed += dataUnits;
    <#elseif DRV_BIT_WIDTH=="16">
    currentJob->dataRxed += dataUnits<<1;
    <#else>
    currentJob->dataRxed += dataUnits<<2;
    </#if>

    <#if DRV_ISR=="ISR">
    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx < PLIB_SPI_RX_8BIT_HW_MARK(${PLIB_INSTANCE}))
    {
        PLIB_SPI_FIFOInterruptModeSelect(${PLIB_INSTANCE}, SPI_FIFO_INTERRUPT_WHEN_RECEIVE_BUFFER_IS_NOT_EMPTY);
    }
    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
    {
        SYS_INT_SourceDisable(${RCV_INT});
        SYS_INT_SourceEnable(${XMIT_INT});
    }
    SYS_INT_SourceStatusClear(${RCV_INT});
    </#if>
    return 0;
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