<#--
/*******************************************************************************
  tmr interrupt Template File

  File Name:
    drv_tmr_int.s

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
<#macro DRV_TMR_ASM_INT DRV_TMR_INSTANCE DRV_TMR_ID TMR_ISR_INT_PRIORITY>
<#if DRV_TMR_ID == "_TIMER_1_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_1>
</#if>
<#if DRV_TMR_ID == "_TIMER_2_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_2>
</#if>
<#if DRV_TMR_ID == "_TIMER_3_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_3>
</#if>
<#if DRV_TMR_ID == "_TIMER_4_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_4>
</#if>
<#if DRV_TMR_ID == "_TIMER_5_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_5>
</#if>
<#if DRV_TMR_ID == "_TIMER_6_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_6>
</#if>
<#if DRV_TMR_ID == "_TIMER_7_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_7>
</#if>
<#if DRV_TMR_ID == "_TIMER_8_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_8>
</#if>
<#if DRV_TMR_ID == "_TIMER_9_VECTOR">
<#assign TMR_ISR_VECTOR = CONFIG_INT_VECT_TIMER_9>
</#if>
<#if DRV_TMR_INSTANCE == "0">
<#assign TMR_ISR_NAME = "DrvTmrInstance0">
</#if>
<#if DRV_TMR_INSTANCE == "1">
<#assign TMR_ISR_NAME = "DrvTmrInstance1">
</#if>
<#if DRV_TMR_INSTANCE == "2">
<#assign TMR_ISR_NAME = "DrvTmrInstance2">
</#if>
<#if DRV_TMR_INSTANCE == "3">
<#assign TMR_ISR_NAME = "DrvTmrInstance3">
</#if>
<#if DRV_TMR_INSTANCE == "4">
<#assign TMR_ISR_NAME = "DrvTmrInstance4">
</#if>
<#if DRV_TMR_INSTANCE == "5">
<#assign TMR_ISR_NAME = "DrvTmrInstance5">
</#if>
<#if DRV_TMR_INSTANCE == "6">
<#assign TMR_ISR_NAME = "DrvTmrInstance6">
</#if>
<#if DRV_TMR_INSTANCE == "7">
<#assign TMR_ISR_NAME = "DrvTmrInstance7">
</#if>
<#if DRV_TMR_INSTANCE == "8">
<#assign TMR_ISR_NAME = "DrvTmrInstance8">
</#if>
/* TMR Instance ${DRV_TMR_INSTANCE} Interrupt */
<@RTOS_ISR VECTOR = TMR_ISR_VECTOR NAME = TMR_ISR_NAME PRIORITY = TMR_ISR_INT_PRIORITY/>
</#macro>

<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
<#if CONFIG_DRV_TMR_INST_0 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="0" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX0 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX0/>
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="1" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX1 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX1/>
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="2" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX2 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX2 />
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="3" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX3 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX3 />
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="4" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX4 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX4 />
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="5" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX5 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX5 />
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="6" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX6 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX6 />
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="7" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX7 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX7 />
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
<@DRV_TMR_ASM_INT DRV_TMR_INSTANCE="8" DRV_TMR_ID=CONFIG_DRV_TMR_ISR_VECTOR_IDX8 TMR_ISR_INT_PRIORITY=CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX8 />
</#if>
</#if>

