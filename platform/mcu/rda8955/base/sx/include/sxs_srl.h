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






#ifndef __SXS_SRL_H__
#define __SXS_SRL_H__

#include "sxs_type.h"

// =============================================================================
// sxs_SetSerialRoute
// -----------------------------------------------------------------------------
/// Configure the user Tx, Status buffer and Rx management functions.
/// @param PortIdx      Port index.
/// @param ReadTxData   User Tx buffer function (manage the buffer that contains
///                     user's Tx data)
/// @param WriteRxData  User Rx function (manage the data received from remote
///                     computer.)
/// @param GetBufStatus User Tx buffer status function (return the amount of
///                     data bytes to be transmitted in the Tx buffer).
/// @param RmtCpuIdx    Remote CPU index.
// =============================================================================
void sxs_SetSerialRoute (u8  PortIdx,
                         u16 (*ReadTxData) (u8 *BufOut, u16 Size, u8 RmtCpuIdx),
                         u16 (*WriteRxData) (u8 *BufIn, u16 Size, u8 RmtCpuIdx),
                         u16 (*GetBufStatus) (u8 RmtCpuIdx),
                         u8 RmtCpuIdx);

// =============================================================================
// sxs_GetSerialStatus
// -----------------------------------------------------------------------------
/// Return the status of the specified uart.
/// @param PortIdx  Port index.
/// @return         Current Status.
// =============================================================================
u8 sxs_GetSerialStatus (u8 PortIdx);

// =============================================================================
// sxs_SetSerialStatus
// -----------------------------------------------------------------------------
/// Set the status of the specified uart.
/// @param Status
/// @param PortIdx  Port index.
/// @return         null.
// =============================================================================
void sxs_SetSerialStatus (u8 Status, u8 PortIdx);

// =============================================================================
// sxs_SetSerialCfg
// -----------------------------------------------------------------------------
/// Set serial configuration.
/// @param Setting  Settings.
/// @param PortIdx  Serial port index.
// =============================================================================
void sxs_SetSerialCfg   (u16 Setting, u8 PortIdx);

// =============================================================================
// sxs_GetSerialCfg
// -----------------------------------------------------------------------------
/// Return the configuration of the specified uart.
/// @param PortIdx  Port index.
/// @return         Current setting.
// =============================================================================
u16  sxs_GetSerialCfg   (u8 PortIdx);

// =============================================================================
// sxs_InitSerial
// -----------------------------------------------------------------------------
/// Serial port initialzation.
/// @param Setting  Settings.
/// @param PortIdx  Port index.
/// @param Route    Remote CPU index for serial route.
// =============================================================================
void sxs_InitSerial     (u16 Setting, u8 PortIdx, u8 Route);

// =============================================================================
// sxs_CloseSerial
// -----------------------------------------------------------------------------
/// Serial port Close.
/// @param PortIdx  Port index.
// =============================================================================
void sxs_CloseSerial (u8 PortIdx);

// =============================================================================
// sxs_SerialFlush
// -----------------------------------------------------------------------------
/// Move data from user Tx buffer to the serial Tx buffer,
///  fill the Tx FIFO if necessary.
///  Release data received to the user.
///  Check for flow control release.
///  Must be called from Idle task.
/// @return         TRUE when at least one user buffer is not empty.
// =============================================================================
u8   sxs_SerialFlush    (void);

// =============================================================================
// sxs_SetEcho
// -----------------------------------------------------------------------------
/// Configure the echo.
/// @param Setting  #SXS_ECHO_ON / #SXS_ECHO_OFF
/// @param PortIdx  Port index.
// =============================================================================
void sxs_SetEcho        (u8 Setting, u8 PortIdx);

// =============================================================================
// sxs_IsSerialOpened
// -----------------------------------------------------------------------------
/// Report the open status of the serial port.
/// @param PortIdx  Port index.
/// @return         \c TRUE if opened.
// =============================================================================
u8   sxs_IsSerialOpened (u8 PortIdx);

// =============================================================================
// sxs_LockFlowControl
// -----------------------------------------------------------------------------
/// Lock the flow control to specified mode
/// @param PortIdx  Port index.
/// @param Status   #SXS_HOLD_RX / #SXS_RELEASE_RX
// =============================================================================
void sxs_LockFlowControl (u8 PortIdx, u8 Status);

// =============================================================================
// sxs_UnlockFlowControl
// -----------------------------------------------------------------------------
/// Unlock and restore the flow control
/// @param PortIdx  Port index.
// =============================================================================
void sxs_UnlockFlowControl (u8 PortIdx);

// =============================================================================
// sxs_SetFlowControl
// -----------------------------------------------------------------------------
/// Configure the flow control
/// @param PortIdx  Port index.
/// @param Status   #SXS_HOLD_RX / #SXS_RELEASE_RX
// =============================================================================
void sxs_SetFlowControl (u8 PortIdx, u8 Status);

