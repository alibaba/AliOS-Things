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


#ifndef  _HAL_COMREGS_H_
#define  _HAL_COMREGS_H_

#include "cs_types.h"

/// @defgroup comregs HAL Comregs
/// This document describes the characteristics of  the Comregs module
/// and how to use it via its HAL API.
///
/// @{
///

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// HAL_COMREGS_XCPU_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the bcpu to the
///     xcpu. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 4 or 8 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_COMREGS_XCPU_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_COMREGS_XCPU_IRQ_SLOT_1,
    HAL_COMREGS_XCPU_IRQ_SLOT_QTY
} HAL_COMREGS_XCPU_IRQ_SLOT_T;


// =============================================================================
// HAL_COMREGS_BCPU_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the xcpu to the
///     bcpu. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 4 or 8 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_COMREGS_BCPU_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_COMREGS_BCPU_IRQ_SLOT_1,
    HAL_COMREGS_BCPU_IRQ_SLOT_QTY
} HAL_COMREGS_BCPU_IRQ_SLOT_T;


#ifdef CHIP_HAS_AP
// =============================================================================
// HAL_APCOMREGS_AP_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the XCPU to the
///     AP. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 8 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_APCOMREGS_AP_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_APCOMREGS_AP_IRQ_SLOT_1,
    HAL_APCOMREGS_AP_IRQ_SLOT_QTY
} HAL_APCOMREGS_AP_IRQ_SLOT_T;


// =============================================================================
// HAL_APCOMREGS_XCPU_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the AP to the
///     XCPU. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 8 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_APCOMREGS_XCPU_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_APCOMREGS_XCPU_IRQ_SLOT_1,
    HAL_APCOMREGS_XCPU_IRQ_SLOT_QTY
} HAL_APCOMREGS_XCPU_IRQ_SLOT_T;
#endif


#ifdef CHIP_HAS_WCPU
// =============================================================================
// HAL_WDCOMREGS_WCPU_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the XCPU to the
///     WCPU. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 4 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_WDCOMREGS_WCPU_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_WDCOMREGS_WCPU_IRQ_SLOT_1,
    HAL_WDCOMREGS_WCPU_IRQ_SLOT_QTY
} HAL_WDCOMREGS_WCPU_IRQ_SLOT_T;


// =============================================================================
// HAL_WDCOMREGS_XCPU_IRQ_SLOT_T
// -----------------------------------------------------------------------------
///     The comregs provides two irq 'slots' from the WCPU to the
///     XCPU. The user can set up independent functions to
///     handle irq occuring on each one of them. \n
///     Their status is 8 bits wide.
///     They are on different IRQ lines, and therefore different priorities
// =============================================================================
typedef enum
{
    /// Prioritary slot
    HAL_WDCOMREGS_XCPU_IRQ_SLOT_0 = 0,
    /// Secondary slot
    HAL_WDCOMREGS_XCPU_IRQ_SLOT_1,
    HAL_WDCOMREGS_XCPU_IRQ_SLOT_QTY
} HAL_WDCOMREGS_XCPU_IRQ_SLOT_T;
#endif


// =============================================================================
// HAL_COMREGS_IRQ_MASK_T
// -----------------------------------------------------------------------------
/// This structures defines a generic mask for each one of the comregs slot.
// =============================================================================
typedef struct
{
    UINT32 mask:8;
} HAL_COMREGS_IRQ_MASK_T;

typedef struct
{
    UINT32 status:8;
} HAL_COMREGS_IRQ_STATUS_T;


// =============================================================================
// HAL_COMREGS_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// This is a generic type for a user irq handler, called when one slot
/// of the comregs trigs an IRQ.
// =============================================================================
/// Type of the user handler function
typedef VOID (*HAL_COMREGS_IRQ_HANDLER_T)(HAL_COMREGS_IRQ_STATUS_T);


// =============================================================================
// HAL_APCOMREGS_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// This is a generic type for a user irq handler, called when one slot
/// of the comregs trigs an IRQ.
// =============================================================================
/// Type of the user handler function
typedef VOID (*HAL_APCOMREGS_IRQ_HANDLER_T)(HAL_COMREGS_IRQ_STATUS_T);


// =============================================================================
// HAL_WDCOMREGS_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// This is a generic type for a user irq handler, called when one slot
/// of the comregs trigs an IRQ.
// =============================================================================
/// Type of the user handler function
typedef VOID (*HAL_WDCOMREGS_IRQ_HANDLER_T)(HAL_COMREGS_IRQ_STATUS_T);


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_ComregsGetSnap
// -----------------------------------------------------------------------------
/// Get the comregs IT snapshot value
// =============================================================================
PUBLIC UINT32 hal_ComregsGetSnap(VOID);

// =============================================================================
// hal_ComregsGetSnapGsm
// -----------------------------------------------------------------------------
/// Get the comregs IT snapshot value
// =============================================================================
PUBLIC UINT32 hal_ComregsGetSnapGsm(VOID);
// =============================================================================
// hal_ComregsSetSnapCfg
// -----------------------------------------------------------------------------
/// Set the snapshot configuration register
/// @param isTrio When \c TRUE, the snapshot wrap value is 3 \n
///                        \c FALSE, it is 2.
// =============================================================================
PUBLIC VOID hal_ComregsSetSnapCfg(BOOL isTrio);


// IRQ handling functions

// =============================================================================
// hal_ComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_ComregsSetIrqHandler(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
                                     HAL_COMREGS_IRQ_HANDLER_T handler);


// =============================================================================
// hal_ComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
                               HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
                               HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_ComregsBcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the BCPU
