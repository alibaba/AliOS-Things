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





#ifndef __SXS_SPY_H__
#define __SXS_SPY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"

typedef struct
{
    u8  SpyIdx;
    u8  Id;
    u16 Data;
} sxs_SpyInfo_t;

typedef struct
{
    u16 Id;
    s16 Data;
} sxs_SpyData_t;


#define SXS_SPY_NAME_ID     0
#define SXS_SPY_STR_ID      1
#define SXS_SPY_DATA_ID     2
#define SXS_SPY_ENABLE_ID   3
#define SXS_SPY_MULTI_ID    4

#define SXS_NB_MAX_SPY      32

u8   sxs_NewSpy  (const ascii *Name, u8 UsrId, const ascii *const *Str, s16 *Data, u8 Size);
void sxs_SpyData (void);
void sxs_SpyStr  (u8 Idx);
void sxs_SpyEnable (u8 SpyIdx, u8 Enable);
void sxs_UsrSpyEnable (u8 UsrId, u8 Enable);
void sxs_SpyDispatchRegistration (u8 UsrId, void (*DispatchFunction)(u8, u8, u16 *), u32 DispatchFields, u8 DispatchDataOffset);

#ifdef __cplusplus
}
#endif

#endif

