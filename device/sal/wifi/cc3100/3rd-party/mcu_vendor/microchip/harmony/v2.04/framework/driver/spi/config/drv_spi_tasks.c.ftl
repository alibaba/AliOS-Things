<#--
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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
<#if !(DRV_SPI_TASKS?has_content)>
<#assign DRV_SPI_TASKS = "TASK_CALL_NO_RTOS">
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX0 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX0 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX0 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX0_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX0_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX1 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX1 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX1 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX1_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX1_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX2 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX2 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX2 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX2_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX2_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX3 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX3 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX3 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX3_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX3_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX4 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX4 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX4 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX4_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX4_Tasks(void);
</#if>
</#if>
</#if>
<#if (DRV_SPI_TASKS == "PROTO") && (CONFIG_DRV_SPI_RTOS_IDX5 == "Standalone")>
 <#if CONFIG_DRV_SPI_IDX5 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX5 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX5_Tasks(ULONG thread_input);
<#else>
void _DRV_SPI_IDX5_Tasks(void);
</#if>
</#if>
</#if>
<#if DRV_SPI_TASKS == "CREATE_TASK">
<#if CONFIG_DRV_SPI_RTOS_IDX0 == "Standalone">
<#if CONFIG_DRV_SPI_IDX0 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX0 == true>
  
    /* Create task for SPI Instance 0 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX0_Tasks"
TASK_NAME="DRV_SPI Instance 0 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX0_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX0_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX1 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX1 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX1 == true>
  
    /* Create task for SPI Instance 1 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX1_Tasks"
TASK_NAME="DRV_SPI Instance 1 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX1_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX1_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX2 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX2 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX2 == true>
  
    /* Create task for SPI Instance 2 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX2_Tasks"
TASK_NAME="DRV_SPI Instance 2 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX2_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX2_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX3 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX3 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX3 == true>
  
    /* Create task for SPI Instance 3 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX3_Tasks"
TASK_NAME="DRV_SPI Instance 3 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX3_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX0_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX4 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX4 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX4 == true>
  
    /* Create task for SPI Instance 4 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX4_Tasks"
TASK_NAME="DRV_SPI Instance 4 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX4_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX4_RTOS_TASK_SIZE/>
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX5 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX5 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX5 == true>
  
    /* Create task for SPI Instance 5 state machine*/
<@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_SPI_IDX5_Tasks"
TASK_NAME="DRV_SPI Instance 5 Tasks" TASK_PRI=CONFIG_DRV_SPI_IDX5_RTOS_TASK_PRIORITY
TASK_STK_SZ=CONFIG_DRV_SPI_IDX5_RTOS_TASK_SIZE/>
  </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX0 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX0 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX0 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '0'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
    </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX1 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX1 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX1 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '1'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx1);
    </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX2 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX2 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX2 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '2'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx2);
    </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX3 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX3 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX3 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '3'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx3);
    </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX4 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX4 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX4 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '4'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx4);
    </#if>
 </#if>
</#if>
<#if (DRV_SPI_TASKS == "TASK_CALL_NO_RTOS") || (DRV_SPI_TASKS == "TASK_CALL" && CONFIG_DRV_SPI_RTOS_IDX5 != "Standalone")>
 <#if CONFIG_DRV_SPI_IDX5 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX5 == true>
    <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '5'>
    DRV_SPI_Tasks(sysObj.spiObjectIdx5);
    </#if>
 </#if>
</#if>
<#if DRV_SPI_TASKS == "LOCAL_FUNCTION">
 <#if CONFIG_DRV_SPI_RTOS_IDX0 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX0 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX0 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX0_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX0_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '0'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx0);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX0_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX1 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX1 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX1 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX1_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX1_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '1'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx1);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX1_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX2 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX2 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX2 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX2_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX2_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '2'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx2);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX2_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX3 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX3 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX3 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX3_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX3_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '3'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx3);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX3_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX4 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX4 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX4 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX4_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX4_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '4'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx4);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX4_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
 <#if CONFIG_DRV_SPI_RTOS_IDX5 == "Standalone">
  <#if CONFIG_DRV_SPI_IDX5 == true && CONFIG_DRV_SPI_TASK_MODE_POLLED_IDX5 == true>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void _DRV_SPI_IDX5_Tasks(ULONG thread_input)
<#else>
void _DRV_SPI_IDX5_Tasks(void)
</#if>
{
<#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
    OS_ERR os_err;
	
</#if> 
    while(1)
    {
        <#if CONFIG_DRV_WIFI_SPI_INSTANCE_INDEX != '5'>
        DRV_SPI_Tasks(sysObj.spiObjectIdx5);
        </#if>
<@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_SPI_IDX5_RTOS_DELAY/>		
    }
 }
  </#if>
 </#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
