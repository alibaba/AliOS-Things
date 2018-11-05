<#--
/*******************************************************************************
  Timer Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr.h.ftl

  Summary:
    Timer Driver Freemarker Template File

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
<#if CONFIG_USE_DRV_TMR == true>
/*** Timer Driver Configuration ***/
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
    <#lt>#define DRV_TMR_INTERRUPT_MODE             true
<#else>
    <#lt>#define DRV_TMR_INTERRUPT_MODE             false
</#if>
<#if CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC">
    <#lt>#define DRV_TMR_INSTANCES_NUMBER           ${CONFIG_DRV_TMR_INSTANCES_NUMBER}
    <#lt>#define DRV_TMR_CLIENTS_NUMBER             ${CONFIG_DRV_TMR_CLIENTS_NUMBER}
</#if>

<#-- Instance 0 -->
    <#if CONFIG_DRV_TMR_INST_0 == true>
        <#lt>/*** Timer Driver 0 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX0          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX0       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX0}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX0       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX0}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX0             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX0}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX0     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX0}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX0           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX0}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX0               ${CONFIG_DRV_TMR_PRESCALE_IDX0}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX0				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX0         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX0         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX0}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX0?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX0         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX0}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX0         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX0 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX0     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX0     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX0     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX0            ${CONFIG_DRV_TMR_POWER_STATE_IDX0}

    </#if>
<#-- Instance 1 -->
    <#if CONFIG_DRV_TMR_INST_1 == true>
                <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX1          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX1       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX1}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX1       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX1}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX1             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX1}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX1     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX1}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX1 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX1}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX1           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX1}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX1               ${CONFIG_DRV_TMR_PRESCALE_IDX1}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX1				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX1         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX1         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX1}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX1?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX1         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX1}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX1         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX1 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX1     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX1     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX1     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX1            ${CONFIG_DRV_TMR_POWER_STATE_IDX1}
    </#if>
<#-- Instance 2 -->
<#if CONFIG_DRV_TMR_INST_2 == true>
/*** Timer Driver 2 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX2          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX2       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX2}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX2       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX2}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX2             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX2}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX2     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX2}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX2 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX2}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX2           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX2}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX2               ${CONFIG_DRV_TMR_PRESCALE_IDX2}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX2				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX2         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX2         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX2}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX2?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX2         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX2}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX2         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX2 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX2     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX2     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX2     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX2            ${CONFIG_DRV_TMR_POWER_STATE_IDX2}
</#if>
<#-- Instance 3 -->
<#if CONFIG_DRV_TMR_INST_3 == true>
/*** Timer Driver 3 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX3          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX3       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX3}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX3       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX3}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX3             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX3}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX3     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX3}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX3 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX3}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX3           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX3}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX3               ${CONFIG_DRV_TMR_PRESCALE_IDX3}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX3				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX3         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX3         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX3}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX3?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX3         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX3}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX3         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX3 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX3     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX3     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX3     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX3            ${CONFIG_DRV_TMR_POWER_STATE_IDX3}

</#if>
<#-- Instance 4 -->
<#if CONFIG_DRV_TMR_INST_4 == true>
/*** Timer Driver 4 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX4          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX4       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX4}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX4       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX4}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX4             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX4}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX4     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX4}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX4 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX4}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX4           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX4}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX4               ${CONFIG_DRV_TMR_PRESCALE_IDX4}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX4				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX4         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX4         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX4}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX4?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX4         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX4}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX4         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX4 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX4     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX4     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX4     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX4            ${CONFIG_DRV_TMR_POWER_STATE_IDX4}

</#if>
<#-- Instance 5 -->
<#if CONFIG_DRV_TMR_INST_5 == true>
/*** Timer Driver 5 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX5          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX5       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX5}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX5       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX5}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX5             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX5}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX5     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX5}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX5 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX5}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX5           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX5}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX5               ${CONFIG_DRV_TMR_PRESCALE_IDX5}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX5				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX5         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX5         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX5}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX5?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX5         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX5}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX5         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX5 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX5     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX5     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX5     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX5            ${CONFIG_DRV_TMR_POWER_STATE_IDX5}

</#if>
<#-- Instance 6 -->
<#if CONFIG_DRV_TMR_INST_6 == true>
/*** Timer Driver 6 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX6          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX6       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX6}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX6       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX6}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX6             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX6}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX6     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX6}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX6 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX6}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX6           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX6}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX6               ${CONFIG_DRV_TMR_PRESCALE_IDX6}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX6				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX6         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX6         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX6}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX6?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX6         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX6}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX6         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX6 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX6     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX6     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX6     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX6            ${CONFIG_DRV_TMR_POWER_STATE_IDX6}

</#if>
<#-- Instance 7 -->
<#if CONFIG_DRV_TMR_INST_7 == true>
/*** Timer Driver 7 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX7          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX7       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX7}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX7       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX7}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX7             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX7}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX7     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX7}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX7 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX7}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX7           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX7}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX7               ${CONFIG_DRV_TMR_PRESCALE_IDX7}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX7				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX7         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX7         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX7}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX7?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX7         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX7}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX7         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX7 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX7     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX7     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX7     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX7            ${CONFIG_DRV_TMR_POWER_STATE_IDX7}

</#if>
<#-- Instance 8 -->
<#if CONFIG_DRV_TMR_INST_8 == true>
/*** Timer Driver 8 Configuration ***/
        <#lt>#define DRV_TMR_PERIPHERAL_ID_IDX8          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8}
        <#lt>#define DRV_TMR_INTERRUPT_SOURCE_IDX8       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX8}
        <#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
            <#lt>#define DRV_TMR_INTERRUPT_VECTOR_IDX8       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX8}
            <#lt>#define DRV_TMR_ISR_VECTOR_IDX8             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX8}
            <#lt>#define DRV_TMR_INTERRUPT_PRIORITY_IDX8     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX8}
			<#if CONFIG_ARCH_ARM == false>
            <#lt>#define DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX8 ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX8}
			</#if>
        </#if>
        <#lt>#define DRV_TMR_CLOCK_SOURCE_IDX8           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX8}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX8               ${CONFIG_DRV_TMR_PRESCALE_IDX8}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX8				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 == "TMR_ID_1" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 == "TMR_ID_3" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 == "TMR_ID_5" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 == "TMR_ID_7" ||
        CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8 == "TMR_ID_9">
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX8         DRV_TMR_OPERATION_MODE_16_BIT
        <#else>
            <#lt>#define DRV_TMR_OPERATION_MODE_IDX8         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX8}
        </#if>
		<#else>
			<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX8?has_content>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX8         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX8}
			<#else>
				<#lt>#define DRV_TMR_OPERATION_MODE_IDX8         DRV_TMR_OPERATION_MODE_16_BIT
			</#if>
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX8 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX8     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX8     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX8     false
		</#if>
        <#lt>#define DRV_TMR_POWER_STATE_IDX8            ${CONFIG_DRV_TMR_POWER_STATE_IDX8}
