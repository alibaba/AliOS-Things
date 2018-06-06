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







#ifndef __SXS_RMT_H__
#define __SXS_RMT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"

//#define __SXS_RMT_CIPH__

#ifdef __SXS_SPLIT_2_CPU__
#define SXS_RMT_SPL_CPU_IDX 0
#endif

#ifndef __EMBEDDED__
// =============================================================================
// sxs_RmtBufferFlush
// -----------------------------------------------------------------------------
/// In simulation, flush the remote buffer.
/// @return TRUE when the buffer is not empty, FALSE otherwise.
// =============================================================================
u8 sxs_RmtBufferFlush (void);
#else
#define sxs_RmtBufferFlush sxs_SerialFlush
#endif

// =============================================================================
// sxs_RmtInit
// -----------------------------------------------------------------------------
/// Data exchange with remote computer initialization.
// =============================================================================
void sxs_RmtInit (bool fullInit);
void sxs_ResetTxRmtData (void);
void sxs_ResetRxRmtData (void);

// =============================================================================
// sxs_SetPrcsRmtData
// -----------------------------------------------------------------------------
/// Set the function to be called to process the data coming from
/// the specified remote processor.
/// @param RmtCpuIdx            Index of the remote processor.
/// @param sxs_PrcsRmtDataFunc  Function for data process.
// =============================================================================
void sxs_SetPrcsRmtData (u8 RmtCpuIdx, void (*sxs_PrcsRmtDataFunc)(u8, u8*, u16));
void sxs_PrcsRmtData (u8 Id, u8 *Data, u16 Length);

// =============================================================================
// sxs_SendToRemote
// -----------------------------------------------------------------------------
/// Format data corresponding to the identified flow and store
/// them in remote buffer.
/// @param Id        Flow Id.
/// @param Data      Pointer onto data or NIL.
/// @param Size      Size of data.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Pointer where data can be stored.
// =============================================================================
u8  *_sxs_SendToRemote (u8 Id, u8 *Data, u16 Size, u8 RmtCpuIdx);

// =============================================================================
// sxs_ReadTxRmtData
// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer.
/// @param DataOut   points a buffer where data must be copied.
/// @param Size      Size in bytes of buffer pointed by DataOut.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Number of bytes copied in buffer.
// =============================================================================
u16  sxs_ReadTxRmtData (u8 *DataOut, u16 Size, u8 RmtCpuIdx);

// =============================================================================
// sxs_WriteRxRmtData
// -----------------------------------------------------------------------------
/// Extract and process a frame coming from the remote computer
/// from the reception buffer.
/// @param DataIn    points to the remote computer Rx data.
/// @param Size      is the number of bytes available in DataIn.
/// @param RmtCpuIdx Remote CPU index.
/// @return          The number of bytes processed in DataIn.
// =============================================================================
u16  sxs_WriteRxRmtData (u8 *DataIn, u16 Size, u8 RmtCpuIdx);

// =============================================================================
// sxs_RmtIdleHook
// -----------------------------------------------------------------------------
/// Process remote data during idle.
// =============================================================================
void sxs_RmtIdleHook (void);

// =============================================================================
// sxs_RmtSuspend
// -----------------------------------------------------------------------------
/// Suspend trace storage remote buffer.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void sxs_RmtSuspend (u8 RmtCpuIdx);

// =============================================================================
// sxs_RmtResume
// -----------------------------------------------------------------------------
/// Resume trace storage in remote buffer.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void sxs_RmtResume (u8 RmtCpuIdx);

// =============================================================================
// sxs_GetRmtBufferStatus
// -----------------------------------------------------------------------------
/// Get the status on the remote buffer.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Number of data present in remote buffer.
// =============================================================================
u16  sxs_GetRmtBufferStatus (u8 RmtCpuIdx);

// =============================================================================
// sxs_SetFnStamp
// -----------------------------------------------------------------------------
/// Start trace time stamping.
/// @param FnStamp   Address of the frame number stamp.
/// @param RmtCpuIdx Remote CPU index.
// =============================================================================
void _sxs_SetFnStamp (u32 *FnStamp, u8 RmtCpuIdx);

// =============================================================================
// sxs_GetFnStamp
// -----------------------------------------------------------------------------
/// Get current stamping value.
/// @param RmtCpuIdx Remote CPU index.
/// @return          Stamp value.
// =============================================================================
u32  _sxs_GetFnStamp (u8 RmtCpuIdx);

// =============================================================================
// sxs_SetUserPrcsRmtData
// -----------------------------------------------------------------------------
/// Set the user function designed to process data from remote computer.
/// @param UserPrcsRmtData  User function.
/// @param RmtCpuIdx        Remote CPU index.
// =============================================================================
void sxs_SetUserPrcsRmtData (u8 (*UserPrcsRmtData) (u8, u8 *, u16), u8 RmtCpuIdx);

// =============================================================================
// sxs_SaveRmtBuffer
// -----------------------------------------------------------------------------
/// Save the remote buffer to the end of head to allow trace recovery after reset.
// =============================================================================
void sxs_SaveRmtBuffer (void);

// =============================================================================
// sxs_GetRmtBuffer
// -----------------------------------------------------------------------------
/// Restore saved remote buffer by sxs_SaveRmtBuffer() and place it in new trace
/// buffer.
// =============================================================================
void sxs_GetRmtBuffer  (void);

// =============================================================================
// sxs_ChangeSerialSpeed
// -----------------------------------------------------------------------------
/// Change serial speed and inform remote computer.
/// @param Speed     New Speed.
/// @param SerialIdx Serial port idx.
// =============================================================================
void sxs_ChangeSerialSpeed (u8 Speed, u8 SerialIdx);

// =============================================================================
// sxs_GetRmtBufInfo
// -----------------------------------------------------------------------------
/// Get the address and length of the first RMT buffer
/// @param addr     Pointer to save the buffer address
/// @param len      Pointer to save the buffer len
// =============================================================================
void sxs_GetRmtBufInfo (u32 *addr, u32 *len);

#ifdef CHIP_HAS_WCPU
// =============================================================================
// sxs_GetWdRmtBufInfo
// -----------------------------------------------------------------------------
/// Get the address and length of the WCPU RMT buffer
/// @param addr     Pointer to save the buffer address
/// @param len      Pointer to save the buffer len
// =============================================================================
void sxs_GetWdRmtBufInfo (u32 *addr, u32 *len);
#endif


#define sxs_SetFnStamp(a)       _sxs_SetFnStamp(a, 0)

#ifdef __cplusplus
}
#endif

#endif

/// @} // <-- End of sx_rmt group
