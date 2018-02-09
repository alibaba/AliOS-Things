/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "ameba_soc.h"
#include "monitor_lib.h"
#include "rtl_consol.h"
#include "freertos_pmu.h"

VOID WlanNormal( IN  u16 argc, IN  u8  *argv[]);

u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
CmdReboot(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	if (_strcmp(argv[0], "uartburn") == 0){
		BKUP_Set(0, BIT_UARTBURN_BOOT);
	}

	if (_strcmp(argv[0], "cpu") == 0){
		//BOOT_RAM_CPUReset();
	}

	/* set system reset happen, should clear this bit when bootup */
	BKUP_Set(0, BIT_SYS_RESET_HAPPEN);
	
	DBG_8195A("\n\rRebooting ...\n\r");
	NVIC_SystemReset();

	return _TRUE;
}

u32
CmdTickPS(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	if (_strcmp(argv[0], "r") == 0){// release
		if (_strcmp(argv[1], "debug") == 0) {
			pmu_tickless_debug(ENABLE);
		} else {
			pmu_tickless_debug(DISABLE);
		}
		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp(argv[0], "a") == 0){// acquire
		pmu_acquire_wakelock(PMU_OS);
	}
	
	if (_strcmp(argv[0], "type") == 0){// PG or CG
		if (_strcmp(argv[1], "pg") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (_strcmp(argv[1], "cg") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp(argv[0], "get") == 0){// get sleep & wake time
		pmu_get_wakelock_hold_stats(NULL);
	}

	if (_strcmp(argv[0], "clear") == 0){// clear sleep & wake time
		pmu_clean_wakelock_stat();
	}
	return _TRUE;
}

u32
CmdRTC(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	RTC_TimeTypeDef RTC_TimeStruct;

	if (_strcmp(argv[0], "get") == 0) {// dump RTC
		RTC_AlarmTypeDef RTC_AlarmStruct_temp;
		RTC_TypeDef* RTC = ((RTC_TypeDef *) RTC_BASE);
		
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		RTC_GetAlarm(RTC_Format_BIN, &RTC_AlarmStruct_temp);

		DBG_8195A("time: %d:%d:%d:%d (%d) \n", RTC_TimeStruct.RTC_Days,
			RTC_TimeStruct.RTC_Hours,
			RTC_TimeStruct.RTC_Minutes,
			RTC_TimeStruct.RTC_Seconds,
			RTC_TimeStruct.RTC_H12_PMAM);
		
		DBG_8195A("alarm: %d:%d:%d:%d (%d) \n", RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Days,
			RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Hours,
			RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Minutes,
			RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Seconds,
			RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_H12_PMAM);
		
		DBG_8195A("ISR: %x \n", RTC->ISR);
		DBG_8195A("ALMR1: %x \n", RTC->ALMR1);
		DBG_8195A("ALMR2: %x \n", RTC->ALMR2);
		DBG_8195A("OSC8M: %d \n", OSC8M_Get());
	}

	if (_strcmp(argv[0], "set") == 0) {
		RTC_TimeStructInit(&RTC_TimeStruct);
		RTC_TimeStruct.RTC_Hours = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
		RTC_TimeStruct.RTC_Minutes = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
		RTC_TimeStruct.RTC_Seconds = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);

		if (_strcmp(argv[5], "pm") == 0) {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_PM;
		} else {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_AM;
		}
		
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}
	
	return _TRUE;
}

u32
CmdWlanNormal(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
#if SUPPORT_INTERACTIVE_MODE

	if(argc<1)
	{
		DBG_8195A("Wrong argument number!\r\n");
		return _FALSE;
	}


	DBG_8195A("Wlan Normal Mode\n");

	WlanNormal( argc, argv);
#endif

	return _TRUE;
}

u32
CmdUartLog(
    IN  u16 argc,
    IN  u8  *argv[]
)
{	
	if (_strcmp(argv[0], "set") == 0) {
		u32 rate = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
		u32 index = 0;

		DBG_8195A("UlogRate: %d, reboot after set baudrate \n", rate);

		while (1) {
			if (0xffffffff == BAUDRATE_TABLE_40M[index][0]) {
				DBG_8195A("UlogRate: Err \n");
				break;
			}
			
			if (rate == BAUDRATE_TABLE_40M[index][0]) {
				FLASH_EreaseDwordsXIP(FLASH_SYSTEM_DATA_ADDR + 0x30, 1);
				FLASH_TxData12BXIP(FLASH_SYSTEM_DATA_ADDR + 0x30, 4, (u8*)&rate);
		
				break;
			}
			
			index++;
		}
	} else {
		u32 time = 0;
		u32 delta = 0;
	
		time=SYSTIMER_TickGet();
		DBG_8195A("123456789a123456789\n");
		delta = SYSTIMER_TickGet() - time;

		/* tick is 30.5 us */
		DBG_8195A("BaudRate: %d K\n", 200*1000*10/(delta*305));
	}

	return _TRUE;
}

static int
cmd_stricmp(const char* str1, const char* str2)
{
	char c1, c2;

	do {
		c1 = *str1++;
		c2 = *str2++;
		if (c1 != c2) {
			char c1_upc = c1 | 0x20;
			if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
				/* characters are not equal an one is in the alphabet range:
				downcase both chars and check again */
				char c2_upc = c2 | 0x20;
				if (c1_upc != c2_upc) {
					/* still not equal */
					/* don't care for < or > */
					return 1;
				}
			} else {
				/* characters are not equal but none is in the alphabet range */
				return 1;
			}
		}
	} while (c1 != 0);
	return 0;
}

