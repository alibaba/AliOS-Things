/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _MONITOR_LIB_H_
#define _MONITOR_LIB_H_

_LONG_CALL_ extern u32 cmd_dump_word(u16 argc, u8  *argv[]);
_LONG_CALL_ extern u32 cmd_write_word(u16 argc, u8  *argv[]);
_LONG_CALL_ extern u32 cmd_flash(u16 argc,  u8  *argv[]);
_LONG_CALL_ extern u32 cmd_efuse(u16 argc, u8  *argv[]);
_LONG_CALL_ u32 cmd_rom_table(void** PTable);

#define CmdDumpWord	cmd_dump_word
#define CmdWriteWord	cmd_write_word
#endif
