/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_CONSOL_H_
#define _RTK_CONSOL_H_

//Log UART
//UART_LOG_CMD_BUFLEN: only 126 bytes could be used for keeping input
//                                     cmd, the last byte is for string end ('\0').
#define UART_LOG_CMD_BUFLEN     127     
#define MAX_ARGV                10



typedef u32 (*ECHOFUNC)(IN u8*,...);    //UART LOG echo-function type.

typedef struct {
        u8  BufCount;                           //record the input cmd char number.
        u8  UARTLogBuf[UART_LOG_CMD_BUFLEN];   //record the input command.
} UART_LOG_BUF, *PUART_LOG_BUF;



typedef struct {
        u8  NewIdx;
        u8  SeeIdx;
        u8  RevdNo;
        u8  EscSTS;
        u8  ExecuteCmd;
        u8  ExecuteEsc;
        u8  BootRdy;
        u8  Resvd;
        PUART_LOG_BUF   pTmpLogBuf;        
        VOID *pfINPUT;
        PCOMMAND_TABLE  pCmdTbl;
        u32 CmdTblSz;
#ifdef CONFIG_UART_LOG_HISTORY        
        u32  CRSTS;
#endif        
#ifdef CONFIG_UART_LOG_HISTORY
        u8  (*pHistoryBuf)[UART_LOG_CMD_BUFLEN];
#endif
} UART_LOG_CTL, *PUART_LOG_CTL;


#define KB_ASCII_NUL    0x00
#define KB_ASCII_BS     0x08
#define KB_ASCII_TAB    0x09
#define KB_ASCII_LF     0x0A
#define KB_ASCII_CR     0x0D
#define KB_ASCII_ESC    0x1B
#define KB_ASCII_SP     0x20
#define KB_ASCII_BS_7F  0x7F
#define KB_ASCII_LBRKT  0x5B         //[

#define KB_SPACENO_TAB  1

#ifdef CONFIG_UART_LOG_HISTORY
#define UART_LOG_HISTORY_LEN    5
#endif

#ifdef CONFIG_DEBUG_LOG
#define _ConsolePrint  DiagPrintf
#else
#define _ConsolePrint
#endif

#define CONSOLE_PREFIX	"<RTL8195A>"

#define CONSOLE_8195A(...)     do {\
    _ConsolePrint("\r"CONSOLE_PREFIX __VA_ARGS__);\
}while(0)

#define AMEBA_CONSOLE_PREFIX	"#"
#define CONSOLE_AMEBA(...)     do {\
	_ConsolePrint("\r"AMEBA_CONSOLE_PREFIX __VA_ARGS__);\
}while(0)

_LONG_CALL_ VOID RtlConsolInit(u32 Boot, u32 TBLSz, VOID *pTBL);
_LONG_CALL_ VOID RtlConsolTaskRom(VOID *Data);
_LONG_CALL_ u32 Strtoul(const u8 *nptr, u8 **endptr, u32 base);
_LONG_CALL_ u32 GetRomCmdNum(VOID);
_LONG_CALL_ VOID UartLogCmdExecute(PUART_LOG_CTL   pUartLogCtlExe);
_LONG_CALL_ VOID RtlConsolRom(u32 MaxWaitCount);
_LONG_CALL_ void LOGUART_SetBaud_FromFlash(void);
#endif //_RTK_CONSOL_H_
