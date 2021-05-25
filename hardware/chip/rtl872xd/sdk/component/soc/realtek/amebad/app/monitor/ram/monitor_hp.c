/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "ameba_soc.h"
#include "freertos_pmu.h"
#include "rom_map.h"

KM4SLEEP_ParamDef dsleep_param;

u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
CmdShellSwitch(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;

	InterruptDis(UART_LOG_IRQ);	
	ipc_send_message(IPC_INT_CHAN_SHELL_SWITCH, NULL);

	return 0;
}

u32
cmd_reboot(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	DBG_8195A("\n\rRebooting ...\n\r");

	if (_strcmp((const char*)argv[0], "uartburn") == 0){

		BKUP_Set(0, BIT_UARTBURN_BOOT);
		
		WDG_InitTypeDef WDG_InitStruct;
		u32 CountProcess;
		u32 DivFacProcess;

		WDG_Scalar(5, &CountProcess, &DivFacProcess);
		WDG_InitStruct.CountProcess = CountProcess;
		WDG_InitStruct.DivFacProcess = DivFacProcess;
		WDG_Init(&WDG_InitStruct);

		WDG_Cmd(ENABLE);

		return _TRUE;
	}

	/* Let KM4 RUN */
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG);
	Temp |= BIT_SOC_BOOT_PATCH_KM4_RUN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG, Temp);

	/* free loguart */
	InterruptDis(UART_LOG_IRQ);	
	ipc_send_message(IPC_INT_CHAN_SHELL_SWITCH, NULL);

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG);
	Temp |= BIT_SOC_BOOT_WAKE_FROM_PS_HS;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG, Temp);
	
	SOCPS_CPUReset();	

	return _TRUE;
}

