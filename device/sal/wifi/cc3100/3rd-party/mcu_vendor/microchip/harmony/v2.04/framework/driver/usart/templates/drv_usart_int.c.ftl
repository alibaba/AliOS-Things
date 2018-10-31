<#--
/*******************************************************************************
  USART Driver Interrupt Handler Template File

  File Name:
    drv_usart_int.c

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
<#if CONFIG_DRV_USART_INTERRUPT_MODE == true>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvUsartInstance0(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX0}))) IntHandlerDrvUsartInstance0_ISR( void );
</#if>
void IntHandlerDrvUsartInstance0(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvUsartInstance0(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX0 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart0);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX0 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart0);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX0 == false>
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_PIC32WK == false>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvUsartTransmitInstance0(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX0}))) IntHandlerDrvUsartTransmitInstance0_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance0(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvUsartTransmitInstance0(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX0 == false>
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_PIC32WK == false>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvUsartReceiveInstance0(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX0}))) IntHandlerDrvUsartReceiveInstance0_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance0(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvUsartReceiveInstance0(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart0);
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
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_PIC32WK == false>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvUsartErrorInstance0(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX0}))) IntHandlerDrvUsartErrorInstance0_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance0(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvUsartErrorInstance0(void)
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
    DRV_USART_TasksError(sysObj.drvUsart0);
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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32WK --> <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX0 -->

<#if CONFIG_DRV_USART_INST_IDX1 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvUsartInstance1(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX1}))) IntHandlerDrvUsartInstance1_ISR( void );
</#if>
void IntHandlerDrvUsartInstance1(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvUsartInstance1(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX1 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart1);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX1 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart1);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX1 == false>
<#if CONFIG_USE_3RDPARTY_RTOS && CONFIG_PIC32WK == false>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvUsartTransmitInstance1(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX1}))) IntHandlerDrvUsartTransmitInstance1_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance1(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvUsartTransmitInstance1(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX1 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvUsartReceiveInstance1(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX1}))) IntHandlerDrvUsartReceiveInstance1_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance1(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvUsartReceiveInstance1(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart1);
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
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvUsartErrorInstance1(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX1}))) IntHandlerDrvUsartErrorInstance1_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance1(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvUsartErrorInstance1(void)
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
    DRV_USART_TasksError(sysObj.drvUsart1);
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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32WK -->  <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX1 -->

<#if CONFIG_DRV_USART_INST_IDX2 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvUsartInstance2(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX2}))) IntHandlerDrvUsartInstance2_ISR( void );
</#if>
void IntHandlerDrvUsartInstance2(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvUsartInstance2(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX2 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart2);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart2);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX2 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart2);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX2 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvUsartTransmitInstance2(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX2}))) IntHandlerDrvUsartTransmitInstance2_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance2(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvUsartTransmitInstance2(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart2);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX2 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvUsartReceiveInstance2(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX2}))) IntHandlerDrvUsartReceiveInstance2_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance2(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvUsartReceiveInstance2(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart2);
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
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvUsartErrorInstance2(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX2}))) IntHandlerDrvUsartErrorInstance2_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance2(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvUsartErrorInstance2(void)
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
    DRV_USART_TasksError(sysObj.drvUsart2);
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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX2 -->

<#if CONFIG_DRV_USART_INST_IDX3 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvUsartInstance3(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX3}))) IntHandlerDrvUsartInstance3_ISR( void );
</#if>
void IntHandlerDrvUsartInstance3(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvUsartInstance3(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX3 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart3);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart3);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX3 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart3);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX3 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvUsartTransmitInstance3(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX3}))) IntHandlerDrvUsartTransmitInstance3_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance3(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvUsartTransmitInstance3(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart3);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX3 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvUsartReceiveInstance3(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX3}))) IntHandlerDrvUsartReceiveInstance3_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance3(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvUsartReceiveInstance3(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart3);
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
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvUsartErrorInstance3(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX3}))) IntHandlerDrvUsartErrorInstance3_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance3(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvUsartErrorInstance3(void)
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
    DRV_USART_TasksError(sysObj.drvUsart3);

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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX3 -->

<#if CONFIG_DRV_USART_INST_IDX4 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvUsartInstance4(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX4}))) IntHandlerDrvUsartInstance4_ISR( void );
</#if>
void IntHandlerDrvUsartInstance4(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvUsartInstance4(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX4 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart4);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart4);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX4 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart4);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX4 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvUsartTransmitInstance4(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX4}))) IntHandlerDrvUsartTransmitInstance4_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance4(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvUsartTransmitInstance4(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart4);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX4 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvUsartReceiveInstance4(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX4}))) IntHandlerDrvUsartReceiveInstance4_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance4(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvUsartReceiveInstance4(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart4);
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
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvUsartErrorInstance4(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX4}))) IntHandlerDrvUsartErrorInstance4_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance4(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvUsartErrorInstance4(void)
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
    DRV_USART_TasksError(sysObj.drvUsart4);
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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX4 -->

<#if CONFIG_DRV_USART_INST_IDX5 == true>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX5}SOFT) _IntHandlerDrvUsartInstance5(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX5}AUTO), vector(${CONFIG_DRV_USART_ISR_VECTOR_IDX5}))) IntHandlerDrvUsartInstance5_ISR( void );
</#if>
void IntHandlerDrvUsartInstance5(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_INT_PRIO_NUM_IDX5}AUTO) _IntHandlerDrvUsartInstance5(void)
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
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX5 == false>
    DRV_USART_TasksTransmit(sysObj.drvUsart5);
</#if>
    DRV_USART_TasksError(sysObj.drvUsart5);
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX5 == false>
    DRV_USART_TasksReceive(sysObj.drvUsart5);
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
</#if> <#-- CONFIG_PIC32MX -->
<#if CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true>
<#if CONFIG_DRV_USART_SUPPORT_TRANSMIT_DMA_IDX5 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX5}SOFT) _IntHandlerDrvUsartTransmitInstance5(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX5}AUTO), vector(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX5}))) IntHandlerDrvUsartTransmitInstance5_ISR( void );
</#if>
void IntHandlerDrvUsartTransmitInstance5(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_XMIT_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_XMIT_INT_PRIO_NUM_IDX5}AUTO) _IntHandlerDrvUsartTransmitInstance5(void)
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
    DRV_USART_TasksTransmit(sysObj.drvUsart5);
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
<#if CONFIG_DRV_USART_SUPPORT_RECEIVE_DMA_IDX5 == false>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX5}SOFT) _IntHandlerDrvUsartReceiveInstance5(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX5}AUTO), vector(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX5}))) IntHandlerDrvUsartReceiveInstance5_ISR( void );
</#if>
void IntHandlerDrvUsartReceiveInstance5(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_RCV_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_RCV_INT_PRIO_NUM_IDX5}AUTO) _IntHandlerDrvUsartReceiveInstance5(void)
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
    DRV_USART_TasksReceive(sysObj.drvUsart5);
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
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX5}SOFT) _IntHandlerDrvUsartErrorInstance5(void)
<#else>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX5}AUTO), vector(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX5}))) IntHandlerDrvUsartErrorInstance5_ISR( void );
</#if>
void IntHandlerDrvUsartErrorInstance5(void)
</#if>
<#else>
void __ISR(${CONFIG_DRV_USART_ERR_ISR_VECTOR_IDX5}, ipl${CONFIG_DRV_USART_ERR_INT_PRIO_NUM_IDX5}AUTO) _IntHandlerDrvUsartErrorInstance5(void)
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
    DRV_USART_TasksError(sysObj.drvUsart5);
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
</#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK-->
</#if> <#-- CONFIG_DRV_USART_INST_IDX5 -->
</#if> <#-- CONFIG_DRV_USART_INTERRUPT_MODE -->
<#--
/*******************************************************************************
 End of File
*/
-->
