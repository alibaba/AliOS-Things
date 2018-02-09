/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

VOID 
HalTimerOpInit_Patch(
    IN  VOID *Data
)
{
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) Data;

    pHalTimerOp->HalGetTimerId = HalGetTimerIdRtl8195a;    
#ifdef CONFIG_CHIP_E_CUT
    pHalTimerOp->HalTimerInit = (BOOL (*)(void*))HalTimerInitRtl8195a_V04;   
#else
    pHalTimerOp->HalTimerInit = (BOOL (*)(void*))HalTimerInitRtl8195a_Patch;   
#endif
#if defined(CONFIG_CHIP_C_CUT) || defined(CONFIG_CHIP_E_CUT)
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountRtl8195aV02;    
#else
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountRtl8195a_Patch;    
#endif
    pHalTimerOp->HalTimerIrqClear = HalTimerIrqClearRtl8195a;
    pHalTimerOp->HalTimerDis = HalTimerDisRtl8195a_Patch;
    pHalTimerOp->HalTimerEn = HalTimerEnRtl8195a_Patch;
    pHalTimerOp->HalTimerDumpReg = HalTimerDumpRegRtl8195a;
}