</#if>
<#if CONFIG_ARCH_ARM == true>
<#-- Instance 9 -->
<#if CONFIG_DRV_TMR_INST_9 == true>
/*** Timer Driver 9 Configuration ***/
#define DRV_TMR_PERIPHERAL_ID_IDX9          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX9}
#define DRV_TMR_INTERRUPT_SOURCE_IDX9       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX9}
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
#define DRV_TMR_INTERRUPT_VECTOR_IDX9       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX9}
#define DRV_TMR_ISR_VECTOR_IDX9             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX9}
#define DRV_TMR_INTERRUPT_PRIORITY_IDX9     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX9}
</#if>
#define DRV_TMR_CLOCK_SOURCE_IDX9           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX9}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX9               ${CONFIG_DRV_TMR_PRESCALE_IDX9}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX9				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX9?has_content>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX9         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX9}
		<#else>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX9         DRV_TMR_OPERATION_MODE_16_BIT
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX9 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX9     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX9     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX9     false
		</#if>
#define DRV_TMR_POWER_STATE_IDX9            ${CONFIG_DRV_TMR_POWER_STATE_IDX9}
</#if>
<#-- Instance 10 -->
<#if CONFIG_DRV_TMR_INST_10 == true>
/*** Timer Driver 10 Configuration ***/
#define DRV_TMR_PERIPHERAL_ID_IDX10          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX10}
#define DRV_TMR_INTERRUPT_SOURCE_IDX10       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX10}
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
#define DRV_TMR_INTERRUPT_VECTOR_IDX10       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX10}
#define DRV_TMR_ISR_VECTOR_IDX10             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX10}
#define DRV_TMR_INTERRUPT_PRIORITY_IDX10     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX10}
</#if>
#define DRV_TMR_CLOCK_SOURCE_IDX10           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX10}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX10               ${CONFIG_DRV_TMR_PRESCALE_IDX10}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX10				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX10?has_content>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX10         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX10}
		<#else>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX10         DRV_TMR_OPERATION_MODE_16_BIT
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX10 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX10     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX10     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX10     false
		</#if>
#define DRV_TMR_POWER_STATE_IDX10            ${CONFIG_DRV_TMR_POWER_STATE_IDX10}
</#if>
<#-- Instance 11 -->
<#if CONFIG_DRV_TMR_INST_11 == true>
/*** Timer Driver 11 Configuration ***/
#define DRV_TMR_PERIPHERAL_ID_IDX11          ${CONFIG_DRV_TMR_PERIPHERAL_ID_IDX11}
#define DRV_TMR_INTERRUPT_SOURCE_IDX11       ${CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX11}
<#if CONFIG_DRV_TMR_INTERRUPT_MODE == true>
#define DRV_TMR_INTERRUPT_VECTOR_IDX11       ${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX11}
#define DRV_TMR_ISR_VECTOR_IDX11             ${CONFIG_DRV_TMR_ISR_VECTOR_IDX11}
#define DRV_TMR_INTERRUPT_PRIORITY_IDX11     ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX11}
</#if>
#define DRV_TMR_CLOCK_SOURCE_IDX11           ${CONFIG_DRV_TMR_CLOCK_SOURCE_IDX11}
		<#if CONFIG_ARCH_ARM == false>
        <#lt>#define DRV_TMR_PRESCALE_IDX11               ${CONFIG_DRV_TMR_PRESCALE_IDX11}
		<#else>
		<#lt>#define DRV_TMR_PRESCALE_IDX11				TMR_PRESCALE_VALUE_1
		</#if>
		<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX11?has_content>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX11         ${CONFIG_DRV_TMR_OPERATION_MODE_IDX11}
		<#else>
			<#lt>#define DRV_TMR_OPERATION_MODE_IDX11         DRV_TMR_OPERATION_MODE_16_BIT
		</#if>
		<#if CONFIG_ARCH_ARM == false>
        <#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX11 == true>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX11     true
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX11     false
        </#if>
        <#else>
            <#lt>#define DRV_TMR_ASYNC_WRITE_ENABLE_IDX11     false
		</#if>
#define DRV_TMR_POWER_STATE_IDX11            ${CONFIG_DRV_TMR_POWER_STATE_IDX11}
</#if>
</#if>
</#if> <#--CONFIG_USE_DRV_TMR == true-->
<#--
/*******************************************************************************
 End of File
*/
-->

