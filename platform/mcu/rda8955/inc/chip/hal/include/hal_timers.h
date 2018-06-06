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


#ifndef _HAL_TIMERS_H_
#define _HAL_TIMERS_H_

#include "cs_types.h"

EXTERN_C_START

#include "hal_error.h"
#include "hal_debug.h"



/// @defgroup timer HAL Timer Driver
///
/// This document describes the characteristics of the timers (RTC,
/// Tick timer, WatchDog timer and delay timer) and how to use them via its
/// Hardware Abstraction Layer API.
///
/// Several timers are available providing the following functions:
/// -# Tick timer:\n
///      - 24 bits counters, decrementing on a clock at (32768 / 2 =) 16384 Hz
///      - Repetitive mode to generate a time constant, with auto-reload
///      - Allows period change while running
///      - Interrupt generation when reaching 0 (if enabled)
/// -# RTC timer:\n
///      - 32 bits counters, incrementing on a clock at 1/256 Hz
///      - Programmable Alarm
///      - Interrupt generation when reaching the alarm time (if enabled)
/// -# WatchDog timer:\n
///      - 24 bits counters, decrementing on a clock at (32768 / 2 =) 16384 Hz
///      - Reset the system if it reaches zero
/// -# Delay timer:\n
///      - 32 bits counter, incrementing on a 16384Hz clock
///      - Starts from zero at boot time: it is a UP-TIME timer.
///      - Wrap
///      - Interrupt generation on a programmable period
///      - Suitable to measure delays
///
/// @par Timer Operation
/// A typical application would use the RTC timer to keep track of the time.
/// The alarm feature could be used for an alarm clock implementation or to
/// schedule low frequency events (below 1Hz).
/// The Tick timer would be used to implement the OS timer
/// The Watchdog is self-named, as is for the delay timer.
///
/// @par Interruption
/// @todo
///
/// @{

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/// Watchdog context saving structure.
/// Using a global variable here is allowed, because it is used in the
/// boot_loader.c, at a place where the function call is not allowed yet
/// (because the code is not yet copied from flash).
EXPORT HAL_DBG_WATCHDOG_CTX_T g_halWatchdogCtx;

// =============================================================================
// MACROS
// =============================================================================

// =============================================================================
// HAL_TIM_MAX_PER
// -----------------------------------------------------------------------------
/// Maximum period programmable in the tick timer
// =============================================================================
#define HAL_TIM_MAX_PER    0xffffff

// =============================================================================
// HAL_TICK1S
// -----------------------------------------------------------------------------
/// Number of tick timer ticks during one second
// =============================================================================
#define HAL_TICK1S 16384

#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND


// =============================================================================
// MS_WAITING
// -----------------------------------------------------------------------------
/// Number of delay timer during one millisecond
// =============================================================================
#define MS_WAITING      *  HAL_TICK1S / 1000


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_TIM_RTC_INT_MODE_T
// -----------------------------------------------------------------------------
/// Those are the interrupt mode available for the RTC timer interruption.
/// RTC can generate interruption periodically, which may programmed to
/// be every second, minute or day.
// =============================================================================
typedef enum
{
    HAL_TIM_RTC_INT_DISABLED = 0, ///<    Interruption disabled
    HAL_TIM_RTC_INT_PER_SEC  = 1, ///<    Interruption every second
    HAL_TIM_RTC_INT_PER_MIN  = 2, ///<    Interruption every min
    HAL_TIM_RTC_INT_PER_HOUR = 3  ///<    Interruption every hour
} HAL_TIM_RTC_INT_MODE_T ;


// =============================================================================
// HAL_TIM_RTC_TIME_T
// -----------------------------------------------------------------------------
/// Structure to store the RTC time
/// It describes second, minute, hour, day, week day, month, and year
/// The year supported is from 2000 to 2127.
// =============================================================================
typedef struct
{
    UINT8 sec;   ///< Second
    UINT8 min;   ///< Minute
    UINT8 hour;  ///< Hour
    UINT8 day;   ///< Day
    UINT8 month; ///< Month
    UINT8 year;  ///< Year
    UINT8 wDay;  ///< Week Day
} HAL_TIM_RTC_TIME_T;




// =============================================================================
// HAL_TIM_RTC_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user fonction called when a RTC interrupt occurs
/// This interrupt can be set to be triggered every second, minute
/// or hour by a call to the setting function #hal_TimRtcSetIrqIntervalMode.
// =============================================================================
typedef VOID (*HAL_TIM_RTC_IRQ_HANDLER_T)(VOID);



// =============================================================================
// HAL_TIM_RTC_ALARM_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user fonction called when a RTC alarm occurs
// =============================================================================
typedef VOID (*HAL_TIM_RTC_ALARM_IRQ_HANDLER_T)(VOID);



