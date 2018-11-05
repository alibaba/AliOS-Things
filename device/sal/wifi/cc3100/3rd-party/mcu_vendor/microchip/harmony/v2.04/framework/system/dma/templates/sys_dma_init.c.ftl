<#--
/*******************************************************************************
  System DMA Service Initialization File

  File Name:
    sys_dma_init.c.ftl

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, configuration bits, and allocates
    any necessary global system resources, such as the systemObjects structure
    that contains the object handles to all the MPLAB Harmony module objects in
    the system.
 *******************************************************************************/

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
<#if CONFIG_USE_SYS_DMA == true>
    sysObj.sysDma = SYS_DMA_Initialize((SYS_MODULE_INIT *)&sysDmaInit);
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH0 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH0}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH0});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH0}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH0});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH1 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH1}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH1});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH1}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH1});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH2 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH2}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH2});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH2}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH2});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH3 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH3}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH3});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH3}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH3});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH4 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH4}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH4});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH4}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH4});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH5 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH5}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH5});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH5}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH5});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH6 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH6}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH6});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH6}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH6});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH7 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH7}, ${CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH7});
    SYS_INT_VectorSubprioritySet(${CONFIG_SYS_DMA_INT_VECTOR_CH7}, ${CONFIG_SYS_DMA_INTERRUPT_SUBPRIORITY_CH7});
</#if>

<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH0 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH0});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH1 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH1});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH2 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH2});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH3 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH3});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH4 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH4});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH5 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH5});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH6 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH6});
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH7 == true>
    SYS_INT_SourceEnable(${CONFIG_SYS_DMA_ISR_SOURCE_CH7});
</#if>

</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
