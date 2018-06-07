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

#ifndef _HAL_TRACE_H_
#define _HAL_TRACE_H_

// =============================================================================
// Types
// =============================================================================
typedef VOID (*HAL_TRACE_RX_PROC_T)(UINT8 id, UINT8 *data, UINT16 len);

// =============================================================================
// HAL_HST_TRACE_PORT_T
// -----------------------------------------------------------------------------
/// Output trace (USB or Host)
// =============================================================================
typedef enum {
    HAL_HST_TRACE_PORT_HOST,
    HAL_HST_TRACE_PORT_USB,
#ifdef CHIP_HAS_AP
    HAL_HST_TRACE_PORT_AP,
#endif
    HAL_HST_TRACE_PORT_NUM
} HAL_HST_TRACE_PORT_T;

// =============================================================================
// Functions
// =============================================================================

// =============================================================================
// hal_HstTraceOpen
// -----------------------------------------------------------------------------
/// Open and configure the host trace port.
/// The configuration is set to comply with the trace tool running on the host.
/// Traces won't be sent after this call. hal_HstTraceSetTxEnabled(TRUE) is
/// needed before it is possible to actually send traces.
///
/// @param setting Not taken into account
/// @param portIdx  Not taken into account
// =============================================================================
VOID hal_HstTraceOpen(UINT16 setting, UINT8 portIdx);

// =============================================================================
// hal_HstTraceClose
// -----------------------------------------------------------------------------
// Close the host trace port
// @param portIdx Not taken into account
// =============================================================================
VOID hal_HstTraceClose(UINT8 portIdx);

// =============================================================================
// hal_HstTraceRequestData
// -----------------------------------------------------------------------------
/// Request trace buffer (and fill header), and notify data have been filled.
///
/// @param id FlowId
/// @param size Size of data to be sent
/// @return NULL if no enough free buffer
// =============================================================================
UINT32 *hal_HstTraceRequestData(UINT8 id, UINT16 size);

// =============================================================================
// hal_HstTraceDataFilled
// -----------------------------------------------------------------------------
/// Notify the requested buffer have been filled (and can be sent if TX enabled).
///
/// @param data The requested buffer
// =============================================================================
VOID hal_HstTraceDataFilled(VOID *data);

// =============================================================================
// hal_HstTraceSwitch (TBD)
// -----------------------------------------------------------------------------
/// Change trace output path
/// @param port Trace path (Host, USB, AP)
// =============================================================================
INLINE VOID hal_HstTraceSwitch(HAL_HST_TRACE_PORT_T port) {}

// =============================================================================
// hal_TraceApSaveDataIn
// -----------------------------------------------------------------------------
/// Save the data from AP for the host trace port input.
/// @param buf Pointer of the data.
/// @param len Number of bytes of the data
/// @return The number of bytes saved
// =============================================================================
INLINE UINT32 hal_TraceApSaveDataIn(CONST UINT8 *buf, UINT32 len) { return 0; }

// =============================================================================
// hal_HstTraceOutput
// -----------------------------------------------------------------------------
/// Combination for hal_HstTraceRequestData and hal_HstTraceDataFilled
///
/// @param id FlowId.
/// @param date Data to be sent.
/// @param size Size of data to be sent
// =============================================================================
BOOL hal_HstTraceOutput(UINT8 id, UINT8 *data, UINT16 size);

// =============================================================================
// hal_HstTraceFetch
// -----------------------------------------------------------------------------
/// Fetch trace data from trace buffer. It should be called at TX disabled.
///
/// @param date Data to be fetched to
/// @param maxlen Maximum length to be fecthed
/// @return Length has been fetched
// =============================================================================
UINT16 hal_HstTraceFetch(UINT8 *data, UINT16 maxlen);

// =============================================================================
// hal_HstTraceSetFnStamp
// -----------------------------------------------------------------------------
/// Set a pointer to get FnStamp
///
/// @param stamp Pointer for FnStamp
// =============================================================================
VOID hal_HstTraceSetFnStamp(UINT32 *stamp);

// =============================================================================
// hal_HstTraceSetTxEnabled/hal_HstTraceTxIsEnabled
// -----------------------------------------------------------------------------
/// Set/get TX enabled. TX can be enabled immediately. However, for disable,
/// there may exist some delay. Data already "on the air" will still be sent.
// =============================================================================
VOID hal_HstTraceSetTxEnabled(BOOL enabled);
BOOL hal_HstTraceTxIsEnabled(VOID);

// =============================================================================
// hal_HstTraceSetRxEnabled/hal_HstTraceRxIsEnabled
// -----------------------------------------------------------------------------
/// Set/get RX enabled. (no real behavior now)
// =============================================================================
VOID hal_HstTraceSetRxEnabled(BOOL enabled);
BOOL hal_HstTraceRxIsEnabled(VOID);

// =============================================================================
// hal_HstTraceTxFlush
// -----------------------------------------------------------------------------
/// Wait queued buffer sent complete
///
/// @ticks Wait ticks at most
/// @return FALSE if timeout
// =============================================================================
BOOL hal_HstTraceTxFlush(UINT32 ticks);

// =============================================================================
// hal_HstTraceIdleCheck
// -----------------------------------------------------------------------------
/// Process RX and TX.
///
/// @return TRUE for either RX or TX is in processing
// =============================================================================
BOOL hal_HstTraceIdleCheck(VOID);

// =============================================================================
// hal_HstTraceSetDdrEnabled (8955 only)
// -----------------------------------------------------------------------------
/// Set whether DDR PSRAM can be used.
///
/// @return FALSE if DDR is still in use at disable DDR usage
// =============================================================================
BOOL hal_HstTraceSetDdrEnabled(BOOL enabled);

// =============================================================================
// hal_hstTraceSetRxProc
// -----------------------------------------------------------------------------
/// Set RX process handler.
///
/// @param proc The RX process handler
// =============================================================================
VOID hal_hstTraceSetRxProc(HAL_TRACE_RX_PROC_T proc);

// =============================================================================
// hal_HstTraceGetBufferHead
// -----------------------------------------------------------------------------
/// Get trace buffer head.
// =============================================================================
UINT32 *hal_HstTraceGetBufferHead();

/// @} // end of trace group

#endif // _HAL_TRACE_H_
