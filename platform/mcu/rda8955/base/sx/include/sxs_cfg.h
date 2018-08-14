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





#ifndef __SXS_CFG_H__
#define __SXS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "cmn_defs.h"

typedef struct
{
    void (*TaskInit)(void);
    void (*MsgParse)(void *);
    void (*TimerParse)(u32 *);
    u8   MailBox;
} sxs_TaskParse_t;

typedef struct
{
    const sxr_TaskDesc_t  TaskDesc;
    const sxs_TaskParse_t *TaskParse;
} sxs_TaskCfg_t;

void sxs_Task (sxs_TaskParse_t *TaskParse);


#ifdef __cplusplus
}
#endif

#endif

