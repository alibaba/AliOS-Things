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





#ifndef __SXS_FLSH_H__
#define __SXS_FLSH_H__

#include "sxs_type.h"
#include "sxr_cnf.h"

#define SXS_FLH_VALID_ID        (1 << 30)
#define SXS_FLH_STACKED_BLOCK   (1 << 29)
#define SXS_FLH_STATIC_BLOCK    (1 << 28)
#define SXS_FLH_PROTECTED_BLOCK (1 << 27)

//#define SXS_FLH_EXTRA_BYTE      (1 << 26)

#define SXS_FLH_EXTRA_BYTE_POS       24
#define SXS_FLH_EXTRA_BYTE_MSK       (3 << SXS_FLH_EXTRA_BYTE_POS)
#define SXS_FLH_SET_EXTRA_BYTE(n)  (((n) & 3)  << SXS_FLH_EXTRA_BYTE_POS)
#define SXS_FLH_GET_EXTRA_BYTE(Id) (((Id) >> SXS_FLH_EXTRA_BYTE_POS) & 3)

#define SXS_FILE_IDX_POS        16
#define SXS_FILE_IDX_MSK        (0xFF << SXS_FILE_IDX_POS)
#define SXS_OFFSET_POS          0
#define SXS_OFFSET_MSK          (0xFFFF << SXS_OFFSET_POS)

#define SXS_GET_FILE_IDX(Id)    ((Id & SXS_FILE_IDX_MSK) >> SXS_FILE_IDX_POS)
#define SXS_SET_FILE_IDX(Idx)   ((Idx << SXS_FILE_IDX_POS) & SXS_FILE_IDX_MSK)


u8    sxs_FlashHandlerInit (void);
void  sxs_FlashPurge (u8 Protect, u8 UpdtStatic);
u8    sxs_AddressToSector (void *Address);
void *sxs_SectorToAddress (u8 SectIdx);
void  sxs_SectorErase (u8 Sector);
void  sxs_FlashWrite (u16 *Dest, u16 *Src, u16 Length);
u8    sxs_FlashBlockWrite (u32 Id, u8 *Data, u16 Size);
u8   *sxs_FlashBlockRead (u32 Id, u8 *Data, u16 Size);
void  sxs_GetFilesLimit (u32 *Start, u32 *End);
void  sxs_FlashBlockDelete (u32 Id, u32 Msk, u8 Protect);
void  sxs_UpdateStaticBank (void);
#endif

