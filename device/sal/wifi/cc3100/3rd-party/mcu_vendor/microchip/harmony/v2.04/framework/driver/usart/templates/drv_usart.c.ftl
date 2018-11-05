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
// <editor-fold defaultstate="collapsed" desc="DRV_USART Initialization Data">
<#if CONFIG_DRV_USART_DRIVER_MODE == "DYNAMIC">
<#-- Instance 0 -->
<#if CONFIG_DRV_USART_INST_IDX0 == true>

const DRV_USART_INIT drvUsart0InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX0?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX0,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX0?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX0, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX0?has_content>
    .mode = DRV_USART_OPER_MODE_IDX0,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX0?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX0,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX0?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX0,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX0?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX0,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX0?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX0,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX0?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX0,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX0?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX0,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX0?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX0?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX0?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX0?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX0,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX0?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX0,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX0?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX0,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX0,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX0,    
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX0?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX0,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX0    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive = DRV_USART_RCV_INT_SRC_IDX0,    
</#if>
};
</#if>
<#-- Instance 1 -->
<#if CONFIG_DRV_USART_INST_IDX1 == true>

const DRV_USART_INIT drvUsart1InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX1?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX1,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX1?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX1, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX1?has_content>
    .mode = DRV_USART_OPER_MODE_IDX1,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX1?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX1,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX1?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX1,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX1?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX1,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX1?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX1,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX1?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX1,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX1?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX1,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX1?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX1?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX1?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX1?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX1,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX1?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX1,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX1?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX1,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX1,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX1?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX1,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX1    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive= DRV_USART_RCV_INT_SRC_IDX1,
</#if>
};
</#if>
<#-- Instance 2 -->
<#if CONFIG_DRV_USART_INST_IDX2 == true>

const DRV_USART_INIT drvUsart2InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX2?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX2,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX2?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX2, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX2?has_content>
    .mode = DRV_USART_OPER_MODE_IDX2,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX2?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX2,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX2?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX2,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX2?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX2,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX2?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX2,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX2?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX2,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX2?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX2,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX2?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX2?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX2?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX2?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX2,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX2?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX2,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX2?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX2,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX2,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX2?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX2,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX2    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive = DRV_USART_RCV_INT_SRC_IDX2,
</#if>
};
</#if>
<#-- Instance 3 -->
<#if CONFIG_DRV_USART_INST_IDX3 == true>

const DRV_USART_INIT drvUsart3InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX3?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX3,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX3?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX3, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX3?has_content>
    .mode = DRV_USART_OPER_MODE_IDX3,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX3?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX3,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX3?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX3,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX3?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX3,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX3?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX3,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX3?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX3,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX3?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX3,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX3?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX3?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX3?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX3?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX3,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX3?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX3,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX3?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX3,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX3,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX3?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX3,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX3    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive = DRV_USART_RCV_INT_SRC_IDX3,
</#if>
};
</#if>
<#-- Instance 4 -->
<#if CONFIG_DRV_USART_INST_IDX4 == true>

const DRV_USART_INIT drvUsart4InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX4?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX4,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX4?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX4, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX4?has_content>
    .mode = DRV_USART_OPER_MODE_IDX4,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX4?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX4,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX4?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX4,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX4?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX4,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX4?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX4,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX4?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX4,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX4?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX4,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX4?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX4?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX4?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX4?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX4,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX4?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX4,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX4?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX4,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX4,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX4?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX4,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX4    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive = DRV_USART_RCV_INT_SRC_IDX4,
</#if>
};
</#if>
<#-- Instance 5 -->
<#if CONFIG_DRV_USART_INST_IDX5 == true>

const DRV_USART_INIT drvUsart5InitData =
{
<#if CONFIG_DRV_USART_POWER_STATE_IDX5?has_content>
    .moduleInit.value = DRV_USART_POWER_STATE_IDX5,
</#if>
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX5?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX5, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX5?has_content>
    .mode = DRV_USART_OPER_MODE_IDX5,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX5?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX5,
</#if>
<#if CONFIG_DRV_USART_INIT_FLAGS_IDX5?has_content>
    .flags = DRV_USART_INIT_FLAGS_IDX5,
</#if>
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX5?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX5,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX5?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX5,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX5?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX5,
</#if>
<#if CONFIG_DRV_USART_HANDSHAKE_MODE_IDX5?has_content>
    .handshake = DRV_USART_HANDSHAKE_MODE_IDX5,
</#if>
<#if CONFIG_DRV_USART_XMIT_INT_SRC_IDX5?has_content>
    .interruptTransmit = DRV_USART_XMIT_INT_SRC_IDX5,
</#if>
<#if CONFIG_DRV_USART_RCV_INT_SRC_IDX5?has_content>
    .interruptReceive = DRV_USART_RCV_INT_SRC_IDX5,
</#if>
<#if CONFIG_DRV_USART_ERR_INT_SRC_IDX5?has_content>
    .interruptError = DRV_USART_ERR_INT_SRC_IDX5,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX5?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX5,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX5?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX5,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX5?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX5,
    .dmaInterruptTransmit = DRV_USART_XMIT_DMA_INT_SRC_IDX5,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
    .dmaInterruptTransmit = DRV_USART_XMIT_INT_SRC_IDX5,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX5?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX5,
    .dmaInterruptReceive = DRV_USART_RCV_DMA_INT_SRC_IDX5    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
    .dmaInterruptReceive = DRV_USART_RCV_INT_SRC_IDX5,
</#if>
};
</#if>
</#if>
// </editor-fold>
<#--
/*******************************************************************************
 End of File
*/
-->

