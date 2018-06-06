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




#ifndef _HAL_TCU_H_
#define _HAL_TCU_H_
#include "cs_types.h"
#include "chip_id.h"
#include "hal_error.h"


/// @defgroup tcu HAL TCU driver
///
/// @{
// =============================================================================
//  MACROS
// =============================================================================

typedef enum
{
    HAL_TCU_TCO_GMSK_ON                                 = 0x00000006,
    HAL_TCU_TCO_TX_OEN                                  = 0x00000007,
    HAL_TCU_TCO_TX_ON                                   = 0x00000008,
    HAL_TCU_TCO_RX_ON                                   = 0x00000009,
    HAL_TCU_TCO_RX_DEC_ON                           = 0x0000000a,
    HAL_TCU_TCO_RX_PDN                                 = 0x0000000b,
} HAL_TCU_TCO_MAPPING_T;


// =============================================================================
// SET_TCO
// -----------------------------------------------------------------------------
/// This define must be used to build the TCU events.
/// It must NOT be used to pass a tcoNumber, for instance it
/// must NOT be used for the parameters passed to the functions:
/// hal_TcuSetTco(), hal_TcuClrTco() or hal_TcuPulseTco().
// =============================================================================
#define SET_TCO(n)           ((n<<1) | 0x1)

// =============================================================================
// CLR_TCO
// -----------------------------------------------------------------------------
/// This define must be used to build the TCU events.
/// It must NOT be used to pass a tcoNumber, for instance it
/// must NOT be used for the parameters passed to the functions:
/// hal_TcuSetTco(), hal_TcuClrTco() or hal_TcuPulseTco().
// =============================================================================
#define CLR_TCO(n)           ((n<<1))

// =============================================================================
// MASK_TCO
// -----------------------------------------------------------------------------
/// This define must be used to build the TCU events.
/// It must NOT be used to pass a tcoNumber, for instance it
/// must NOT be used for the parameters passed to the functions:
/// hal_TcuSetTco(), hal_TcuClrTco() or hal_TcuPulseTco().
// =============================================================================
#define MASK_TCO(n)          (1<<n)

// =============================================================================
// HAL_TCU_MASK_DTX
// -----------------------------------------------------------------------------
/// These defines are used to disable the different Tx signals in case of DTX.
// =============================================================================
#define HAL_TCU_MASK_TCO_GMSK_ON             MASK_TCO(HAL_TCU_TCO_GMSK_ON)
#define HAL_TCU_MASK_TCO_TX_OEN              MASK_TCO(HAL_TCU_TCO_TX_OEN)
#define HAL_TCU_MASK_TCO_TX_ON               MASK_TCO(HAL_TCU_TCO_TX_ON)

// =============================================================================
// HAL_TCU_MASK_RX
// -----------------------------------------------------------------------------
/// These defines are used to disable the different Rx signals in case of Rx on.
// =============================================================================
#define HAL_TCU_MASK_TCO_RX_PDN      MASK_TCO(HAL_TCU_TCO_RX_PDN)
#define HAL_TCU_MASK_TCO_RX_DEC_ON      MASK_TCO(HAL_TCU_TCO_RX_DEC_ON)

// Values copied by "tcu.h". If tcu.h changes these values need to be updated.
#define HAL_TCU_MASK_RAMP                   (31<<19)
#define HAL_TCU_MASK_DIGRF_STROBE            (1<<25)
#define HAL_TCU_MASK_RF_SPI_MARKED_CMD       (1<<31)

// =============================================================================
// TCO
// -----------------------------------------------------------------------------
/// This define can be used for the parameters passed to the functions:
/// hal_TcuSetTco(), hal_TcuClrTco() or hal_TcuPulseTco().
// =============================================================================
#define TCO(n)               (n)

#define TCO_UNUSED           (-1)

#ifdef CHIP_DIE_8909
// =============================================================================
// HWP_TCU
// -----------------------------------------------------------------------------
/// This define must be used to build the TCU instance base address.
/// It is used to build the MACRO hwp_tcu, because TCU instance
/// is different when active RAT is GSM or NBIOT.
// =============================================================================
#define HWP_TCU(addr)           ((HWP_TCU_T*) KSEG1(addr))

// =============================================================================
// hwp_tcu
// -----------------------------------------------------------------------------
/// This define must be used to build the TCU HW FU pointer.
/// It is used to replace the MACRO hwp_tcu, because TCU HW FU
/// is different for GSM and NBIOT.
// =============================================================================
#define hwp_tcu                 (HWP_TCU(TCU))

