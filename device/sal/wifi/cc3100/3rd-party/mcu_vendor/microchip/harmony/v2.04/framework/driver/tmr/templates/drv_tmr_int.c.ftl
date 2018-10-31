<#--
/*******************************************************************************
  Timer Driver Interrupt Handler Template File

  File Name:
    drv_tmr_int.c

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

<#macro DRV_TMR_C_INT DRV_TMR_INSTANCE DRV_TMR_ALARM_FUNCS>
    <#if DRV_TMR_INSTANCE == "0">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX0>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX0>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX0>
    </#if>
    <#if DRV_TMR_INSTANCE == "1">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX1>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX1>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX1>
    </#if>
    <#if DRV_TMR_INSTANCE == "2">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX2>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX2>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX2>
    </#if>
    <#if DRV_TMR_INSTANCE == "3">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX3>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX3>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX3>
    </#if>
    <#if DRV_TMR_INSTANCE == "4">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX4>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX4>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX4>
    </#if>
    <#if DRV_TMR_INSTANCE == "5">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX5>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX5>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX5>
    </#if>
    <#if DRV_TMR_INSTANCE == "6">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX6>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX6>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX6>
    </#if>
    <#if DRV_TMR_INSTANCE == "7">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX7>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX7>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX7>
    </#if>
    <#if DRV_TMR_INSTANCE == "8">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX8>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX8>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX8>
    </#if>
        <#if DRV_TMR_INSTANCE == "9">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX9>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX9>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX9>
    </#if>
	    <#if DRV_TMR_INSTANCE == "10">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX10>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX10>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX10>
    </#if>
	    <#if DRV_TMR_INSTANCE == "11">
        <#assign TMR_INTPRIO = CONFIG_DRV_TMR_INT_PRIO_NUM_IDX11>
        <#assign TMR_VECTOR = CONFIG_DRV_TMR_ISR_VECTOR_IDX11>
        <#assign TMR_STATIC_INT_SRC = CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX11>
    </#if>
    <#if CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC" || CONFIG_DRV_TMR_INTERRUPT_MODE == true>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                <#lt>void __ISR(${TMR_VECTOR}, ipl${TMR_INTPRIO}SOFT) IntHandlerDrvTmrInstance${DRV_TMR_INSTANCE}(void)
            <#else>
                <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
                    <#lt>void __attribute__( (interrupt(ipl${TMR_INTPRIO}AUTO), vector(${TMR_VECTOR}))) IntHandlerDrvTmrInstance${DRV_TMR_INSTANCE}_ISR( void );
                </#if>
				<#if CONFIG_ARCH_ARM == false>
                <#lt>void IntHandlerDrvTmrInstance${DRV_TMR_INSTANCE}(void)
				<#else>
				<#lt>void ${TMR_VECTOR}(void)
				</#if>
            </#if>
        <#else>
          <#if CONFIG_ARCH_ARM == false>
            <#lt>void __ISR(${TMR_VECTOR}, ipl${TMR_INTPRIO}AUTO) IntHandlerDrvTmrInstance${DRV_TMR_INSTANCE}(void)
          <#else>
            <#lt>void ${TMR_VECTOR}(void)
          </#if>
        </#if>
        <#lt>{
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                <#lt>   /* Call ThreadX context save.  */
                <#lt>   _tx_thread_context_save();
            </#if>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
                <#lt>    OS_EnterNestableInterrupt();
            </#if>
        </#if>
        <#if CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC">
            <#lt>    DRV_TMR_Tasks(sysObj.drvTmr${DRV_TMR_INSTANCE});
        <#else>
		<#if CONFIG_ARCH_ARM == false>
            <#lt>    PLIB_INT_SourceFlagClear(INT_ID_0,${TMR_STATIC_INT_SRC});
			</#if>
            <#if DRV_TMR_ALARM_FUNCS == true>
                <#lt>    DRV_TMR${DRV_TMR_INSTANCE}_Tasks();
            </#if>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                <#lt>   /* Call ThreadX context restore.  */
                <#lt>   _tx_thread_context_restore();
            </#if>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
                <#lt>    OS_LeaveNestableInterrupt();
            </#if>
        </#if>
        <#lt>}
    </#if>
</#macro>

<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
    <#if CONFIG_DRV_TMR_INST_0 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="0" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX0/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_1 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="1" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX1/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_2 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="2" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX2/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_3 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="3" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX3/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_4 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="4" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX4/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_5 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="5" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX5/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_6 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="6" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX6/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_7 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="7" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX7/>
    </#if>
    <#if CONFIG_DRV_TMR_INST_8 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="8" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX8/>
    </#if>
	<#if CONFIG_ARCH_ARM == true>
	<#if CONFIG_DRV_TMR_INST_9 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="9" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX9/>
    </#if>
	<#if CONFIG_DRV_TMR_INST_10 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="10" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX10/>
    </#if>
	<#if CONFIG_DRV_TMR_INST_11 == true>
        <@DRV_TMR_C_INT DRV_TMR_INSTANCE="11" DRV_TMR_ALARM_FUNCS=CONFIG_DRV_TMR_ALARM_FUNCS_IDX11/>
    </#if>
	</#if>
</#if> <#--CONFIG_DRV_TMR_INTERRUPT_MODE == true-->

<#--
/*******************************************************************************
 End of File
*/
-->
