/*******************************************************************************
  SPI Driver Functions for Static Standard Buffer Driver Tasks Functions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_static_rm_tasks.c

  Summary:
    SPI driver tasks functions

  Description:
    The SPI device driver provides a simple interface to manage the SPI
    modules on Microchip microcontrollers. This file contains implemenation
    for the SPI driver.

  Remarks:
  This file is generated from framework/driver/spi/template/drv_spi_static_rm_tasks.c.ftl
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
<#if DRV_MODE == "Master" && DRV_BUFFER=="RM">
int32_t DRV_SPI${DRV_INSTANCE}_MasterRMSend${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    /* Check to see if we have any more bytes to transmit */
    if (currentJob->dataLeftToTx + currentJob->dummyLeftToTx == 0)
    {
        <#if DRV_ISR=="ISR">
        /* We don't have any more data to send make sure the transmit interrupt is disabled */
        pDrvObj->txEnabled = false;
        </#if>
        return 0;
    }
    /* Check to see if the transmit buffer is empty*/
    if (!PLIB_SPI_TransmitBufferIsEmpty(${PLIB_INSTANCE}))
    {
        return 0;
    }
    /* Make sure that we don't have something in progress and overrun the RX buffer */
    if (pDrvObj->symbolsInProgress != 0)
    {
        return 0;
    }
    if (currentJob->dataLeftToTx != 0)
    {
    /* Transmit the data & update the counts */
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, currentJob->txBuffer[currentJob->dataTxed]);
        currentJob->dataTxed++;
        currentJob->dataLeftToTx--;
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, ((uint16_t*)currentJob->txBuffer)[currentJob->dataTxed>>1]);
        currentJob->dataTxed+=2;
        currentJob->dataLeftToTx-=2;
    <#else>
        PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, ((uint32_t*)currentJob->txBuffer)[currentJob->dataTxed>>2]);
        currentJob->dataTxed+=4;
        currentJob->dataLeftToTx-=4;
    </#if>
    }
    else
    {
        /* Transmit the dummy data & update the counts */
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, (uint8_t)pDrvObj->dummyByteValue);
        currentJob->dummyLeftToTx--;
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, (uint16_t)pDrvObj->dummyByteValue);
        currentJob->dummyLeftToTx-=2;
    <#else>
        PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, (uint32_t)pDrvObj->dummyByteValue);
        currentJob->dummyLeftToTx-=4;
    </#if>
    }
    /* We now have a symbol in progress*/
    pDrvObj->symbolsInProgress = 1;

    return 0;
}

int32_t DRV_SPI${DRV_INSTANCE}_MasterRMReceive${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    if (currentJob == NULL)
    {
        return 0;
    }
    if (PLIB_SPI_ReceiverBufferIsFull(${PLIB_INSTANCE}))
    {
        /* We have data waiting in the SPI buffer */
        if (currentJob->dataLeftToRx != 0)
        {
            /* Receive the data and updates the count */
    <#if DRV_BIT_WIDTH=="8">
            currentJob->rxBuffer[currentJob->dataRxed] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
            currentJob->dataRxed++;
            currentJob->dataLeftToRx --;
    <#elseif DRV_BIT_WIDTH=="16">
            ((uint16_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>1] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
            currentJob->dataRxed+=2;
            currentJob->dataLeftToRx -=2;
    <#else>
            ((uint32_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>2] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
            currentJob->dataRxed+=4;
            currentJob->dataLeftToRx -=4;
    </#if>
        }
        else
        {
            /* No Data but dummy data: Note: We cannot just clear the
               buffer because we have to keep track of how many symbols/units we
               have received, and the number may have increased since we checked
               how full the buffer is.*/
    <#if DRV_BIT_WIDTH=="8">
            PLIB_SPI_BufferRead(${PLIB_INSTANCE});
            //SYS_CONSOLE_MESSAGE("Rd ");
            currentJob->dummyLeftToRx--;
    <#elseif DRV_BIT_WIDTH=="16">
            PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
            currentJob->dummyLeftToRx-=2;
    <#else>
            PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
            currentJob->dummyLeftToRx-=4;
    </#if>
        }
        /* No longer have a symbol in progress */
        pDrvObj->symbolsInProgress = 0;
    }

    return 0;
}
</#if>

<#if DRV_MODE == "Slave" && DRV_BUFFER=="RM">
int32_t DRV_SPI${DRV_INSTANCE}_SlaveRMSend${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;
    if (currentJob == NULL)
    {
        return 0;
    }
    if (!PLIB_SPI_TransmitBufferIsEmpty(${PLIB_INSTANCE}))
    {
        return 0;
    }

    // Add in DMA stuff here

    <#if DRV_BIT_WIDTH=="8">
    PLIB_SPI_BufferWrite(${PLIB_INSTANCE}, currentJob->txBuffer[currentJob->dataTxed]);
    currentJob->dataTxed++;
    currentJob->dataLeftToTx--;
    <#elseif DRV_BIT_WIDTH=="16">
    PLIB_SPI_BufferWrite16bit(${PLIB_INSTANCE}, ((uint16_t*)currentJob->txBuffer)[currentJob->dataTxed>>1]);
    currentJob->dataTxed+=2;
    currentJob->dataLeftToTx-=2;
    <#else>
    PLIB_SPI_BufferWrite32bit(${PLIB_INSTANCE}, ((uint32_t*)currentJob->txBuffer)[currentJob->dataTxed>>2]);
    currentJob->dataTxed+=4;
    currentJob->dataLeftToTx-=4;
    </#if>

    <#if DRV_ISR=="ISR">
    SYS_INT_SourceStatusClear(${XMIT_INT});
    </#if>
    return 0;


}

int32_t DRV_SPI${DRV_INSTANCE}_SlaveRMReceive${DRV_BIT_WIDTH}Bit${DRV_ISR}( struct DRV_SPI_OBJ * pDrvObj )
{
    register DRV_SPI_JOB_OBJECT * currentJob = pDrvObj->currentJob;

    if (currentJob->dataLeftToRx + currentJob->dummyLeftToRx == 0)
    {
        return 0;
    }

    if (!PLIB_SPI_ReceiverBufferIsFull(${PLIB_INSTANCE}))
    {
        return 0;
    }

    // Put in DMA stuff here
    if (currentJob->dataLeftToRx != 0)
    {
    <#if DRV_BIT_WIDTH=="8">
        currentJob->rxBuffer[currentJob->dataRxed] = PLIB_SPI_BufferRead(${PLIB_INSTANCE});
        currentJob->dataRxed++;
        currentJob->dataLeftToRx--;
    <#elseif DRV_BIT_WIDTH=="16">
        ((uint16_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>1] = PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
        currentJob->dataRxed+=2;
        currentJob->dataLeftToRx-=2;
    <#else>
        ((uint32_t*)(currentJob->rxBuffer))[currentJob->dataRxed>>2] = PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
        currentJob->dataRxed+=4;
        currentJob->dataLeftToRx-=4;
    </#if>
    }
    else
    {
    <#if DRV_BIT_WIDTH=="8">
        PLIB_SPI_BufferRead(${PLIB_INSTANCE});
        currentJob->dummyLeftToRx--;
    <#elseif DRV_BIT_WIDTH=="16">
        PLIB_SPI_BufferRead16bit(${PLIB_INSTANCE});
        currentJob->dummyLeftToRx-=2;
    <#else>
        PLIB_SPI_BufferRead32bit(${PLIB_INSTANCE});
        currentJob->dummyLeftToRx-=4;
    </#if>
    }

    <#if DRV_ISR=="ISR">
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