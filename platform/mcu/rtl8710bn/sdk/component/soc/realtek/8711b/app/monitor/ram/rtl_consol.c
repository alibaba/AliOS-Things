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
#include "rtl_consol.h"
#include "strproc.h"

extern volatile UART_LOG_CTL    UartLogCtl;
extern volatile UART_LOG_CTL    *pUartLogCtl;
extern u8                       *ArgvArray[MAX_ARGV];
extern UART_LOG_BUF              UartLogBuf;

extern COMMAND_TABLE    UartLogRamCmdTable[];
#ifdef CONFIG_UART_LOG_HISTORY
extern u8  UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN];
#endif

#ifdef CONFIG_KERNEL
u32		Consol_TaskRdy;
_sema	Consol_Sema;
#else
// Since ROM code will reference this typedef, so keep the typedef same size
u32		Consol_TaskRdy;
void		*Consol_Sema;
#endif

extern monitor_cmd_handler Get_HalCmd(char* argv);

_LONG_CALL_
extern u8
UartLogCmdChk(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);

_LONG_CALL_
extern VOID
ArrayInitialize(
    IN  u8  *pArrayToInit,
    IN  u8  ArrayLen,
    IN  u8  InitValue
);

_LONG_CALL_
extern VOID
UartLogHistoryCmd(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);

//=================================================


/* Minimum and maximum values a `signed long int' can hold.
   (Same as `int').  */
#ifndef __LONG_MAX__
#if defined (__alpha__) || (defined (__sparc__) && defined(__arch64__)) || defined (__sparcv9) || defined (__s390x__)
#define __LONG_MAX__ 9223372036854775807L
#else
#define __LONG_MAX__ 2147483647L
#endif /* __alpha__ || sparc64 */
#endif
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif




//======================================================
//<Function>:  UartLogIrqHandleRam
//<Usage   >:  To deal with Uart-Log RX IRQ
//<Argus    >:  VOID
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
VOID
UartLogIrqHandleRam
(
    VOID * Data
)
{
    u8      UartReceiveData = 0;
    //For Test
    BOOL    PullMode = _FALSE;

    u32 IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    UartReceiveData = DiagGetChar(PullMode);

    //KB_ESC chk is for cmd history, it's a special case here.
    if (UartReceiveData == KB_ASCII_ESC) {
        //4 Esc detection is only valid in the first stage of boot sequence (few seconds)
        if (pUartLogCtl->ExecuteEsc != _TRUE)
        {
            pUartLogCtl->ExecuteEsc = _TRUE;
            (*pUartLogCtl).EscSTS = 0;
        }
        else
        {
            //4 the input commands are valid only when the task is ready to execute commands
            if ((pUartLogCtl->BootRdy == 1)  
#ifdef CONFIG_KERNEL 
                ||(Consol_TaskRdy == 1)
#endif          
            )
            {
                if ((*pUartLogCtl).EscSTS==0)
                {
                    (*pUartLogCtl).EscSTS = 1;
                }
            }
            else
            {
                (*pUartLogCtl).EscSTS = 0;
            }            
        }
    }
    else if ((*pUartLogCtl).EscSTS==1){
        if (UartReceiveData != KB_ASCII_LBRKT){
            (*pUartLogCtl).EscSTS = 0;
        }
        else{
            (*pUartLogCtl).EscSTS = 2;
        }
    }

    else{
        if ((*pUartLogCtl).EscSTS==2){
            (*pUartLogCtl).EscSTS = 0;
#ifdef CONFIG_UART_LOG_HISTORY
            if ((UartReceiveData=='A')|| UartReceiveData=='B'){
                UartLogHistoryCmd(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1);
            }
#endif
        }
        else{
            if (UartLogCmdChk(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1)==2)
            {
                //4 check UartLog buffer to prevent from incorrect access
                if (pUartLogCtl->pTmpLogBuf != NULL)
                {
                    pUartLogCtl->ExecuteCmd = _TRUE;
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
    				if (Consol_TaskRdy)
    					rtw_up_sema_from_isr((_sema *)&Consol_Sema);
#endif
                }
                else
                {
                    ArrayInitialize((u8 *)pUartLogCtl->pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
                }
            }
        }
    }
    DiagSetIsrEnReg(IrqEn);

}



VOID
RtlConsolInitRam(
    IN  u32     Boot,
    IN  u32     TBLSz,
    IN  VOID    *pTBL    
)
{
    u32 stack_size = 0;

    UartLogBuf.BufCount = 0;
    ArrayInitialize(&UartLogBuf.UARTLogBuf[0],UART_LOG_CMD_BUFLEN,'\0');
    pUartLogCtl = &UartLogCtl;
    
    pUartLogCtl->NewIdx = 0;
    pUartLogCtl->SeeIdx = 0;
    pUartLogCtl->RevdNo = 0;
    pUartLogCtl->EscSTS = 0;
    pUartLogCtl->BootRdy = 0;
    pUartLogCtl->pTmpLogBuf = &UartLogBuf;
#ifdef CONFIG_UART_LOG_HISTORY
    pUartLogCtl->CRSTS = 0;
    pUartLogCtl->pHistoryBuf = &UartLogHistoryBuf[0];
#endif
    pUartLogCtl->pfINPUT = (VOID*)&DiagPrintf;
    pUartLogCtl->pCmdTbl = (PCOMMAND_TABLE) pTBL;
    pUartLogCtl->CmdTblSz = TBLSz;
#ifdef CONFIG_KERNEL    
    Consol_TaskRdy = 0;
#endif
    //executing boot sequence
    if (Boot == ROM_STAGE)
    {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc = _FALSE;
    }
    else
    {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc= _TRUE;//don't check Esc anymore
#if defined(CONFIG_KERNEL)
        /* Create a Semaphone */
        rtw_init_sema((_sema*)&(Consol_Sema), 0);
        Consol_TaskRdy = 0;

        stack_size = 512;
#if defined (__GNUC__) /* for Verification need more stack */
        stack_size += 1024;
#endif

#if defined( PLATFORM_FREERTOS)
        if (pdTRUE != xTaskCreate( RtlConsolTaskRam, (const char * const)"LOGUART_TASK", stack_size, NULL, tskIDLE_PRIORITY + 5 , NULL))
        {
            DiagPrintf("Create Log UART Task Err!!\n");
        }
#elif defined( PLATFORM_ALIOS)
        ktask_t *g_log_uart_task;
        if (0 != krhino_task_dyn_create(&g_log_uart_task, "LOGUART_TASK", 0, 6, 0, stack_size, (task_entry_t)RtlConsolTaskRam, 1))
        {
            DiagPrintf("Create Log UART Task Err!!\n");
        }
#endif


#endif    
    }	

    CONSOLE_AMEBA();
}

extern u8** GetArgv(const u8 *string);
#if SUPPORT_LOG_SERVICE
extern char log_buf[LOG_SERVICE_BUFLEN];
extern _sema log_rx_interrupt_sema;
#endif
//======================================================
u32 console_cmd_exec(u8  argc, u8  **argv)
{
#if defined(CONFIG_WIFI_NORMAL)
	if(argc == 0) {
		return FALSE;
	}
	
#if SUPPORT_LOG_SERVICE
	rtw_up_sema((_sema *)&log_rx_interrupt_sema);
#endif
	ArrayInitialize(argv[0], sizeof(argv[0]) ,0);
	
	//(*pUartLogBuf).BufCount = 0;
	//ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');

	return TRUE;
#endif
}
//======================================================
// overload original RtlConsolTaskRam

u32
UartLogCmdExecuteRam(
	u8  argc, u8  **argv
){
	monitor_cmd_handler cmd_handler = NULL;

	if (argc > 0){
		cmd_handler = Get_HalCmd(argv[0]);

#if 0
		if (cmd_handler == NULL) {
			cmd_handler = Get_TestCmd(argv[0]);
		}
#endif

		if (cmd_handler != NULL) {
			StrUpr(argv[0]);
			cmd_handler((argc-1) , (argv+1));
			ArrayInitialize(argv[0], sizeof(argv[0]) ,0);
			return TRUE;
		}
	}

	//(*pUartLogBuf).BufCount = 0;
	//ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');

	return FALSE;
}


VOID
RtlConsolTaskRam(
    VOID *Data
)
{
	u32 ret = TRUE;
	
#if defined(CONFIG_WIFI_NORMAL)
#if SUPPORT_LOG_SERVICE
	log_service_init();
#endif
#endif

	//4 Set this for UartLog check cmd history
#ifdef CONFIG_KERNEL
	Consol_TaskRdy = 1;
#endif
#ifndef CONFIG_KERNEL   
	pUartLogCtl->BootRdy = 1;
#endif
	do{
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
		rtw_down_sema((_sema *)&Consol_Sema);
#endif
		if (pUartLogCtl->ExecuteCmd) {
			u8  argc = 0;
			u8  **argv;
			PUART_LOG_BUF   pUartLogBuf = pUartLogCtl->pTmpLogBuf;
#if defined(CONFIG_WIFI_NORMAL)
#if SUPPORT_LOG_SERVICE
			_strncpy(log_buf, (const u8*)&(*pUartLogBuf).UARTLogBuf[0], LOG_SERVICE_BUFLEN-1);
#endif
#endif
			argc = GetArgc((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
			argv = GetArgv((const u8*)&((*pUartLogBuf).UARTLogBuf[0])); /* UARTLogBuf will be changed */

			if (argc > 0) {
				/* FPGA Verification */
				ret = UartLogCmdExecuteRam(argc, argv);

				/* normal for LOG service */
				if (ret == FALSE) {
					ret = console_cmd_exec(argc, argv);
				}

				(*pUartLogBuf).BufCount = 0;
				ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
			} else {
				/*In some exception case, even if argc parsed is 0(when the first character value in log buffer is '\0'), 
				log buffer may not be empty and log buffer counter may not be zero. If not clean log buffer and counter
				, some error will happen. Therefore, clean log buffer and initialize buffer counter when it occurs.*/
				if((*pUartLogBuf).BufCount != 0) {
					(*pUartLogBuf).BufCount = 0;
					ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
				}
				CONSOLE_AMEBA();
			}
			pUartLogCtl->ExecuteCmd = _FALSE;

			pmu_set_sysactive_time(10000);
		}
	}while(1);
}

/**
  * @brief    Set UartLog Baud Rate use baudrate val.
  * @param  BaudRate: Baud Rate Val, like 115200 (unit is HZ).
  * @retval  True/False
  */
int LOGUART_SetBaud(u32 BaudRate)
{
	UART_INTConfig(UART2_DEV, RUART_IER_ERBI | RUART_IER_ELSI, DISABLE);
	UART_RxCmd(UART2_DEV, DISABLE);

	while (UART_Writable(UART2_DEV) == 0);
	
	UART_SetBaud(UART2_DEV, BaudRate);
	
	UART_INTConfig(UART2_DEV, RUART_IER_ERBI | RUART_IER_ELSI, ENABLE);
	UART_RxCmd(UART2_DEV, ENABLE);
	
	return _TRUE;
}

void LOGUART_SetBaud_FromFlash(void)
{
	SYSTEM_DATA *SysData = (SYSTEM_DATA *)(SPI_FLASH_BASE + FLASH_SYSTEM_DATA_ADDR);

	/* 0xFFFFFFFF is 115200 */
	if (SysData->UlogRate != 0xFFFFFFFF) {
		DBG_8195A("LOGUART_SetBaud %d \n\n", SysData->UlogRate);
		LOGUART_SetBaud(SysData->UlogRate);
	}
}

VOID ReRegisterPlatformLogUart(VOID)
{
	DIAG_UartReInit((IRQ_FUN) UartLogIrqHandleRam);
	NVIC_SetPriority(UART_LOG_IRQ, 10); /* this is rom_code_patch */

	LOGUART_SetBaud_FromFlash();

#if !TASK_SCHEDULER_DISABLED  
	RtlConsolInitRam((u32)RAM_STAGE,(u32)GetRamCmdNum(),(VOID*)UartLogRamCmdTable);
#else
	RtlConsolInitRam((u32)ROM_STAGE,(u32)GetRamCmdNum(),(VOID*)UartLogRamCmdTable);
#endif


}
