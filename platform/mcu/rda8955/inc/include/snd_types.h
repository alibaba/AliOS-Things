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


#ifndef _SND_TYPES_H_
#define _SND_TYPES_H_


#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// SND_SPK_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the speaker volume.
// =============================================================================
#ifdef CHIP_HAS_AP
typedef enum
{
    SND_SPK_MUTE                                = 0x00000000,
    SND_SPK_VOL_1                               = 0x00000001,
    SND_SPK_VOL_2                               = 0x00000002,
    SND_SPK_VOL_3                               = 0x00000003,
    SND_SPK_VOL_4                               = 0x00000004,
    SND_SPK_VOL_5                               = 0x00000005,
    SND_SPK_VOL_6                               = 0x00000006,
    SND_SPK_VOL_7                               = 0x00000007,
    SND_SPK_VOL_QTY                             = 0x00000008
} SND_SPK_LEVEL_T;
#else
typedef enum
{
    SND_SPK_MUTE                                = 0x00000000,
    SND_SPK_VOL_1                               = 0x00000001,
    SND_SPK_VOL_2                               = 0x00000002,
    SND_SPK_VOL_3                               = 0x00000003,
    SND_SPK_VOL_4                               = 0x00000004,
    SND_SPK_VOL_5                               = 0x00000005,
    SND_SPK_VOL_6                               = 0x00000006,
    SND_SPK_VOL_7                               = 0x00000007,
    SND_SPK_VOL_8                               = 0x00000008,
    SND_SPK_VOL_9                               = 0x00000009,
    SND_SPK_VOL_10                              = 0x0000000A,
    SND_SPK_VOL_11                              = 0x0000000B,
    SND_SPK_VOL_12                              = 0x0000000C,
    SND_SPK_VOL_13                              = 0x0000000D,
    SND_SPK_VOL_14                              = 0x0000000E,
    SND_SPK_VOL_15                              = 0x0000000F,

    SND_SPK_VOL_QTY
} SND_SPK_LEVEL_T;
#endif



// ============================================================================
// SND_MIC_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the mic: mute or enabled, volume
/// in call, volume in recording.
// =============================================================================
typedef enum
{
    SND_MIC_MUTE                                = 0x00000000,
    SND_MIC_ENABLE                              = 0x00000001,
    SND_MIC_VOL_CALL                            = 0x00000001,
    SND_MIC_VOL_RECORD                          = 0x00000002,
    SND_MIC_VOL_FM                                  = 0x00000003,
    SND_MIC_VOL_QTY                             = 0x00000004
} SND_MIC_LEVEL_T;


// ============================================================================
// SND_SIDE_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the side tone volume. The value
/// SND_SIDE_VOL_TEST is used for mic to spk simple loop back test.
// =============================================================================
#ifdef CHIP_HAS_AP
typedef enum
{
    SND_SIDE_MUTE                               = 0x00000000,
    SND_SIDE_VOL_1                              = 0x00000001,
    SND_SIDE_VOL_2                              = 0x00000002,
    SND_SIDE_VOL_3                              = 0x00000003,
    SND_SIDE_VOL_4                              = 0x00000004,
    SND_SIDE_VOL_5                              = 0x00000005,
    SND_SIDE_VOL_6                              = 0x00000006,
    SND_SIDE_VOL_7                              = 0x00000007,
    SND_SIDE_VOL_TEST                           = 0x00000008,
    SND_SIDE_VOL_QTY                            = 0x00000009
} SND_SIDE_LEVEL_T;
#else
typedef enum
{
    SND_SIDE_MUTE                               = 0x00000000,
    SND_SIDE_VOL_1                              = 0x00000001,
    SND_SIDE_VOL_2                              = 0x00000002,
    SND_SIDE_VOL_3                              = 0x00000003,
    SND_SIDE_VOL_4                              = 0x00000004,
    SND_SIDE_VOL_5                              = 0x00000005,
    SND_SIDE_VOL_6                              = 0x00000006,
    SND_SIDE_VOL_7                              = 0x00000007,
    SND_SIDE_VOL_8                              = 0x00000008,
    SND_SIDE_VOL_9                              = 0x00000009,
    SND_SIDE_VOL_10                              = 0x0000000a,
    SND_SIDE_VOL_11                              = 0x0000000b,
    SND_SIDE_VOL_12                              = 0x0000000c,
    SND_SIDE_VOL_13                              = 0x0000000d,
    SND_SIDE_VOL_14                              = 0x0000000e,
    SND_SIDE_VOL_15                              = 0x0000000f,
    SND_SIDE_VOL_TEST,
    SND_SIDE_VOL_QTY
} SND_SIDE_LEVEL_T;
#endif

