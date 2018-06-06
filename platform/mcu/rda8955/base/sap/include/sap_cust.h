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




#ifndef __SAP_CUST_H__
#define __SAP_CUST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "itf_api.h"


#ifdef __SAP_CUST_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

/******************************************************************************
       PROTOCOL STACK CONFIGURATION
******************************************************************************/
//===============================================================================
//  Scope      : This structure gives StackCom's Protocol Stack Configuration
DefExtern const api_StackCfg_t sap_StackCfgDefal
#ifdef __SAP_CUST_VAR__
=
{
    // Plmn Selection Mode
    .PlmnSelMode = API_PSEL_AUTO,

    // UCS2 alphabet support
    .Ucs2Support = FALSE,

    // PFC support
    .PfcSupport = FALSE,

    // GSM ciphering algorithms
    .A5Support = 0,

    // GPRS ciphering algorithms
    .GeaSupport = API_GEA1|API_GEA2,

    // MS Multislot Class
    //            API_MSCLASS_10,
    .MSClass = API_MSCLASS_12,
    //API_MSCLASS_7,
    // API_MSCLASS_2,
#ifdef __EGPRS__
    .MSEGPRSClass = API_MSECLASS_12,
#endif
    // MS power classes (GMSK)
    .G400PwrClass = API_CLASS_4, // GSM4x
    .G850PwrClass = API_CLASS_4, // GSM850
    .G900PwrClass = API_CLASS_4 , // GSM900x
    .D1800PwrClass = API_CLASS_1 , // DCS1800
    .P1900PwrClass = API_CLASS_1 , // PCS1900

#ifdef __EGPRS__
    .api_PskPwrCap = API_CLASS_E1,
#endif

    /*
    ** Non Drx parameters
    */
    .NonDrxTimer = API_NDRXTIM_2S, // Requested duration of transfer non-DRX mode period
    .SplitPgCycle = 8,              // SplitPgCycle

    /*
    ** Supported Channel Modes
    */
    .ChanModeSupport = (API_SIG_ONLY | API_SPEECH_V1 | API_SPEECH_V2 | API_SPEECH_V3
#ifdef __CS_DATA__
    | API_12_KBS
#endif
                       ),

    /*
    ** SS screening indicator
    */
    .SsScreenInd = API_SS_PHASE2,

    // CC timers
    .Tmpty = 10, // Tmpty duration in seconds
    .Tect = 10, // Tect  duration in seconds

    // CRSS and SS supported service
    .SsSupported = (API_SS_AOC|API_SS_CALLHOLD|API_SS_MPTY),

    // GPRS TEST MODE A : Nb of bytes in LLC PDUs

    .TstLlcPduSize = 1560, // Valid if belongs to [140 ... 1560]

    // GPRS automatic Attach at power on/switch on

#ifdef __AUTO_GPRS_ATT__
    .AutoGprsAttach = TRUE,
#else
    .AutoGprsAttach = FALSE,
#endif

    // API Debug mode interface.
    // Bit map of wanted fields for each debug type
    .SpyFieldBitMap =
    {
        0x0,   // L1 debug info for serving cell
        0x0,     // L1 debug info for near cells
        0xF00, // RR debug info
        0x0,     // RLU debug info
        0x8003,// MMP debug info
        0x0,   // CC debug info
        0x0,     // CB debug info
        0x2     // LLC debug info
    },
#ifdef __EGPRS__
    .EgprsUl8PSK = FALSE,//as now we donot support uplink 8PSK
#endif
#ifdef __MULTI_RAT__
    .UMTSFDDSUPPORT = 1,
    .UMTS384MTDDSUPPORT= 0,
    .UMTSCDMA2000SUPPORT= 0,
#endif
}
#endif
;

DefExtern const api_StackDynCfg_t sap_StackDynCfgVal
#ifdef __SAP_CUST_VAR__
=
{
    // GPRS Access Method (1: One phase access, 2: Two phase access)
    API_GACCMETH_TWO_PHASES,
    200
}
#endif
;

#if (STK_MAX_SIM==1)
DefExtern api_StackCfg_t sap_StackCfg ;
DefExtern api_StackDynCfg_t sap_StackDynCfg;
#else
DefExtern api_StackCfg_t sap_StackCfg[STK_MAX_SIM];
DefExtern api_StackDynCfg_t sap_StackDynCfg[STK_MAX_SIM];
#endif



//==============================================================================
//==============================================================================
//      Information to be stored in a non volatile rewritable memory
//==============================================================================
//==============================================================================

// (see 3GPP TS 23.060 section 13.4)
// this do not need double
#ifdef __GPRS__
DefExtern u8 GsimUnaware[32]
#ifdef __SAP_CUST_VAR__
=
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00,
    0x01
}
#endif
;
#endif //__GPRS__


#undef DefExtern


#ifdef __cplusplus
}
#endif

#endif
