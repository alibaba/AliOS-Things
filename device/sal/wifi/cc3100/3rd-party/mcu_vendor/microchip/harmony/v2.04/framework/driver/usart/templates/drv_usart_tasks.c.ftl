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
<#if (CONFIG_DRV_USART_INTERRUPT_MODE == false)>
<#if !(DRV_USART_TASKS?has_content)>
<#assign DRV_USART_TASKS = "TASK_CALL_NO_RTOS">
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX0 == "Standalone")>
<#if CONFIG_DRV_USART_INST_IDX0 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX0_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX0_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX1 == "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX1 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX1_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX1_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX2 == "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX2 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX2_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX2_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX3 == "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX3 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX3_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX3_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX4 == "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX4 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX4_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX4_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_USART_TASKS == "PROTO") && (CONFIG_DRV_USART_RTOS_IDX5 == "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX5 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX5_Tasks(ULONG thread_input);
<#else>
void _DRV_USART_IDX5_Tasks(void);
</#if>
</#if>
</#if>
<#if DRV_USART_TASKS == "CREATE_TASK">
 <#if CONFIG_DRV_USART_RTOS_IDX0 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX0 == true>

    /* Create task for USART Instance 0 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX0_Tasks"
TASK_NAME="DRV_USART Instance 0 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX0_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX0_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX1 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX1 == true>
  
    /* Create task for USART Instance 1 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX1_Tasks"
TASK_NAME="DRV_USART Instance 1 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX1_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX1_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX2 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX2 == true>
  
    /* Create task for USART Instance 2 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX2_Tasks"
TASK_NAME="DRV_USART Instance 2 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX2_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX2_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX3 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX3 == true>
  
    /* Create task for USART Instance 3 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX3_Tasks"
TASK_NAME="DRV_USART Instance 3 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX3_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX3_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX4 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX4 == true>
  
    /* Create task for USART Instance 4 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX4_Tasks"
TASK_NAME="DRV_USART Instance 4 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX4_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX4_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX5 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX5 == true>
  
    /* Create task for USART Instance 5 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_USART_IDX5_Tasks"
TASK_NAME="DRV_USART Instance 5 Tasks" TASK_PRI=CONFIG_DRV_USART_IDX5_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_USART_IDX5_RTOS_TASK_SIZE/>
  </#if>
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX0 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX0 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
    DRV_USART_TasksError (sysObj.drvUsart0);
    DRV_USART_TasksReceive(sysObj.drvUsart0);
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX1 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX1 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
    DRV_USART_TasksError (sysObj.drvUsart1);
    DRV_USART_TasksReceive(sysObj.drvUsart1);
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX2 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX2 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart2);
    DRV_USART_TasksError (sysObj.drvUsart2);
    DRV_USART_TasksReceive(sysObj.drvUsart2);
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX3 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX3 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart3);
    DRV_USART_TasksError (sysObj.drvUsart3);
    DRV_USART_TasksReceive(sysObj.drvUsart3);
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX4 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX4 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart4);
    DRV_USART_TasksError (sysObj.drvUsart4);
    DRV_USART_TasksReceive(sysObj.drvUsart4);
 </#if>
</#if>
<#if (DRV_USART_TASKS == "TASK_CALL_NO_RTOS") || (DRV_USART_TASKS == "TASK_CALL" && CONFIG_DRV_USART_RTOS_IDX5 != "Standalone")>
 <#if CONFIG_DRV_USART_INST_IDX5 == true>
    DRV_USART_TasksTransmit(sysObj.drvUsart5);
    DRV_USART_TasksError (sysObj.drvUsart5);
    DRV_USART_TasksReceive(sysObj.drvUsart5);
 </#if>
</#if>
<#if DRV_USART_TASKS == "LOCAL_FUNCTION">
 <#if CONFIG_DRV_USART_RTOS_IDX0 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX0 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX0_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX0_Tasks(void)
</#if>
 {
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart0);
        DRV_USART_TasksError (sysObj.drvUsart0);
        DRV_USART_TasksReceive(sysObj.drvUsart0);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX0_RTOS_DELAY/>		
    }
 }
  </#if>
</#if>
<#if CONFIG_DRV_USART_RTOS_IDX1 == "Standalone">
<#if CONFIG_DRV_USART_INST_IDX1 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX1_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX1_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart1);
        DRV_USART_TasksError (sysObj.drvUsart1);
        DRV_USART_TasksReceive(sysObj.drvUsart1);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX1_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX2 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX2 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX2_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX2_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart2);
        DRV_USART_TasksError (sysObj.drvUsart2);
        DRV_USART_TasksReceive(sysObj.drvUsart2);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX2_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX3 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX3 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX3_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX3_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart3);
        DRV_USART_TasksError (sysObj.drvUsart3);
        DRV_USART_TasksReceive(sysObj.drvUsart3);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX3_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX4 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX4 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX4_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX4_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart4);
        DRV_USART_TasksError (sysObj.drvUsart4);
        DRV_USART_TasksReceive(sysObj.drvUsart4);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX4_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_USART_RTOS_IDX5 == "Standalone">
  <#if CONFIG_DRV_USART_INST_IDX5 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_USART_IDX5_Tasks(ULONG thread_input)
<#else>
void _DRV_USART_IDX5_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        DRV_USART_TasksTransmit(sysObj.drvUsart5);
        DRV_USART_TasksError (sysObj.drvUsart5);
        DRV_USART_TasksReceive(sysObj.drvUsart5);
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_USART_IDX5_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
