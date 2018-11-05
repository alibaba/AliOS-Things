<#--
/*******************************************************************************
  USART Driver Initialization File

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
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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
<#if CONFIG_DRV_USART_DRIVER_MODE = "DYNAMIC">
<#if CONFIG_DRV_USART_INST_IDX0 == true>
    sysObj.drvUsart0 = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)&drvUsart0InitData);
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
    sysObj.drvUsart1 = DRV_USART_Initialize(DRV_USART_INDEX_1, (SYS_MODULE_INIT *)&drvUsart1InitData);
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
    sysObj.drvUsart2 = DRV_USART_Initialize(DRV_USART_INDEX_2, (SYS_MODULE_INIT *)&drvUsart2InitData);
</#if>
</#if>
<#if CONFIG_DRV_USART_DRIVER_MODE = "STATIC">
<#if CONFIG_DRV_USART_INST_IDX0 == true>
    sysObj.drvUsart0 = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)NULL);
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
    sysObj.drvUsart1 = DRV_USART_Initialize(DRV_USART_INDEX_1, (SYS_MODULE_INIT *)NULL);
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
    sysObj.drvUsart2 = DRV_USART_Initialize(DRV_USART_INDEX_2, (SYS_MODULE_INIT *)NULL);
</#if>
</#if>
<#if CONFIG_USE_SYS_INT == true && CONFIG_DRV_USART_INTERRUPT_MODE == true>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_USART_INT_SRC_IDX0}, ${CONFIG_DRV_USART_INT_PRIORITY_IDX0});
</#if>
<#if CONFIG_DRV_USART_INST_IDX1 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_USART_INT_SRC_IDX1}, ${CONFIG_DRV_USART_INT_PRIORITY_IDX1});
</#if>
<#if CONFIG_DRV_USART_INST_IDX2 == true>
    SYS_INT_VectorPrioritySet(${CONFIG_DRV_USART_INT_SRC_IDX2}, ${CONFIG_DRV_USART_INT_PRIORITY_IDX2});
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
