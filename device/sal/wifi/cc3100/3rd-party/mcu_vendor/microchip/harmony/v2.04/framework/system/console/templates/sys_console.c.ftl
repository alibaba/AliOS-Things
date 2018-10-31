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
// <editor-fold defaultstate="collapsed" desc="SYS_CONSOLE Initialization Data">
<#if CONFIG_SYS_CONSOLE_MODE == "DYNAMIC">
/*** System Console Initialization Data ***/

SYS_MODULE_OBJ sysConsoleObjects[] = { SYS_MODULE_OBJ_INVALID };

<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "APPIO_CONSOLE" || CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "APPIO_CONSOLE">
/* Declared in console device implementation (sys_console_appio.c) */
extern SYS_CONSOLE_DEV_DESC consAppIODevDesc;

<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "APPIO_CONSOLE">
SYS_CONSOLE_INIT consAppIOInit0 =
{
    .moduleInit = {0},
    .consDevDesc = &consAppIODevDesc,
};
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "APPIO_CONSOLE">
SYS_CONSOLE_INIT consAppIOInit1 =
{
    .moduleInit = {0},
    .consDevDesc = &consAppIODevDesc,
};
</#if>
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "UART_CONSOLE" || CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "UART_CONSOLE">
/* Declared in console device implementation (sys_console_uart.c) */
extern SYS_CONSOLE_DEV_DESC consUsartDevDesc;
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">

OS_TCB  _SYS_CONSOLE_IDX1_Tasks_TCB;
CPU_STK _SYS_CONSOLE_IDX1_TasksStk[${CONFIG_SYS_CONSOLE_IDX1_RTOS_TASK_SIZE}];

</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "UART_CONSOLE">
SYS_CONSOLE_INIT consUsartInit0 =
{
    .moduleInit = {0},
    .consDevDesc = &consUsartDevDesc,
};
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "UART_CONSOLE">
SYS_CONSOLE_INIT consUsartInit1 =
{
    .moduleInit = {0},
    .consDevDesc = &consUsartDevDesc,
};
</#if>
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "USB_CDC_CONSOLE" || CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "USB_CDC_CONSOLE">
/* Declared in console device implementation (sys_console_usb_cdc.c) */
extern SYS_CONSOLE_DEV_DESC consUsbCdcDevDesc;

<#if CONFIG_SYS_CONSOLE_SOURCE_IDX0 == "USB_CDC_CONSOLE">
SYS_CONSOLE_INIT consUsbInit0 =
{
    .moduleInit = {0},
    .consDevDesc = &consUsbCdcDevDesc,
};
</#if>
<#if CONFIG_SYS_CONSOLE_SOURCE_IDX1 == "USB_CDC_CONSOLE">
SYS_CONSOLE_INIT consUsbInit1 =
{
    .moduleInit = {0},
    .consDevDesc = &consUsbCdcDevDesc,
};
</#if>
</#if></#if>
// </editor-fold>
<#--
/*******************************************************************************
 End of File
*/
-->
