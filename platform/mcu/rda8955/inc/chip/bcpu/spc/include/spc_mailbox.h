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


#ifndef _SPC_MAILBOX_H_
#define _SPC_MAILBOX_H_


#if (SPC_IF_VER == 0)
#include "spc_mailbox_v0.h"
#elif (SPC_IF_VER == 1)
#include "spc_mailbox_v1.h"
#elif (SPC_IF_VER == 2)
#include "spc_mailbox_v2.h"
#elif (SPC_IF_VER == 3)
#include "spc_mailbox_v3.h"
#elif (SPC_IF_VER == 4)
#include "spc_mailbox_v4.h"
#elif (SPC_IF_VER == 5)
#include "spc_mailbox_v5.h"
#else
#include "spc_mailbox_std.h"
#endif

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================



#endif

