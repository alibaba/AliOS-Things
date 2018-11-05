<#--
/*******************************************************************************
  System Console Initialization File

  File Name:
    sys_console_init.c

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
<#if CONFIG_SYS_CONSOLE_MODE == "STATIC">
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, NULL);
</#if>
<#if CONFIG_SYS_CONSOLE_MODE == "DYNAMIC">
<#if CONFIG_SYS_CONSOLE_INST_IDX0 == true>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "APPIO_CONSOLE">
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&consAppIOInit0);
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "UART_CONSOLE">
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&consUsartInit0);
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "USB_CDC_CONSOLE">
    sysObj.sysConsole0 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&consUsbInit0);
</#if></#if>
<#if CONFIG_SYS_CONSOLE_INST_IDX1 == true>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "APPIO_CONSOLE">
    sysObj.sysConsole1 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_1, (SYS_MODULE_INIT *)&consAppIOInit1);
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "UART_CONSOLE">
    sysObj.sysConsole1 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_1, (SYS_MODULE_INIT *)&consUsartInit1);
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "USB_CDC_CONSOLE">
    sysObj.sysConsole1 = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_1, (SYS_MODULE_INIT *)&consUsbInit1);
</#if></#if></#if>

<#--
/*******************************************************************************
 End of File
*/
-->