// ============================================================================
// SND_TONE_ATTENUATION_T
// -----------------------------------------------------------------------------
/// Attenuation of the tone. The attenuation can be set to 0 dB, -3 dB, -9 dB and
/// -15dB.
// =============================================================================
typedef enum
{
/// No attenuation
    SND_TONE_0DB                                = 0x00000000,
/// -3dB
    SND_TONE_M3DB                               = 0x00000001,
/// -9db
    SND_TONE_M9DB                               = 0x00000002,
/// -15dB
    SND_TONE_M15DB                              = 0x00000003,
    SND_TONE_VOL_QTY                            = 0x00000004
} SND_TONE_ATTENUATION_T;


// ============================================================================
// SND_TONE_TYPE_T
// -----------------------------------------------------------------------------
/// Tone types. The DTMF Tones are used to inform the user that the number is being
/// composed. All the standard DTMF are available: 0 to 9, A to D, pound and star.
/// \n The Comfort Tones are used to inform the user on the current state of the
/// call: Ringing, Busy, Unavailable... All frequencies needed to do the standard
/// Comfort Tones are available: 425 Hz, 950 Hz, 1400 Hz and 1800 Hz. \n
// =============================================================================
typedef enum
{
/// Tone when the '0' key
    SND_DTMF_0                                  = 0x00000000,
/// Tone when the '1' key
    SND_DTMF_1                                  = 0x00000001,
/// Tone when the '2' key
    SND_DTMF_2                                  = 0x00000002,
/// Tone when the '3' key
    SND_DTMF_3                                  = 0x00000003,
/// Tone when the '4' key
    SND_DTMF_4                                  = 0x00000004,
/// Tone when the '5' key
    SND_DTMF_5                                  = 0x00000005,
/// Tone when the '6' key
    SND_DTMF_6                                  = 0x00000006,
/// Tone when the '7' key
    SND_DTMF_7                                  = 0x00000007,
/// Tone when the '8' key
    SND_DTMF_8                                  = 0x00000008,
/// Tone when the '9' key
    SND_DTMF_9                                  = 0x00000009,
    SND_DTMF_A                                  = 0x0000000A,
    SND_DTMF_B                                  = 0x0000000B,
    SND_DTMF_C                                  = 0x0000000C,
    SND_DTMF_D                                  = 0x0000000D,
/// Tone when the * key
    SND_DTMF_S                                  = 0x0000000E,
/// Tone when the # key
    SND_DTMF_P                                  = 0x0000000F,
/// Comfort tone at 425 Hz
    SND_COMFORT_425                             = 0x00000010,
/// Comfort tone at 950 Hz
    SND_COMFORT_950                             = 0x00000011,
/// Comfort tone at 1400 Hz
    SND_COMFORT_1400                            = 0x00000012,
/// Confort tone at 1800 Hz
    SND_COMFORT_1800                            = 0x00000013,
/// No tone is emitted
    SND_NO_TONE                                 = 0x00000014
} SND_TONE_TYPE_T;


// ============================================================================
// SND_ITF_T
// -----------------------------------------------------------------------------
/// That type provide a way to identify the different audio interface.
// =============================================================================
typedef enum
{
    SND_ITF_RECEIVER                            = 0x00000000,
    SND_ITF_EAR_PIECE                           = 0x00000001,
    SND_ITF_LOUD_SPEAKER                        = 0x00000002,
    SND_ITF_BLUETOOTH                           = 0x00000003,
    SND_ITF_FM                                  = 0x00000004,
    SND_ITF_TV                                  = 0x00000005,
/// Number (max) of available interface to the SND driver
    SND_ITF_QTY                                 = 0x00000006,
    SND_ITF_CLOSE                               = 0x000000AA,
    SND_ITF_NONE                                = 0x000000FF
} SND_ITF_T;


// ============================================================================
// SND_EQUALIZER_MODE_T
// -----------------------------------------------------------------------------
/// SND equalizer modes enumerator
// =============================================================================
typedef enum
{
    SND_EQUALIZER_OFF                           = 0xFFFFFFFF,
    SND_EQUALIZER_NORMAL                        = 0x00000000,
    SND_EQUALIZER_BASS                          = 0x00000001,
    SND_EQUALIZER_DANCE                         = 0x00000002,
    SND_EQUALIZER_CLASSICAL                     = 0x00000003,
    SND_EQUALIZER_TREBLE                        = 0x00000004,
    SND_EQUALIZER_PARTY                         = 0x00000005,
    SND_EQUALIZER_POP                           = 0x00000006,
    SND_EQUALIZER_ROCK                          = 0x00000007,
    SND_EQUALIZER_CUSTOM                        = 0x00000008,
    SND_EQUALIZER_QTY                           = 0x00000009
} SND_EQUALIZER_MODE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SND_FREQUENCY_BARS_T
// -----------------------------------------------------------------------------
/// Audio track frequency bars
// =============================================================================
typedef UINT8 SND_FREQUENCY_BARS_T[10];





#endif

