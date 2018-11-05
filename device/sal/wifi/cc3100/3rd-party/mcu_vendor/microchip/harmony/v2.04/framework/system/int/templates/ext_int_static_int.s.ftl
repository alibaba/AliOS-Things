<#--
/*******************************************************************************
  External Interrupt Handler Template File

  File Name:
    ext_int_static_int.s.ftl

  Summary:


  Description:
    It generates code that is added to system_interrupt.c in order to handle
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

<#macro SYS_INT_ASM_INT SYS_INT_INSTANCE SYS_INT_ID EXT_INT_ISR_PRIO>
<#assign EXT_INT_ISR_PRIO = " ">
<#if SYS_INT_ID == "INT_EXTERNAL_INT_SOURCE0">
<#assign EXT_INT_ISR_VECTOR = CONFIG_INT_VECT_EXTERNAL_0>
</#if>
<#if SYS_INT_ID == "INT_EXTERNAL_INT_SOURCE1">
<#assign EXT_INT_ISR_VECTOR = CONFIG_INT_VECT_EXTERNAL_1>
</#if>
<#if SYS_INT_ID == "INT_EXTERNAL_INT_SOURCE2">
<#assign EXT_INT_ISR_VECTOR = CONFIG_INT_VECT_EXTERNAL_2>
</#if>
<#if SYS_INT_ID == "INT_EXTERNAL_INT_SOURCE3">
<#assign EXT_INT_ISR_VECTOR = CONFIG_INT_VECT_EXTERNAL_3>
</#if>
<#if SYS_INT_ID == "INT_EXTERNAL_INT_SOURCE4">
<#assign EXT_INT_ISR_VECTOR = CONFIG_INT_VECT_EXTERNAL_4>
</#if>
<#if SYS_INT_INSTANCE == "0">
<#assign EXT_INT_ISR_NAME = "ExternalInterruptInstance0">
</#if>
<#if SYS_INT_INSTANCE == "1">
<#assign EXT_INT_ISR_NAME = "ExternalInterruptInstance1">
</#if>
<#if SYS_INT_INSTANCE == "2">
<#assign EXT_INT_ISR_NAME = "ExternalInterruptInstance2">
</#if>
<#if SYS_INT_INSTANCE == "3">
<#assign EXT_INT_ISR_NAME = "ExternalInterruptInstance3">
</#if>
<#if SYS_INT_INSTANCE == "4">
<#assign EXT_INT_ISR_NAME = "ExternalInterruptInstance4">
</#if>
/* External Interrupt Instance ${SYS_INT_INSTANCE} Interrupt */
<@RTOS_ISR VECTOR = EXT_INT_ISR_VECTOR NAME = EXT_INT_ISR_NAME PRIORITY = EXT_INT_ISR_PRIO/>
</#macro>

<#if CONFIG_EXT_INT_INST_IDX0 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX0 == true>
<@SYS_INT_ASM_INT SYS_INT_INSTANCE="0" SYS_INT_ID=CONFIG_EXT_INT_PERIPHERAL_ID_IDX0 EXT_INT_ISR_PRIO=CONFIG_EXT_INT_PRIORITY_IDX0/>
</#if>
<#if CONFIG_EXT_INT_INST_IDX1 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX1 == true>
<@SYS_INT_ASM_INT SYS_INT_INSTANCE="1" SYS_INT_ID=CONFIG_EXT_INT_PERIPHERAL_ID_IDX1 EXT_INT_ISR_PRIO=CONFIG_EXT_INT_PRIORITY_IDX1/>
</#if>
<#if CONFIG_EXT_INT_INST_IDX2 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX2 == true>
<@SYS_INT_ASM_INT SYS_INT_INSTANCE="2" SYS_INT_ID=CONFIG_EXT_INT_PERIPHERAL_ID_IDX2 EXT_INT_ISR_PRIO=CONFIG_EXT_INT_PRIORITY_IDX2/>
</#if>
<#if CONFIG_EXT_INT_INST_IDX3 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX3 == true>
<@SYS_INT_ASM_INT SYS_INT_INSTANCE="3" SYS_INT_ID=CONFIG_EXT_INT_PERIPHERAL_ID_IDX3 EXT_INT_ISR_PRIO=CONFIG_EXT_INT_PRIORITY_IDX3/>
</#if>
<#if CONFIG_EXT_INT_INST_IDX4 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX4 == true>
<@SYS_INT_ASM_INT SYS_INT_INSTANCE="4" SYS_INT_ID=CONFIG_EXT_INT_PERIPHERAL_ID_IDX4 EXT_INT_ISR_PRIO=CONFIG_EXT_INT_PRIORITY_IDX4/>
</#if>
