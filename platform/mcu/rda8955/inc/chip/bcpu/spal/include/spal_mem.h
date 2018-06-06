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


#ifndef _SPAL_MEM_H_
#define _SPAL_MEM_H_

#include "cs_types.h"


// =============================================================================
// BB_SRAM
// -----------------------------------------------------------------------------
/// Macro used to put some specific data in BB SRAM.
// =============================================================================
#define SPAL_BBSRAM_PATCH_TEXT      __attribute__((section(".bbsram_patch_text")))
#define SPAL_BBSRAM_PATCH_RODATA    __attribute__((section(".bbsram_patch_rodata")))
#define SPAL_BBSRAM_PATCH_DATA      __attribute__((section(".bbsram_patch_data")))
#define SPAL_BBSRAM_PATCH_UCDATA    __attribute__((section(".bbsram_patch_ucdata")))
#define SPAL_BBSRAM_PATCH_UCBSS     __attribute__((section(".bbsram_patch_ucbss")))
#define SPAL_BBSRAM_PATCH_BSS       __attribute__((section(".bbsram_patch_bss")))


// =============================================================================
// DUALPORT_SRAM
// -----------------------------------------------------------------------------
/// Macro used to put some specific data in DUALPORT SRAM.
// =============================================================================
#if (SPC_IF_VER >= 4)
#define SPAL_DUALPORT_SRAM_FUNC     __attribute__((section(".dualport_sram_text")))
#define SPAL_DUALPORT_SRAM_RODATA   __attribute__((section(".dualport_sram_rodata")))
#define SPAL_DUALPORT_SRAM_DATA     __attribute__((section(".dualport_sram_data")))
#define SPAL_DUALPORT_SRAM_UNCACHED_DATA    __attribute__((section(".dualport_sram_ucdata")))
#define SPAL_DUALPORT_SRAM_UNCACHED_BSS     __attribute__((section(".dualport_sram_ucbss")))
#define SPAL_DUALPORT_SRAM_BSS      __attribute__((section(".dualport_sram_bss")))
#endif


// =============================================================================
// INT_SRAM
// -----------------------------------------------------------------------------
/// Macro used to put some specific data in INT SRAM.
// =============================================================================
#define SPAL_INTSRAM_PATCH_TEXT     __attribute__((section(".sramtext")))
#define SPAL_INTSRAM_PATCH_RODATA   __attribute__((section(".sramrodata")))
#define SPAL_INTSRAM_PATCH_DATA     __attribute__((section(".sramdata")))
#define SPAL_INTSRAM_PATCH_UCDATA   __attribute__((section(".sramucdata")))
#define SPAL_INTSRAM_PATCH_UCBSS    __attribute__((section(".sramucbss")))
#define SPAL_INTSRAM_PATCH_BSS      __attribute__((section(".srambss")))


// =============================================================================
// SPAL_FUNC_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in internal SRAM
// =============================================================================
#if (SPC_IF_VER < 4)
#define SPAL_FUNC_INTERNAL   SPAL_INTSRAM_PATCH_TEXT
#else
#define SPAL_FUNC_INTERNAL   SPAL_BBSRAM_PATCH_TEXT
#endif


// =============================================================================
// SPAL_DATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal SRAM
// =============================================================================
#if (SPC_IF_VER < 4)
#define SPAL_DATA_INTERNAL   SPAL_INTSRAM_PATCH_DATA
#else
#define SPAL_DATA_INTERNAL   SPAL_BBSRAM_PATCH_DATA
#endif


// =============================================================================
// SPAL_UNCACHED_DATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal SRAM
// =============================================================================
#if (SPC_IF_VER < 4)
#define SPAL_UNCACHED_DATA_INTERNAL   SPAL_INTSRAM_PATCH_UCDATA
#else
#define SPAL_UNCACHED_DATA_INTERNAL   SPAL_BBSRAM_PATCH_UCDATA
#endif


// =============================================================================
// SPAL_BBSRAM_GLOBAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific data in the ".bbsram_globals" section in
/// BBSRAM.
// =============================================================================
#define  SPAL_BBSRAM_GLOBAL  __attribute__((section(".bbsram_globals")))


///  @} <- End of the mem_map group


#endif

