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


#ifndef SPP_CFG_H
#define SPP_CFG_H

#include "cs_types.h"
#include "chip_id.h"
#include "spp_gsm.h"

///  @defgroup cfg_func SPP Configuration
///@{

#define EQ_SHIFT_MAX_NUM 8

typedef struct
{
    // Flags
    BOOL bypassDcoc;
    BOOL correctCS1;
    BOOL correctCS4;
    BOOL equ_hburst_mode;
    // Parameters
    INT16 symb_max;
    INT16 symb_min;
    UINT8 fof_nb_symb;
    UINT8 fof_start_1;
    UINT8 fof_start_2;
    UINT8 fof_corr_gap;
    UINT32 agc_threshold;
    UINT8 mon_win_size;
    UINT8 flag_enableDC;
    UINT8 flag_enableIQ;
    INT8 shift_bm_offset;
    INT8 shift_sb_offset;
    // function
#if (SPC_IF_VER >= 2)
    BOOL (*fcch_detect_func) (UINT32* Ptr_old,
                              UINT32* Ptr_new,
                              SPP_FCCH_PARAM_T* Params,
                              SPP_FCCH_RESULT_T* Res);
    UINT8 fcch_size;
    BOOL fcch_sliding_energy;
    BOOL fcch_hw_sliding_window;
#endif

#if (SPC_IF_VER >= 5 && CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
    /// equalizer's output shift table
    UINT8 eqShiftTable[EQ_SHIFT_MAX_NUM];
#endif

#ifdef CHIP_DIE_8955
    BOOL                            TofFixedEnable;
    BOOL                            DigitalAgc;
    UINT8                           DigitalAgcTarget;
    BOOL                            EnhanceMode;
    BOOL                            ChannelTracking;
    BOOL                            TapsShorten;
    BOOL                            Ht100QrPrefilter;
    BOOL                            IsSaicStep2Enable;
    UINT8                           SaicDiagDelta1;
    UINT8                           SaicDiagDelta2;
    UINT8                           CgIternum;
    UINT16                          Ht100QrNoise;
    BOOL                           GcfMode;
    UINT8                           FadingChannelCnt;
    UINT8                           FadingChannel8TapNum;
    UINT8                           FadingChannel8TapState;
    INT16                           NoiseRatio;
    BOOL                           InterfernceDetect;
    BOOL                           Ht100Mode;
    UINT8                           RxBufferIndex;
    INT8                            TapsShortFac;
#endif

} SPP_GLOBAL_PARAMS_T;

// =============================================================================
// spp_InitGlobalParams
// -----------------------------------------------------------------------------
/// This function MUST be in RAM and not in ROM !
/// This function initializes the global parameter structure used by SPP.
/// This allows to select (and tune) some SPP processing options.
// =============================================================================
PUBLIC VOID spp_Open(SPP_GLOBAL_PARAMS_T * globalParams);

///@}

#endif //  SPP_CFG_H
