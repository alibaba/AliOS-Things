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

#ifndef __SXR_USRMEM_H__
#define __SXR_USRMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SXR_USRMEM_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define SXR_NB_USER_CLUSTER 32
DefExtern struct
{
    void *Add;
    u16   Size;
    u16   AlignSize;
    u8    NbCluster;
    u8    FirstCluster;
    u16   NbAllocation;
} sxr_UsrCluster [SXR_NB_USER_CLUSTER];

DefExtern u8 sxr_NbUsrCluster;

#ifdef __SXR_RELEASED_PTR_TO_NIL__
void _sxr_UsrFree (void **Ptr);
#define sxr_UsrFree(a)  _sxr_UsrFree((void *)&(a))
#else
void sxr_UsrFree (void *Ptr);
#define _sxr_UsrFree(a) sxr_UsrFree(a)
#endif




void *sxr_UsrMalloc (u32 Size);
void sxr_NewUsrCluster (u16 Size, u16 NbCluster, u8 Idx);

#undef DefExtern

#ifdef __cplusplus
}
#endif

#endif //__SXR_USRMEM_H__
