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


#ifndef _SPCP_DEBUG_H_
#define _SPCP_DEBUG_H_

#include "spal_debug.h"

// separation marker for dump
#define SPC_DBG_DUMP_EQU_IN_MARKER 0xAAAA
#define SPC_DBG_DUMP_EQU_OUT_MARKER 0xBBBB
#define SPC_DBG_DUMP_DEC_IN_MARKER 0xCCCC
#define SPC_DBG_DUMP_DEC_OUT_MARKER 0xDDDD
// GSM CCH (GPRS)
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_GSM_CCH_EQU_IN 0x1
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_GSM_CCH_EQU_OUT 0x2
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_GSM_CCH_DEC_IN 0x4
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_GSM_CCH_DEC_OUT 0x8

// GSM TCH
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_GSM_TCH_EQU_IN 0x10
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_GSM_TCH_EQU_OUT 0x20
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_GSM_TCH_DEC_IN 0x40
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_GSM_TCH_DEC_OUT 0x80
// EGPRS
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_EGPRS_EQU_IN 0x100
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_EGPRS_EQU_OUT 0x200
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_EGPRS_DEC_IN 0x400
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_EGPRS_DEC_OUT 0x800

// PORFILE
#define CPSPCDECODESTART 0x7500
#define CPSPCDECODEMAX 0x5


#ifdef SPC_FUNC_PROFILING
#define SPC_PROFILE_FUNCTION_ENTER(eventName) \
        spal_DbgPxtsProfileFunctionEnter(SPAL_DBG_PXTS_SPC, (CP_ ## eventName))

#define SPC_PROFILE_FUNCTION_EXIT(eventName) \
        spal_DbgPxtsProfileFunctionExit(SPAL_DBG_PXTS_SPC, (CP_ ## eventName))
#else
#define SPC_PROFILE_FUNCTION_ENTER(eventName)
#define SPC_PROFILE_FUNCTION_EXIT(eventName)
#endif


#ifdef SPC_WIN_PROFILING
#define SPC_PROFILE_WINDOW_ENTER(eventName) \
        spal_DbgPxtsProfileWindowEnter(SPAL_DBG_PXTS_SPC, (CP_ ## eventName))

#define SPC_PROFILE_WINDOW_EXIT(eventName) \
        spal_DbgPxtsProfileWindowExit(SPAL_DBG_PXTS_SPC, (CP_ ## eventName))
#else
#define SPC_PROFILE_WINDOW_ENTER(eventName)
#define SPC_PROFILE_WINDOW_EXIT(eventName)
#endif


#ifdef SPC_PULSE_PROFILING
#define SPC_PROFILE_PULSE(eventName) \
        spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_SPC, (CP_ ## eventName))
#else
#define SPC_PROFILE_PULSE(eventName)
#endif


#define SPC_PROFILE_BB_RX_WIN(type) \
        spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_BB_RX_WIN, \
        ( (type>=GSM_WIN_TYPE_MONIT && type<=GSM_WIN_TYPE_INTRF) ? \
          (CP_BB_RX_WIN_MONIT+type-GSM_WIN_TYPE_MONIT) : CP_BB_RX_WIN_UNKNOWN ) )


#define SPC_PROFILE_PULSE_BLKDEC(idx)  \
    spal_DbgPxtsProfilePulse(SPAL_DBG_PXTS_SPC, \
        CPSPCDECODESTART+ ((idx>CPSPCDECODEMAX)?CPSPCDECODEMAX:idx) )


#endif // _SPCP_DEBUG_H_

