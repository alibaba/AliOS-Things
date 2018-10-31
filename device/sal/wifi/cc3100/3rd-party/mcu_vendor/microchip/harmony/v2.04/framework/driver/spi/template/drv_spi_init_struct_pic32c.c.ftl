<#--
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
 -->
// <editor-fold defaultstate="collapsed" desc="DRV_SPI Initialization Data"> 
 /*** SPI Driver Initialization Data ***/
 <#macro DRV_SPI_INIT_STRUCT_C INST_INDEX>
 <#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
  /*** Driver Instance ${INST_INDEX}  ***/
 const DRV_SPI_INIT drvSpi${INST_INDEX}InitData =
 {
    .spiId = DRV_SPI_SPI_ID_IDX${INST_INDEX},
    .taskMode = DRV_SPI_TASK_MODE_IDX${INST_INDEX},
    .spiMode = DRV_SPI_SPI_MODE_IDX${INST_INDEX},
    .commWidth = DRV_SPI_COMM_WIDTH_IDX${INST_INDEX},
    .spiClkSrc = DRV_SPI_BAUD_CLOCK_IDX${INST_INDEX},
    .baudRate = DRV_SPI_BAUD_RATE_IDX${INST_INDEX},
    .clockMode = DRV_SPI_CLOCK_MODE_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_INTERRUPT_MODE"] == true>
    .interruptSPI = DRV_SPI_INT_VECTOR_IDX${INST_INDEX},
    </#if>
    .dummyByteValue = DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX${INST_INDEX},
    .queueSize = DRV_SPI_QUEUE_SIZE_IDX${INST_INDEX},
    <#if CONFIG_DRV_SPI_USE_DMA>
    <#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${INST_INDEX}"] == true>
    .txDmaChannel = DRV_SPI_TX_DMA_CHANNEL_IDX${INST_INDEX},
    .rxDmaChannel = DRV_SPI_RX_DMA_CHANNEL_IDX${INST_INDEX},
    .dmaChannelTransmitTrigger = DRV_SPI_TX_DMA_CHANNEL_TRIG_IDX${INST_INDEX},
    .dmaChannelReceiveTrigger = DRV_SPI_RX_DMA_CHANNEL_TRIG_IDX${INST_INDEX},
    .interruptDMA = DRV_SPI_DMA_INT_VECTOR_IDX${INST_INDEX},
    <#else>
    .txDmaChannel = DMA_CHANNEL_NONE,
    .rxDmaChannel = DMA_CHANNEL_NONE,
    </#if>
    </#if>
 };
 </#if>
 </#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_INIT_STRUCT_C "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_INIT_STRUCT_C "1"/>
</#if>
// </editor-fold>