u32
CmdTickPS(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	if (_strcmp((const char*)argv[0], "r") == 0){// release
		if (argc >=2 && _strcmp((const char*)argv[1], "debug") == 0) {
			pmu_tickless_debug(ENABLE);
		} else {
			pmu_tickless_debug(DISABLE);
		}
		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp((const char*)argv[0], "a") == 0){// acquire
		pmu_acquire_wakelock(PMU_OS);
	}
	
	if (_strcmp((const char*)argv[0], "type") == 0){// PG or CG
		if (argc >=2 && _strcmp((const char*)argv[1], "pg") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (argc >=2  && _strcmp((const char*)argv[1], "cg") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp((const char*)argv[0], "dslp") == 0){
		u32 duration_ms = _strtoul((const char*)(argv[1]), (char **)NULL, 10);

		dsleep_param.dlps_enable = TRUE;
		dsleep_param.sleep_time = duration_ms;

		ipc_send_message(IPC_INT_KM4_TICKLESS_INDICATION, (u32)&dsleep_param);

		while (1);
	}

	if (_strcmp((const char*)argv[0], "get") == 0){// get sleep & wake time
		DBG_8195A("lockbit:%x \n", pmu_get_wakelock_status());
		DBG_8195A("dslp_lockbit:%x\n", pmu_get_deepwakelock_status());
	}

	return _TRUE;
}

u32
CmdRTC(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	
	RTC_TimeTypeDef RTC_TimeStruct;

	if (_strcmp((const char*)argv[0], "get") == 0) {// dump RTC
		RTC_AlarmTypeDef RTC_AlarmStruct_temp;

		
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
	}

	if (_strcmp((const char*)argv[0], "set") == 0) {
		RTC_TimeStructInit(&RTC_TimeStruct);
		RTC_TimeStruct.RTC_Hours = _strtoul((const char*)(argv[1]), (char **)NULL, 10);
		RTC_TimeStruct.RTC_Minutes = _strtoul((const char*)(argv[2]), (char **)NULL, 10);
		RTC_TimeStruct.RTC_Seconds = _strtoul((const char*)(argv[3]), (char **)NULL, 10);

		if (_strcmp((const char*)argv[5], "pm") == 0) {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_PM;
		} else {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_AM;
		}
		
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}
	
	return _TRUE;
}

u32
CmdLogBuf(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	
	if (_strcmp((const char*)argv[0], "on") == 0) {
		ConfigDebugBuffer = 1;
	} else {
		ConfigDebugBuffer = 0;
	}

	DiagPrintf("AAAAA\n");
	DiagPrintfD("BBBBB\n");
		
	return _TRUE;
}

u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	( void ) argc;
	
	u8 EfuseBuf[1024];
	u32 index;
	u8 ret = 0;

	/* efuse wmap 0x0 2 2187 */
	/* efuse wmap 0x18 4 01020304 */
	if (_strcmp((const char*)argv[0], "wmap") == 0) {
		u32 Addr = _strtoul((const char*)(argv[1]), (char **)NULL, 16);
		u32 Len = _strtoul((const char*)(argv[2]), (char **)NULL, 16);
		char* DString = (char*)argv[3];
		u32 Cnt;

		Cnt = _strlen(DString);
		if (Cnt%2) {
			MONITOR_LOG("string length(%d) should be odd \n", Cnt);
			return FALSE;
		}

		Cnt = Cnt/2;
		if (Cnt != Len) {
			MONITOR_LOG("Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt:Len;
		}

		MONITOR_LOG("efuse wmap write len:%d, string len:%d\n", Len, Cnt << 1);

		for (index= 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index*2], DString[index*2+1]);
		}
		
		EFUSE_LMAP_WRITE(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char*)argv[0], "rmap") == 0) {
		MONITOR_LOG("efuse rmap \n");
		
		ret = EFUSE_LMAP_READ(EfuseBuf);
		if (ret == _FAIL) {
			MONITOR_LOG("EFUSE_LogicalMap_Read fail \n");
		}

		for (index = 0; index < 1024; index+=16) {
			MONITOR_LOG("EFUSE[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
				EfuseBuf[index], EfuseBuf[index+1], EfuseBuf[index+2], EfuseBuf[index+3],
				EfuseBuf[index+4], EfuseBuf[index+5], EfuseBuf[index+6], EfuseBuf[index+7],
				EfuseBuf[index+8], EfuseBuf[index+9], EfuseBuf[index+10], EfuseBuf[index+11],
				EfuseBuf[index+12], EfuseBuf[index+13], EfuseBuf[index+14], EfuseBuf[index+15]);
		}
	}

	if (_strcmp((const char*)argv[0], "rraw") == 0) {
		MONITOR_LOG("efuse rraw\n");
	
		for (index = 0; index< EFUSE_REAL_CONTENT_LEN; index++) {
			if ((index < EFUSE_SECURE_START) || (index > EFUSE_SECURE_END)) {
				EFUSE_PMAP_READ8(0, index, EfuseBuf + index, L25EOUTVOLTAGE);
			} else {
				EfuseBuf[index] = 0xFF;
			}
		}

		for (index= 0; index < EFUSE_REAL_CONTENT_LEN; index+=16) {
			MONITOR_LOG("RawMap[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
				EfuseBuf[index], EfuseBuf[index+1], EfuseBuf[index+2], EfuseBuf[index+3],
				EfuseBuf[index+4], EfuseBuf[index+5], EfuseBuf[index+6], EfuseBuf[index+7],
				EfuseBuf[index+8], EfuseBuf[index+9], EfuseBuf[index+10], EfuseBuf[index+11],
				EfuseBuf[index+12], EfuseBuf[index+13], EfuseBuf[index+14], EfuseBuf[index+15]);
		}
	}

	/* efuse wraw 0xA0 1 aa */
	/* efuse wraw 0xA0 2 aabb */
	/* efuse wraw 0xA0 4 aabbccdd */
	if (_strcmp((const char*)argv[0], "wraw") == 0) {
		u32 Addr = _strtoul((const char*)(argv[1]), (char **)NULL, 16);
		u32 Len = _strtoul((const char*)(argv[2]), (char **)NULL, 16);
		char* DString = (char*)argv[3];
		u32 Cnt;

		Cnt = _strlen(DString);
		if (Cnt%2) {
			MONITOR_LOG("string length(%d) should be odd \n", Cnt);
			return FALSE;
		}

		Cnt = Cnt/2;
		if (Cnt != Len) {
			MONITOR_LOG("Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt:Len;
		}
	
		for (index= 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index*2], DString[index*2+1]);
		}		

		MONITOR_LOG("efuse wraw write len:%d, string len:%d\n", Len, Cnt << 1);
		
		for (index = 0; index < Len; index++) {
			MONITOR_LOG("wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			EFUSE_PMAP_WRITE8(0, Addr + index, EfuseBuf[index], L25EOUTVOLTAGE);
		}
	}

	return 0;
}

u32
CmdCloseUltraLowPower(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	wifi_config.wifi_ultra_low_power = FALSE;

	return _TRUE;
}

