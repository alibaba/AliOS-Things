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


#ifndef _HAL_USB_MONITOR_H_
#define _HAL_USB_MONITOR_H_

#include "cs_types.h"

#include "hal_usb.h"


// =============================================================================
// MACROS
// =============================================================================

#define HAL_HOST_USB_FIFO_READ_SIZE              (32)
#define HAL_HOST_USB_BUFFER_OUT_SIZE             (2048)
#define HAL_HOST_USB_BUFFER_IN_SIZE              (1032)

// =============================================================================
// TYPES
// =============================================================================

typedef enum
{
    HAL_HOST_USB_MODE_BOOT                      = 0x00000001,
    HAL_HOST_USB_MODE_SX                        = 0x00000002
} HAL_HOST_USB_MODE_T;

// =============================================================================
// HAL_HOST_USB_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback for decoding trame
// =============================================================================
typedef VOID (*HAL_HOST_USB_CALLBACK_T)(VOID *buffer, UINT16 size);

// ============================================================================
// HAL_HOST_USB_MONITOR_FIFO_ELEM_T
// -----------------------------------------------------------------------------
///
// =============================================================================


typedef struct
{
    UINT32                         Addr;                         //0x00000000
    UINT8                          Rid;                          //0x00000004
    UINT16                         Size;                         //0x00000006
} HAL_HOST_USB_MONITOR_FIFO_ELEM_T; //Size : 0x8



// ============================================================================
// HAL_HOST_USB_MONITOR_BUFFER_IN_T
// -----------------------------------------------------------------------------
///
// =============================================================================


typedef struct
{
    UINT8                          Buffer[HAL_HOST_USB_BUFFER_IN_SIZE]; //0x00000000
} HAL_HOST_USB_MONITOR_BUFFER_IN_T; //Size : 0x408



// ============================================================================
// HAL_HOST_USB_MONITOR_CTX_T
// -----------------------------------------------------------------------------
///
// =============================================================================


typedef struct
{
    HAL_HOST_USB_MODE_T            MonitorMode;                  //0x00000000
    UINT8                          BufferOut[HAL_HOST_USB_BUFFER_OUT_SIZE]; //0x00000004
    HAL_HOST_USB_MONITOR_BUFFER_IN_T BufferIn[2];                //0x0x00000804
    HAL_HOST_USB_MONITOR_FIFO_ELEM_T Fifo[HAL_HOST_USB_FIFO_READ_SIZE]; //0x0x00001014
} HAL_HOST_USB_MONITOR_CTX_T; //Size : 0x1114

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID);

// =============================================================================
// hal_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb
// =============================================================================
PUBLIC VOID hal_HostUsbClose(VOID);

#endif // _HAL_USB_MONITOR_H_
