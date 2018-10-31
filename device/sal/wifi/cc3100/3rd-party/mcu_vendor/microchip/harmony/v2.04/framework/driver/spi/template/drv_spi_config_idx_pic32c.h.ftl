<#--
/*******************************************************************************
  SPI Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_idx0.h.ftl

  Summary:
    SPI Driver Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->
<#assign combinedQueueDepth = 0>
<#assign DRV_SPI_DMA = 0>


<#macro DRV_SPI_CONFIG_H INST_INDEX>
/* SPI Driver Instance ${INST_INDEX} Configuration */
#define DRV_SPI_SPI_ID_IDX${INST_INDEX} 				      ${.vars["CONFIG_DRV_SPI_SPI_ID_IDX${INST_INDEX}"]}
<#if .vars["CONFIG_DRV_SPI_INTERRUPT_MODE"] == true>
#define DRV_SPI_TASK_MODE_IDX${INST_INDEX}                    DRV_SPI_TASK_MODE_ISR
#define DRV_SPI_INT_VECTOR_IDX${INST_INDEX}  			      ${.vars["CONFIG_DRV_SPI_INTERRUPT_VECTOR_IDX${INST_INDEX}"]}
#define DRV_SPI_INT_PRIORITY_IDX${INST_INDEX}			      ${.vars["CONFIG_DRV_SPI_INT_PRIORITY_IDX${INST_INDEX}"]}
<#else>
#define DRV_SPI_TASK_MODE_IDX${INST_INDEX}                    DRV_SPI_TASK_MODE_POLLED
</#if>
#define DRV_SPI_SPI_MODE_IDX${INST_INDEX}				      DRV_SPI_MODE_${.vars["CONFIG_DRV_SPI_SPI_MODE_IDX${INST_INDEX}"]}
#define DRV_SPI_COMM_WIDTH_IDX${INST_INDEX} 			      SPI_COMMUNICATION_WIDTH_${.vars["CONFIG_DRV_SPI_COMM_WIDTH_N_BIT_IDX${INST_INDEX}"]}BITS
#define DRV_SPI_BAUD_CLOCK_IDX${INST_INDEX}         	      ${.vars["CONFIG_DRV_SPI_CLOCK_SOURCE_IDX${INST_INDEX}"]}
#define DRV_SPI_BAUD_RATE_IDX${INST_INDEX} 				      ${.vars["CONFIG_DRV_SPI_BAUD_RATE_IDX${INST_INDEX}"]}
#define DRV_SPI_CLOCK_MODE_IDX${INST_INDEX} 			      ${.vars["CONFIG_DRV_SPI_CLOCK_MODE_IDX${INST_INDEX}"]}
#define DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX${INST_INDEX}    ${.vars["CONFIG_DRV_SPI_TRANSMIT_DUMMY_BYTE_VALUE_IDX${INST_INDEX}"]}
#define DRV_SPI_QUEUE_SIZE_IDX${INST_INDEX} 			      ${.vars["CONFIG_DRV_SPI_QUEUE_SIZE_IDX${INST_INDEX}"]}

<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${INST_INDEX}"] == true>
    <#assign txIndex=.vars["CONFIG_DRV_SPI_TX_DMA_INST_IDX${INST_INDEX}"]/>
    <#assign rxIndex=.vars["CONFIG_DRV_SPI_RX_DMA_INST_IDX${INST_INDEX}"]/>
    <#if .vars["CONFIG_DRV_SPI_SPI_ID_IDX${INST_INDEX}"] == "SPI_ID_0">
        <#assign trigggerTX="DMA_TRIGGER_SPI_0_TX"/>
        <#assign trigggerRX="DMA_TRIGGER_SPI_0_RX"/>
    <#elseif .vars["CONFIG_DRV_SPI_SPI_ID_IDX${INST_INDEX}"] == "SPI_ID_1">
        <#assign trigggerTX="DMA_TRIGGER_SPI_1_TX"/>
        <#assign trigggerRX="DMA_TRIGGER_SPI_1_RX"/>
    </#if>
#define DRV_SPI_TX_DMA_CHANNEL_IDX${INST_INDEX} 		      ${.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${txIndex}"]}
#define DRV_SPI_TX_DMA_CHANNEL_TRIG_IDX${INST_INDEX} 		  ${trigggerTX}
#define DRV_SPI_RX_DMA_CHANNEL_IDX${INST_INDEX} 		      ${.vars["CONFIG_SYS_DMA_CHANNEL_ID_IDX${rxIndex}"]}
#define DRV_SPI_RX_DMA_CHANNEL_TRIG_IDX${INST_INDEX} 		  ${trigggerRX}
#define DRV_SPI_DMA_INT_VECTOR_IDX${INST_INDEX}  			  XDMAC_IRQn
</#if>
</#macro>


<#macro DRV_SPI_CONFIG_H_QUEUE_DEPTH INST_INDEX>
<#assign combinedQueueDepth = combinedQueueDepth + .vars["CONFIG_DRV_SPI_QUEUE_SIZE_IDX${INST_INDEX}"]?number>

<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${INST_INDEX}"] == true>
<#assign DRV_SPI_DMA = 1>
</#if>

</#macro>

<#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_CONFIG_H "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_CONFIG_H "1"/>
</#if>
</#if>

<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_CONFIG_H_QUEUE_DEPTH "0"/>
</#if>
<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_CONFIG_H_QUEUE_DEPTH "1"/>
</#if>
#define DRV_SPI_QUEUE_DEPTH_COMBINED              ${combinedQueueDepth}
#define DRV_SPI_DMA                               ${DRV_SPI_DMA}
<#if CONFIG_DRV_SPI_DRIVER_MODE == "DYNAMIC">
<#if CONFIG_DRV_SPI_USE_DMA>
    <#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX0"] == true && .vars["CONFIG_DRV_SPI_USE_DMA_IDX1"] == true>
        <#if ((.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX0"]?number)>=(.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX1"]?number))>
            <#assign dummyBufferSize=.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX0"]/>
        <#else>
            <#assign dummyBufferSize=.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX1"]/>
        </#if>
    <#elseif .vars["CONFIG_DRV_SPI_USE_DMA_IDX0"] == true && .vars["CONFIG_DRV_SPI_USE_DMA_IDX1"] == false>
        <#assign dummyBufferSize=.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX0"]/>
    <#elseif .vars["CONFIG_DRV_SPI_USE_DMA_IDX0"] == false && .vars["CONFIG_DRV_SPI_USE_DMA_IDX1"] == true>
        <#assign dummyBufferSize=.vars["CONFIG_DRV_SPI_DMA_DUMMY_BUFFER_SIZE_IDX1"]/>
    </#if>
#define DRV_SPI_DMA_DUMMY_BUFFER_SIZE             ${dummyBufferSize}
</#if>
</#if>