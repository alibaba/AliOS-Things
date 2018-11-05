<#--
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
 -->
// <editor-fold defaultstate="collapsed" desc="DRV_USART Initialization Data">
<#if CONFIG_DRV_USART_DRIVER_MODE == "DYNAMIC">
<#-- Instance 0 -->
<#if CONFIG_DRV_USART_INST_IDX0 == true>

const DRV_USART_INIT drvUsart0InitData =
{
<#if CONFIG_DRV_USART_PERIPHERAL_ID_IDX0?has_content>
    .usartID = DRV_USART_PERIPHERAL_ID_IDX0, 
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_IDX0?has_content>
    .mode = DRV_USART_OPER_MODE_IDX0,
</#if>
<#if CONFIG_DRV_USART_OPER_MODE_DATA_IDX0?has_content>
    .modeData.AddressedModeInit.address = DRV_USART_OPER_MODE_DATA_IDX0,
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
<#if CONFIG_DRV_USART_INT_SRC_IDX0?has_content>
    .interruptUSART = DRV_USART_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX0?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX0,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX0?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX0,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX0?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX0,
    .dmaChannelTransmitTrigger = DRV_USART_XMIT_DMA_CH_TRIG_IDX0,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,  
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX0?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX0,
    .dmaChannelReceiveTrigger = DRV_USART_RCV_DMA_CH_TRIG_IDX0,     
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,  
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX0?has_content || CONFIG_DRV_USART_RCV_DMA_CH_IDX0?has_content>
    .interruptDMA = DRV_USART_DMA_INT_SRC_IDX0,  
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
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX1?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX1,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX1?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX1,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX1?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX1,
</#if>
<#if CONFIG_DRV_USART_INT_SRC_IDX1?has_content>
    .interruptUSART = DRV_USART_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX1?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX1,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX1?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX1,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX1?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX1,
    .dmaChannelTransmitTrigger = DRV_USART_XMIT_DMA_CH_TRIG_IDX1,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX1?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX1,
    .dmaChannelReceiveTrigger = DRV_USART_RCV_DMA_CH_TRIG_IDX1,     
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX1?has_content || CONFIG_DRV_USART_RCV_DMA_CH_IDX1?has_content>
    .interruptDMA = DRV_USART_DMA_INT_SRC_IDX1,  
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
<#if CONFIG_DRV_USART_BRG_CLOCK_IDX2?has_content>
    .brgClock = DRV_USART_BRG_CLOCK_IDX2,
</#if>
<#if CONFIG_DRV_USART_LINE_CNTRL_IDX2?has_content>
    .lineControl = DRV_USART_LINE_CNTRL_IDX2,
</#if>
<#if CONFIG_DRV_USART_BAUD_RATE_IDX2?has_content>
    .baud = DRV_USART_BAUD_RATE_IDX2,
</#if>
<#if CONFIG_DRV_USART_INT_SRC_IDX2?has_content>
    .interruptUSART = DRV_USART_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_USART_XMIT_QUEUE_SIZE_IDX2?has_content>
    .queueSizeTransmit = DRV_USART_XMIT_QUEUE_SIZE_IDX2,
</#if>
<#if CONFIG_DRV_USART_RCV_QUEUE_SIZE_IDX2?has_content>
    .queueSizeReceive = DRV_USART_RCV_QUEUE_SIZE_IDX2,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX2?has_content>
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX2,
    .dmaChannelTransmitTrigger = DRV_USART_XMIT_DMA_CH_TRIG_IDX2,    
<#else>
    .dmaChannelTransmit = DMA_CHANNEL_NONE,
</#if>
<#if CONFIG_DRV_USART_RCV_DMA_CH_IDX2?has_content>
    .dmaChannelReceive = DRV_USART_RCV_DMA_CH_IDX2,
    .dmaChannelReceiveTrigger = DRV_USART_RCV_DMA_CH_TRIG_IDX2,    
<#else>
    .dmaChannelReceive = DMA_CHANNEL_NONE,
</#if>
<#if CONFIG_DRV_USART_XMIT_DMA_CH_IDX2?has_content || CONFIG_DRV_USART_RCV_DMA_CH_IDX2?has_content>
    .interruptDMA = DRV_USART_DMA_INT_SRC_IDX2,  
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

