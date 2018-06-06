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



#include "cs_types.h"
#include "hal_map_engine.h"
#include "chip_id.h"

#define SPC_VERSION_NUMBER SPC_IF_VER

// Tweak the version string
#ifdef CHIP_BYPASS_ROM
#define SPC_VERSION_STRING "SPC recompiled (not using rom)"
#elif (defined (REWRITE_ROM_AT_BOOT))
#define SPC_VERSION_STRING "SPC recompiled (FPGA soft rom)"
#else
// Romed version.

#define SPC_VERSION_STRING "SPC in rom"

#endif // Recompiled, soft rom or chip ?

// include generated headers (to get svn versions)
#include "spcp_version.h"


// =============================================================================
// g_spcMapVersion
// -----------------------------------------------------------------------------
/// Contains the version of SPC.
// =============================================================================
PROTECTED CONST HAL_MAP_VERSION_T g_spcMapVersion
    = SPC_VERSION_STRUCT;