u32
CmdTsfTest(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	
	u32 temp;
	if (_strcmp((const char*)argv[0], "read") == 0){// release
		if (_strcmp((const char*)argv[1], "560") == 0) {
			DBG_8195A("0x40080560=%x\n",HAL_READ32(WIFI_REG_BASE,0x560));
		} else if (_strcmp((const char*)argv[1], "568") == 0) {
			DBG_8195A("0x40080568=%x\n",HAL_READ32(WIFI_REG_BASE,0x568));
		} else if (_strcmp((const char*)argv[1], "all") == 0) {
			DBG_8195A("0x40080560=%x 0x40080568=%x\n",HAL_READ32(WIFI_REG_BASE,0x560), HAL_READ32(WIFI_REG_BASE,0x568));
		}
	}
	if (_strcmp((const char*)argv[0], "set") == 0){// release
		temp = HAL_READ32(WIFI_REG_BASE,0x560);
		HAL_WRITE32(WIFI_REG_BASE,0x568,temp);
		DBG_8195A("0x40080560=%x 0x40080568=%x\n",HAL_READ32(WIFI_REG_BASE,0x560), HAL_READ32(WIFI_REG_BASE,0x568));
	}

	return 0;
}
	

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   shell_cmd_table[] = {
	{(const u8*)"?",		0, CmdRamHelp,		(const u8*)"\tHELP (?) : \n"
											"\t\t Print help messag\n"},
	{(const u8*)"DW",		2, cmd_dump_word,	(const u8*)"\tDW <Address, Hex>\n"
											"\t\t Dump memory dword or Read Hw dword register; \n"
											"\t\t Can Dump only one dword at the same time \n"
											"\t\t Unit: 4Bytes \n"},
	{(const u8*)"EW",		2, cmd_write_word,	(const u8*)"\tEW <Address, Hex>\n"
											"\t\t Write memory dword or Write Hw dword register \n"
											"\t\t Can write only one dword at the same time \n"
											"\t\t Ex: EW Address Value \n"},
	{(const u8*)"FLASH",	8, cmd_flash,			(const u8*)"\tFLASH \n"
											"\t\t erase chip \n"
											"\t\t erase sector addr \n"
											"\t\t erase block addr \n"
											"\t\t read addr len \n"
											"\t\t write addr data \n"},
	{(const u8*)"EFUSE",	8, cmd_efuse_protect,			(const u8*)"\tEFUSE \n"
											"\t\t wmap addr len data\n"
											"\t\t rmap \n"
											"\t\t <wmap 0x00 2 8195> efuse[0]=0x95, efuse [1]=0x81\n"
											"\t\t <wmap 0xF0 4 11223344> [0xF0]=0x22, [0xF1]=0x11, [0xF2]=0x444, [0xF3]=0x33\n"},
	{(const u8*)"REBOOT",	4, cmd_reboot,		(const u8*)"\tREBOOT \n"
											"\t\t reboot \n"
											"\t\t reboot uartburn \n"},
	{(const u8*)"TICKPS",	4, CmdTickPS,		(const u8*)"\tTICKPS \n"
											"\t\t r: release os wakelock \n"
											"\t\t a: acquire os wakelock \n"},
	{(const u8*)"RTC",		4, CmdRTC,			(const u8*)"\tRTC \n"
											"\t\t get\n"},
	{(const u8*)"APTEST",	4, CmdCloseUltraLowPower,	(const u8*)"\tAPTEST \n"
												"\t\t get\n"},
	{(const u8*)"TSFTEST",	4, CmdTsfTest,	(const u8*)"\tTSFTEST \n"
												"\t\t get\n"},
	{(const u8*)">>",		4, CmdShellSwitch,	(const u8*)"\t>> \n"
											"\t\t switch shell to KM0 \n"},
	{(const u8*)"LOGBUF",	4, CmdLogBuf,	(const u8*)"\tLOGBUF \n"
											"\t\t KM0 help to print KM4 log\n"},
};

u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) argv;
	
	COMMAND_TABLE* cmd_table = (COMMAND_TABLE*)__cmd_table_start__;
	u32 cmd_mum = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));
	u32	index ;

	DiagPrintfD("----------------- TEST COMMAND MODE HELP %d [%08x:%08x]------------------\n",
		cmd_mum, __cmd_table_start__, __cmd_table_end__);
	for( index=0  ; index < cmd_mum; index++ )
	{
		if( cmd_table[index].msg )
		{
			DiagPrintfD( "%s\n",cmd_table[index].msg );
		}
	}
	DiagPrintfD("----------------- TEST COMMAND MODE END  %d [%08x:%08x]------------------\n",
		cmd_mum, __cmd_table_start__, __cmd_table_end__);

	return _TRUE ;
}
