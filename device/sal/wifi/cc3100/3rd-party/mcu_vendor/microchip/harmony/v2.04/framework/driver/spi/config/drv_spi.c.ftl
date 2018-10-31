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
 <#macro DRV_SPI_INIT_C_DEFS INST_INDEX>
 <#if CONFIG_DRV_SPI_DYNAMIC_MODE == true>
  /*** Index ${INST_INDEX}  ***/
 DRV_SPI_INIT drvSpi${INST_INDEX}InitData =
 {
    .spiId = DRV_SPI_SPI_ID_IDX${INST_INDEX},
    .taskMode = DRV_SPI_TASK_MODE_IDX${INST_INDEX},
    .spiMode = DRV_SPI_SPI_MODE_IDX${INST_INDEX},
    .allowIdleRun = DRV_SPI_ALLOW_IDLE_RUN_IDX${INST_INDEX},
    .spiProtocolType = DRV_SPI_SPI_PROTOCOL_TYPE_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_SPI_PROTOCOL_TYPE_IDX${INST_INDEX}"] == "DRV_SPI_PROTOCOL_TYPE_STANDARD">
        <#if .vars["CONFIG_DRV_SPI_SPI_MODE_SLAVE_IDX${INST_INDEX}"] == true>
    .spiSlaveSSPin = DRV_SPI_SPI_USE_SS_FOR_SLAVE_IDX${INST_INDEX},
        </#if>
    <#elseif .vars["CONFIG_DRV_SPI_SPI_PROTOCOL_TYPE_IDX${INST_INDEX}"] == "DRV_SPI_PROTOCOL_TYPE_FRAMED">
    .frameSyncPulse = DRV_SPI_FRAME_SYNC_PULSE_IDX${INST_INDEX},
    .framePulsePolarity = DRV_SPI_FRAME_PULSE_POLARITY_IDX${INST_INDEX},
    .framePulseDirection = DRV_SPI_FRAME_PULSE_DIRECTION_IDX${INST_INDEX},
    .framePulseEdge = DRV_SPI_FRAME_PULSE_EDGE_IDX${INST_INDEX},
    .framePulseWidth = DRV_SPI_FRAME_PULSE_WIDTH_IDX${INST_INDEX},
    <#elseif .vars["CONFIG_DRV_SPI_SPI_PROTOCOL_TYPE_IDX${INST_INDEX}"] == "DRV_SPI_PROTOCOL_TYPE_AUDIO">
    .audioTransmitMode = DRV_SPI_AUDIO_TRANSMIT_MODE_IDX${INST_INDEX},
    .audioProtocolMode = DRV_SPI_AUDIO_PROTOCOL_MODE_IDX${INST_INDEX},
    </#if>
    .commWidth = DRV_SPI_COMM_WIDTH_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_CLOCK_SOURCE_IDX${INST_INDEX}"] == .vars["CONFIG_DRV_SPI_CLOCK_SOURCE2_IDX${INST_INDEX}"]>
    .baudClockSource = DRV_SPI_CLOCK_SOURCE_IDX${INST_INDEX},
    </#if>
    <#if .vars["CONFIG_DRV_SPI_CLOCK_SOURCE_IDX${INST_INDEX}"] == "SPI_BAUD_RATE_PBCLK_CLOCK">
    .spiClk = DRV_SPI_SPI_CLOCK_IDX${INST_INDEX},
    </#if>
    .baudRate = DRV_SPI_BAUD_RATE_IDX${INST_INDEX},
    .bufferType = DRV_SPI_BUFFER_TYPE_IDX${INST_INDEX},
    .clockMode = DRV_SPI_CLOCK_MODE_IDX${INST_INDEX},
    .inputSamplePhase = DRV_SPI_INPUT_PHASE_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_TASK_MODE_ISR_IDX${INST_INDEX}"] == true>
    .txInterruptSource = DRV_SPI_TX_INT_SOURCE_IDX${INST_INDEX},
    .rxInterruptSource = DRV_SPI_RX_INT_SOURCE_IDX${INST_INDEX},
    .errInterruptSource = DRV_SPI_ERROR_INT_SOURCE_IDX${INST_INDEX},
    </#if>
    .dummyByteValue = DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${INST_INDEX}"] == true>
    .txDmaChannel =         DRV_SPI_TX_DMA_CHANNEL_IDX${INST_INDEX},
    .txDmaThreshold =       DRV_SPI_TX_DMA_THRESHOLD_IDX${INST_INDEX},
    .rxDmaChannel =         DRV_SPI_RX_DMA_CHANNEL_IDX${INST_INDEX},
    .rxDmaThreshold =       DRV_SPI_RX_DMA_THRESHOLD_IDX${INST_INDEX},
    </#if>
    .queueSize = DRV_SPI_QUEUE_SIZE_IDX${INST_INDEX},
    .jobQueueReserveSize = DRV_SPI_RESERVED_JOB_IDX${INST_INDEX},
    <#if .vars["CONFIG_DRV_SPI_BUFFER_STANDARD_IDX${INST_INDEX}"] == true && .vars["CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX${INST_INDEX}"] == true>
    .numTrfsSmPolled = DRV_SPI_TRANS_PER_SM_RUN_IDX${INST_INDEX},
    </#if>
 };
 </#if>
 </#macro>
<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_INIT_C_DEFS "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_INIT_C_DEFS "1"/>
</#if>
<#if CONFIG_DRV_SPI_IDX2 == true>
    <@DRV_SPI_INIT_C_DEFS "2"/>
</#if>
<#if CONFIG_DRV_SPI_IDX3 == true>
    <@DRV_SPI_INIT_C_DEFS "3"/>
</#if>
<#if CONFIG_DRV_SPI_IDX4 == true>
    <@DRV_SPI_INIT_C_DEFS "4"/>
</#if>
<#if CONFIG_DRV_SPI_IDX5 == true>
    <@DRV_SPI_INIT_C_DEFS "5"/>
</#if>
// </editor-fold>