/* TCU FU base address */
extern UINT32 TCU;
#endif

// =============================================================================
//  Types
// =============================================================================

// =============================================================================
// HAL_TCU_EVENT_ID_T
// -----------------------------------------------------------------------------
/// This type lists all the TCU events
// =============================================================================
typedef enum
{
    // TODO: should get rid of useless event
    /// Clear TCO #0
    HAL_TCU_TCO_0_CLR, //0
    /// Set TCO #0
    HAL_TCU_TCO_0_SET,
    HAL_TCU_TCO_1_CLR,
    HAL_TCU_TCO_1_SET,
    HAL_TCU_TCO_2_CLR,
    HAL_TCU_TCO_2_SET,
    HAL_TCU_TCO_3_CLR,
    HAL_TCU_TCO_3_SET,
    HAL_TCU_TCO_4_CLR,
    HAL_TCU_TCO_4_SET,
    HAL_TCU_TCO_5_CLR, // 10
    HAL_TCU_TCO_5_SET,
    /// stop modulation
    HAL_TCU_STOP_GMSK,
    /// starts modulation and output on IQ DAC
    HAL_TCU_START_GMSK,
    HAL_TCU_HIGHZ_IQ_DAC,
    HAL_TCU_DRIVE_IQ_DAC,
    HAL_TCU_DISABLE_IQ_DAC,
    HAL_TCU_ENABLE_IQ_DAC,
    /// disable IQ ADC
    HAL_TCU_DISABLE_IQ_ADC,
    /// enable IQ ADC
    HAL_TCU_ENABLE_IQ_ADC,
    /// stop recording IQ samples
    HAL_TCU_STOP_RFIN_RECORD, // 20
    /// start recording IQ samples
    HAL_TCU_START_RFIN_RECORD,
    /// Clear RF_PDN
    HAL_TCU_CLR_PDN,
    /// Set RF_PDN
    HAL_TCU_SET_PDN,
    /// sends a sequence of commands to the RF chip through spi
    HAL_TCU_SEND_SPI_CMD,
    HAL_TCU_NEXT_GAIN,
    HAL_TCU_FIRST_GAIN,
    /// set the next value in fifo on frequency control DAC
    HAL_TCU_SET_NEXT_FC,
    /// send the pre-programmed ramp0 and stay at the final level
    HAL_TCU_PA_USE_RAMP0,
    HAL_TCU_PA_USE_RAMP1,
    HAL_TCU_PA_USE_RAMP2, // 30
    HAL_TCU_PA_USE_RAMP3,
    HAL_TCU_PA_USE_RAMP4,
    HAL_TCU_RX_SOC,
    HAL_TCU_DIGRF_STB,
    /// trigger one irq on the BCPU
    HAL_TCU_TRIGGER_BCPU_SOFTIRQ0,
    HAL_TCU_TRIGGER_BCPU_SOFTIRQ1,
    /// trigger one irq on the XCPU
    HAL_TCU_TRIGGER_XCPU_SOFTIRQ0,
    HAL_TCU_TRIGGER_XCPU_SOFTIRQ1,
    /// End of the WakeUp mode
    HAL_TCU_WAKEUP_DONE,
    HAL_TCU_NO_EVENT = 0x3f
} HAL_TCU_EVENT_ID_T;

typedef VOID (*HAL_TCU_SYNC_DONE_HANDLER_T)(VOID);
typedef VOID (*HAL_TCU_RESTART_HANDLER_T)(INT32);
typedef VOID (*HAL_TCU_CALIBDONE_HANDLER_T)(VOID);

#ifdef CHIP_DIE_8909
/* TCU sync done handler */
extern HAL_TCU_SYNC_DONE_HANDLER_T g_tcuSyncDoneHandler;

/* TCU restart handler */
extern HAL_TCU_RESTART_HANDLER_T g_tcuRestartHandler;

/* TCU calibdone handler */
extern HAL_TCU_CALIBDONE_HANDLER_T g_tcuCalibDoneHandler;
#endif

// =============================================================================
// hal_TcuInit
// -----------------------------------------------------------------------------
/// Initialize the TCU
/// This function opens the TCU and configures the TCO polarities.
/// @param tcoPolarity Bitfield where '0' means the corresponding. Only the
/// lowest 17 bits are relevant.
/// TCO output is active high and '1' that it is active low
// =============================================================================
PUBLIC VOID hal_TcuOpen(UINT32 tcoPolarity);


// =============================================================================
// hal_TcuStart
// -----------------------------------------------------------------------------
/// Start the TCU with an initial Value
/// @param value Value loaded in the TCU counter when it starts.
// =============================================================================
PUBLIC VOID hal_TcuStart (UINT16 value);



