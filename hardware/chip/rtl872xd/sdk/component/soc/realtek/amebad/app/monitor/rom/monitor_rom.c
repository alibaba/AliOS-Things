/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

//======================================================
//<Function>:  CmdTest
//<Usage   >:  This function is a demo test function.
//<Argus    >:  argc --> number of argus
//                   argv --> pointer to a cmd parameter array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================

static u32 cmd_rom_help(u16 argc, u8  *argv[]);

SHELL_ROM_TEXT_SECTION
u32 cmd_dump_word(u16 argc, u8  *argv[])
{
	u32 Src;
	//u32 Len,LenIndex;
	u32 OTF_Enable = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN;

	if(argc<1)
	{
		MONITOR_LOG("Wrong argument number!\r\n");
		return _FALSE;
	}
	
	if(argv[0]) {
		Src= _strtoul((const char*)(argv[0]), (char **)NULL, 16);
	} else {
		MONITOR_LOG("Wrong argument number!\r\n");
		return _FALSE;		
	}
				
	//if(!argv[1])
	//	Len = 1;
	//else
	//	Len = _strtoul((const char*)(argv[1]), (char **)NULL, 10);	
    
	Src &= ~(0x03);

	/* read encrypt image for FW protection */
	if ((Src & 0x08000000) != 0) {
		if (OTF_Enable != 0) {
			RSIP_OTF_Cmd(DISABLE);
		}
	}

	MONITOR_LOG("%08X: %08X \n", Src, *(u32 *)(Src));

	if ((Src & 0x08000000) != 0) {
		if (OTF_Enable != 0) {	
			RSIP_OTF_Cmd(ENABLE);
			Cache_Flush();
		}
	}

	return _TRUE;

}

SHELL_ROM_TEXT_SECTION
u32 cmd_write_word(u16 argc, u8  *argv[])
{
	
	u32 Src;
	u32 Value;
	
	Src = _strtoul((const char*)(argv[0]), (char **)NULL, 16);		

	Src &= ~(0x03);

	Value= _strtoul((const char*)(argv[1]), (char **)NULL, 16);
	MONITOR_LOG("%08X: %08X \n", Src, Value);

	*(volatile u32 *)(Src) = Value;
	
	return 0;
}

SHELL_ROM_TEXT_SECTION
u32 cmd_flash(u16 argc,  u8  *argv[])
{
	if (_strcmp((const char*)argv[0], "erase") == 0) {/* SPI Flash Chip Erase Command */
		u32 AddrTemp = 0;
	
		MONITOR_LOG("Erase Falsh Start\n");

		AddrTemp = _strtoul((const char*)(argv[2]), (char **)NULL, 16);

		if (_strcmp((const char*)argv[1], "chip") == 0) {
			MONITOR_LOG("Erase Chip start!!\n");
			
			FLASH_Erase(EraseChip, 0);
			
			MONITOR_LOG("Erase Chip success!!\n");
		} else if (_strcmp((const char*)argv[1], "block") == 0) {
			MONITOR_LOG("Erase block = %x\n",(AddrTemp));

			FLASH_Erase(EraseBlock, AddrTemp);  	

			MONITOR_LOG("Erase block success!!\n");
		} else if (_strcmp((const char*)argv[1], "sector") == 0) {
			MONITOR_LOG("Erase Sector = %x\n",(AddrTemp));

			FLASH_Erase(EraseSector, AddrTemp);  		

			MONITOR_LOG("Erase Sector success!!\n");
		}

		/* for calibration use */
		FLASH_TxData12B(0, 4, (u8*)SPIC_CALIB_PATTERN);
		Cache_Flush();

		MONITOR_LOG("Erase Falsh End\n");
	}

	if (_strcmp((const char*)argv[0], "read") == 0) {
		u32 address = 0;
		u32 len = 0;
		u32 idx = 0;
		u32 OTF_Enable = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN;
		
		MONITOR_LOG("Falsh read\n");

		/* read encrypt image for FW protection */
		if (OTF_Enable) {
			RSIP_OTF_Cmd(DISABLE);
		}
		
		address = _strtoul((const char*)(argv[1]), (char **)NULL, 16);
		len = _strtoul((const char*)(argv[2]), (char **)NULL, 10)/4;

		for (idx = 0; idx < len; idx++) {
			MONITOR_LOG("%08x: %08x %08x  %08x %08x \n", address, HAL_READ32(SPI_FLASH_BASE, address),
				HAL_READ32(SPI_FLASH_BASE, address+4), HAL_READ32(SPI_FLASH_BASE, address+8),
				HAL_READ32(SPI_FLASH_BASE, address+12));
			address = address + 16;
		}

		if (OTF_Enable) {
			RSIP_OTF_Cmd(ENABLE);
			Cache_Flush();
		}
	}

	if (_strcmp((const char*)argv[0], "write") == 0) {
		u32 address = 0;
		u32 data = 0;

		address = _strtoul((const char*)(argv[1]), (char **)NULL, 16);
		data = _strtoul((const char*)(argv[2]), (char **)NULL, 16);
	
		MONITOR_LOG("Falsh write addr:%x data:%x \n", address, data);

		FLASH_TxData12B(address, 4, (u8*)&data); //max is 12
		Cache_Flush();
	}

	if (_strcmp((const char*)argv[0], "status") == 0) {
		u8 status[2];
		u8 wstatus = _strtoul((const char*)(argv[2]), (char **)NULL, 16);

		if (_strcmp((const char*)argv[1], "get") == 0) {
			FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, &status[0]);
			if (flash_init_para.FLASH_Status2_exist) {
				FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status2, 1, &status[1]);
			}
			
			MONITOR_LOG("status: %x %x\n", status[0], status[1]);
		}
		
		if (_strcmp((const char*)argv[1], "set1") == 0) {
			FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 1, &wstatus);
		}

		if (_strcmp((const char*)argv[1], "set2") == 0) {
			if(!flash_init_para.FLASH_cmd_wr_status2){
				FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_status, 1, &status[0]);
				status[1] = wstatus;
				
				FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status, 2, status);
			}else{
				FLASH_SetStatus(flash_init_para.FLASH_cmd_wr_status2, 1, &wstatus);
			}
		}
	}
	
	if (_strcmp((const char*)argv[0], "cache") == 0) {
		if (_strcmp((const char*)argv[1], "disable") == 0) {
			MONITOR_LOG("cache disable \n");
			Cache_Enable(DISABLE);
		} else if (_strcmp((const char*)argv[1], "enable") == 0) {
			MONITOR_LOG("cache enable \n");
			Cache_Enable(ENABLE);
		} else if (_strcmp((const char*)argv[1], "flush") == 0) {
			MONITOR_LOG("cache flush \n");
			Cache_Flush();
		}
	}

	return 0;
}

