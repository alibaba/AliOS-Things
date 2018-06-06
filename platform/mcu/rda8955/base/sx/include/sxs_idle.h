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


#include "sxs_type.h"

//------------------------------------------------------------------
// FS idle hook functions

extern void sxs_RegisterFsIdleHookFunc(void (*funcPtr)(void));

extern void sxs_FsIdleHook(void);

//------------------------------------------------------------------
// Debug idle hook functions

extern bool sxs_RegisterDebugIdleHookFunc(void (*funcPtr)(void));

extern bool sxs_ActivateDebugIdleHookFunc(void (*funcPtr)(void));

extern void sxs_DebugIdleHook(void);