// =============================================================================
// hal_TcuStop
// -----------------------------------------------------------------------------
/// Stops the TCU
// =============================================================================
PUBLIC VOID hal_TcuStop(VOID);



// =============================================================================
// hal_TcuSetTco
// -----------------------------------------------------------------------------
/// This function sets the corresponding TCO line to its
/// active level (which depends on TCO's polarity)
/// @param tcoNumber: index of the TCO to be set,
/// you can use the define TCO(n). No need to shift the number.
// =============================================================================
PUBLIC VOID hal_TcuSetTco(UINT8 tcoNumber);



// =============================================================================
// hal_TcuClrTco
// -----------------------------------------------------------------------------
/// This function sets the corresponding TCO line to its
/// inactive level (which depends on TCO's polarity)
/// @param tcoNumber: index of the TCO to be reset,
/// you can use the define TCO(n). No need to shift the number.
// =============================================================================
PUBLIC VOID hal_TcuClrTco(UINT8 tcoNumber);



// =============================================================================
// hal_TcuPulseTco
// -----------------------------------------------------------------------------
/// This function  must only be called when the TCU timings are not available.
/// Send a pulse on a TCO.
/// @param tcoNumber: index of the TCO, you can use the define TCO(n). No need
/// to shift the number.
/// @param time Time in number of 16384Hz tick between the rise and the fall
// =============================================================================
PUBLIC VOID hal_TcuPulseTco(UINT8 tcoNumber, UINT16 time);



// =============================================================================
// hal_TcuGetCount
// -----------------------------------------------------------------------------
/// Get the TCU counter
/// Only the less significant 14 bits are
/// relevant.
/// @return The TCU counter
// =============================================================================
PUBLIC UINT32 hal_TcuGetCount(VOID);



// =============================================================================
// hal_TcuSetWrap
// -----------------------------------------------------------------------------
/// Set the wrap value
/// It is the time when the programming table becomes
/// the active table. The counter's next value is 0.
/// Only the less significant 14 bits are
/// relevant to set the counter.
/// @param value Wrap Value
// =============================================================================
PUBLIC VOID hal_TcuSetWrap(UINT32 value);


// =============================================================================
// hal_TcuSetEvent
// -----------------------------------------------------------------------------
/// Set a TCU event.
/// This function handles the increment of TCU's event
/// pointer.
/// @param evId Event id, that is the event to set.
/// @param time Time when the foresaid event is to be
/// triggered.
// =============================================================================
PUBLIC VOID hal_TcuSetEvent( HAL_TCU_EVENT_ID_T evId, UINT16 time);


// =============================================================================
// hal_TcuClrEvtRange
// -----------------------------------------------------------------------------
/// Remove a range of events.
///
/// @param first first event to be removed.
/// @param last last event to be removed.
// =============================================================================
PUBLIC VOID hal_TcuClrEvtRange(UINT8 first, UINT8 last);

// =============================================================================
// hal_TcuClrAllEvents
// -----------------------------------------------------------------------------
/// Remove all active and programmed events
///
// =============================================================================
PUBLIC VOID hal_TcuClrAllEvents(VOID);


// =============================================================================
// hal_TcuSetEvtPtr
// -----------------------------------------------------------------------------
/// Set TCU's event pointer
/// @param tcuPtr new pointer position.
// =============================================================================
PUBLIC VOID hal_TcuSetEvtPtr(UINT8 tcuPtr);



// =============================================================================
// hal_TcuGetEvtPtr
// -----------------------------------------------------------------------------
/// Get current value of TCU's event pointer.
/// @return The current value of the TCU's event pointer.
// =============================================================================
PUBLIC UINT8 hal_TcuGetEvtPtr(VOID);


// =============================================================================
// hal_TcuEnableSync
// -----------------------------------------------------------------------------
//  Enable the TCU sync procedure.
//  TCU counter starts from loadValue when receiving sync signal from NB global counter
/// @param loadValue .
/// @param syncDoneHandler .
/// @return HAL_ERR_T
// =============================================================================
PUBLIC HAL_ERR_T hal_TcuEnableSync(UINT32 loadValue, HAL_TCU_SYNC_DONE_HANDLER_T syncDoneHandler);

PUBLIC VOID hal_TcuEarlyInit();

PUBLIC VOID hal_TcuSetBaseAddr(UINT32 activeRAT);

/// @} // <-- End of the TCU group
#endif // _HAL_TCU_H_



