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





#ifndef __SXS_RMC_H__
#define __SXS_RMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"

#define SXS_RMC_YIELD    0x80
#define SXS_NB_RPC_PARAM 4

enum
{
    SXS_SPY_RMC,
    SXS_TRACE_RMC = SXS_RMC_YIELD, // 0x80
    SXS_DUMP_RMC,
    SXS_RPC_RSP_RMC,
    SXS_TIME_STAMP_RMC,

    SXS_READ_RMC, //0x84
    SXS_WRITE_RMC,
    SXS_TRACE_LEVEL_RMC,
    SXS_RPC_RMC,
    SXS_DWL_START_RMC, // 0x88
    SXS_DWL_RMC,
    SXS_MBX_RMC,
    SXS_RAZ_RMC,
    SXS_INIT_RMC, // 0x8c
    SXS_SERIAL_RMC,
    SXS_WIN_RMC,
    SXS_MEM_RMC,
    SXS_AT_RMC, // 0x90
    SXS_DATA_RMC,
    SXS_DATACTRL_RMC,

    SXS_EXIT_RMC, // 0x93

    SXS_PROFILE_RMC, // 0x94

    SXS_WCPU_RMC, // 0x95
    SXS_NBIOT_RMC, // 0x96
};


typedef struct
{
    u8 NbId;              // Number of Id trace descriptor
    u8 Pad [3];
    u8 Data [4];          // 3 bytes / descriptor.
} sxs_FlwInit_t;


typedef struct
{
    u16 Id;
    u16 Size;
    ascii Fmt [4];
    const u8 *Address;
    u8  Data [];
} sxs_FlwDmp_t;

typedef struct
{
    u16 Size;
    u8  Access;
    u8  ReadId;
    u8 *Address;
    u8  Data [4];
} sxs_FlwRW_t;

typedef  struct
{
    u8  Id;
    u8  flwCtrl;
    u16 BitMap;
} sxs_FlwTLev_t;

enum {SXS_RMC_REQ, SXS_RMC_IND};

typedef struct
{
    u8  Id;
    u8  SerialIdx;
    u8  Speed;
} sxs_FlwSerial_t;


// Define to set the Len field in FlwRpc_t

#define SXS_RPC_NO_PARAM      0xFF
#define SXS_RPC_VALUE_PARAM      0  // Means default len = 4
#define SXS_RPC_POINTER_PARAM

// Defines to set the Id of FlwRpc_t when a return value is expected.

#define SXS_RPC_RETURN_VOID    0

#define SXS_RPC_USR_ID_POS     0
#define SXS_RPC_USR_ID_MSK     0xFF    // 8 bits for user Id
#define SXS_SET_RPC_USR_ID(Id) (((u32)Id & SXS_RPC_USR_ID_MSK) << SXS_RPC_USR_ID_POS)
#define SXS_GET_RPC_USR_ID(Id) ((Id >> SXS_RPC_USR_ID_POS) & SXS_RPC_USR_ID_MSK)

#define SXS_RPC_RET_SZE_POS       8
#define SXS_RPC_RET_SZE_MSK       0xFF   // 8 bits for return size.
#define SXS_SET_RPC_RET_SZE(Size) (((u32)Size & SXS_RPC_RET_SZE_MSK) << SXS_RPC_RET_SZE_POS)
#define SXS_GET_RPC_RET_SZE(Id)   ((Id >> SXS_RPC_RET_SZE_POS) & SXS_RPC_RET_SZE_MSK)

#define SXS_RPC_JOB            ((u32)1 << 16)
#define SXS_RPC_RETURN_POINTER ((u32)1 << 17)
#define SXS_RPC_RETURN_VALUE   ((u32)1 << 18)

typedef struct
{
    u32 (*Rpc)(u32, ...);
    u32   Id;
    u8    Len [SXS_NB_RPC_PARAM];
    u32   Param [1];
} sxs_FlwRpc_t;

typedef struct
{
    u32 Id;
    u32 Data [1];
} sxs_FlwRpcRsp_t;

typedef struct
{
    u16 Idx;
    u16 Size;
    u32 Data [1];
} sxs_DwlData_t;

typedef struct
{
    u16 Idx;
    u8  Ack;
} sxs_DwlAck_t;

typedef struct
{
    u8 Mbx;
    u8 MsgStatus;
    u8 WaitResponse;
    u8 Pad;
    u32 Data [1];
} sxs_RmtMbx_t;

typedef struct
{
    u8  NbWin;
    u8  TimeStamp;     // TRUE => Fn available.
    u8  Pad [2];
    u32 Fn;
    u32 Data [1];
} sxs_FlwWin_t;


void sxs_RmcCtxInit (void);
void sxs_InitRmc (void);


#ifdef __cplusplus
}
#endif
#endif

