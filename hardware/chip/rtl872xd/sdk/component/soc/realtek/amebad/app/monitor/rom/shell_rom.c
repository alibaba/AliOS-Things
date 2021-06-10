/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include <stdarg.h>
#include "strproc.h"

#include "k_api.h"

SHELL_ROM_BSS_SECTION volatile UART_LOG_CTL	 shell_ctl;
SHELL_ROM_BSS_SECTION u8						*shell_argv_array[MAX_ARGV];
SHELL_ROM_BSS_SECTION UART_LOG_BUF			shell_buf;
#ifdef CONFIG_UART_LOG_HISTORY
SHELL_ROM_BSS_SECTION u8						shell_history_cmd[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN];
#endif
SHELL_ROM_BSS_SECTION u32					shell_recv_all_data_onetime;

//=================================================

//======================================================
//<Function>:  shell_array_init
//<Usage   >:  This function is to initialize an array with a given value.
//<Argus    >:  pArrayToInit --> pointer to an array
//                   ArrayLen      --> number of array elements
//                   InitValue     --> initial value for the array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
VOID shell_array_init(u8  *pArrayToInit,  u8  ArrayLen,  u8  InitValue)
{
	u8  ItemCount =0;

	for (ItemCount=0; ItemCount<ArrayLen; ItemCount++){
		*(pArrayToInit+ItemCount) = InitValue;
	}
}


//======================================================
//<Function>:  shell_get_argc
//<Usage   >:  This function is to calcute how many parameters in the cmd string
//<Argus    >:  string      --> pointer to an input cmd string
//<Return   >:  number of parameters in the string.(****except the cmd itself****)
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_
u8 shell_get_argc(const   u8  *string)
{
	u16 argc;
	u8  *pStr;

	argc = 0;
	pStr = (u8 *)string;

	while(*pStr){
		if ((*pStr != ' ') && (*pStr)){
			argc++;
			while ((*pStr != ' ') && (*pStr)){
				pStr++;
			}

			continue;
		}

		pStr++;
	}

	if (argc >= MAX_ARGV){
		argc = MAX_ARGV - 1;
	}

	return argc;
}

//======================================================
//<Function>:  shell_get_argv
//<Usage   >:  This function is to transfer a cmd string to a char array.
//<Argus    >:  string      --> pointer to an input cmd string
//<Return   >:  pointer to a char array
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
u8** shell_get_argv(const   u8  *string)
{
	u8  ArgvCount = 0;
	u8  *pStr;

	shell_array_init((u8*)shell_argv_array, MAX_ARGV*sizeof(char *), 0);
	pStr = (u8 *)string;

	while(*pStr){
		shell_argv_array[ArgvCount] = pStr;
		while((*pStr != ' ') && (*pStr)){
			pStr++;
		}

		*(pStr++) = '\0';

		while((*pStr == ' ') && (*pStr)){
			pStr++;
		}

		ArgvCount++;

		if (ArgvCount == MAX_ARGV){
			break;
		}
	}

	return (u8 **)&shell_argv_array;
}




