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

#include "hal_gpio.h"

#ifndef _HAL_LPS_H_
#define _HAL_LPS_H_

/// @defgroup lps HAL LPS driver
///
///  Switching the system to 32kHz must be done while keeping track of the
///  network time in order for the mobile to keep synchronized. To do so, the
///  32kHz clock is calibrated against the system clock to ensure keeping track
///  of the network time with a sufficient precision. As the crystal frequency
///  may slightly vary with temperature, this calibration must be done regularly.
///  This is to ensure the desired precision remains valid over time.
/// @{

// =============================================================================
//  MACROS
// =============================================================================
#define HAL_LPS_SF_Min 15

#define HAL_PM3_THRESHOLD 12600

#define HAL_LSP_TRACE(level, tstmap, format, ...) \
     hal_DbgTrace(_HAL | level, tstmap, format, ##__VA_ARGS__)


// =============================================================================
//  TYPES
// =============================================================================
// =============================================================================
// HAL_LPS_POWERUP_TIMINGS_T
// -----------------------------------------------------------------------------
/// LPS power up state machine timings
// =============================================================================
typedef struct
{
    /// LPS Power Up: Controlled output State1 to Controlled Output State2 delay
    UINT32 puCoState1ToCoState2:5;
    /// Reserved
    UINT32 :1;
    /// LPS Power Up: Controlled Output State2 to RF Clock Enable State delay
    UINT32 puCoState2ToRfClkEnState:5;
    /// Reserved
    UINT32 :1;
    /// LPS Power Up: RF Clock Enable State to RF Clock Ready State delay
    UINT32 puRfClkEnStateToRfClkReadyState:12;
    /// LPS Power Up: RF Clock Ready State to Pll Clock Ready State
    UINT32 puRfClkReadyStateToPllClockReadyState:5;
    /// Reserved
    UINT32 :2;
    /// LPS Power Up: Wait Pll lock bit before going in Pll Clock Ready State
    UINT32 puWaitPllLock:1;
} HAL_LPS_POWERUP_TIMINGS_T;

// =============================================================================
// HAL_LPS_POWERUP_CTRL_T
// -----------------------------------------------------------------------------
/// LPS power up state machine state and action configuration
// =============================================================================
typedef struct
{
    /// Reserved
    UINT32      :8;
    /// All controllable output are enabled after power up, the four following
    /// fields describe the state during power down.
    /// LPS Power Up: controlled Output 0 power down state \n
    ///         0:Inactive - 1: Active
    UINT32 puCO0ForceOn:1;
    /// LPS Power Up: controlled Output 1 power down state \n
    ///         0:Inactive - 1: Active
    UINT32 puCO1ForceOn:1;
    /// LPS Power Up: CLK_RF_26M_EN power down state \n
    ///         0:Inactive - 1: Active
    UINT32 puClkRfEnForceOn:1;
    /// LPS Power Up: SPIs power down state \n
    ///         0:Inactive - 1: Active
    UINT32 puSpiForceOn:1;
    /// The following 3 fields describe general state: polatity of outputs
    /// LPS Power Up: controlled Output 0 polarity \n
    ///         0:Active High - 1: Active Low
    UINT32 puCO0Pol:1;
    /// LPS Power Up: controlled Output 1 polarity \n
    ///         0:Active High - 1: Active Low
    UINT32 puCO1Pol:1;
    /// LPS Power Up: CLK_RF_26M_EN polarity \n
    ///         0:Active High - 1: Active Low
    UINT32 puClkRfEnPol:1;
    /// Reserved
    UINT32 :1;
    /// The following fields describe the action configuration for power up
    /// LPS Power Up: State where the SPI is turned on
    UINT32 puSpiOnState:3;
    /// Reserved
    UINT32 :1;
    /// LPS Power Up: Mask the CO output to activate in Controlled Output State1
    ///     Bit 0: Controls the LPSCO[0] pin, 1 to enable in Controlled Output State1
    ///     Bit 1: Controls the LPSCO[1] pin, 1 to enable in Controlled Output State1
    UINT32 puCoActivationMaskInCoState1:2;
    /// LPS Power Up: Mask the CO output to activate in Controlled Output State2
    ///     Bit 0: Controls the LPSCO[0] pin, 1 to enable in Controlled Output State2
    ///     Bit 1: Controls the LPSCO[1] pin, 1 to enable in Controlled Output State2
    UINT32 puCoActivationMaskInCoState2:2;
    /// LPS Power Up: Mask the activation of CLK_RF_26M_EN in RF Clock Enable State
    //      0: Do not activate CLK_RF_26M_EN - 1: Activate CLK_RF_26M_EN
    UINT32 puMaskRfClkEn:1;
    /// LPS Power Up: Mask the activation of the Pll in RF Clock Ready State
    ///     0: Do not activate Pll - 1: Activate Pll
    UINT32 puMaskPllEn:1;
    /// LPS Power Up: Mask the switch to RF clock in RF Clock Ready State
    ///     0: No Switch to RF clock - 1: switch to RF clock
    UINT32 puMaskSwitchToRfClk:1;
    /// LPS Power Up: Mask the switch to Pll clock in Pll Clock Ready State
    ///     0: No Switch to Pll clock - 1: switch to Pll clock
    UINT32 puMaskSwitchToPllClk:1;
    /// Reserved - must be 0
    UINT32 :1;
    /// LPS Power Up: Enable TCU wakeup during the power up state machine
    /// Set this bit to 1 if ther is a need to generate events from the TCU in the Ready State
    /// before reaching the Done state
    UINT32 puEnableTcuWakeup:1;
} HAL_LPS_POWERUP_CTRL_T;

// =============================================================================
// HAL_LPS_CONFIG_T
// -----------------------------------------------------------------------------
/// LPS power up state machine configuration
// =============================================================================
typedef struct
{
    HAL_LPS_POWERUP_TIMINGS_T puTimings;
    HAL_LPS_POWERUP_CTRL_T puCtrl;
    UINT8 lpsWakeUpFrameNbr;
    UINT8 lpsAccuracy;
} HAL_LPS_CONFIG_T;


typedef enum {
    LPS_SLEEP_START=0,
    LPS_SLEEP_END=1,
    LPS_DEEP_SLEEP_START=2,
    LPS_DEEP_SLEEP_END=3,
    LPS_SHUTDOWN_START=4,

	NUM_OF_LPS_SLEEP_STATUS=LPS_SHUTDOWN_START+1,
} HAL_LPS_SLEEP_STATUS;



// =============================================================================
// HAL_LPS_CALLBACK_T
// -----------------------------------------------------------------------------
/// LPS power up state machine configuration
// =============================================================================
typedef VOID HAL_LPS_CALLBACK_T(VOID);

typedef VOID HAL_LPS_OS_CALLBACK_T(UINT8);
// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_LpsOpen
// -----------------------------------------------------------------------------
/// Initialize the lps module
/// @param powerupCfg State machine configuration
// =============================================================================
PUBLIC VOID hal_LpsOpen(CONST HAL_LPS_CONFIG_T *powerupCfg);

// =============================================================================
// hal_LpsClose
// -----------------------------------------------------------------------------
/// Close the LPS service
// =============================================================================
PUBLIC VOID hal_LpsClose(VOID);


// =============================================================================
// hal_LpsSetControlPin
// -----------------------------------------------------------------------------
/// Configure the pin used by lps to enter le deepest low power mode
/// (ECO_MODE or LP_MODE pin)
/// @param lpModePin Must be a GPO or HAL_GPO_NONE for current implementation.
// =============================================================================
PUBLIC VOID hal_LpsSetControlPin(CONST HAL_APO_ID_T lpModePin);


// =============================================================================
// hal_LpsCalibrate
// -----------------------------------------------------------------------------
/// Start a LPS calibration
/// This function is called to calibrate the 32K clock against the
/// system clock
/// This function must be called at every frame interrupt to keep the calibration up to date
/// If a valid calibration is already present, this function will do nothing
// =============================================================================
PUBLIC VOID hal_LpsCalibrate(VOID);




// =============================================================================
// hal_LpsInvalidateCalib
// -----------------------------------------------------------------------------
/// Invalidate the previous LPS calib results
/// The crystal accuracy also varies over time, in
/// particular it varies with the temperature, so this calibration must be done
/// regularly to keep it valid.
/// This function, invalidating the previous calibration, a new
/// clock calibration process by #hal_LpsCalibrate is needed after.
/// One policy for the clock calibration could be to do it every hour while in
/// idle. When the mobile is active there is no need for clock calibration as
/// some resources being active the system will never enter the low power mode.
/// \n
/// So the clock calibration should be done when activating the screen saver,
/// and then every hour until the mobile is activated again. There is a
/// compromise between the accuracy and the power consumption to find.
/// This function can be called asynchronously
// =============================================================================
PUBLIC VOID hal_LpsInvalidateCalib(VOID);


// =============================================================================
// hal_LpsSkipFrame
// -----------------------------------------------------------------------------
/// Skip a given number of frame.
/// During which we are sleeping.
/// This function is called in pal_FintSuspend, to do the suspend sequence.
/// @param frameQty Number of frame to skip
/// @return TRUE if the FINT has been suspended, FALSE when the FINT can not be
/// suspended   at this time
// =============================================================================
PUBLIC BOOL hal_LpsSkipFrame (UINT16 frameQty);



// =============================================================================
// hal_LpsResumeFrame
// -----------------------------------------------------------------------------
/// Resume from skip frame
/// This function properly unmask the Fint, stopping
/// to skip frame.
// =============================================================================
PUBLIC VOID hal_LpsResumeFrame(VOID);



// =============================================================================
// hal_LpsGetElapsedFrames
// -----------------------------------------------------------------------------
/// Get the number of elapsed frames during the skip frame process.
/// @return the current number of elapsed frame.
// =============================================================================
PUBLIC UINT32 hal_LpsGetElapsedFrames(VOID);


// =============================================================================
// hal_LpsSleep
// -----------------------------------------------------------------------------
/// Enter the low power state (32kHz) whenever possible
/// When not possible, the function will minimize the power by shuting down the CPU
/// clock
/// This function must be called in the Idle Task
// =============================================================================
PUBLIC VOID hal_LpsSleep (VOID);


// ============================================================================
// hal_LpsForceOsTimAlignEnable
// ----------------------------------------------------------------------------
/// Enable Os timer align to lps wakeup
/// @param mask.
// ============================================================================
PUBLIC VOID hal_LpsForceOsTimAlignEnable(BOOL mask);

// =============================================================================
// hal_LpsAllowSkipFrame
// -----------------------------------------------------------------------------
/// @parameter allow Call this function with \c allow = TRUE when to allow
/// frame skipping, and false to forbid it.
/// @param lastEvt date of last event programmed in current active frame
/// only used when \c allow = TRUE to ensure the event is send before entering
/// low power.
// =============================================================================
PUBLIC VOID hal_LpsAllowSkipFrame(BOOL allow, UINT16 lastEvt);

// =============================================================================
// hal_LpsSetCallback
// -----------------------------------------------------------------------------
/// @parameter cbfunc function to be called at the end of each power up, should
/// just send an event or similar simple code.
// =============================================================================
PUBLIC VOID hal_LpsSetCallback(HAL_LPS_CALLBACK_T* cbFunc);

// =============================================================================
// hal_LpsCOCtrl
// -----------------------------------------------------------------------------
/// Change the state of LPSCOs
/// @param puCtrl only the following fields are used:
///  - HAL_LPS_POWERUP_CTRL_T.puCO0ForceOn
///  - HAL_LPS_POWERUP_CTRL_T.puCO1ForceOn
///  - HAL_LPS_POWERUP_CTRL_T.puCO0Pol
///  - HAL_LPS_POWERUP_CTRL_T.puCO1Pol
///  .
// =============================================================================
PUBLIC VOID hal_LpsCOCtrl(HAL_LPS_POWERUP_CTRL_T puCtrl);


// =============================================================================
// hal_LpsRemainingFramesReady
// -----------------------------------------------------------------------------
/// Checks if PAL can go to low power mode for given number of frames.
/// @param frameNum the number of frames that can be in low power mode;
/// @return TRUE if can, FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_LpsRemainingFramesReady(UINT32 frameNum);


#ifdef CHIP_DIE_8909
PUBLIC VOID hal_LpsSetSleepCallback(HAL_LPS_SLEEP_STATUS status,HAL_LPS_CALLBACK_T* cbFunc);

// =============================================================================
// hal_LpsNBCalibrate
// -----------------------------------------------------------------------------
/// Start a LPS NB calibration
/// This function is called to calibrate the 32K clock against the
/// system clock
/// This function must be called at every frame interrupt to keep the calibration up to date
/// If a valid calibration is already present, this function will do nothing
// =============================================================================
PUBLIC VOID hal_LpsNBCalibrate(VOID);

PUBLIC BOOL hal_sleepReq (UINT32 frameQty,UINT16 lpsLastEvt);
PUBLIC UINT32 hal_LpsNBGetElapsedFrames(VOID);
VOID hal_LpsReadyForPM3();
VOID* hal_LpsGetCtx(u32* length);
#endif

/// @} // <-- End of the lps group

#endif //_HAL_LPS_H_



