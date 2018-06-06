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



#ifndef _HAL_FINT_H_
#define _HAL_FINT_H_


/// @defgroup fint HAL Frame interrupt managing
///
/// @{


// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_FINT_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user handler function
// =============================================================================
typedef  VOID (*HAL_FINT_IRQ_HANDLER_T)(VOID);


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_FintIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user function called in case of FINT
/// @param handler Handler function to set
// =============================================================================
PUBLIC VOID hal_FintIrqSetHandler(HAL_FINT_IRQ_HANDLER_T handler);

// =============================================================================
// hal_FintIrqSetMask
// -----------------------------------------------------------------------------
/// Set the mask for the FINT
/// @param mask Mask to set. If \c TRUE, FINT are enabled. If \c FALSE,
/// they don't occur.
// =============================================================================
PUBLIC VOID hal_FintIrqSetMask(BOOL mask);


// =============================================================================
// hal_FintIrqGetMask
// -----------------------------------------------------------------------------
/// This function is used to recover the FINT mask
/// @return The Fint mask: \c TRUE if FINT can happen, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_FintIrqGetMask(VOID);

// =============================================================================
// hal_FintIrqGetStatus
// -----------------------------------------------------------------------------
/// Check the Frame interrupt status.
/// @return \c TRUE if the FINT had occured \n
///         \c FALSE otherwise
// =============================================================================
PUBLIC BOOL hal_FintIrqGetStatus(VOID);


/// @} <-- End of the fint group

#endif // _HAL_FINT_H_


