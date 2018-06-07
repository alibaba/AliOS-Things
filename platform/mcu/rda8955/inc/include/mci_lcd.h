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


#ifndef _MCI_LCD_H_
#define _MCI_LCD_H_

#include "cs_types.h"

// ============================================================================
// Defines
// ============================================================================

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
    MCI_LCD_BIAS,
    MCI_LCD_CONTRAST,
    MCI_LCD_LINE_RATE,
    MCI_LCD_TEMP_COMP,
    MCI_LCD_BRIGHTNESS,
    MCI_LCD_PARAM_NUM
} MCI_LCD_PARAM_T;

// ============================================================================
// MCI_LCD_BPFUN_T
// ----------------------------------------------------------------------------
/// Type of the bypass function
/// buffer : buffer accessed by the BlockWrite_DI function
/// startx, starty, endx, endy defines the area refreshed by the BlockWrite_DI
// ============================================================================
typedef VOID (*MCI_LCD_BPFUN_T)(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
// ============================================================================
// Functions
// ============================================================================

// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
/// Initialize LCM and fill the LCD screen with the background color
/// @param background : color to use to fill the screen
/// @return none
// ============================================================================
VOID mci_LcdInit(UINT32 background);


// ============================================================================
// mci_LcdPowerOn
// ----------------------------------------------------------------------------
/// Turns the LCM power on and off<br/>
/// @param on : TRUE to turn on the LCM power, FALSE to turn if off
/// @return none
// ============================================================================
VOID mci_LcdPowerOn(BOOL on);


// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
/// Turns the LCD screen on and off<br/>
/// @param on : TRUE to turn on the screen, FALSE to turn it off
/// @return none
// ============================================================================
VOID mci_LcdScreenOn(BOOL on);

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
/// Copy the selected area of the frame buffer to the LCD<br/>
/// @param  startx : top left corner x value
/// @param  starty : top left corner y value
/// @param  endx : bottom right corner x value
/// @param  endy : bottom right corner y value
/// @return none
// ============================================================================
VOID mci_LcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
/// Returns the width and height of the LCD module
/// @param out_LCD_width : lcd screen width
/// @param out_LCD_height : lcd screen height
/// @return none
// ============================================================================
VOID mci_LcdGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height);

// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
/// Set the LCM in sleep mode
/// @return none
// ============================================================================
VOID mci_LcdSleep(VOID);

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
/// Wake up the LCM
/// @return none
// ============================================================================
VOID mci_LcdWakeup(VOID);

// ============================================================================
// mci_LcdPartialOn
// ----------------------------------------------------------------------------
/// Enables only the LCD lines between start_line and end_line<br/>
/// This allows to reduce power consumption while still displaying a part of
/// the screen. The area outside of the selected lines is black<br/>
/// <b>Currently does nothing in all the drivers</b>
/// @param start_line : top line of the enabled area
/// @param end_line : bottom line of the enabled area
/// @return none
// ============================================================================
VOID mci_LcdPartialOn(UINT16 start_line,UINT16 end_line);

// ============================================================================
// mci_LcdPartialOff
// ----------------------------------------------------------------------------
/// Return to normal mode. Enable all the lines of the LCD<br/>
/// <b>Currently does nothing in all the drivers</b>
/// @return none
// ============================================================================
VOID mci_LcdPartialOff(VOID);

// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
/// Fill the screen with the selected color
/// @param background : Color to use to fill the buffer in 565 RGB format
/// @return none
// ============================================================================
VOID mci_LcdClearScreen(UINT16 background);

// ============================================================================
// UINT8 mci_LcdGetParam(MCI_LCD_PARAM_T param_id)
// ----------------------------------------------------------------------------
/// returns the parameter value selected by param_id<br/>
/// <b>Currently does nothing and returns 0 in all the drivers</b>
/// @param type : Parameter ID
/// @return Parameter value
// ============================================================================
UINT8 mci_LcdGetParam(MCI_LCD_PARAM_T param_id);

// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
/// Set brightness level
/// @param level : brightness level 0 (dim) - 7 (bright)
/// @return none
// ============================================================================
VOID mci_LcdSetBrightness(UINT8 level);

// ============================================================================
// mci_LcdSetBias
// ----------------------------------------------------------------------------
/// set Bias value<br/>
/// @param bias : bias value
/// @return none
// ============================================================================
VOID mci_LcdSetBias(UINT8 bias);

// ============================================================================
// mci_LcdSetContrast
// ----------------------------------------------------------------------------
/// set Contrast value<br/>
/// @param contrast : contrast value
/// @return none
// ============================================================================
VOID mci_LcdSetContrast(UINT8 contrast);

// ============================================================================
// mci_LcdSetLineRate
// ----------------------------------------------------------------------------
/// set line rate value<br/>
/// @param linerate : line rate value
/// @return none
// ============================================================================
VOID mci_LcdSetLineRate(UINT8 linerate);

// ============================================================================
// mci_LcdSetTempComp
// ----------------------------------------------------------------------------
/// set temp compensation value<br/>
/// @param compensate : temp compensation value
/// @return none
// ============================================================================
VOID mci_LcdSetTempComp(UINT8 temp);

// ============================================================================
// mci_LcdStartBypass
// ----------------------------------------------------------------------------
/// Bypass the mci_LcdBlockWrite function
/// Instead of preforming the blit operation, the mci_LcdBlockWrite will call
/// the bypass function with the buffer to write and the area to write
/// @param bypassFun : bypass function
/// @return TRUE if bypass succeeded - FALSE if the LCD was already bypassed
// ============================================================================
BOOL mci_LcdStartBypass(MCI_LCD_BPFUN_T bypassFun);

// ============================================================================
// mci_LcdStopBypass
// ----------------------------------------------------------------------------
/// Disable Bypass of the BlockWrite_DI function
/// @return none
// ============================================================================
VOID mci_LcdStopBypass();

#if (CHIP_HAS_GOUDA == 1)

// ============================================================================
// mci_LcdlayerMerge
// ----------------------------------------------------------------------------
VOID mci_LcdLayerMerge(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

#endif // (CHIP_HAS_GOUDA == 1)

#ifdef DUAL_LCD
VOID mci_LcdSubClearScreen(UINT16 background);
VOID mci_LcdSubInit(UINT32 background);
BOOL mci_LcdSubStartBypass(MCI_LCD_BPFUN_T bypassFun);
VOID mci_LcdSubLayerMerge(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
VOID mci_LcdSubBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
VOID mci_LcdSubGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height);
VOID mci_LcdSubSleep(VOID);
VOID mci_LcdSubWakeup(VOID);
VOID mci_LcdSubPartialOn(UINT16 startLine,UINT16 endLine);
VOID mci_LcdSubPartialOff(VOID);
VOID mci_LcdSubScreenOn(BOOL on);
VOID mci_LcdSubSetContrast(UINT8 contrast);
#endif
/// @}
#endif //_MCI_LCD_H_
