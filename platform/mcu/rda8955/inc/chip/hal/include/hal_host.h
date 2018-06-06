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



#ifndef _HAL_HOST_H_
#define _HAL_HOST_H_

EXTERN_C_START

/// @defgroup host HAL Host Port Driver/Client
/// That group describes briefly the exchange media available
/// betweem HAL and a Host (Personal Computer, Mac, etc)
/// connected through the Host Port.
///
/// @{
///


#include "cs_types.h"



// =============================================================================
// hal_HstSendEvent
// -----------------------------------------------------------------------------
/// Send an event through the host port
/// @param ch Event sent through the host port.
// =============================================================================
PUBLIC BOOL hal_HstSendEvent(UINT32 ch);



// =============================================================================
// hal_HstWaitLastEventSent
// -----------------------------------------------------------------------------
/// Check if the last event has been sent on the host port.
/// You can't send new event after this function
// =============================================================================
PUBLIC VOID hal_HstWaitLastEventSent(VOID);



// @} // <-- End of the host group.

EXTERN_C_END

#endif




