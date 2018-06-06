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



#ifndef _HAL_RDA_AUDIO_H_
#define _HAL_RDA_AUDIO_H_

#include "cs_types.h"
#include "chip_id.h"
#include "hal_aif.h"
#include "hal_timers.h"
#include "hal_sys_cfg.h"

// =============================================================================
//  MACROS
// =============================================================================

// ----------------
// Times
// ----------------
#define AUD_TIME_POWER_ON_DEEP   (1 MS_WAITING)
#define AUD_TIME_POWER_ON_REF    (11) // 500 us, upper_int(HAL_TICK1S/1000/2) + 2
#define AUD_TIME_POWER_ON_OTHER  (6) // 200 us, upper_int(HAL_TICK1S/1000/5) + 2
#define AUD_TIME_SET_FM_MODE     (1 MS_WAITING)

#define AUD_TIME_RETRY           (2 MS_WAITING)


// =============================================================================
//  TYPES
// =============================================================================

typedef VOID (*HAL_AUD_CALLBACK_T)(VOID);

typedef enum
{
    // Identifiers for internal only
    HAL_AUD_USER_NULL,
    // Identifiers for API parameter
    HAL_AUD_USER_CODEC,
    HAL_AUD_USER_FM,
    HAL_AUD_USER_LINEIN,
    HAL_AUD_USER_LINEIN_WITH_CODEC,
    HAL_AUD_USER_AIF_ONLY,
#ifdef VMIC_POWER_ON_WORKAROUND
    HAL_AUD_USER_EARPIECE,
#endif
} HAL_AUD_USER_T;

typedef enum
{
    HP_DETECT_REQ_SRC_AUD = 0,
    HP_DETECT_REQ_SRC_FM,
} HP_DETECT_REQ_SRC_T;

typedef union
{
    UINT32 reg;
    struct
    {
        BOOL noDeepPd : 1;
        BOOL noRefPd : 1;
        BOOL noMicPd : 1;
        BOOL noAuxMicPd : 1;
        BOOL noAdcPd : 1;
        BOOL noDacPd : 1;
        UINT32 : 2;
        BOOL noDacReset : 1;
    };
} HAL_AUD_PD_T;
#define HAL_AUD_PD_MASK (0x13f)

typedef union
{
    UINT32 reg;
    struct
    {
        BOOL auxMicSel : 1;
        BOOL spkSel : 1;
        BOOL spkMonoSel : 1;
        BOOL rcvSel : 1;
        BOOL headSel : 1;
        BOOL fmSel : 1;
    };
} HAL_AUD_SEL_T;

typedef union
{
    UINT32 reg;
    struct
    {
        UINT32 micGain : 4;
        BOOL noMicMute : 1;
    };
} HAL_AUD_MIC_CFG_T;

typedef union
{
    UINT32 reg;
    struct
    {
        UINT32 spkGain : 4;
        BOOL noSpkMute : 1;
    };
} HAL_AUD_SPK_CFG_T;

typedef struct
{
    HAL_AUD_USER_T curUser;
    HAL_AUD_SEL_T orgSelCfg;
    HAL_AUD_SEL_T curSelCfg;
    BOOL spkPaForceOn;
    BOOL spkLeftChanSel;
    BOOL spkRightChanSel;
} HAL_AUD_CODEC_MODE_PARAM_T;

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

PUBLIC VOID hal_AudEnableRecvMode(BOOL enable);
PUBLIC BOOL hal_AudOutputMuted(VOID);

PUBLIC BOOL hal_AudOpen(HAL_AUD_USER_T user, HAL_AUD_CALLBACK_T cbFunc);
PUBLIC BOOL hal_AudClose(HAL_AUD_USER_T user);
PUBLIC VOID hal_AudPowerOn(HAL_AUD_PD_T pdStatus);
PUBLIC VOID hal_AudPowerDown(HAL_AUD_PD_T pdStatus);
PUBLIC VOID hal_AudDeepPowerDown(VOID);
PUBLIC VOID hal_AudForcePowerDown(VOID);
PUBLIC  BOOL hal_AudPreOpen();

/// Must wait for AUD_TIME_SET_FM_MODE if setting to non-FM mode
PUBLIC VOID hal_AudSetFmMode(BOOL isFmMode);
PUBLIC VOID hal_AudSetVoiceMode(BOOL isVoiceMode);
PUBLIC VOID hal_AudEnableDac(BOOL on);
PUBLIC VOID hal_AudEnableCodecDsp(BOOL on);
PUBLIC VOID hal_AudSetSampleRate(HAL_AUD_USER_T user, HAL_AIF_FREQ_T sampleRate);
PUBLIC VOID hal_AudSetAdcSampleRate(HAL_AIF_FREQ_T dacRate, HAL_AIF_FREQ_T adcRate, BOOL IsPlaying);
PUBLIC VOID hal_AudEnableHpDetect(BOOL on, HP_DETECT_REQ_SRC_T src);
PUBLIC VOID hal_AudBypassNotchFilterReq(BOOL bypass);

PUBLIC VOID hal_AudSetDacVolume(UINT32 vol);
PUBLIC VOID hal_AudSetAdcVolume(UINT32 vol);
PUBLIC UINT32 hal_AudGetDacVolumn();

PUBLIC VOID hal_AudForceReceiverMicSelection(BOOL on);
PUBLIC BOOL hal_GetAudForceReceiverMicSelectionFlag(VOID);

PUBLIC VOID hal_AudSetSelCfg(HAL_AUD_SEL_T selCfg);
PUBLIC VOID hal_AudSetMicCfg(HAL_AUD_MIC_CFG_T micCfg);
PUBLIC VOID hal_AudSetSpkCfg(HAL_AUD_SPK_CFG_T spkCfg);
PUBLIC VOID hal_AudSetRcvGain(UINT32 gain);
PUBLIC VOID hal_AudSetHeadGain(UINT32 gain);

PUBLIC HAL_AUD_PD_T hal_AudGetPowerStatus(VOID);
PUBLIC HAL_AUD_SEL_T hal_AudGetSelCfg(VOID);
#if defined(CHIP_DIE_8809E) || defined(CHIP_DIE_8955)
PUBLIC UINT32 hal_AudEnRcv(BOOL enable);
PUBLIC UINT32 hal_AudEnHpVcom(BOOL enable);
#endif
PUBLIC HAL_AUD_MIC_CFG_T hal_AudGetMicCfg(VOID);
PUBLIC HAL_AUD_SPK_CFG_T hal_AudGetSpkCfg(VOID);
PUBLIC UINT32 hal_AudGetRcvGain(VOID);
PUBLIC UINT32 hal_AudGetHeadGain(VOID);

PUBLIC UINT32 hal_AudRcvGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudHeadGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudSpkGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudMicGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudDacGainDb2Val(INT32 db);
PUBLIC UINT32 hal_AudAdcGainDb2Val(INT32 db);
PUBLIC VOID hal_AudEnableDSSI(BOOL on);
#ifdef CHIP_DIE_8809E
PUBLIC VOID hal_AudEnableAuxpll (BOOL on);
PUBLIC VOID hal_AudSetAuxPllConfig(HAL_SYS_PLL_PROFILE_T Mode);
#endif
PROTECTED VOID hal_AudSetCodecMode(HAL_AUD_CODEC_MODE_PARAM_T *modeParam);

#endif // _HAL_RDA_AUDIO_H_

