<#--
/*******************************************************************************
  Timer Driver Initialization File

  File Name:
    drv_tmr_init.c

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
<#if CONFIG_USE_DRV_TMR == true>
<#if CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC">

<#if CONFIG_DRV_TMR_INST_0 == true>
    sysObj.drvTmr0 = DRV_TMR_Initialize(DRV_TMR_INDEX_0, (SYS_MODULE_INIT *)&drvTmr0InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
    sysObj.drvTmr1 = DRV_TMR_Initialize(DRV_TMR_INDEX_1, (SYS_MODULE_INIT *)&drvTmr1InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
    sysObj.drvTmr2 = DRV_TMR_Initialize(DRV_TMR_INDEX_2, (SYS_MODULE_INIT *)&drvTmr2InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
    sysObj.drvTmr3 = DRV_TMR_Initialize(DRV_TMR_INDEX_3, (SYS_MODULE_INIT *)&drvTmr3InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
    sysObj.drvTmr4 = DRV_TMR_Initialize(DRV_TMR_INDEX_4, (SYS_MODULE_INIT *)&drvTmr4InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
    sysObj.drvTmr5 = DRV_TMR_Initialize(DRV_TMR_INDEX_5, (SYS_MODULE_INIT *)&drvTmr5InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
    sysObj.drvTmr6 = DRV_TMR_Initialize(DRV_TMR_INDEX_6, (SYS_MODULE_INIT *)&drvTmr6InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
    sysObj.drvTmr7 = DRV_TMR_Initialize(DRV_TMR_INDEX_7, (SYS_MODULE_INIT *)&drvTmr7InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
    sysObj.drvTmr8 = DRV_TMR_Initialize(DRV_TMR_INDEX_8, (SYS_MODULE_INIT *)&drvTmr8InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_9 == true>
    sysObj.drvTmr9 = DRV_TMR_Initialize(DRV_TMR_INDEX_9, (SYS_MODULE_INIT *)&drvTmr9InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_10 == true>
    sysObj.drvTmr10 = DRV_TMR_Initialize(DRV_TMR_INDEX_10, (SYS_MODULE_INIT *)&drvTmr10InitData);
</#if>
<#if CONFIG_DRV_TMR_INST_11 == true>
    sysObj.drvTmr11 = DRV_TMR_Initialize(DRV_TMR_INDEX_11, (SYS_MODULE_INIT *)&drvTmr11InitData);
</#if>
<#if CONFIG_USE_SYS_INT == true && CONFIG_DRV_TMR_INTERRUPT_MODE == true>

<#if CONFIG_DRV_TMR_INST_0 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX0}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX0});
	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX0}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX0});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX1}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX1});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX1}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX1});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX2}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX2});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX2}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX2});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX3}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX3});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX3}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX3});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX4}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX4});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX4}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX4});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX5}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX5});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX5}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX5});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX6}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX6});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX6}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX6});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX7}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX7});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX7}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX7});
	</#if>
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX8}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX8});
    	<#if CONFIG_ARCH_ARM == false>
    SYS_INT_VectorSubprioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX8}, ${CONFIG_DRV_TMR_INTERRUPT_SUB_PRIORITY_IDX8});
	</#if>
</#if>
<#if CONFIG_ARCH_ARM == true>
<#if CONFIG_DRV_TMR_INST_9 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX9}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX9});
</#if>
<#if CONFIG_DRV_TMR_INST_10 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX10}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX10});
</#if>
<#if CONFIG_DRV_TMR_INST_11 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_TMR_INTERRUPT_VECTOR_IDX11}, ${CONFIG_DRV_TMR_INTERRUPT_PRIORITY_IDX11});
</#if>
</#if>
</#if> <#--CONFIG_USE_SYS_INT == true && CONFIG_DRV_TMR_INTERRUPT_MODE == tru-->
</#if> <#--CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC"-->
</#if> <#--CONFIG_USE_DRV_TMR == true-->

<#--
/*******************************************************************************
 End of File
*/
-->