///
/// @param slot BCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_ComregsBcpuIrqTrigger(HAL_COMREGS_BCPU_IRQ_SLOT_T slot,
                                      HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the BPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_ComregsXcpuIrqTrigger(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
                                      HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ComregsBcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the BCPU
/// @param slot BCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsBcpuIrqGetStatus(HAL_COMREGS_BCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_ComregsXcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the XCPU
/// @param slot BCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ComregsXcpuIrqGetStatus(HAL_COMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_ComregsBcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear
/// @param slot BCPU IRQ slot on which clear  the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_ComregsBcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_T slot,
        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which clear  the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
///
// =============================================================================
PUBLIC VOID hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_T slot,
        HAL_COMREGS_IRQ_MASK_T mask);


#ifdef CHIP_HAS_AP
// =============================================================================
// hal_ApComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_ApComregsSetIrqHandler(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot,
                                       HAL_APCOMREGS_IRQ_HANDLER_T handler);


// =============================================================================
// hal_ApComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ApComregsSetMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot,
                                 HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ApComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_ApComregsClrMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot,
                                 HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ApComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsGetMask(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_ApComregsApIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the AP
///
/// @param slot AP IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the AP
/// leave the corresponding bit unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsApIrqTrigger(HAL_APCOMREGS_AP_IRQ_SLOT_T slot,
                                      HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ApComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU from AP
///
/// @param slot AP IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the AP
/// leave the corresponding bit unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsXcpuIrqTrigger(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot,
                                        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ApComregsXcpuIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the XCPU from AP
/// @param slot AP XCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsXcpuIrqGetStatus(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_ApComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU from AP
///
/// @param slot AP XCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_ApComregsXcpuIrqClearCause(HAL_APCOMREGS_XCPU_IRQ_SLOT_T slot,
        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_ApComregsApIrqGetStatus
// -----------------------------------------------------------------------------
/// Get status of IRQ triggerd on the AP from XCPU
/// @param slot AP XCPU IRQ slot from which to get the status.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_ApComregsApIrqGetStatus(HAL_APCOMREGS_AP_IRQ_SLOT_T slot);
#endif // CHIP_HAS_AP


#ifdef CHIP_HAS_WCPU
// =============================================================================
// IRQ handling functions
// =============================================================================
// hal_WdComregsSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq occurs
/// on the corresponding slot
///
/// @param slot The slot to associate an handler with
/// @param handler The handler function
///
// =============================================================================
PUBLIC VOID hal_WdComregsSetIrqHandler(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,
                                       HAL_WDCOMREGS_IRQ_HANDLER_T handler);


// =============================================================================
// hal_WdComregsSetMask
// -----------------------------------------------------------------------------
/// Set the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the set function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be set. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_WdComregsSetMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,
                                 HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_WdComregsClrMask
// -----------------------------------------------------------------------------
/// Clear the mask of a slot
///
/// The width is the one specified for each slot.
/// Only the lowest bits are used. This mask operate on a
/// set-clear basis. This is the clear function
///
/// @param slot Slot whose mask is to be set
/// @param mask Mask to set. Bits to '1' will be cleared. The ones to
/// '0' remain unchanged
// =============================================================================
PUBLIC VOID hal_WdComregsClrMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,
                                 HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_WdComregsGetMask
// -----------------------------------------------------------------------------
/// Get the mask of a slot
///
/// @param slot The slot whose masks is wanted
/// @return The value of the mask set for the slot given
/// as a parameter.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsGetMask(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_WdComregsWcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the WCPU
///
/// @param slot WCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the WPCU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsWcpuIrqTrigger(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot,
                                        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_WdComregsXcpuIrqTrigger
// -----------------------------------------------------------------------------
/// Trigger IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which trigger the IRQ.
/// @param mask IRQ cause sent to the XCPU
/// leave the corresponding bit unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsXcpuIrqTrigger(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,
                                        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_WdComregsWcpuIrqGetCauseStatus
// -----------------------------------------------------------------------------
/// Get cause of IRQ triggerd on the WCPU
/// @param slot WCPU IRQ slot from which to get the cause.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsWcpuIrqGetCauseStatus(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_WdComregsXcpuIrqGetCauseStatus
// -----------------------------------------------------------------------------
/// Get cause of IRQ triggerd on the XCPU
/// @param slot XCPU IRQ slot from which to get the cause.
/// @return The interrupt status of the given slot.
// =============================================================================
PUBLIC HAL_COMREGS_IRQ_MASK_T hal_WdComregsXcpuIrqGetCauseStatus(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot);


// =============================================================================
// hal_WdComregsWcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear
/// @param slot WCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
// =============================================================================
PUBLIC VOID hal_WdComregsWcpuIrqClearCause(HAL_WDCOMREGS_WCPU_IRQ_SLOT_T slot,
        HAL_COMREGS_IRQ_MASK_T mask);


// =============================================================================
// hal_WdComregsXcpuIrqClearCause
// -----------------------------------------------------------------------------
/// Clear IRQs on the XCPU
///
/// @param slot XCPU IRQ slot on which clear the IRQ cause.
/// @param mask IRQ cause to clear, leaving the others unchanged.
///
// =============================================================================
PUBLIC VOID hal_WdComregsXcpuIrqClearCause(HAL_WDCOMREGS_XCPU_IRQ_SLOT_T slot,
        HAL_COMREGS_IRQ_MASK_T mask);
#endif // CHIP_HAS_WCPU


/// @}

#endif // _HAL_COMREGS_H_