const COMMAND_TABLE   UartLogRamCmdTable[] = {
#if (CONFIG_INIC_EN == 0)
	{(const u8*)"?",		0, CmdRamHelp,		(const u8*)"\tHELP (?) : \n"
											"\t\t Print help messag\n"},
	{(const u8*)"DW",		2, CmdDumpWord,	(const u8*)"\tDW <Address, Hex>\n"
											"\t\t Dump memory dword or Read Hw dword register; \n"
											"\t\t Can Dump only one dword at the same time \n"
											"\t\t Unit: 4Bytes \n"},
	{(const u8*)"EW",		2, CmdWriteWord,	(const u8*)"\tEW <Address, Hex>\n"
											"\t\t Write memory dword or Write Hw dword register \n"
											"\t\t Can write only one dword at the same time \n"
											"\t\t Ex: EW Address Value \n"},

	{(const u8*)"DB",		2, CmdDumpByte,	(const u8*)"\tDW <Address, Hex>\n"
											"\t\t Dump memory byte or Read Hw byte register; \n"
											"\t\t Can Dump only one byte at the same time \n"
											"\t\t Unit: 1Bytes \n"},
	{(const u8*)"EB",		2, CmdWriteByte,		(const u8*)"\tEW <Address, Hex>\n"
											"\t\t Write memory byte or Write Hw byte register \n"
											"\t\t Can write only one byte at the same time \n"
											"\t\t Ex: EW Address Value \n"},
	{(const u8*)"FLASH",	8, CmdFlash,			(const u8*)"\tFLASH \n"
											"\t\t erase chip \n"
											"\t\t erase sector addr \n"
											"\t\t erase block addr \n"
											"\t\t read addr len \n"
											"\t\t write addr data \n"},
	{(const u8*)"EFUSE",	8, CmdEfuse,			(const u8*)"\tEFUSE \n"
											"\t\t wmap addr len data\n"
											"\t\t rmap \n"},
	{(const u8*)"REBOOT",	4, CmdReboot,		(const u8*)"\tREBOOT \n"
											"\t\t reboot \n"
											"\t\t reboot uartburn \n"},
	{(const u8*)"TICKPS",	4, CmdTickPS,		(const u8*)"\tTICKPS \n"
											"\t\t r: release os wakelock \n"
											"\t\t a: acquire os wakelock \n"},
	{(const u8*)"RTC",		4, CmdRTC,			(const u8*)"\tRTC \n"
											"\t\t get\n"},
	{(const u8*)"WLAN",	4, CmdWlanNormal,	(const u8*)"\tWLAN \n"
											"\t\t WLAN Driver Cmd\n"},
	{(const u8*)"ULOG",	4, CmdUartLog,	(const u8*)"\tULOG \n"
											"\t\t set xxxx (like 3000000)\n"},
#endif
};

monitor_cmd_handler Get_HalCmd(char* argv)
{
	u32 CmdCnt = 0;
	u32 CmdNum = (sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE));
	monitor_cmd_handler cmd_handler = NULL;
	
	for (CmdCnt = 0; CmdCnt< CmdNum; CmdCnt++) {          
		if ((cmd_stricmp(argv, UartLogRamCmdTable[CmdCnt].cmd))==0){
			cmd_handler = UartLogRamCmdTable[CmdCnt].func;
			break;
		}
	}

	return cmd_handler;
}

u32
GetRamCmdNum(
    VOID
)
{
	return (sizeof(UartLogRamCmdTable)/sizeof(COMMAND_TABLE));
}

//======================================================
//<Function>:  CmdTest
//<Usage   >:  This function is a demo test function.
//<Argus    >:  argc --> number of argus
//                   argv --> pointer to a cmd parameter array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================

u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	u32	LoopINdex ;

	DBG_8195A("----------------- COMMAND MODE HELP ------------------\n");
	for( LoopINdex=0  ; LoopINdex < (sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE)) ; LoopINdex++ )
	{
		if( UartLogRamCmdTable[LoopINdex].msg )
		{
			DBG_8195A( "%s\n",UartLogRamCmdTable[LoopINdex].msg );
		}
	}
	DBG_8195A("----------------- COMMAND MODE END  ------------------\n");

	return _TRUE ;
}

