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
<#if (CONFIG_USE_DRV_TMR == true) && (CONFIG_DRV_TMR_INTERRUPT_MODE == false)>
    <#if (CONFIG_DRV_TMR_DRIVER_MODE == "DYNAMIC") >
        <#if !(DRV_TMR_TASKS?has_content)>
            <#assign DRV_TMR_TASKS = "TASK_CALL_NO_RTOS">
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX0 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_0 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX0_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX0_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX1 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_1 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX1_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX1_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX2 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_2 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX2_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX2_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX3 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_3 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX3_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX3_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX4 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_4 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX4_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX4_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX5 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_5 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX5_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX5_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX6 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_6 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX6_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX6_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX7 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_7 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX7_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX7_Tasks(void);
                </#if>
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX8 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_8 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX8_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX8_Tasks(void);
                </#if>
            </#if>
        </#if>
		<#if CONFIG_ARCH_ARM == true>
		        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX9 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_9 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX9_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX9_Tasks(void);
                </#if>
            </#if>
        </#if>
		        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX10 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_10 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX10_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX10_Tasks(void);
                </#if>
            </#if>
        </#if>
		        <#if (DRV_TMR_TASKS == "PROTO") && (CONFIG_DRV_TMR_RTOS_IDX11 == "Standalone")>
            <#if CONFIG_DRV_TMR_INST_11 == true>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                    <#lt>void _DRV_TMR_IDX11_Tasks(ULONG thread_input);
                <#else>
                    <#lt>void _DRV_TMR_IDX11_Tasks(void);
                </#if>
            </#if>
        </#if>
		</#if>
        <#if DRV_TMR_TASKS == "CREATE_TASK">
            <#if CONFIG_DRV_TMR_RTOS_IDX0 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_0 == true>
                    <#lt>/* Create task for TMR Instance 0 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX0_Tasks"
    TASK_NAME="DRV_TMR Instance 0 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX0_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX0_RTOS_TASK_SIZE/>
                    </#if>
                </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX1 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_1 == true>
                    <#lt>/* Create task for TMR Instance 1 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX1_Tasks"
    TASK_NAME="DRV_TMR Instance 1 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX1_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX1_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX2 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_2 == true>
                    <#lt>/* Create task for TMR Instance 2 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX2_Tasks"
    TASK_NAME="DRV_TMR Instance 2 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX2_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX2_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX3 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_3 == true>
                    <#lt>/* Create task for TMR Instance 3 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX3_Tasks"
    TASK_NAME="DRV_TMR Instance 3 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX3_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX3_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX4 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_4 == true>
                    <#lt>/* Create task for TMR Instance 4 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX4_Tasks"
    TASK_NAME="DRV_TMR Instance 4 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX4_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX4_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX5 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_5 == true>
                    <#lt>/* Create task for TMR Instance 5 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX5_Tasks"
    TASK_NAME="DRV_TMR Instance 5 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX5_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX5_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX6 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_6 == true>
                    <#lt>/* Create task for TMR Instance 6 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX6_Tasks"
    TASK_NAME="DRV_TMR Instance 6 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX6_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX6_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX7 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_7 == true>
                    <#lt>/* Create task for TMR Instance 7 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX7_Tasks"
    TASK_NAME="DRV_TMR Instance 7 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX7_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX7_RTOS_TASK_SIZE/>
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX8 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_8 == true>
                    <#lt>/* Create task for TMR Instance 8 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX8_Tasks"
    TASK_NAME="DRV_TMR Instance 8 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX8_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX8_RTOS_TASK_SIZE/>
                </#if>
            </#if>
			<#if CONFIG_ARCH_ARM == true>
            <#if CONFIG_DRV_TMR_RTOS_IDX9 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_9 == true>
                    <#lt>/* Create task for TMR Instance 9 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX9_Tasks"
    TASK_NAME="DRV_TMR Instance 9 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX9_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX9_RTOS_TASK_SIZE/>
                    </#if>
                </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX10 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_10 == true>
                    <#lt>/* Create task for TMR Instance 10 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX10_Tasks"
    TASK_NAME="DRV_TMR Instance 10 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX10_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX10_RTOS_TASK_SIZE/>
                    </#if>
                </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX11 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_11 == true>
                    <#lt>/* Create task for TMR Instance 11 state machine*/
    <@RTOS_TASK_CREATE RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_FUNC_NAME="_DRV_TMR_IDX11_Tasks"
    TASK_NAME="DRV_TMR Instance 11 Tasks" TASK_PRI=CONFIG_DRV_TMR_IDX11_RTOS_TASK_PRIORITY
    TASK_STK_SZ=CONFIG_DRV_TMR_IDX11_RTOS_TASK_SIZE/>
                    </#if>
                </#if>
				</#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX0 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_0 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr0);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX1 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_1 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr1);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX2 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_2 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr2);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX3 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_3 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr3);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX4 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_4 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr4);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX5 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_5 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr5);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX6 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_6 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr6);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX7 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_7 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr7);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX8 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_8 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr8);
            </#if>
        </#if>
		<#if CONFIG_ARCH_ARM == true>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX9 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_9 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr9);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX10 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_10 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr10);
            </#if>
        </#if>
        <#if (DRV_TMR_TASKS == "TASK_CALL_NO_RTOS") || (DRV_TMR_TASKS == "TASK_CALL" && CONFIG_DRV_TMR_RTOS_IDX11 != "Standalone")>
            <#if CONFIG_DRV_TMR_INST_11 == true>
                <#lt>DRV_TMR_Tasks(sysObj.drvTmr11);
            </#if>
        </#if>
		</#if>
        <#if DRV_TMR_TASKS == "LOCAL_FUNCTION">
            <#if CONFIG_DRV_TMR_RTOS_IDX0 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_0 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX0_Tasks(ULONG thread_input)
                    <#else>
                        <#lt>void _DRV_TMR_IDX0_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr0);
    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX0_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX1 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_1 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX1_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX1_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr1);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX1_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX2 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_2 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX2_Tasks(ULONG thread_input)
                    <#else>
                        <#lt>void _DRV_TMR_IDX2_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr2);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX2_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX3 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_3 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX3_Tasks(void)
                    <#else>
                        <#lt>void _DRV_TMR_IDX3_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr3);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX3_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX4 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_4 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX4_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX4_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr4);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX4_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX5 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_5 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX5_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX5_Tasks(void);
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr5);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX5_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX6 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_6 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX6_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX6_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr6);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX6_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX7 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_7 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX7_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX7_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr7);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX7_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX8 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_8 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX8_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX8_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr8);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX8_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
			<#if CONFIG_ARCH_ARM == true>
            <#if CONFIG_DRV_TMR_RTOS_IDX9 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_9 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX9_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX9_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr9);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX9_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX10 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_10 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX10_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX10_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr10);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX10_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
            <#if CONFIG_DRV_TMR_RTOS_IDX11 == "Standalone">
                <#if CONFIG_DRV_TMR_INST_11 == true>
                    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
                        <#lt>void _DRV_TMR_IDX11_Tasks(ULONG thread_input);
                    <#else>
                        <#lt>void _DRV_TMR_IDX11_Tasks(void)
                    </#if>
                    <#lt>{
                    <#if CONFIG_3RDPARTY_RTOS_USED == "uC/OS-III">
                        <#lt>    OS_ERR os_err;
                        <#lt>
                    </#if>
                    <#lt>    while(1)
                    <#lt>    {
                    <#lt>        DRV_TMR_Tasks(sysObj.drvTmr11);
                    <@RTOS_TASK_DELAY RTOS_NAME=CONFIG_3RDPARTY_RTOS_USED TASK_DELAY=CONFIG_DRV_TMR_IDX11_RTOS_DELAY/>
                    <#lt>    }
                    <#lt>}
                </#if>
            </#if>
			</#if>
        </#if>
    <#else>
        <#if CONFIG_DRV_TMR_INST_0 == true>
            <#lt>    DRV_TMR0_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_1 == true>
            <#lt>    DRV_TMR1_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_2 == true>
            <#lt>    DRV_TMR2_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_3 == true>
            <#lt>    DRV_TMR3_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_4 == true>
            <#lt>    DRV_TMR4_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_5 == true>
            <#lt>    DRV_TMR5_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_6 == true>
            <#lt>    DRV_TMR6_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_7 == true>
            <#lt>    DRV_TMR7_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_8 == true>
            <#lt>    DRV_TMR8_Tasks();
        </#if>
		<#if CONFIG_ARCH_ARM == true>
        <#if CONFIG_DRV_TMR_INST_9 == true>
            <#lt>    DRV_TMR9_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_10 == true>
            <#lt>    DRV_TMR10_Tasks();
        </#if>
        <#if CONFIG_DRV_TMR_INST_11 == true>
            <#lt>    DRV_TMR11_Tasks();
        </#if>
		</#if>
    </#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