// =============================================================================
// sxs_GetFlowControlStatus
// -----------------------------------------------------------------------------
/// Get current flow control status
/// @param PortIdx  Port index.
/// @return Flow control status
// =============================================================================
u8 sxs_GetFlowControlStatus (u8 PortIdx);

// =============================================================================
// sxs_SetModemStatusNotification
// -----------------------------------------------------------------------------
/// Set the modem satus notification function. Such a function is called each
/// time a change is detected on a signal set in the mask.
/// @param PortIdx  Port index.
/// @param Mask     a combination of #SXS_DSR_ON, #SXS_CTS_ON and #SXS_RING_ON.
/// @param StatusNotification   Pointer on the notification function.
// =============================================================================
void sxs_SetModemStatusNotification (u8 PortIdx, u8 Mask, void (*StatusNotification)(u8 PortIdx, u8 Status, u8 LastStatus));

// =============================================================================
// sxs_SetModemStatus
// -----------------------------------------------------------------------------
///
/// @param Status   a combination of #SXS_DSR_ON / #SXS_DSR_OFF,
//                  #SXS_CTS_ON / #SXS_CTS_OFF, #SXS_RING_ON / #SXS_RING_OFF and
//                  #SXS_DCD_ON / #SXS_DCD_OFF.
/// @param PortIdx  Port index.
// =============================================================================
void sxs_SetModemStatus (u8 Status, u8 PortIdx);

#define SXS_NB_DATA_POS     0
#define SXS_NB_DATA_MSK     (0x03 << SXS_NB_DATA_POS)
#define SXS_NB_STOP_POS     2
#define SXS_NB_STOP_MSK     (0x01 << SXS_NB_STOP_POS)
#define SXS_PARITY_POS      3
#define SXS_PARITY_MSK      (0x03 << SXS_PARITY_POS)

#define SXS_RATE_POS        5
#define SXS_RATE_MSK        (0x0F << SXS_RATE_POS)

#define SXS_5_DATA          0
#define SXS_6_DATA          1
#define SXS_7_DATA          2
#define SXS_8_DATA          3

#define SXS_1_STOP          0
#define SXS_1_5_STOP        1
#define SXS_2_STOP          2

#define SXS_PARITY_DISABLE  0
#define SXS_PARITY_ODD      1
#define SXS_PARITY_EVEN     3

enum
{
    SXS_R460800, SXS_R230400, SXS_R115200,
    SXS_R57600,  SXS_R38400,  SXS_R19200,
    SXS_R9600,   SXS_R4800,   SXS_R2400,
    SXS_R1200,   SXS_R600,    SXS_R300,
    SXS_R75,
    SXS_NB_SERIAL_RATE
};

#define SXS_AUTO_BAUD         (1 << 31)
#define SXS_RATE_NOT_AVAILABLE 0

#define SXS_SET_NB_DATA(a) ((a << SXS_NB_DATA_POS)   & SXS_NB_DATA_MSK)
#define SXS_SET_NB_STOP(a) ((a << SXS_NB_STOP_POS)   & SXS_NB_STOP_MSK)
#define SXS_SET_PARITY(a)  ((a << SXS_PARITY_POS)    & SXS_PARITY_MSK)
#define SXS_SET_RATE(a)    ((a << SXS_RATE_POS)      & SXS_RATE_MSK)

#define SXS_GET_NB_DATA(a) ((a & SXS_NB_DATA_MSK)   >> SXS_NB_DATA_POS)
#define SXS_GET_NB_STOP(a) ((a & SXS_NB_STOP_MSK)   >> SXS_NB_STOP_POS)
#define SXS_GET_PARITY(a)  ((a & SXS_PARITY_MSK)    >> SXS_PARITY_POS)
#define SXS_GET_RATE(a)    ((a & SXS_RATE_MSK)      >> SXS_RATE_POS)

#define SXS_GET_FRAMING(a) (SXS_GET_NB_DATA(a) | SXS_GET_NB_STOP(a) | SXS_GET_PARITY(a))

#define SXS_ECHO_ON  1
#define SXS_ECHO_OFF 0

#define SXS_DSR_ON    (1 << 0)
#define SXS_CTS_ON    (1 << 1)
#define SXS_DCD_ON    (1 << 2)
#define SXS_RING_ON   (1 << 3)
#define SXS_DSR_OFF   (1 << 4)
#define SXS_CTS_OFF   (1 << 5)
#define SXS_DCD_OFF   (1 << 6)
#define SXS_RING_OFF  (1 << 7)

#define SXS_DTR_ON    (1 << 0)
#define SXS_RTS_ON    (1 << 1)
#define SXS_BREAK_ON  (1 << 2)
#define SXS_RX_3PLUS  (1 << 3)

#define SXS_HOLD_RX    (1 << 0)
#define SXS_RELEASE_RX (1 << 1)
#define SXS_HOLD_TX    (1 << 2)
#define SXS_RELEASE_TX (1 << 3)

extern const u32 sxs_SerialRateList [SXS_NB_SERIAL_RATE];

#endif

/// @} // <-- End of sx_serial group
