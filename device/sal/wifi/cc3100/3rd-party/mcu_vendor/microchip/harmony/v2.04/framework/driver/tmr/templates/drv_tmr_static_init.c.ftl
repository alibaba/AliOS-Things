<#--
/*******************************************************************************
  TMR Driver Initialization call 

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_static_init.c.ftl

  Summary:
    This template is referenced in system_init.c.ftl to call the TMR driver
    initialization routine.

  Description:
    The OC device driver initialization routine sets the Timer functionality.
    
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
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTOCULAR PURPOSE.
IN NO EVENT SHALL MOCROCHIP OR ITS LOCENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STROCT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVOCES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END
-->
<#if CONFIG_USE_DRV_TMR == true>
<#if CONFIG_DRV_TMR_DRIVER_MODE == "STATIC">
<#macro DRV_TMR_STATIC_API DRV_INSTANCE>
    /*Initialize TMR${DRV_INSTANCE} */
    DRV_TMR${DRV_INSTANCE}_Initialize();
</#macro>
<#if CONFIG_DRV_TMR_INST_0 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="0"/>
</#if>
<#if CONFIG_DRV_TMR_INST_1 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="1"/>
</#if>
<#if CONFIG_DRV_TMR_INST_2 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="2"/>
</#if>
<#if CONFIG_DRV_TMR_INST_3 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="3"/>
</#if>
<#if CONFIG_DRV_TMR_INST_4 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="4"/>
</#if>
<#if CONFIG_DRV_TMR_INST_5 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="5"/>
</#if>
<#if CONFIG_DRV_TMR_INST_6 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="6"/>
</#if>
<#if CONFIG_DRV_TMR_INST_7 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="7"/>
</#if>
<#if CONFIG_DRV_TMR_INST_8 == true>
<@DRV_TMR_STATIC_API DRV_INSTANCE="8"/>
</#if>
</#if> <#--CONFIG_DRV_TMR_DRIVER_MODE == "STATIC"-->
</#if> <#--CONFIG_USE_DRV_TMR == true-->
<#--
/*******************************************************************************
 End of File
*/
-->