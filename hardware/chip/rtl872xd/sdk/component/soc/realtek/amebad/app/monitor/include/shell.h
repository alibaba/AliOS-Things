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
#define MAX_ARGV                16

typedef u32 (*ECHOFUNC)(IN u8*,...);    //UART LOG echo-function type.
typedef u32 (*monitor_cmd_handler)(u16 argc, u8* argv[]);

typedef struct {
        u8  BufCount;                           //record the input cmd char number.
        u8  UARTLogBuf[UART_LOG_CMD_BUFLEN];   //record the input command.
} UART_LOG_BUF, *PUART_LOG_BUF;

typedef struct _COMMAND_TABLE_ {
    const   u8* cmd;
    u16     ArgvCnt;
    u32     (*func)(u16 argc, u8* argv[]);
    const   u8* msg;
}COMMAND_TABLE, *PCOMMAND_TABLE;


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
        u32  CRSTS;
#ifdef CONFIG_UART_LOG_HISTORY
        u8  (*pHistoryBuf)[UART_LOG_CMD_BUFLEN];
#endif

	void (*GiveSema)(void);
	u32 shell_task_rdy;
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

//#ifdef CONFIG_DEBUG_LOG
#define _ConsolePrint  DiagPrintf
//#else
//#define _ConsolePrint
//#endif

#define AMEBA_CONSOLE_PREFIX	"#"
#define CONSOLE_AMEBA(...)     do {\
	_ConsolePrint("\r"AMEBA_CONSOLE_PREFIX __VA_ARGS__);\
}while(0)

_LONG_CALL_ VOID shell_init_rom(u32 TBLSz, VOID *pTBL);
_LONG_CALL_ VOID shell_task_rom(VOID *Data);
_LONG_CALL_ VOID shell_rom(u32 MaxWaitCount);
_LONG_CALL_ void shell_uart_irq_rom(void * Data);

_LONG_CALL_
extern VOID
shell_cmd_history(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);
_LONG_CALL_
extern u8
shell_cmd_chk(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);

_LONG_CALL_
extern VOID
shell_array_init(
    IN  u8  *pArrayToInit,
    IN  u8  ArrayLen,
    IN  u8  InitValue
);
extern u8** shell_get_argv(const   u8  *string);
extern u8 shell_get_argc(const   u8  *string);

VOID shell_init_ram(VOID);
void shell_switch_ipc_int(VOID *Data, u32 IrqStatus, u32 ChanNum);

#define RtlConsolTaskRom		shell_task_rom

extern u32 shell_recv_all_data_onetime;
extern u32 shell_interrupt_on;
#endif //_RTK_CONSOL_H_
