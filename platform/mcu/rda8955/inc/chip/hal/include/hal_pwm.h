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




#ifndef _HAL_PWM_H_
#define _HAL_PWM_H_

#include "cs_types.h"

/// @defgroup pwm HAL PWM Driver
/// This document describes the PWM driver API.
///
/// The PWM consists of three independent modulation schemes which can be used
/// concurrently, the PWL (Pulse Width Light), the PWT (Pulse Width Tone)
/// - also named Buzzer - and the LPG (Light Pulse Generator).
/// @par
/// Features of the PWL:
/// - Pseudo random bit sequence with output on-time proportional to the
///   programmed threshold value
/// - Minimizes flicker
/// - One of the PWL can pulse between two previously defined level
/// - The outputs of the two pwls can be permuted
/// .
/// @par
/// Features of the PWT/Buzzer:
/// - Generates square wave output capable of driving a piezo-electric speaker
/// - Variable frequency between 349Hz and 5276Hz with 12 half-tone
///   frequencies per octave
/// - Programmable volume
/// .
/// @par
/// Features of the LPG:
/// - Generates pulsed output to drive an external LED
/// - Variable period and on time.
///
/// @par PWL Operation
/// The PWL module is optimized for driving backlights and LEDs where
/// pseudo-random pulses with a programmable average on-time are desired. The
/// resulting output will be non-symmetric and will help reduce flicker
/// effects. A low-pass filter can be added to convert the PWL output to an
/// analog value.
/// A pulse mode is available. In that mode, the pulse average on-time
/// ranges from a minimum value to a maximum value in a given time,
/// periodically.
/// 2 PWL modules are included, whose output can be permuted.
///
/// @par PWT Operation
/// The PWT module generates a 'symmetric' square wave whose duty cycle is also
/// variable between 0 and 50% (As opposed to the pseudo-random output in the
/// case of PWL). The generated tones have been tested for half-tone frequency
/// increments between 349Hz and 5276Hz. Basic volume control can be achieved
/// by varying the duty cycle, but the actual values depend on the final
/// frequency and the buzzer being used. Also, changing the duty cycle will
/// change the harmonics, and thus will have an effect on the pitch of the
/// note, but for very coarse operation, this mechanism should suffice.
///
/// @par LPG Operation
/// The LPG module generates pulse whose time is variable between 15 ms and
/// 1/4s at a variable period from 125 ms to 3 seconds.
///
///
/// @{
///


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
//
// -----------------------------------------------------------------------------
///
// =============================================================================

// =============================================================================
// HAL_PWL_ID_T
// -----------------------------------------------------------------------------
/// Used to select a PWL output
// =============================================================================
typedef enum
{
    /// invalid IO type
    HAL_PWL_INVALID = 0,

    /// no IO type
    HAL_PWL_NONE,

    /// PWL0
    HAL_PWL_0,

    /// PWL1
    HAL_PWL_1,

    HAL_PWL_QTY
} HAL_PWL_ID_T;



// =============================================================================
// HAL_LPG_PERIOD_T
// -----------------------------------------------------------------------------
/// The possible LPG periods, that range from 125ms to 1.75 s.
// =============================================================================
typedef enum
{
    /// 125 ms
    LPG_PER_125,        // 0:   8
    /// 250ms
    LPG_PER_250,        // 1:   16
    /// 500ms
    LPG_PER_500,        // 2:   32
    /// 0.75 sec
    LPG_PER_750,       // 3:   64
    /// 1 sec
    LPG_PER_1000,       // 4:   96
    /// 1.25 sec
    LPG_PER_1250,       // 5:   128
    /// 1.5 sec
    LPG_PER_1500,       // 6:   160
    /// 1.75 sec
    LPG_PER_1750,       // 7:   192
    LPG_PER_QTY
} HAL_LPG_PERIOD_T ;

// =============================================================================
// HAL_LPG_ON_T
// -----------------------------------------------------------------------------
/// The LPG pulse duration ranges from 15.6 ms to 0.234 s
// =============================================================================
typedef enum
{
    /// 15.6 ms
    LPG_ON_01 = 1,      // 1:   2 1/64 s
    /// 31.2 ms
    LPG_ON_02,          // 2:   3 2/64 s
    /// 46.9 ms
    LPG_ON_03,          // 3:   4 3/64 s
    /// 62.5 ms
    LPG_ON_04,          // 4:   5 4/64 s
    /// 78.1 ms
    LPG_ON_05,          // 5:   6 5/64 s
    /// 93.7 ms
    LPG_ON_06,          // 6:   7 6/64 s
    /// 0.11 s
    LPG_ON_07,          // 7:   8 7/64 s
    /// 0.125 s
    LPG_ON_08,          // 8:   9 8/64 s
    /// 0.141 s
    LPG_ON_09,          // 9:   10 9/64 s
    /// 0.156 s
    LPG_ON_10,          // 10:  11 10/64 s
    /// 0.172 s
    LPG_ON_11,          // 11:  12 11/64 s
    /// 0.187 s
    LPG_ON_12,          // 12:  13 12/64 s
    /// 0.203 s
    LPG_ON_13,          // 13:  14 13/64 s
    /// 0.219 s
    LPG_ON_14,          // 14:  15 14/64 s
    /// 0.234 s
    LPG_ON_15,          // 15:  16 15/64 s
    LPG_ON_QTY
} HAL_LPG_ON_T;

