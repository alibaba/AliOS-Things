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





#ifndef __SXR_TIM_H__
#define __SXR_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "sxs_type.h"
#include "sxr_cnf.h"

#define SXR_NO_DUE_DATE         0xFFFFFFFF
#define SXR_NO_MSK              0xFFFFFFFF

#define SXR_FUNCTION_TIMER_POS  14
#define SXR_FUNCTION_TIMER      ((u16)1 << SXR_FUNCTION_TIMER_POS)
#define SXR_MBX_TIMER_POS       15
#define SXR_MBX_TIMER           ((u16)1 << SXR_MBX_TIMER_POS)

#define SXR_TIMER_PARAM_IDX     0
#define SXR_TIMER_FUNCTION_IDX  1

typedef struct
{
    u32 Ctx;
    u32 Msk;
    u16 Id;
    u8  Idx;
} sxr_Timer_t;

typedef struct
{
    u16 Id;
    u16 IdMsk;
    u32 Ctx [SXR_TIMER_CTX_SIZE];
    u32 Msk [SXR_TIMER_CTX_SIZE];
} sxr_TimerEx_t;

typedef struct
{
    u32 Ctx;
    u32 Msk;
    u8  Idx;
    u16 Id;
    u32 *Tab [SXR_TIMER_LIST_SIZE];
    u16 NbTimerMax;
} sxr_TimerList_t;

void sxr_InitTimer (void (*TimerDelivery)(u32 *),
                    void (*SetCntValue)(u32),
                    u32  (*GetCntValue)(void),
                    u8   I2d);

void sxr_SetTimer    (u32 Period, u32 Ctx[], u16 Id, u8 I2d);
u32  sxr_DeleteTimer (sxr_Timer_t *Timer, u32 *Ctx, u8 DelAll, u8 I2d);
u32  sxr_DeleteComplexTimer (sxr_Timer_t *Timer, u32 *Ctx, u8 DelAll, void (*Free)(u16, u32 *), u8 I2d);
u32  sxr_DeleteTimerEx      (sxr_TimerEx_t *Timer, u32 *Ctx, u8 DelAll, void (*Free)(u16, u32 *), u8 I2d);

void sxr_Tampax      (u32 NbTimePeriod, u8 I2d);
u8   sxr_IsTimerSet  (sxr_Timer_t *Timer, u8 I2d);
u8   sxr_IsTimerExSet(sxr_TimerEx_t *Timer, u8 I2d);
u8   sxr_TimerList   (sxr_Timer_t *Timer, u32 *Tab, u8 Max, u8 I2d);
bool sxr_TimerListEmpty (u8 I2d);
u32  sxr_NextTimerQuery (u16 Mask, u8 I2d);
u32  sxr_TimerQuery  (sxr_Timer_t *Timer, u8 I2d);
sxr_Timer_t *sxr_PeriodToTimer (sxr_Timer_t *Timer, u32 Period, u8 I2d);
u32  sxr_TimerExQuery(sxr_TimerEx_t *Timer, u8 I2d);

// =============================================================================
// sxr_StartMbxTimer
// -----------------------------------------------------------------------------
/// Start a timer such that after required period, the event
/// given in parameter is sent in the required mail box.
///
/// @param period   Period to wait,
/// @param evt      Event to be sent.
/// @param mbx      Mail box number
/// @param i2d      Timer instance. (SXR_REGULAR_TIMER or SXR_FRAMED_TIMER)
// =============================================================================
void sxr_StartMbxTimer (u32 Period, u32 *Evt, u8 Mbx, u8 I2d);

// =============================================================================
// sxr_StopMbxTimer
// -----------------------------------------------------------------------------
/// Stop the identified timer, identified by the mailbox id and the timer id.
///
/// @param evt      Event to be sent.
/// @param mbx      Mail box number
/// @param i2d      Timer instance. (SXR_REGULAR_TIMER or SXR_FRAMED_TIMER)
/// @return         \c TRUE when timer found, \c FALSE otherwise.
// =============================================================================
u8 sxr_StopMbxTimer (u32 *evt, u8 mbx, u8 i2d);


void _sxr_StartFunctionTimer (u32 Period, void (*Function)(void *), void *Param, u16 Id, u8 I2d);
u32  _sxr_StopFunctionTimer (void (*Function)(void *), u8 I2d);
u32 _sxr_StopFunctionTimer2 (void (*Function)(void *),void *Param, u8 I2d);

u8   sxr_CheckTimer (void);
void sxr_PrintTimer (sxr_Timer_t *Timer, u8 I2d);


/* -----------------  User friendly Timer interface  --------------------
| Reminder:
| - Function timer: the given function is called with specified parameter
|   after required period.
| - Mbx timer: the given event is sent in the given mail box after required
|   period.
|
| - Framed timer correspond to a high priority timer instance and are reserved
|   for lower layer (NOT RR !).
|
*/

#define sxr_StartFunctionTimer(Period, Function, Param, Id) \
 _sxr_StartFunctionTimer ((u32)Period, (void (*)(void *))Function, (void *)Param, (u16)Id, SXR_REGULAR_TIMER)

#define sxr_StopFunctionTimer(Function) \
 _sxr_StopFunctionTimer ((void (*)(void *))Function, SXR_REGULAR_TIMER)

#define sxr_StopFunctionTimer2(Function,Param) \
 _sxr_StopFunctionTimer2 ((void (*)(void *))Function, Param,SXR_REGULAR_TIMER)

#define sxr_StartFunctionFramedTimer(Period, Function, Param, Id) \
 _sxr_StartFunctionTimer ((u32)Period, (void (*)(void *))Function, (void *)Param, (u16)Id, SXR_FRAMED_TIMER)

void sxs_StartTimer   (u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx);
u8   sxs_StopTimer    (u32 Id, void *Data, u8 Mbx);
u8   sxs_StopAllTimer (u32 Id, void *Data, u8 Mbx);
u32  sxs_ExistTimer   (u32 Id);

#ifdef __cplusplus
}
#endif

#endif

