/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "osdep_service.h"
#include <stdarg.h>
#include "strproc.h"

#if defined ( __ICCARM__ )
#pragma section=".cmd.table.data"

SECTION(".data") u8* __cmd_table_start__;
SECTION(".data") u8* __cmd_table_end__;
#endif

extern volatile UART_LOG_CTL		shell_ctl;
extern UART_LOG_BUF				shell_buf;
#ifdef CONFIG_UART_LOG_HISTORY
extern u8							shell_history_cmd[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN];
#endif

extern COMMAND_TABLE    shell_cmd_table[];

_sema shell_sema;

#if defined(CONFIG_WIFI_NORMAL)
#if SUPPORT_LOG_SERVICE
extern char log_buf[LOG_SERVICE_BUFLEN];
extern _sema log_rx_interrupt_sema;
extern void log_service_init(void);
#endif
#endif

static monitor_cmd_handler shell_get_cmd(char* argv)
{
	PCOMMAND_TABLE  pCmdTbl = shell_ctl.pCmdTbl;
	u32 CmdCnt = 0;
	u32 CmdNum = shell_ctl.CmdTblSz;
	monitor_cmd_handler cmd_handler = NULL;
	
	for (CmdCnt = 0; CmdCnt< CmdNum; CmdCnt++) {          
		if ((_stricmp(argv, (const char *)pCmdTbl[CmdCnt].cmd))==0){
			cmd_handler = pCmdTbl[CmdCnt].func;
			break;
		}
	}

	return cmd_handler;
}

static void shell_give_sema(void)
{
	if (shell_ctl.shell_task_rdy) {
		portBASE_TYPE taskWoken = pdFALSE;
		rtw_up_sema_from_isr(&shell_sema);
	}
}
//======================================================
u32 shell_cmd_exec(u8  argc, u8  **argv)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) *argv;
	
#if defined(CONFIG_WIFI_NORMAL)
	if(argc == 0) {
		return FALSE;
	}
	
#if SUPPORT_LOG_SERVICE
	rtw_up_sema((_sema *)&log_rx_interrupt_sema);
#endif
	shell_array_init(argv[0], sizeof(argv[0]) ,0);
	
#endif
	return TRUE;
}

//======================================================
static u32 shell_cmd_exec_ram(u8  argc, u8  **argv)
{
	monitor_cmd_handler cmd_handler = NULL;

	if (argc > 0){
		cmd_handler = shell_get_cmd((char *)argv[0]);

		if (cmd_handler != NULL) {
			_strupr(argv[0]);
			cmd_handler((argc-1) , (argv+1));
			shell_array_init(argv[0], sizeof(argv[0]) ,0);
			return TRUE;
		}
	}

	//(*pUartLogBuf).BufCount = 0;
	//shell_array_init(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');

	return FALSE;
}


static VOID shell_task_ram(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;

	u32 ret = TRUE;
	
	//4 Set this for UartLog check cmd history
	shell_ctl.shell_task_rdy = 1;
	shell_ctl.BootRdy = 1;

	do{
		rtw_down_sema(&shell_sema);

		if (shell_ctl.ExecuteCmd) {
			u8  argc = 0;
			u8  **argv;
			PUART_LOG_BUF   pUartLogBuf = shell_ctl.pTmpLogBuf;
#if defined(CONFIG_WIFI_NORMAL)
#if SUPPORT_LOG_SERVICE
			_strncpy(log_buf, (const char*)&(*pUartLogBuf).UARTLogBuf[0], LOG_SERVICE_BUFLEN-1);
#endif
#endif

			argc = shell_get_argc((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
			argv = (u8**)shell_get_argv((const u8*)&((*pUartLogBuf).UARTLogBuf[0])); /* UARTLogBuf will be changed */

			if (argc > 0) {
				/* FPGA Verification */
				ret = shell_cmd_exec_ram(argc, argv);

				/* normal for LOG service */
				if (ret == FALSE) {
					ret = shell_cmd_exec(argc, argv);
				}
				
				(*pUartLogBuf).BufCount = 0;
				shell_array_init(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
			} else {
				/*In some exception case, even if argc parsed is 0(when the first character value in log buffer is '\0'), 
				log buffer may not be empty and log buffer counter may not be zero. If not clean log buffer and counter
				, some error will happen. Therefore, clean log buffer and initialize buffer counter when it occurs.*/
				if((*pUartLogBuf).BufCount != 0) {
					(*pUartLogBuf).BufCount = 0;
					shell_array_init(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
				}
				CONSOLE_AMEBA();
			}
			shell_ctl.ExecuteCmd = _FALSE;

			pmu_set_sysactive_time(10000);
		}
	}while(1);
}

VOID shell_init_ram(VOID)
{
#if defined(CONFIG_WIFI_NORMAL)
#if SUPPORT_LOG_SERVICE
//	log_service_init();
#endif
#endif

#ifdef AMEBAD_TODO
	LOGUART_SetBaud_FromFlash();
#endif

#if defined ( __ICCARM__ )
	__cmd_table_start__ = (u8*)__section_begin(".cmd.table.data");
	__cmd_table_end__ = (u8*)__section_end(".cmd.table.data");
#endif

	shell_ctl.pCmdTbl = (PCOMMAND_TABLE)__cmd_table_start__;
	shell_ctl.CmdTblSz = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));
	
	shell_ctl.ExecuteCmd = _FALSE;
	shell_ctl.ExecuteEsc= _TRUE;//don't check Esc anymore
	shell_ctl.GiveSema = shell_give_sema;
	

	/* Create a Semaphone */
	rtw_init_sema(&shell_sema, 0);
	rtw_down_timeout_sema(&shell_sema, 1/portTICK_RATE_MS);


	aos_task_new("LOGUART_TASK", shell_task_ram, NULL, 1024*4);
#if 0
	if (pdTRUE != xTaskCreate( shell_task_ram, (const char * const)"LOGUART_TASK", 1024*2, 
		NULL, 5 , NULL))
	{
		DiagPrintf("Create Log UART Task Err!!\n");
	}
#endif

	//CONSOLE_AMEBA();
}

void shell_switch_ipc_int(VOID *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	( void ) Data;
	( void ) IrqStatus;
	( void ) ChanNum;

	u8 CpuId = IPCM0_DEV->IPCx_CPUID;

	if (CpuId == 1) {
		DBG_8195A("KM4 shell\n");
		InterruptEn(UART_LOG_IRQ, 10);
		pmu_set_sysactive_time(1000);
	} else {
		DBG_8195A("KM0 shell\n");
		InterruptEn(UART_LOG_IRQ_LP, 10);
	}
}
