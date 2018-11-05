<#--
/*******************************************************************************
  Port System Service Interrupt Handler Template File

  File Name:
    sys_port_int.c

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
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true>
<#macro SYS_PORTS_MZ_CN_ISR PORT_CHANNEL INT_CHANNEL INT_CHANNEL_VECTOR INT_CALLBACK>
<#if CONFIG_USE_3RDPARTY_RTOS>
   <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
      void __ISR(${INT_CHANNEL_VECTOR}, IPL${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}SOFT) _IntHandlerChangeNotification_${PORT_CHANNEL}(void)
      <#else>
         <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
            void __attribute__( (interrupt(ipl${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}AUTO), vector(${INT_CHANNEL_VECTOR}))) IntHandlerChangeNotification_${PORT_CHANNEL}_ISR( void );
         </#if>
         void IntHandlerChangeNotification_${PORT_CHANNEL}(void)
      </#if>
<#else>
   void __ISR(${INT_CHANNEL_VECTOR}, ipl${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}AUTO) _IntHandlerChangeNotification_${PORT_CHANNEL}(void)
</#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context save.  */
   _tx_thread_context_save();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    ${INT_CALLBACK}
    PLIB_INT_SourceFlagClear(INT_ID_0,${INT_CHANNEL});
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
</#macro>
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_A_CN_USED == "Y">
   <#if CONFIG_USE_DRV_OVM7690 == true>
     <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortA"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_A"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_A_VECTOR"
        INT_CALLBACK = "DRV_CAMERA_OVM7690_HsyncEventHandler(sysObj.drvOvm7690);"/>
   <#else>
     <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortA"
       INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_A"
       INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_A_VECTOR"
       INT_CALLBACK = ""/>
    </#if>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_B_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortB"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_B"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_B_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_C_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortC"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_C"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_C_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_D_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortD"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_D"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_D_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_E_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortE"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_E"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_E_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_F_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortF"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_F"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_F_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_G_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortG"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_G"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_G_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_H_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortH"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_H"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_H_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_J_CN_USED == "Y">
   <#if CONFIG_USE_DRV_OVM7690 == true>
     <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortJ"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_J"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_J_VECTOR"
        INT_CALLBACK = "DRV_CAMERA_OVM7690_VsyncEventHandler(sysObj.drvOvm7690);"/>
   <#else>
     <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortJ"
       INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_J"
       INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_J_VECTOR"
       INT_CALLBACK = ""/>
    </#if>
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_K_CN_USED == "Y">
    <@SYS_PORTS_MZ_CN_ISR PORT_CHANNEL = "PortK"
        INT_CHANNEL = "INT_SOURCE_CHANGE_NOTICE_K"
        INT_CHANNEL_VECTOR = "_CHANGE_NOTICE_K_VECTOR"
        INT_CALLBACK = ""/>
</#if>
<#else> <#-- pic32mx -->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(_CHANGE_NOTICE_VECTOR, IPL${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}SOFT) _IntHandlerChangeNotification(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}AUTO), vector(_CHANGE_NOTICE_VECTOR))) IntHandlerChangeNotification_ISR( void );
</#if>
void IntHandlerChangeNotification(void)
</#if>
<#else>
void __ISR(_CHANGE_NOTICE_VECTOR, ipl${CONFIG_SYS_PORTS_CN_INTERRUPT_PRIORITY_NUM}AUTO) _IntHandlerChangeNotification(void)
</#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context save.  */
   _tx_thread_context_save();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
    /* TODO: Add code to process interrupt here */
<#if CONFIG_HAVE_PPS == true>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_A_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_A);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_B_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_B);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_C_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_C);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_D_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_D);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_E_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_E);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_F_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_F);
</#if>
<#if CONFIG_USE_SYS_PORTS_CN_INTERRUPT == true && CONFIG_SYS_PORT_G_CN_USED == "Y">
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_G);
</#if>
<#else>
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE);
    <#if CONFIG_DRV_WIFI_INTERRUPT_SYSTEM_SERVICE == "Change Notification">
      <#if CONFIG_DRV_WIFI_DEVICE == "MRF24WN">
    WDRV_MRF24WN_ISR();
      <#elseif CONFIG_DRV_WIFI_DEVICE == "WINC1500">
        <#if CONFIG_BSP_PIC32MX_ETH_SK!false || CONFIG_BSP_PIC32MX_ETH_SK2!false>
    if (PLIB_PORTS_PinGet(PORTS_ID_0, WDRV_IRQN_PORT_CHANNEL_READ, WDRV_IRQN_BIT_POS_READ) == 0)
        <#else>
    if (PLIB_PORTS_PinGet(PORTS_ID_0, WDRV_IRQN_PORT_CHANNEL, WDRV_IRQN_BIT_POS) == 0)
      </#if>
        WDRV_WINC1500_ISR();
      </#if>
    </#if>
</#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
</#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
</#if>
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
