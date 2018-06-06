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


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_BT_HOST_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// This is a generic type for a user irq handler, called when BT has some
/// data to send to host.
// =============================================================================
typedef VOID (*HAL_BT_HOST_IRQ_HANDLER_T)(VOID);


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_BtSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq indicating that BT has sent some data
/// to host.
// =============================================================================
PUBLIC VOID hal_BtSetHostIrqHandler(HAL_BT_HOST_IRQ_HANDLER_T handler);


// =============================================================================
// hal_BtStart
// -----------------------------------------------------------------------------
/// Start BT CPU.
// =============================================================================
PUBLIC VOID hal_BtStart(VOID *main, VOID *irqHdlr, VOID *stackStartAddr);


// =============================================================================
// hal_BtStop
// -----------------------------------------------------------------------------
/// Stop BT CPU.
// =============================================================================
PUBLIC VOID hal_BtStop(VOID);


// =============================================================================
// hal_BtWakeUp
// -----------------------------------------------------------------------------
/// Wake up BT CPU.
// =============================================================================
PUBLIC VOID hal_BtWakeUp(VOID);


// =============================================================================
// hal_BtNotifyDataSent
// -----------------------------------------------------------------------------
/// Tell BT that host just sends some data.
// =============================================================================
PUBLIC VOID hal_BtNotifyDataSent(VOID);



