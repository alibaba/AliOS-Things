<#--
/*******************************************************************************
  ADC Driver Interrupt Handler Template File

  File Name:
    sys_ports_int.s

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
<#macro SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME>
<#if PORT_NAME == "A">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_A>
<#assign CN_ISR_NAME = "ChangeNotification_PortA">
</#if>
<#if PORT_NAME == "B">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_B>
<#assign CN_ISR_NAME = "ChangeNotification_PortB">
</#if>
<#if PORT_NAME == "C">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_C>
<#assign CN_ISR_NAME = "ChangeNotification_PortC">
</#if>
<#if PORT_NAME == "D">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_D>
<#assign CN_ISR_NAME = "ChangeNotification_PortD">
</#if>
<#if PORT_NAME == "E">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_E>
<#assign CN_ISR_NAME = "ChangeNotification_PortE">
</#if>
<#if PORT_NAME == "F">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_F>
<#assign CN_ISR_NAME = "ChangeNotification_PortF">
</#if>
<#if PORT_NAME == "G">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_G>
<#assign CN_ISR_NAME = "ChangeNotification_PortG">
</#if>
<#if PORT_NAME == "H">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_H>
<#assign CN_ISR_NAME = "ChangeNotification_PortH">
</#if>
<#if PORT_NAME == "J">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_J>
<#assign CN_ISR_NAME = "ChangeNotification_PortJ">
</#if>
<#if PORT_NAME == "K">
<#assign CN_ISR_VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE_K>
<#assign CN_ISR_NAME = "ChangeNotification_PortK">
</#if>
/* Port ${PORT_NAME} Change Notice Interrupt */
<@RTOS_ISR VECTOR = CN_ISR_VECTOR NAME = CN_ISR_NAME PRIORITY = CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY/>
</#macro>
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true>
<#if CONFIG_SYS_PORT_A_USED == "Y" && CONFIG_SYS_PORT_A_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "A"/> </#if>
<#if CONFIG_SYS_PORT_B_USED == "Y" && CONFIG_SYS_PORT_B_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "B"/> </#if>
<#if CONFIG_SYS_PORT_C_USED == "Y" && CONFIG_SYS_PORT_C_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "C"/> </#if>
<#if CONFIG_SYS_PORT_D_USED == "Y" && CONFIG_SYS_PORT_D_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "D"/> </#if>
<#if CONFIG_SYS_PORT_E_USED == "Y" && CONFIG_SYS_PORT_E_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "E"/> </#if>
<#if CONFIG_SYS_PORT_F_USED == "Y" && CONFIG_SYS_PORT_F_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "F"/> </#if>
<#if CONFIG_SYS_PORT_G_USED == "Y" && CONFIG_SYS_PORT_G_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "G"/> </#if>
<#if CONFIG_SYS_PORT_H_USED == "Y" && CONFIG_SYS_PORT_H_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "H"/> </#if>
<#if CONFIG_SYS_PORT_J_USED == "Y" && CONFIG_SYS_PORT_J_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "J"/> </#if>
<#if CONFIG_SYS_PORT_K_USED == "Y" && CONFIG_SYS_PORT_K_CN_USED == "Y"> <@SYS_PORTS_MZ_CN_ASM_ISR PORT_NAME = "K"/> </#if>
<#else>
/* Change Notice Interrupt */
<@RTOS_ISR VECTOR = CONFIG_INT_VECT_CHANGE_NOTICE NAME = "ChangeNotification" PRIORITY = CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY/>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
