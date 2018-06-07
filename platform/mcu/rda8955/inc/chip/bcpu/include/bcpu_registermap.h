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


#ifndef _BCPU_REGISTERMAP_H_
#define _BCPU_REGISTERMAP_H_

#include "cs_types.h"
#include "hal_map_engine.h"

// =============================================================================
// bcpu_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
/// THIS FUNCTION IS IN BCPU MODULE BUT WILL BE EXECUTED BY XCPU !!
// =============================================================================
PUBLIC VOID bcpu_RegisterYourself(VOID);

#endif // _BCPU_REGISTERMAP_H_