// =============================================================================
// FUNCTIONS
// =============================================================================



// =============================================================================
// hal_TimRtcGetTime
// -----------------------------------------------------------------------------
/// This function returns the current RTC time in the
/// HAL_TIM_RTC_TIME_T structure whose pointer is passed as a parameter.
/// The validity of the clock has to be checked beforehand.
///
/// @param rtcTime Pointer to the structure which will store after the call
/// of the function the current RTC time.
/// @return #HAL_ERR_NO or #HAL_ERR_TIM_RTC_NOT_VALID
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcGetTime(HAL_TIM_RTC_TIME_T* rtcTime);



// =============================================================================
// hal_TimRtcSetTime
// -----------------------------------------------------------------------------
/// This function loads the RTC timer with the \c rtcTime value.
/// This
/// function should only be used at the initialization of the device. This
/// timer will continue to increment even in deep power down mode.
///
/// @param rtcTime Pointer to the date and time to load in the RTC timer
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcSetTime(CONST HAL_TIM_RTC_TIME_T* rtcTime);



// =============================================================================
// hal_TimRtcIrqIntervalMode
// -----------------------------------------------------------------------------
/// This function is used to configure the interval of the RTC interval IRQ:
/// second, minute or hour.
/// @param mode Interval of the RTC interrupt.
// =============================================================================
PUBLIC VOID hal_TimRtcSetIrqIntervalMode(HAL_TIM_RTC_INT_MODE_T mode);



// =============================================================================
// hal_TimRtcNotProgrammed
// -----------------------------------------------------------------------------
/// This function checks if the RTC has been programmed or not. If it
/// has not been programmed, this function will automatically disable the RTC
/// alarm if it is set.
///
/// @return \c TRUE if the RTC has not been programmed, and clear the RTC alarm
/// in that case if it had been programmed. \c FALSE if the RTC has been
/// programmed.
// =============================================================================
PUBLIC BOOL hal_TimRtcNotProgrammed(VOID);



// =============================================================================
// hal_TimRtcIrqSetHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC interrupt occurs
/// @param handler User function called when a RTC interruption occurs
// =============================================================================
PUBLIC VOID hal_TimRtcIrqSetHandler(HAL_TIM_RTC_IRQ_HANDLER_T handler);



// =============================================================================
// hal_TimRtcIrqSetMask
// -----------------------------------------------------------------------------
/// This function is used to set or clear the mask of the RTC IRQ
/// @param mask If \c TRUE, RTC IRQ can occur and trig the user handler.
///             IF \c FALSE, they can't.
// =============================================================================
PUBLIC VOID hal_TimRtcIrqSetMask(BOOL mask);



// =============================================================================
// hal_TimRtcIrqGetMask
// -----------------------------------------------------------------------------
/// This function is use to retrieve the RTC IRQ mask.
/// @return \c TRUE is the mask is set or \c FALSE if the RTC IRQ can't occur.
// =============================================================================
PUBLIC BOOL hal_TimRtcIrqGetMask(VOID);



// =============================================================================
// hal_TimRtcSetAlarm
// -----------------------------------------------------------------------------
/// This function sets up an alarm which will be triggered when the RTC timer
/// reaches the time in \c alarmTime, and enable it.
///
/// An interrupt will be generated when this alarm is reached. \n
/// <b> The interrupt handler will automatically mask this interrupt, and
/// clear it.</b>
///
/// @param alarmTime Time of alarm
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if a previous load
/// is still in progress.
// =============================================================================
PUBLIC HAL_ERR_T hal_TimRtcSetAlarm(CONST HAL_TIM_RTC_TIME_T* alarmTime);



// =============================================================================
// hal_TimRtcAlarmEnable
// -----------------------------------------------------------------------------
/// Enable the RTC alarm.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmEnable(VOID);



// =============================================================================
// hal_TimRtcAlarmDisable
// -----------------------------------------------------------------------------
/// Disables the alarm on the RTC
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmDisable(VOID);



// =============================================================================
// hal_TimRtcAlarmIrqSetHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC alarm occurs
/// @param handler User function called when a RTC alarm interrupt occurs.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmIrqSetHandler(HAL_TIM_RTC_ALARM_IRQ_HANDLER_T handler);


// =============================================================================
// hal_TimRtcAlarmIrqSetModemHandler
// -----------------------------------------------------------------------------
/// This function is used to set the handler called when a
/// RTC alarm occurs for modem
/// @param handler User function called when a RTC alarm interrupt occurs.
// =============================================================================
PUBLIC VOID hal_TimRtcAlarmIrqSetModemHandler(HAL_TIM_RTC_ALARM_IRQ_HANDLER_T handler);


