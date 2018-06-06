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


#ifndef _CAM_TYPES_H_
#define _CAM_TYPES_H_



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// CAM_PARAM_T
// -----------------------------------------------------------------------------
/// This type lists the parameters of the camera that can be set by the
/// #cams_SetParam function
// =============================================================================
typedef enum
{
    /// Flash. Possible values are defined by the
    /// CAM_PARAM_FLASH_T type.
    CAM_PARAM_FLASH,

    /// Exposure. Possible values are defined by the
    /// CAM_PARAM_EXPOSURE_T type.
    CAM_PARAM_EXPOSURE,

    /// Exposure. Possible values are defined by the
    /// CAM_PARAM_WHITE_BALANCE_T type.
    CAM_PARAM_WHITE_BALANCE,

    /// Exposure. Possible values are defined by the
    /// CAM_PARAM_SPECIAL_EFFECT_T type.
    CAM_PARAM_SPECIAL_EFFECT,

    /// Reserved parameter identifier.
    CAM_PARAM_RESERVED_0,

    /// Reserved parameter identifier.
    CAM_PARAM_RESERVED_1,

    /// Reserved parameter identifier.
    CAM_PARAM_RESERVED_2,

    /// Contrast. Possible values are defined by the
    /// CAM_PARAM_CONTRAST_T type.
    CAM_PARAM_CONTRAST,

    /// Brightness. Possible values are defined by the
    /// CAM_PARAM_BRIGHTNESS_T type.
    CAM_PARAM_BRIGHTNESS,

    /// Night mode. Possible values are defined by the
    /// CAM_PARAM_NIGHT_MODE_T type.
    CAM_PARAM_NIGHT_MODE,

    /// Reserved parameter identifier.
    CAM_PARAM_RESERVED_3,

    /// Reserved parameter identifier.
    CAM_PARAM_RESERVED_4,

    CAM_PARAM_QTY
} CAM_PARAM_T;


// =============================================================================
// CAM_PARAM_FLASH_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_FLASH camera flash parameter.
// =============================================================================
typedef enum
{
    CAM_PARAM_FLASH_NONE           = 0x00,
    CAM_PARAM_FLASH_AUTO           = 0x01,
    CAM_PARAM_FLASH_FORCED         = 0x02,
    CAM_PARAM_FLASH_FILLIN         = 0x03,
    CAM_PARAM_FLASH_REDEYEREDUCE   = 0x04
} CAM_PARAM_FLASH_T;


// =============================================================================
// CAM_PARAM_EXPOSURE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_EXPOSURE camera exposure parameter.
// =============================================================================
typedef enum
{
    CAM_PARAM_EXPOSURE_NEGATIVE_2  = 0x00,
    CAM_PARAM_EXPOSURE_NEGATIVE_1  = 0x01,
    CAM_PARAM_EXPOSURE_AUTO        = 0x02,
    CAM_PARAM_EXPOSURE_POSITIVE_1  = 0x03,
    CAM_PARAM_EXPOSURE_POSITIVE_2  = 0x04
} CAM_PARAM_EXPOSURE_T;


// =============================================================================
// CAM_PARAM_WHITE_BALANCE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_WHITE_BALANCE camera white balance
/// parameter.
// =============================================================================
typedef enum
{
    CAM_PARAM_WHITE_BALANCE_AUTO       = 0x00,
    CAM_PARAM_WHITE_BALANCE_DAYLIGHT   = 0x01,
    CAM_PARAM_WHITE_BALANCE_TUNGSTEN   = 0x02,
    CAM_PARAM_WHITE_BALANCE_OFFICE     = 0x03,
    CAM_PARAM_WHITE_BALANCE_CLOUDY     = 0x04,
} CAM_PARAM_WHITE_BALANCE_T;


// =============================================================================
// CAM_PARAM_SPECIAL_EFFECT_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_SPECIAL_EFFECT camera special effect
/// selection.
// =============================================================================
typedef enum
{
    CAM_PARAM_SPECIAL_EFFECT_NORMAL                = 0x00,
    CAM_PARAM_SPECIAL_EFFECT_ANTIQUE               = 0x01,
    CAM_PARAM_SPECIAL_EFFECT_REDISH                = 0x02,
    CAM_PARAM_SPECIAL_EFFECT_GREENISH              = 0x03,
    CAM_PARAM_SPECIAL_EFFECT_BLUEISH               = 0x04,
    CAM_PARAM_SPECIAL_EFFECT_BLACKWHITE            = 0x05,
    CAM_PARAM_SPECIAL_EFFECT_NEGATIVE              = 0x06,
    CAM_PARAM_SPECIAL_EFFECT_BLACKWHITE_NEGATIVE   = 0x07
} CAM_PARAM_SPECIAL_EFFECT_T;


// =============================================================================
// CAM_PARAM_CONTRAST_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_CONTRAST camera contrast.
// =============================================================================
typedef enum
{
    CAM_PARAM_CONTRAST_0,
    CAM_PARAM_CONTRAST_1,
    CAM_PARAM_CONTRAST_2,
    CAM_PARAM_CONTRAST_3,
    CAM_PARAM_CONTRAST_4,
    CAM_PARAM_CONTRAST_5,
    CAM_PARAM_CONTRAST_6,
    CAM_PARAM_CONTRAST_7
} CAM_PARAM_CONTRAST_T;


// =============================================================================
// CAM_PARAM_BRIGHTNESS_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_BRIGHTNESS camera brightness.
// =============================================================================
typedef enum
{
    CAM_PARAM_BRIGHTNESS_0,
    CAM_PARAM_BRIGHTNESS_1,
    CAM_PARAM_BRIGHTNESS_2,
    CAM_PARAM_BRIGHTNESS_3,
    CAM_PARAM_BRIGHTNESS_4,
    CAM_PARAM_BRIGHTNESS_5,
    CAM_PARAM_BRIGHTNESS_6,
    CAM_PARAM_BRIGHTNESS_7
} CAM_PARAM_BRIGHTNESS_T;



// =============================================================================
// CAM_PARAM_NIGHT_MODE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAM_PARAM_NIGHT_MODE camera night mode.
// =============================================================================
typedef enum
{
    CAM_PARAM_NIGHT_MODE_OFF,
    CAM_PARAM_NIGHT_MODE_ON
} CAM_PARAM_NIGHT_MODE_T;


// =============================================================================
// CAM_CONFIG_T
// -----------------------------------------------------------------------------
/// This array describes a complete set defining any configurable option
/// for the camera.
/// This is an array indexed with CAM_PARAM_T enum elements, whose data are
/// of the respective parameter configuration types.
// =============================================================================
typedef UINT32 CAM_CONFIG_T[CAM_PARAM_QTY];



#endif // _CAM_TYPES_H_