// =============================================================================
// MACROS
// =============================================================================

/// PWM system clock divider setting
#define HAL_PWM_DIV     5

// =============================================================================
// FUNCTIONS
// =============================================================================




// =============================================================================
// hal_BuzzStart
// -----------------------------------------------------------------------------
/// This function makes the PWM module output a tone.
///
/// This function shares a resource with the functions #hal_PwlGlow and #hal_PwlSelLevel.
/// This function requests the common resource to #HAL_SYS_FREQ_26M.
///
/// @param noteFreq This is the desired frequency for the generated
/// tone. (Minimum 152 Hz)
/// @param level Generated tone rough level, from 0 (mute) to 100 (full level).
// =============================================================================
PUBLIC VOID hal_BuzzStart(UINT16 noteFreq, UINT16 level);

// =============================================================================
// hal_BuzzStop
// -----------------------------------------------------------------------------
/// Stop the PWM module from outputting a tone
///
/// This function shares a resource with the functions #hal_PwlGlow and #hal_PwlSelLevel.
/// This function release the common resource to #HAL_SYS_FREQ_32K if the other functions don't need it.
///
// =============================================================================
PUBLIC VOID hal_BuzzStop(VOID);



// =============================================================================
// hal_PwlGlow
// -----------------------------------------------------------------------------
/// Set the average on time for the configured PWL output.
/// Thus, the integrated output represents a certain luminosity level
/// It can also be operated in pulse mode between two boundary values.
/// The time span between those boundaries is the \c pulsePeriod
///
/// This function shares a resource with the functions #hal_BuzzStart and #hal_PwlSelLevel.
/// This function manage the common resource: if the PWL output is not forced to 0 or 1
/// the common resource is taken to #HAL_SYS_FREQ_26M,
/// else it is released according to other functions need.
///
/// @param  levelMin The average on-time can be set via this value if we
/// don't operate in pulse mode. A value of
/// 0 will force the PWL output to 0 while a value of 0xff will force the
/// value to 1. All intermediate values will generate an average duty cycle
/// of (level/255 * 100). \n
/// If in pulse mode, this is the lower boundary for PWL pulse
/// @param levelMax Ignored if not in pulse mode, this is the upper
/// boundary for PWL pulse
/// @param pulsePeriod Ignored if not in pulse mode, this is the period of
/// pulse when pulsing is enabled
/// @param pulse Enable the pulse mode if \c TRUE.
// =============================================================================
PUBLIC VOID hal_PwlGlow(UINT8 levelMin, UINT8 levelMax,
                        UINT8 pulsePeriod, BOOL pulse);

// =============================================================================
// hal_PwlSelLevel
// -----------------------------------------------------------------------------
///  Set the average on time for the selected PWL output
/// Thus, the integrated output represents a certain luminosity level
///
/// This function shares a resource with the functions #hal_BuzzStart and #hal_PwlGlow.
/// This function manage the common resource: if the PWL output is not forced to 0 or 1
/// the common resource is taken to #HAL_SYS_FREQ_26M,
/// else it is released according to other functions need.
///
/// @param  level The average on-time can be set via this value. A value of
/// 0 will force the PWL output to 0 while a value of 0xff will force the
/// value to 1. All intermediate values will generate an average duty cycle
/// of (level/255 * 100)
// =============================================================================
PUBLIC VOID hal_PwlSelLevel(HAL_PWL_ID_T id, UINT8 level);

// =============================================================================
// hal_LpgStart
// -----------------------------------------------------------------------------
/// This function starts the LPG with the appopriate settings for period and
/// on time.
///
/// @param period This sets the LPG period
/// @param onTime This sets the LPG on time
// =============================================================================
PUBLIC VOID hal_LpgStart(HAL_LPG_PERIOD_T period, HAL_LPG_ON_T onTime);



// =============================================================================
// hal_LpgStop
// -----------------------------------------------------------------------------
/// Stops the LPG module.
// =============================================================================
PUBLIC VOID hal_LpgStop(VOID);



// =============================================================================
// hal_PwlGlowingOutputDisable
// -----------------------------------------------------------------------------
/// Disables the output of the glowing Pwl (forced to 0), or reenable it.
/// @param disable If \c TRUE, the output is disabled,
///                if \c FALSE, the output returns to its normal behaviour.
// =============================================================================
PUBLIC VOID hal_PwlGlowingOutputDisable(BOOL disable);



// =============================================================================
// hal_PwlOutputDisable
// -----------------------------------------------------------------------------
/// Disables the output of the selected Pwl (forced to 0), or reenable it.
/// @param disable If \c TRUE, the output is disabled,
///                if \c FALSE, the output returns to its normal behaviour.
// =============================================================================
PUBLIC VOID hal_PwlOutputDisable(HAL_PWL_ID_T id, BOOL disable);


///  @} <- End of pwm group



#endif // _HAL_PWM_H_


