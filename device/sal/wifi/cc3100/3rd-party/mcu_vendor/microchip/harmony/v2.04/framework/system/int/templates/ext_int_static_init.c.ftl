<#--
/*******************************************************************************
  EXT Int Initialization Call 

  Company:
    Microchip Technology Inc.

  File Name:
    ext_int_static_init.c.ftl

  Summary:


  Description:


  Remarks:
    None.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END
-->

<#if CONFIG_EXT_INT_INST_IDX0 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX0 == true>
    /*Setup the ${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX0} and Enable it*/
    SYS_INT_VectorPrioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX0}, ${CONFIG_EXT_INT_PRIORITY_IDX0});
    SYS_INT_VectorSubprioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX0}, ${CONFIG_EXT_INT_SUB_PRIORITY_IDX0});
    SYS_INT_ExternalInterruptTriggerSet(${CONFIG_EXT_INT_PERIPHERAL_ID_IDX0},${CONFIG_EXT_INT_POLARITY_IDX0});
<#if CONFIG_EXT_INT_ENABLE_IDX0 == true>
    SYS_INT_SourceEnable(${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX0});
</#if>
</#if>

<#if CONFIG_EXT_INT_INST_IDX1 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX1 == true>
    /*Setup the ${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX1} and Enable it*/
    SYS_INT_VectorPrioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX1}, ${CONFIG_EXT_INT_PRIORITY_IDX1});
    SYS_INT_VectorSubprioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX1}, ${CONFIG_EXT_INT_SUB_PRIORITY_IDX1});
    SYS_INT_ExternalInterruptTriggerSet(${CONFIG_EXT_INT_PERIPHERAL_ID_IDX1},${CONFIG_EXT_INT_POLARITY_IDX1});
<#if CONFIG_EXT_INT_ENABLE_IDX1 == true>
    SYS_INT_SourceEnable(${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX1});
</#if>
</#if>

<#if CONFIG_EXT_INT_INST_IDX2 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX2 == true>
    /*Setup the ${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX2} and Enable it*/
    SYS_INT_VectorPrioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX2}, ${CONFIG_EXT_INT_PRIORITY_IDX2});
    SYS_INT_VectorSubprioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX2}, ${CONFIG_EXT_INT_SUB_PRIORITY_IDX2});
    SYS_INT_ExternalInterruptTriggerSet(${CONFIG_EXT_INT_PERIPHERAL_ID_IDX2},${CONFIG_EXT_INT_POLARITY_IDX2});
<#if CONFIG_EXT_INT_ENABLE_IDX2 == true>
    SYS_INT_SourceEnable(${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX2});
</#if>
</#if>

<#if CONFIG_EXT_INT_INST_IDX3 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX3 == true>
    /*Setup the ${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX3} and Enable it*/
    SYS_INT_VectorPrioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX3}, ${CONFIG_EXT_INT_PRIORITY_IDX3});
    SYS_INT_VectorSubprioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX3}, ${CONFIG_EXT_INT_SUB_PRIORITY_IDX3});
    SYS_INT_ExternalInterruptTriggerSet(${CONFIG_EXT_INT_PERIPHERAL_ID_IDX3},${CONFIG_EXT_INT_POLARITY_IDX3});
<#if CONFIG_EXT_INT_ENABLE_IDX3 == true>
    SYS_INT_SourceEnable(${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX3});
</#if>
</#if>

<#if CONFIG_EXT_INT_INST_IDX4 == true && CONFIG_EXT_INT_GENERATE_CODE_IDX4 == true>
    /*Setup the ${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX4} and Enable it*/
    SYS_INT_VectorPrioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX4}, ${CONFIG_EXT_INT_PRIORITY_IDX4});
    SYS_INT_VectorSubprioritySet(${CONFIG_EXT_INT_INTERRUPT_VECTOR_IDX4}, ${CONFIG_EXT_INT_SUB_PRIORITY_IDX4});
    SYS_INT_ExternalInterruptTriggerSet(${CONFIG_EXT_INT_PERIPHERAL_ID_IDX4},${CONFIG_EXT_INT_POLARITY_IDX4});
<#if CONFIG_EXT_INT_ENABLE_IDX4 == true>
    SYS_INT_SourceEnable(${CONFIG_EXT_INT_INTERRUPT_SOURCE_IDX4});
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