// =============================================================================
// hal_TimWatchDogOpen
// -----------------------------------------------------------------------------
/// This function starts a watchdog timer which will start with an initial value
/// of \c delay and decrement by one every 2 periods of a 32 khz clock. When
/// the watchdog timer reaches 0, a reset is generated.
///
/// @param delay Number of 16384 Hz clock cycles the WatchDog timer counts down
/// before reaching 0.
// =============================================================================
PUBLIC VOID hal_TimWatchDogOpen(UINT32 delay);


// =============================================================================
// hal_TimWatchDogSetPeriod
// -----------------------------------------------------------------------------
/// Setup the watchdog timer which will start at \c delay and decrement by one
/// every 2 clocks 32kHz, after #. Will generate a reset when zero is reached
/// If the WD timer is already started, the timer will be reseted to the new
/// value.
/// If the WD timer is not started yet, this will only setup the configuration.
/// The WD timer will be started the next time #hal_TimWatchDogKeepAlive is
/// called
/// @param delay Number of 16384 Hz clock cycles the WatchDog timer counts down
/// before reaching 0.
// =============================================================================
PUBLIC VOID hal_TimWatchDogSetPeriod(UINT32 delay);



// =============================================================================
// hal_TimWatchDogKeepAlive
// -----------------------------------------------------------------------------
/// Reloads the watchdog timer
/// This function restarts the watchdog timer with the \c delay used to start
/// it (#hal_TimWatchDogOpen) or to set it up (#hal_TimWatchDogSetPeriod),
/// preventing the reset from occurring.
// =============================================================================
PUBLIC VOID hal_TimWatchDogKeepAlive(VOID);



// =============================================================================
// hal_TimWatchDogClose
// -----------------------------------------------------------------------------
/// Disables the watchdog timer
/// This function disables the watchdog timer. A reset will not occur because
/// of the watchdog, even if #hal_TimWatchDogKeepAlive is not called. The
/// watchdog can be re-enabled by #hal_TimWatchDogOpen.
// =============================================================================
PUBLIC VOID hal_TimWatchDogClose(VOID);



// ============================================================================
// hal_TimSetExpirationTime/hal_TimChangeExpirationTime
// ----------------------------------------------------------------------------
/// Set the next time when the OS timer will wrap. Also, the aligned HW tick
/// will be kept for calculating elapsed ticks. For SET, current HW tick will
/// be used. For CHANGE, just add delta to the aligned HW tick.
/// @param nextPeriod The next period before the new wrap.
// ============================================================================
PUBLIC VOID hal_TimSetExpirationTime(UINT32 nextPeriod);
PUBLIC VOID hal_TimChangeExpirationTime(UINT32 delta, UINT32 nextPeriod);



// ============================================================================
// hal_TimGetElapsedTime
// ----------------------------------------------------------------------------
/// Return the elapsed ticks.
// ============================================================================
PUBLIC UINT32 hal_TimGetElapsedTime(VOID);



// =============================================================================
// hal_TimDelay
// -----------------------------------------------------------------------------
/// This function is to be used when we want to wait for a delay.
/// Attention must be paid to the fact that this is a blocking function
/// and that it could be avoided as possible, and OS function should be
/// used instead. But sometimes, we cannot use the OS functions ...
///
/// @param delay The delay to wait, in number of 16 kHz periods.
// =============================================================================
PUBLIC VOID hal_TimDelay(UINT32 delay);



// =============================================================================
// hal_TimGetUpTime
// -----------------------------------------------------------------------------
/// Return the up time of the system in 16384Hz ticks.
/// The uptime is given as a 32 bits numbers, therefore the user of the
/// function must take care of the count wrap, that will occurs every
/// three days.
///
/// @return Up time, in 16384Hz ticks.
// =============================================================================
PUBLIC UINT32 hal_TimGetUpTime(VOID);



// =============================================================================
// hal_TimeWdRecover
// -----------------------------------------------------------------------------
// Called early in the boot process when watchdog occured, so running in the memory
// context when the WDog occured. Displays the watchdog context.
// =============================================================================
PUBLIC VOID hal_TimeWdRecover(VOID);

// ============================================================================
// hal_TickIrqHandler
// ----------------------------------------------------------------------------
/// IRQ handler for the OS timer, called when this timer wrap.
// ============================================================================
PUBLIC VOID hal_TickIrqHandler(VOID);

#ifdef CHIP_DIE_8909
PUBLIC VOID hal_WDTOpen(UINT32 delay);
VOID hal_WDTRestart();
#endif

///  @} <- End of timer group

EXTERN_C_END

#endif // _HAL_TIMERS_H_