SHELL_ROM_TEXT_SECTION
u32 cmd_efuse(u16 argc, u8  *argv[])
{
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
		
		ret = EFUSE_LogicalMap_Read(EfuseBuf);
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
				EFUSERead8(0, index, EfuseBuf + index, L25EOUTVOLTAGE);
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
			EFUSEWrite8(0, Addr + index, EfuseBuf[index], L25EOUTVOLTAGE);
		}
	}

	return 0;
}

SHELL_ROM_TEXT_SECTION
static u32
cmd_reboot(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	if (_strcmp((const char*)argv[0], "uartburn") == 0){
		BKUP_Set(0, BIT_UARTBURN_BOOT);
	}

	MONITOR_LOG("\n\rRebooting ...\n\r");
	NVIC_SystemReset();

	return _TRUE;
}

SHELL_ROM_DATA_SECTION
static COMMAND_TABLE   shell_cmd_table_rom[] = {
	{(const u8*)"?",		0, cmd_rom_help,		(const u8*)"\tHELP (?) \n"
											"\t\t Print this help messag\n"},
	{(const u8*)"DW",	4, CmdDumpWord,	(const u8*)"\tDW \n"
											"\t\t <Address, Hex> <Len, Dec>: \n"
											"\t\t Dump memory word or Read Hw word register"},
	{(const u8*)"EW",	4, CmdWriteWord,	(const u8*)"\tEW \n"
											"\t\t <Address, Hex> <Value, Hex>: \n"
											"\t\t Write memory word or Write Hw word register \n"
											"\t\t Can write one word at the same time \n"
											"\t\t Ex: EW Address Value0 Value1"},
	{(const u8*)"FLASH",	8, cmd_flash,			(const u8*)"\tFLASH \n"
											"\t\t erase chip \n"
											"\t\t erase sector addr \n"
											"\t\t erase block addr \n"
											"\t\t read addr len \n"
											"\t\t write addr data \n"},
	{(const u8*)"EFUSE",	8, cmd_efuse,			(const u8*)"\tEFUSE \n"
											"\t\t wmap addr len data\n"
											"\t\t rmap \n"
											"\t\t autoload \n"},
	{(const u8*)"REBOOT",	4, cmd_reboot,		(const u8*)"\tREBOOT \n"
											"\t\t <item, string> : \n"
											"\t\t item: uartburn or N/A \n"
											"\t\t \n"},
};

SHELL_ROM_TEXT_SECTION
u32 cmd_rom_table(void** PTable)
{
	*PTable = (void*)&shell_cmd_table_rom;
	
	return (sizeof(shell_cmd_table_rom)/sizeof(COMMAND_TABLE));
}

SHELL_ROM_TEXT_SECTION
static u32 cmd_rom_help(u16 argc, u8  *argv[])
{
	u32	LoopINdex ;

	MONITOR_LOG("----------------- COMMAND MODE HELP ------------------\n");
	for( LoopINdex=0  ; LoopINdex < (sizeof(shell_cmd_table_rom) / sizeof(COMMAND_TABLE)) ; LoopINdex++ )
	{
		if( shell_cmd_table_rom[LoopINdex].msg )
		{
			MONITOR_LOG( "%s\n",shell_cmd_table_rom[LoopINdex].msg );
		}
	}
	MONITOR_LOG("----------------- COMMAND MODE END  ------------------\n");

	return _TRUE ;
}

#pragma GCC diagnostic pop