<#--
/*******************************************************************************
  System DMA Service Interrupt Handler Template File

  File Name:
    sys_dma_int.s

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to run the system.  It
	generates code that is added to system_interrupt.c in order to handle
	all interrupts.
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
<#macro SYS_DMA_ASM_INT DMA_INSTANCE DMA_CHANNEL DMA_INTPRIORITY>
<#if CONFIG_PIC32MZ == true>
<#if DMA_CHANNEL == "DMA_CHANNEL_0">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA0>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_1">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA1>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_2">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA2>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_3">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA3>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_4">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA4>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_5">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA5>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_6">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA6>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_7">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA7>
</#if>
<#else>
<#if DMA_CHANNEL == "DMA_CHANNEL_0">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_0>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_1">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_1>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_2">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_2>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_3">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_3>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_4">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_4>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_5">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_5>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_6">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_6>
</#if>
<#if DMA_CHANNEL == "DMA_CHANNEL_7">
<#assign DMA_ISR_VECTOR = CONFIG_INT_VECT_DMA_7>
</#if>
</#if>
<#if DMA_INSTANCE == "0">
<#assign DMA_ISR_NAME = "SysDmaInstance0">
</#if>
<#if DMA_INSTANCE == "1">
<#assign DMA_ISR_NAME = "SysDmaInstance1">
</#if>
<#if DMA_INSTANCE == "2">
<#assign DMA_ISR_NAME = "SysDmaInstance2">
</#if>
<#if DMA_INSTANCE == "3">
<#assign DMA_ISR_NAME = "SysDmaInstance3">
</#if>
<#if DMA_INSTANCE == "4">
<#assign DMA_ISR_NAME = "SysDmaInstance4">
</#if>
<#if DMA_INSTANCE == "5">
<#assign DMA_ISR_NAME = "SysDmaInstance5">
</#if>
<#if DMA_INSTANCE == "6">
<#assign DMA_ISR_NAME = "SysDmaInstance6">
</#if>
<#if DMA_INSTANCE == "7">
<#assign DMA_ISR_NAME = "SysDmaInstance7">
</#if>
/* Sytem DMA Instance ${DMA_INSTANCE} Interrupt */
<@RTOS_ISR VECTOR = DMA_ISR_VECTOR NAME = DMA_ISR_NAME PRIORITY = DMA_INTPRIORITY/>
</#macro>

<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH0 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="0" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX0 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH0/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH1 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="1" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX1 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH1/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH2 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="2" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX2 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH2/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH3 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="3" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX3 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH3/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH4 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="4" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX4 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH4/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH5 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="5" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX5 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH5/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH6 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="6" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX6 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH6/>
</#if>
<#if CONFIG_SYS_DMA_INTERRUPT_MODE_CH7 == true>
<@SYS_DMA_ASM_INT DMA_INSTANCE="7" DMA_CHANNEL=CONFIG_SYS_DMA_CHANNEL_ID_IDX7 DMA_INTPRIORITY=CONFIG_SYS_DMA_INTERRUPT_PRIORITY_CH7/>
</#if>
