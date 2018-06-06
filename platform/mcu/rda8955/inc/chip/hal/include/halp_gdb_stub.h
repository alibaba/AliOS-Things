/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _HALP_GDB_STUB_H_
#define _HALP_GDB_STUB_H_

#define HAL_GDB_REGNUM 38
//  all register + command and parameter for command
#define HAL_GDB_REGNUM_XTD (HAL_GDB_REGNUM + 3)
#define HAL_GDB_CONTEXT_SIZE (HAL_GDB_REGNUM_XTD * 4)

#define HAL_GDB_CMD_OFF HAL_GDB_REGNUM
#define HAL_GDB_CMDPARAM_OFF (HAL_GDB_REGNUM + 1)
#define HAL_GDB_CMDPARAM2_OFF (HAL_GDB_REGNUM + 2)

//  command between jade cpus and debug server
#define HAL_GDB_CMD_DONE 0x00

#define HAL_GDB_CMD_FLUSH_CACHE 0x01
#define HAL_GDB_CMD_STEP 0x02
#define HAL_GDB_CMD_CONTINUE 0x04
#define HAL_GDB_SET_BREAKPOINT 0x05
#define HAL_GDB_UNSET_BREAKPOINT 0x06

#define HAL_GDB_CMD_BREAKPOINT 0x10
#define HAL_GDB_CMD_PRINT 0x20

#ifndef CT_ASM

/// Function used to flush both caches.
PUBLIC VOID hal_GdbFlushCache(VOID);

/// Function used to flush the data cache.
PUBLIC VOID hal_GdbFlushCacheData(VOID);
PROTECTED VOID hal_DbgGdbHstSendEvent(UINT32 ch);

#ifdef GDB_STUB_SUPPORT
PROTECTED BOOL hal_GdbIsInGdbLoop(VOID);
#else
#define hal_GdbIsInGdbLoop() (0)
#endif

PROTECTED VOID hal_GdbDisableDebugIrq(VOID);

#endif // !CT_ASM

#endif //  _HALP_GDB_STUB_H_
