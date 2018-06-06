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



#ifndef _HAL_OVERLAY_H_
#define _HAL_OVERLAY_H_


#include "cs_types.h"
#include "hal_error.h"

/// @defgroup overlay HAL Overlay Driver
/// The overlaying mechanism is used to share fast memory
/// between several codes requiring high performance memory access, but not
/// at the same time.
///
/// @par
/// Several memory locations can be shared. They are described by the
/// #HAL_OVERLAY_MEMORY_LOCATION_T type.
///
/// @par Usage
/// For each memory location, a set of overlay is defined. Overlay represents
/// a set of code and data that are present at the same time at a given
/// memory location. Overlays for a given memory location are
/// exclusives from each others. They need to be loaded before the code or
/// data contained in an overlay can be used. (Function #hal_OverlayLoad()).
///
/// Before an other instance can be loaded at a memory location, the previous
/// instance must be unloaded (#hal_OverlayUnload()).
///
/// @par Definition of an overlay content (TODO)
/// IMPORTANT NOTE: Files in an overlay MUST BE PART OF A LIB.
///
/// @{

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// HAL_OVERLAY_MEMORY_LOCATION_T
// -----------------------------------------------------------------------------
/// Defines the memory location available for overlaying.
/// This is used to build the #HAL_OVERLAY_ID_T type.
/// Not all location may exist on a chip.
// =============================================================================
typedef enum
{
    /// Internal SRAM memory.
    HAL_OVERLAY_MEMORY_LOCATION_INT_SRAM = 0x0,

    /// Fast CPU memory (Not available in Greenstone.).
    HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM = 0x00010000,

} HAL_OVERLAY_MEMORY_LOCATION_T;


// =============================================================================
// HAL_OVERLAY_ID_T
// -----------------------------------------------------------------------------
/// This Id is used to refer to a given overlay instance. Each memory location
/// has HAL_OVERLAY_ID_QTY possible overlays that can be loaded on this, and
/// only this, memory location.
// =============================================================================
typedef enum
{
    /// The first Ids refers to overlay
    /// loadable in internal SRAM.
    HAL_OVERLAY_INT_SRAM_ID_0 = HAL_OVERLAY_MEMORY_LOCATION_INT_SRAM,
    HAL_OVERLAY_INT_SRAM_ID_1,
    HAL_OVERLAY_INT_SRAM_ID_2,
    HAL_OVERLAY_INT_SRAM_ID_3,
    HAL_OVERLAY_INT_SRAM_ID_4,
    HAL_OVERLAY_INT_SRAM_ID_5,
    HAL_OVERLAY_INT_SRAM_ID_QTY,

    /// Those overlay are loadable in
    /// the fast CPU ram.
    /// (Not available in Greenstone).
    HAL_OVERLAY_FAST_CPU_RAM_ID_0 = HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM,
    HAL_OVERLAY_FAST_CPU_RAM_ID_1,
    HAL_OVERLAY_FAST_CPU_RAM_ID_2,
    HAL_OVERLAY_FAST_CPU_RAM_ID_3,
    HAL_OVERLAY_FAST_CPU_RAM_ID_4,
    HAL_OVERLAY_FAST_CPU_RAM_ID_5,
    HAL_OVERLAY_FAST_CPU_RAM_ID_QTY,

} HAL_OVERLAY_ID_T;



// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// hal_OverlayLoad
// -----------------------------------------------------------------------------
/// Load the \c id overlay in memory. It is loaded at the memory specified by
/// the id value (Internal SRAM, etc ...)
///
/// @param id Id of the overlay of \c location memory to load.
/// @return #HAL_ERR_NO when everything is fine.
///         #HAL_ERR_RESOURCE_BUSY when this memory location is already
/// being used by another overlay.
// =============================================================================
PUBLIC HAL_ERR_T hal_OverlayLoad(HAL_OVERLAY_ID_T id);


// =============================================================================
// hal_OverlayUnload
// -----------------------------------------------------------------------------
/// Unload the \c id overlay from memory.
///
/// @param id Id of the overlay of \c location memory to load.
/// @return #HAL_ERR_NO when everything is fine.
///         #HAL_ERR_RESOURCE_BUSY when this memory location is already
/// being used by another overlay.
// =============================================================================
PUBLIC HAL_ERR_T hal_OverlayUnload(HAL_OVERLAY_ID_T id);


// ========================================================================
// hal_OverlayIsLoaded
// -----------------------------------------------------------------------------
/// Check if the \c id overlay is loaded into memory.
///
/// @param id Id of the overlay we check if it is loaded.
/// @return \c TRUE if \c Id is loaded,
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_OverlayIsLoaded(HAL_OVERLAY_ID_T id);

/// @} <- End of the overlay group.


#endif  //_HAL_OVERLAY_H_



