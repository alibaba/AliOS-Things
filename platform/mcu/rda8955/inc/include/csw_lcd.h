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


#ifndef _CSW_LCD_H_
#define _CSW_LCD_H_

#include "cs_types.h"
#include "mci_lcd.h"

// ============================================================================
// Defines
// ============================================================================
// ----------------------------------------------------------------------------
#ifndef CLEANER_IF
VOID Init_DI(UINT32 background, VOID **buf_addr);
#else
#define Init_DI mci_LcdInit
#endif
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/// VOID PWRON_DI(BOOL on);
// ----------------------------------------------------------------------------
#define PWRON_DI mci_LcdPowerOn

// ----------------------------------------------------------------------------
/// VOID BrightLevel_DI(UINT8 level);
// ----------------------------------------------------------------------------
#define BrightLevel_DI mci_LcdSetBrightness

// ----------------------------------------------------------------------------
/// VOID SCREENON_DI(BOOL on);
// ----------------------------------------------------------------------------
#define SCREENON_DI mci_LcdScreenOn

// ----------------------------------------------------------------------------
/// VOID BlockWrite_DI(UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
/// <b>!!!! IMPORTANT !!!!: Uses an implicit frame buffer that can be accessed through
/// get_lcd_frame_buffer_address() <br/>
/// Would strongly recommand to pass the frame buffer pointer as a parameter
/// to avoid this hidden interface<br/>
/// !!! This would only affect 4 lines in the MMI code and one line for each LCD driver !!!</b>
// ----------------------------------------------------------------------------
#ifndef CLEANER_IF
VOID BlockWrite_DI(UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
#else
#define BlockWrite_DI mci_LcdBlockWrite
#endif

// ----------------------------------------------------------------------------
/// VOID GetSize_DI(UINT16 *out_LCD_width,UINT16 *out_LCD_height)
// ----------------------------------------------------------------------------
#define GetSize_DI mci_LcdGetDimension

// ----------------------------------------------------------------------------
/// VOID SleepIn_DI(VOID)
// ----------------------------------------------------------------------------
#define SleepIn_DI mci_LcdSleep

// ----------------------------------------------------------------------------
/// VOID SleepOut(VOID)
// ----------------------------------------------------------------------------
#define SleepOut_DI mci_LcdWakeup

// ----------------------------------------------------------------------------
/// VOID PartialOn_DI(UINT16 start_line,UINT16 end_line);
// ----------------------------------------------------------------------------
#define PartialOn_DI mci_LcdPartialOn

// ----------------------------------------------------------------------------
///VOID PartialOff_DI(VOID)
// ----------------------------------------------------------------------------
#define PartialOff_DI mci_LcdPartialOff

// ----------------------------------------------------------------------------
///VOID ClearScreen_DI(UINT16 background)
// ----------------------------------------------------------------------------
#define ClearScreen_DI mci_LcdClearScreen

// ----------------------------------------------------------------------------
///UINT8 partial_display_align_DI(VOID)
// ----------------------------------------------------------------------------
/// ???????????????<br/>
/// <b>Currently does nothing and returns 0 in all the drivers</b>
/// @return ??????
UINT8 partial_display_align_DI(VOID);

// ----------------------------------------------------------------------------
///UINT8 get_param_number_DI(lcd_func_type type)
// ----------------------------------------------------------------------------
#define get_param_number_DI mci_LcdGetParam

// ----------------------------------------------------------------------------
///VOID set_bias_DI(UINT8 *bias)
// ----------------------------------------------------------------------------
#define set_bias_DI mci_LcdSetBias

// ----------------------------------------------------------------------------
///VOID set_contrast_DI(UINT8 *contrast)
// ----------------------------------------------------------------------------
#define set_contrast_DI mci_LcdSetContrast

// ----------------------------------------------------------------------------
///VOID set_linerate_DI(UINT8 *linerate)
// ----------------------------------------------------------------------------
#define set_linerate_DI mci_LcdSetLineRate

// ----------------------------------------------------------------------------
///VOID set_temp_compensate_DI(UINT8 *linerate)
// ----------------------------------------------------------------------------
#define set_temp_compensate_DI mci_LcdSetTempComp

// THE FOLLOWING FUNCTIONS ARE ONLY MEANINGFULL WITH AN EXTERNAL MEDIA CHIP
// Leave them undefined
// ----------------------------------------------------------------------------
// lcd_update_image_DI
// ----------------------------------------------------------------------------
///  ????????????????? External media chip refresh picture?<br/>
/// <b>This function is only implemented for the Vimicro</b>
/// @param startx : startx
/// @param starty : starty
/// @param Width : Width
/// @param Height : Height
/// @return none
// ============================================================================
VOID lcd_update_image_DI(UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height);

// ============================================================================
// lcd_chip_init_DI
// ----------------------------------------------------------------------------
/// ?????????????????? External media chip init?<br/>
/// <b>This function is only implemented for the Vimicro</b>
/// @return none
// ============================================================================
VOID lcd_chipinit_DI(VOID);

// ============================================================================
// Types
// ============================================================================
// ============================================================================
// lcd_func_type
// ----------------------------------------------------------------------------
/// Enum describing the ID of the various parameters related to LCD
// ============================================================================
typedef enum
{
    lcd_Bais_func,
    lcd_Contrast_func,
    lcd_LineRate_func,
    lcd_Temperature_Compensation_func
} lcd_func_type;

/// @}
#endif //_CSW_LCD_H_
