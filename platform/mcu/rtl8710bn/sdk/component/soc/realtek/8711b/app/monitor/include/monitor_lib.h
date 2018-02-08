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


VOID 
DumpForOneBytes(
    IN  u8 *pData, 
    IN  u8   Len
);

_LONG_CALL_
extern u32
CmdDumpByte(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdDumpHalfWord(
    IN  u16 argc,
    IN  u8  *argv[]
);


_LONG_CALL_
extern u32
CmdDumpWord(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdWriteByte(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32 CmdFlash(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32 CmdEfuse(
    IN  u16 argc,
    IN  u8  *argv[]
);

#endif
