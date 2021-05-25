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
	( void ) *argv;

	km4_set_wake_event(BIT_HP_WEVT_UART_STS);
	vTaskDelay(3);

	if ( km4_status_on()) {
		InterruptDis(UART_LOG_IRQ_LP);
		ipc_send_message(IPC_INT_CHAN_SHELL_SWITCH, NULL);
	}

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
	
	FLASH_ClockSwitch(BIT_SHIFT_FLASH_CLK_XTAL, TRUE);

	UART_RxCmd(UART2_DEV, DISABLE);
	RCC_PeriphClockSource_UART(UART2_DEV, UART_RX_CLK_XTAL_40M);
	UART_RxMonitorCmd(UART2_DEV, DISABLE);
	UART_SetBaud(UART2_DEV, 115200);
	UART_RxCmd(UART2_DEV, ENABLE);	
	
	if (_strcmp((const char*)argv[0], "uartburn") == 0){
		/* make KM4 sleep*/
		FLASH_Write_Lock();
		BKUP_Set(0, BIT_UARTBURN_BOOT);
	}

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
	/* To avoid gcc warnings */
	( void ) argc;
	
	if (_strcmp((const char*)argv[0], "r") == 0){// release
		if (_strcmp((const char*)argv[1], "debug") == 0) {
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
		if (_strcmp((const char*)argv[1], "pg") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (_strcmp((const char*)argv[1], "cg") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp((const char*)argv[0], "dslp") == 0){
		u32 duration_ms = _strtoul((const char*)(argv[1]), (char **)NULL, 10);

		SOCPS_AONTimer(duration_ms);
		SOCPS_AONTimerCmd(ENABLE);

		SOCPS_DeepSleep_RAM();

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
		RTC_TimeStruct.RTC_Hours = _strtoul((const char*)(argv[1]), (char  **)NULL, 10);
		RTC_TimeStruct.RTC_Minutes = _strtoul((const char*)(argv[2]), (char  **)NULL, 10);
		RTC_TimeStruct.RTC_Seconds = _strtoul((const char*)(argv[3]), (char  **)NULL, 10);

		if (_strcmp((const char*)argv[5], "pm") == 0) {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_PM;
		} else {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_AM;
		}
		
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}
	
	return _TRUE;
}

/**
  * @brief  write p-efuse to patch a register when power on.
  * @param  page: 3bit, when you set 4800_03C4, page=0x03
  * @param  addr: 8bit, when you set 4800_03C4, page=0xC4
  * @param  val: 8bit, when you set 4800_03C4, 4800_03C4[7:0]=val
  * @retval  None
***/
void cmd_efuse_extpath_write(u8 page, u8 addr, u8 val)
{
	u8 efuse_hear_b0;
	u8 efuse_hear_b1;
	u32 start_addr;

	efuse_hear_b0 = 0xF; //fixed
	efuse_hear_b0 |= page << 5; //0x03 << 5=>4800_0300

	efuse_hear_b1 = 0x0E;	//1x16bit=>efuse_b0 & efuse_b1
	efuse_hear_b1 |= 0x0E << 4; //4800_0000

	/* write at the end of logical mapping address */
	/* so that hardware can aotoload it */
	start_addr = LOGICAL_MAP_SECTION_LEN - EFUSE_RemainLength();

	DBG_8195A("app_efuse_extpath_write: 0x%x 0x%x\n", start_addr, EFUSE_RemainLength());
	DBG_8195A("app_efuse_extpath_write: %x \n", efuse_hear_b0);
	DBG_8195A("app_efuse_extpath_write: %x \n", efuse_hear_b1);
	DBG_8195A("app_efuse_extpath_write: %x \n", addr);
	DBG_8195A("app_efuse_extpath_write: %x \n", val);
	
	EFUSE_PMAP_WRITE8(0, start_addr, efuse_hear_b0, L25EOUTVOLTAGE);
	EFUSE_PMAP_WRITE8(0, start_addr+1, efuse_hear_b1, L25EOUTVOLTAGE);
	EFUSE_PMAP_WRITE8(0, start_addr+2, addr, L25EOUTVOLTAGE); //0xC=>4800_03C4
	EFUSE_PMAP_WRITE8(0, start_addr+3, val, L25EOUTVOLTAGE); //4800_03C4[8:0]=val
}

u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	u8 EfuseBuf[1024];
	u32 index;
	u8 ret = 0;
	/* To avoid gcc warnings */
	( void ) argc;

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

	if (_strcmp((const char*)argv[0], "patch") == 0) {
		/* close PAD_A0~PAD_A9 & PAD_F0~PAD_F6 */
		/* GPIO is PA7~PA11, PA22/23/24, PA27~PB3 */
		cmd_efuse_extpath_write(0x03, 0x40, 0xEF);//0x4800_0340[7:0]=0xEF
	}

	return 0;
}

u32
CmdCTC(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;

	if (_strcmp((const char*)argv[0], "dump") == 0) {// dump RTC
		CapTouch_DbgDumpReg(CAPTOUCH_DEV);
	}

	if (_strcmp((const char*)argv[0], "etc") == 0) {
		u32 ch = 0;

		for (ch = 0; ch < 2; ch++) {			
			CapTouch_DbgDumpETC(CAPTOUCH_DEV, ch);
		}
	}

	return _TRUE;
}

u32
CmdHsSdm32K(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) *argv;

	u32 temp;
	
	DBG_8195A("ENABLE HS SDM 32K\n");
	
	/*osc clk enable*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL2);
	temp &= (~BIT1);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_XTAL_CTRL2, temp);

	/*select xtal 40M*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG2);
	temp &= (~(0xf<<16));
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG2, temp);

	/*enable hs sdm*/
	temp = HAL_READ32(WIFI_REG_BASE, 0xe8);
	temp &= (~0x1f);
	HAL_WRITE32(WIFI_REG_BASE, 0xe8, temp);

	temp = HAL_READ32(WIFI_REG_BASE, 0xec);
	temp |= (0x3<<30);
	HAL_WRITE32(WIFI_REG_BASE, 0xec, temp);

	/*select hs_sdm & 32k*/
	temp = HAL_READ32(WIFI_REG_BASE, 0x0);
	temp &= (~(0x3<<20));
	temp |= (0x1<<20); 
	HAL_WRITE32(WIFI_REG_BASE, 0x0, temp);

	return _TRUE;
}


CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   shell_cmd_table[] = {
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
	{(const u8*)"FLASH",	8, cmd_flash,			(const u8*)"\tFLASH \n"
											"\t\t erase chip \n"
											"\t\t erase sector addr \n"
											"\t\t erase block addr \n"
											"\t\t read addr len \n"
											"\t\t write addr data \n"},
	{(const u8*)"EFUSE",	8, cmd_efuse_protect,			(const u8*)"\tEFUSE \n"
											"\t\t wmap addr len data\n"
											"\t\t rmap \n"},
	{(const u8*)"REBOOT",	4, cmd_reboot,		(const u8*)"\tREBOOT \n"
											"\t\t reboot \n"
											"\t\t reboot uartburn \n"},
	{(const u8*)"TICKPS",	4, CmdTickPS,		(const u8*)"\tTICKPS \n"
											"\t\t r: release os wakelock \n"
											"\t\t a: acquire os wakelock \n"},
	{(const u8*)"RTC",		4, CmdRTC,			(const u8*)"\tRTC \n"
											"\t\t get\n"},
	{(const u8*)"CTC",		4, CmdCTC,			(const u8*)"\tCTC \n"
											"\t\t dump/etc\n"},
	{(const u8*)">>",		4, CmdShellSwitch,	(const u8*)"\t>> \n"
											"\t\t switch shell to KM4 \n"},
	{(const u8*)"HSSDM32K",	0, CmdHsSdm32K,	(const u8*)"\tHSSDM32K \n"
											"\t\t enable hs sdm 32k\n"},
};

u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	( void ) argc;
	( void ) *argv;

	COMMAND_TABLE* cmd_table = (COMMAND_TABLE*)__cmd_table_start__;
	u32 cmd_mum = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));
	u32	index ;

	DiagPrintfD("----------------- TEST COMMAND MODE HELP %d------------------\n", cmd_mum);
	for( index=0  ; index < cmd_mum; index++ )
	{
		if( cmd_table[index].msg )
		{
			DiagPrintfD( "%s\n",cmd_table[index].msg );
		}
	}
	DiagPrintfD("----------------- TEST COMMAND MODE END  %x------------------\n", cmd_mum);

	return _TRUE ;
}

