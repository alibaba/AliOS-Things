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


#ifndef __SPAL_IRQ_H__
#define __SPAL_IRQ_H__


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "global_macros.h"
#include "bb_irq.h"
#include "sys_ctrl.h"
#if (SPC_IF_VER == 4) // For 8808 only
#include "cfg_regs.h"
#endif

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define SPAL_IRQ_CHIP_PROD_ID_8808       0x8808

#define SPAL_IRQ_CHIP_PROD_ID_8808S      0x880a


//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------

// ============================================================================
// SPAL_IRQ_STATUS_T
// ----------------------------------------------------------------------------
/// This structure is used to represent the irq status
// ============================================================================
typedef enum
{
    /// Frame interrupt
    SPAL_IRQ_FINT,
    /// Rx channel half buffer interrupt
    SPAL_IRQ_RX_HALF,
    /// Rx channel full buffer interrupt
    SPAL_IRQ_RX_FULL,
    /// Tcu0 interrupt
    SPAL_IRQ_TCU0,
    /// Tcu0 interrupt
    SPAL_IRQ_TCU1,
    /// Unknown IRQ, used for IRQ hook
    SPAL_IRQ_OTHER,

} SPAL_IRQ_STATUS_T;

// ============================================================================
// SPAL_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// User handler function type
// ============================================================================
typedef VOID (*SPAL_IRQ_HANDLER_T)(SPAL_IRQ_STATUS_T);


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


// =============================================================================
// spal_IrqEnterCriticalSection
// -----------------------------------------------------------------------------
/// This function enters in a critical section (relative to the hardware
/// interrupts) and return a status that should be given to the
/// #spal_IrqExitCriticalSection function.
///
/// @return  The \c status that should be given to the #spal_IrqExitCriticalSection
/// function.
// =============================================================================
INLINE UINT32 spal_IrqEnterCriticalSection(VOID)
{
    UINT32 status = hwp_bbIrq->SC;
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    return status;
}


// =============================================================================
// spal_IrqExitCriticalSection
// -----------------------------------------------------------------------------
/// This function leaves the critical section. The <CODE>status</CODE>
/// parameter is the return value of the #spal_IrqEnterCriticalSection call
/// that opened the critical section.
///
/// @param status The value returned by #spal_IrqEnterCriticalSection.
// =============================================================================
INLINE VOID spal_IrqExitCriticalSection(UINT32 status)
{
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    hwp_bbIrq->SC = status;
}

// =============================================================================
// spal_IrqWriteCpuSleep
// -----------------------------------------------------------------------------
/// This function writes <CODE>mode</CODE> in the CPU Sleep register.
///
/// @param mode
// =============================================================================
INLINE VOID spal_IrqWriteCpuSleep(UINT32 mode)
{
#if (SPC_IF_VER == 4) // For 8808 only
    if (GET_BITFIELD(hwp_configRegs->CHIP_ID, CFG_REGS_PROD_ID) == SPAL_IRQ_CHIP_PROD_ID_8808)
    {
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
        hwp_sysCtrl->Clk_BB_Disable = SYS_CTRL_DISABLE_BB_BCPU;
        hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    }
#endif
    hwp_bbIrq->Cpu_Sleep = mode;
}

// =============================================================================
// spal_IrqReadCpuSleep
// -----------------------------------------------------------------------------
/// This function reads and returns the value of CPU Sleep register.
///
/// @return  The \c mode
// =============================================================================
INLINE UINT32 spal_IrqReadCpuSleep(VOID)
{
    UINT32 mode = hwp_bbIrq->Cpu_Sleep;
    return mode;
}

#endif // __SPAL_IRQ_H__