//======================================================
//<Function>:  shell_cmd_exec_rom
//<Usage   >:  This function is to parse Uart-Log cmds. If there's a matched
//                      one, it goes to execute that.
//<Argus    >:  pointer to Uart-Log cmd temp buffer
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
static VOID shell_cmd_exec_rom(PUART_LOG_CTL   pUartLogCtlExe)
{
	u8  CmdCnt = 0;
	u8  argc = 0;
	u8  **argv;
	u32  CmdNum;
	PUART_LOG_BUF   pUartLogBuf = pUartLogCtlExe->pTmpLogBuf;
	PCOMMAND_TABLE pCmdTbl = pUartLogCtlExe->pCmdTbl;

	argc = shell_get_argc((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
	argv = shell_get_argv((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
	CmdNum = pUartLogCtlExe->CmdTblSz;

	if (argc > 0){
		_strupr(argv[0]);

		for (CmdCnt = 0; CmdCnt< CmdNum; CmdCnt++){          
			if ((_strcmp((const char*)argv[0], (const char*)(pCmdTbl[CmdCnt].cmd)))==0){
				pCmdTbl[CmdCnt].func((argc-1) , (argv+1));
				shell_array_init(argv[0], sizeof(argv[0]) ,0);
				break;
			}
		}
	}

	(*pUartLogBuf).BufCount = 0;
	shell_array_init(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
}


//======================================================
//<Function>:  shell_show_backspace
//<Usage   >:  To dispaly backspace on the target interface.
//<Argus    >:  BackLen      --> backspace number to show
//                   EchoFlag     --> a control flag to show msg or not.
//                   pfINPUT      --> func pointer to the display function
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
static VOID shell_show_backspace(u8  BackLen, u8  EchoFlag, VOID *pfINPUT)
{
	u8  BackCnt;
	ECHOFUNC pfEcho;
	pfEcho = (ECHOFUNC)pfINPUT;

	for(BackCnt=0; BackCnt<BackLen; BackCnt++){
		if (EchoFlag){
			pfEcho((u8*)"%c",KB_ASCII_BS);
			pfEcho((u8*)" ");
			pfEcho((u8*)"%c",KB_ASCII_BS);
		}
	}
}

//======================================================
//<Function>:  shell_recall_oldcmd
//<Usage   >:  To retrieve the old cmd and may show it on the target interface.
//<Argus    >:  pTmpBuf      --> cmd temp buffer, the retrieved cmd will be stored in
//                                            it.
//                   OldBuf         --> pointer to an cmd buffer.
//                   EchoFlag      --> a control flag to show msg or not.
//                   pfINPUT       --> func pointer to the display function
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
#ifdef CONFIG_UART_LOG_HISTORY
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
static VOID shell_recall_oldcmd(
	PUART_LOG_BUF  pTmpBuf,
	u8  *OldBuf,
	u8  EchoFlag,
	VOID *pfINPUT)
{

	ECHOFUNC pfEcho;
	pfEcho = (ECHOFUNC)pfINPUT;

	_strcpy((char*)&((*pTmpBuf).UARTLogBuf[0]), (char*)OldBuf);
	(*pTmpBuf).BufCount = _strlen((char*)&(*pTmpBuf).UARTLogBuf[0]);

	if (EchoFlag){
		pfEcho(&((*pTmpBuf).UARTLogBuf[0]));
	}

}

//======================================================
//<Function>:  shell_cmd_history
//<Usage   >:  To deal with history-cmds, get the next/previous cmd and show it.
//<Argus    >:  RevData         --> a received data.
//                   prvUartLogCtl  --> pointer to UART_LOG_CTL
//                   EchoFlag        --> a control flag to show msg or not.
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
VOID shell_cmd_history(
	u8  RevData,
	UART_LOG_CTL    *prvUartLogCtl,
	u8  EchoFlag)
{
	ECHOFUNC pfEcho;
	pfEcho = (ECHOFUNC)(*prvUartLogCtl).pfINPUT;

	if (RevData == 'A'){
		if ((*prvUartLogCtl).CRSTS == 0){
			if ((*prvUartLogCtl).RevdNo > 0){
				if ((*prvUartLogCtl).SeeIdx == 0){
					(*prvUartLogCtl).SeeIdx = (*prvUartLogCtl).RevdNo - 1;
				}
				else{
					(*prvUartLogCtl).SeeIdx--;
				}
			}
		}

		shell_show_backspace((*(*prvUartLogCtl).pTmpLogBuf).BufCount, EchoFlag, pfEcho);

		if ((*prvUartLogCtl).RevdNo > 0){
			shell_recall_oldcmd((*prvUartLogCtl).pTmpLogBuf , (u8*)&(*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).SeeIdx],
				1, (VOID*)pfEcho);

		}

		(*prvUartLogCtl).CRSTS = 0;

	}
	else if (RevData == 'B'){

		if ((*prvUartLogCtl).RevdNo > 0){
			if ((*prvUartLogCtl).SeeIdx == ((*prvUartLogCtl).RevdNo-1)){
				(*prvUartLogCtl).SeeIdx = 0;
			}
			else{
				(*prvUartLogCtl).SeeIdx++;
			}
		}

		shell_show_backspace(((*(*prvUartLogCtl).pTmpLogBuf).BufCount), 1, pfEcho);

		if ((*prvUartLogCtl).RevdNo > 0){
			shell_recall_oldcmd((*prvUartLogCtl).pTmpLogBuf, (u8*)&(*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).SeeIdx],
				1, (VOID*)pfEcho);
		}

		(*prvUartLogCtl).CRSTS = 0;

	}
}
#endif

//======================================================
//<Function>:  shell_cmd_chk
//<Usage   >:  To deal with Uart-Log input characters
//<Argus    >:  RevData         --> a received data.
//                   prvUartLogCtl  --> pointer to UART_LOG_CTL
//                   EchoFlag        --> a control flag to show msg or not.
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
u8 shell_cmd_chk(
	u8  RevData,
	UART_LOG_CTL    *prvUartLogCtl,
	u8  EchoFlag)
{
	u8  RTNSTS = 0;
	u8  SpaceCount = 0;

	//Cast the input function pointer to the actual echo function pointer type-----
	ECHOFUNC pfEcho;
	pfEcho = (ECHOFUNC)(*prvUartLogCtl).pfINPUT;
	//----------------------------------------------------------------

	if (((*(*prvUartLogCtl).pTmpLogBuf).BufCount<(UART_LOG_CMD_BUFLEN-1)) ||
	(((*(*prvUartLogCtl).pTmpLogBuf).BufCount==(UART_LOG_CMD_BUFLEN-1)) &&
	((RevData == KB_ASCII_CR)||(RevData == '\0')||
	(RevData == KB_ASCII_BS) || (RevData == KB_ASCII_BS_7F)))){

		if (RevData == 0xFF){   //invlid char, ignore it
			RTNSTS = 1;
		}

		if (RevData == KB_ASCII_LF){
			RTNSTS = 1;
		}
		else if ((RevData == KB_ASCII_CR)||(RevData == '\0')){
			RTNSTS = 2;
			(*prvUartLogCtl).CRSTS = 1;
			(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = '\0';
			if (EchoFlag){
				pfEcho((u8*)"\r\n");
			}

			if((*(*prvUartLogCtl).pTmpLogBuf).BufCount != 0)
			{
#ifdef CONFIG_UART_LOG_HISTORY
				_strcpy((u8*)&((*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).NewIdx]),
				    (const u8*)&((*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf));

				((*prvUartLogCtl).RevdNo < UART_LOG_HISTORY_LEN) ?
				((*prvUartLogCtl).RevdNo++) : ((*prvUartLogCtl).RevdNo = UART_LOG_HISTORY_LEN);

				(*prvUartLogCtl).SeeIdx = (*prvUartLogCtl).NewIdx;
				((*prvUartLogCtl).NewIdx < (UART_LOG_HISTORY_LEN-1)) ?
				((*prvUartLogCtl).NewIdx++) : ((*prvUartLogCtl).NewIdx = 0);


#endif

			}
		}
		else if ((RevData == KB_ASCII_BS) || (RevData == KB_ASCII_BS_7F)){
			RTNSTS = 1;

			if ((*(*prvUartLogCtl).pTmpLogBuf).BufCount > 0){
				(*(*prvUartLogCtl).pTmpLogBuf).BufCount--;

				shell_show_backspace(1,1,(VOID *)pfEcho);
			}
		}
		else if (RevData == KB_ASCII_TAB){  //the TAB is fixed to be one space(' ')
			RTNSTS = 1;
			for(SpaceCount=0; SpaceCount< KB_SPACENO_TAB; SpaceCount++){
				(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = ' ';
				(*(*prvUartLogCtl).pTmpLogBuf).BufCount++;
			}

			if (EchoFlag){
				pfEcho((u8 *)" ");
			}

		}
		else{
			RTNSTS = 1;
			if (EchoFlag){
				pfEcho((u8*)"%c",RevData);
			}

			(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = RevData;
			(*(*prvUartLogCtl).pTmpLogBuf).BufCount++;
		}
	}
	else if ((*(*prvUartLogCtl).pTmpLogBuf).BufCount==(UART_LOG_CMD_BUFLEN-1)){
		RTNSTS = 1;

		pfEcho((u8*)"\n\r <<<Too long cmd string.>>> \n");
		(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = '\0';
		pfEcho(&(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[0]);
	}

	return RTNSTS;
}


//======================================================
//<Function>:  shell_uart_irq_rom
//<Usage   >:  To deal with Uart-Log RX IRQ
//<Argus    >:  VOID
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
SHELL_ROM_TEXT_SECTION _LONG_CALL_
VOID shell_uart_irq_rom(VOID * Data)
{
	krhino_intrpt_enter();
	/* To avoid gcc warnings */
	( void ) Data;
	
	u8      UartReceiveData = 0;
	//For Test
	BOOL    PullMode = _FALSE;
	u32 IrqEn = LOGUART_GetIMR();

	LOGUART_SetIMR(0);

recv_again:
	UartReceiveData = LOGUART_GetChar(PullMode);

    extern int g_uart_init;
    if(g_uart_init != 0)
    {
        extern size_t uart0_rx_buffer_produce(const void *buf, size_t count);
        (void)uart0_rx_buffer_produce(&UartReceiveData, 1);
    }

	//KB_ESC chk is for cmd history, it's a special case here.
	if (UartReceiveData == KB_ASCII_ESC) {
		//4 Esc detection is only valid in the first stage of boot sequence (few seconds)
		if (shell_ctl.ExecuteEsc != _TRUE) {
			shell_ctl.ExecuteEsc = _TRUE;
			shell_ctl.EscSTS = 0;
		} else {
			//4 the input commands are valid only when the task is ready to execute commands
			if ((shell_ctl.BootRdy == 1)  ||(shell_ctl.shell_task_rdy == 1)) {
				if (shell_ctl.EscSTS==0) {
					shell_ctl.EscSTS = 1;
				}
			} else {
				shell_ctl.EscSTS = 0;
			}            
		}
	} else if (shell_ctl.EscSTS==1) {
		if (UartReceiveData != KB_ASCII_LBRKT){
			shell_ctl.EscSTS = 0;
		} else {
			shell_ctl.EscSTS = 2;
		}
	} else {
		if (shell_ctl.EscSTS==2){
			shell_ctl.EscSTS = 0;
#ifdef CONFIG_UART_LOG_HISTORY
			if ((UartReceiveData=='A')|| UartReceiveData=='B'){
				shell_cmd_history(UartReceiveData,(UART_LOG_CTL *)&shell_ctl,1);
			}
#endif
		} else{
		}
	}

	/* recv all data one time */
	if (shell_recv_all_data_onetime == 1) {
		if (LOGUART_Readable())
			goto recv_again;
	}

	LOGUART_SetIMR(IrqEn);
	krhino_intrpt_exit();
}


SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
VOID shell_init_rom(u32 TBLSz, VOID *pTBL)
{
	shell_buf.BufCount = 0;
	shell_array_init(&shell_buf.UARTLogBuf[0],UART_LOG_CMD_BUFLEN,'\0');

	shell_ctl.NewIdx = 0;
	shell_ctl.SeeIdx = 0;
	shell_ctl.RevdNo = 0;
	shell_ctl.EscSTS = 0;
	shell_ctl.BootRdy = 0;
	shell_ctl.pTmpLogBuf = &shell_buf;
#ifdef CONFIG_UART_LOG_HISTORY
	shell_ctl.CRSTS = 0;
	shell_ctl.pHistoryBuf = &shell_history_cmd[0];
#endif
	shell_ctl.pfINPUT = (VOID *)&DiagPrintf;

	if (TBLSz != 0) {
		shell_ctl.pCmdTbl = (PCOMMAND_TABLE) pTBL;
		shell_ctl.CmdTblSz = TBLSz;
	} else {
		void *PTable = NULL;
		u32 TblSize = cmd_rom_table(&PTable);
	
		shell_ctl.pCmdTbl = (PCOMMAND_TABLE) PTable;
		shell_ctl.CmdTblSz = TblSize;
	}
	shell_ctl.shell_task_rdy = 0;

	//executing boot sequence
	shell_ctl.ExecuteCmd = _FALSE;
	shell_ctl.ExecuteEsc = _FALSE;

	CONSOLE_AMEBA();
}

SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
VOID shell_task_rom(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;
	
	//4 Set this for UartLog check cmd history
#ifndef CONFIG_KERNEL   
	shell_ctl.BootRdy = 1;
#endif
	do{

		if ((shell_ctl.ExecuteCmd) == _TRUE) {            
			shell_cmd_exec_rom((PUART_LOG_CTL)&shell_ctl);
			CONSOLE_AMEBA();
			shell_ctl.ExecuteCmd = _FALSE;
		}
	}while(1);
}

SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
static BOOLEAN shell_exit(u32 MaxWaitCount)
{
	u32 WaitCount = 0;

	do{
		if (WaitCount > MaxWaitCount)
		{
			return _TRUE;// go back to the normal boot sequence
		}

		DelayUs(100);

		//        DBG_8195A("Wait %d\n",WaitCount);
		WaitCount++;

		//4 there is a ESC key input in Boot Sequence check stage
		if (shell_ctl.ExecuteEsc == _TRUE)
		{
			CONSOLE_AMEBA();

			shell_ctl.EscSTS = 0;

			break;            
		}
	}while(1);

	return _FALSE;

}


SHELL_ROM_TEXT_SECTION _LONG_CALL_ 
VOID shell_rom(u32 MaxWaitCount)
{

	if (shell_exit(MaxWaitCount)) {
		//Go back to normal boot sequence
		return;
	}

	/* rom cmd consol need this */
	LOG_MASK(LEVEL_ERROR, 0xFFFFFFFF);

	shell_ctl.BootRdy = 1;

	//4 Stay in console stage
	do{        
		if ((shell_ctl.ExecuteCmd) == _TRUE) {
			shell_cmd_exec_rom((PUART_LOG_CTL)&shell_ctl);
			CONSOLE_AMEBA();
			shell_ctl.ExecuteCmd = _FALSE;
		}
	}while(1);
}

