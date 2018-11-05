<#--
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
// <editor-fold defaultstate="collapsed" desc="DRV_Timer Initialization Data">
<#if CONFIG_USE_DRV_TMR == true>
<#if CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC">
/*** TMR Driver Initialization Data ***/

<#-- Instance 0 -->
<#if CONFIG_DRV_TMR_INST_0 == true>
const DRV_TMR_INIT drvTmr0InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX0?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX0,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX0?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX0,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX0?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX0,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX0?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX0,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX0?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX0,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX0?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX0,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX0?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX0,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX0 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 1 -->
<#if CONFIG_DRV_TMR_INST_1 == true>
const DRV_TMR_INIT drvTmr1InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX1?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX1,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX1?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX1,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX1?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX1,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX1?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX1,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX1?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX1,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX1?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX1,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX1?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX1,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX1 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 2 -->
<#if CONFIG_DRV_TMR_INST_2 == true>
const DRV_TMR_INIT drvTmr2InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX2?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX2,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX2?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX2,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX2?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX2,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX2?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX2,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX2?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX2,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX2?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX2,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX2?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX2,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX2 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 3 -->
<#if CONFIG_DRV_TMR_INST_3 == true>
const DRV_TMR_INIT drvTmr3InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX3?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX3,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX3?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX3,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX3?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX3,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX3?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX3,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX3?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX3,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX3?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX3,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX3?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX3,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX3 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 4 -->
<#if CONFIG_DRV_TMR_INST_4 == true>
const DRV_TMR_INIT drvTmr4InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX4?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX4,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX4?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX4,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX4?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX4,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX4?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX4,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX4?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX4,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX4?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX4,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX4?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX4,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX4 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 5 -->
<#if CONFIG_DRV_TMR_INST_5 == true>
const DRV_TMR_INIT drvTmr5InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX5?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX5,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX5?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX5,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX5?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX5,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX5?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX5,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX5?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX5,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX5?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX5,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX5?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX5,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX5 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 6 -->
<#if CONFIG_DRV_TMR_INST_6 == true>
const DRV_TMR_INIT drvTmr6InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX6?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX6,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX6?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX6,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX6?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX6,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX6?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX6,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX6?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX6,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX6?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX6,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX6?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX6,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX6 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 7 -->
<#if CONFIG_DRV_TMR_INST_7 == true>
const DRV_TMR_INIT drvTmr7InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX7?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX7,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX7?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX7,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX7?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX7,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX7?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX7,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX7?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX7,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX7?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX7,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX7?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX7,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX7 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#-- Instance 8 -->
<#if CONFIG_DRV_TMR_INST_8 == true>
const DRV_TMR_INIT drvTmr8InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX8?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX8,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX8?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX8,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX8?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX8,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX8?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX8,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX8?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX8,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX8?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX8,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX8?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX8,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX8 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#if CONFIG_ARCH_ARM == true>
<#-- Instance 9 -->
<#if CONFIG_DRV_TMR_INST_9 == true>
const DRV_TMR_INIT drvTmr9InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX9?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX9,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX9?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX9,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX9?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX9,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX9?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX9,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX9?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX9,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX9?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX9,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX9?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX9,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX9 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#if CONFIG_DRV_TMR_INST_10 == true>
<#-- Instance 10 -->
const DRV_TMR_INIT drvTmr10InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX10?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX10,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX10?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX10,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX10?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX10,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX10?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX10,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX10?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX10,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX10?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX10,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX10?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX10,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX10 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
<#if CONFIG_DRV_TMR_INST_11 == true>
<#-- Instance 11 -->
const DRV_TMR_INIT drvTmr11InitData =
{
<#if CONFIG_DRV_TMR_POWER_STATE_IDX11?has_content>
    .moduleInit.sys.powerState = DRV_TMR_POWER_STATE_IDX11,
</#if>
<#if CONFIG_DRV_TMR_PERIPHERAL_ID_IDX11?has_content>
    .tmrId = DRV_TMR_PERIPHERAL_ID_IDX11,
</#if>
<#if CONFIG_DRV_TMR_CLOCK_SOURCE_IDX11?has_content>
    .clockSource = DRV_TMR_CLOCK_SOURCE_IDX11,
</#if>
<#if CONFIG_DRV_TMR_PRESCALE_IDX11?has_content>
    .prescale = DRV_TMR_PRESCALE_IDX11,
</#if>
<#if CONFIG_DRV_TMR_OPERATION_MODE_IDX11?has_content>
    .mode = DRV_TMR_OPERATION_MODE_IDX11,
<#else>
    .mode = DRV_TMR_OPERATION_MODE_16_BIT,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_SOURCE_IDX11?has_content == true>
    .interruptSource = DRV_TMR_INTERRUPT_SOURCE_IDX11,
</#if>
<#if CONFIG_DRV_TMR_INTERRUPT_MODE_IDX11?has_content>
    .mode = DRV_TMR_INTERRUPT_MODE_IDX11,
</#if>
<#if CONFIG_DRV_TMR_ASYNC_WRITE_ENABLE_IDX11 == true>
    .asyncWriteEnable = true,
<#else>
    .asyncWriteEnable = false,
</#if>
};
</#if>
</#if>
</#if>
</#if>
// </editor-fold>
<#--
/*******************************************************************************
 End of File
*/
-->
