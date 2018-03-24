/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.40 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              STMicroelectronics International NV, 39, Chemin du Champ-des Filles, 1228 Plan Les Ouates, Geneva, SWITZERLAND
Licensed SEGGER software: emWin
License number:           GUI-00429
License model:            Buyout SRC [Buyout Source Code License, signed November 29th 2012]
Licensed product:         -
Licensed platform:        STMs ARM Cortex-M based 32 BIT CPUs
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2012-12-07 - 2017-12-31
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUI__BiDi2_Brackets_800.h
Purpose     : Bracket pairs according to Unicode V8.0.0
---------------------------END-OF-HEADER------------------------------
*/

#ifdef GUI__BIDI2
//  CodePoint
//  |       Paired bracket
//  |       |       Bracket type
//  |       |       |
//  V       V       V
  { 0x0028, 0x0029, BPT_O },  // LEFT PARENTHESIS
  { 0x0029, 0x0028, BPT_C },  // RIGHT PARENTHESIS
  { 0x005B, 0x005D, BPT_O },  // LEFT SQUARE BRACKET
  { 0x005D, 0x005B, BPT_C },  // RIGHT SQUARE BRACKET
  { 0x007B, 0x007D, BPT_O },  // LEFT CURLY BRACKET
  { 0x007D, 0x007B, BPT_C },  // RIGHT CURLY BRACKET
  { 0x0F3A, 0x0F3B, BPT_O },  // TIBETAN MARK GUG RTAGS GYON
  { 0x0F3B, 0x0F3A, BPT_C },  // TIBETAN MARK GUG RTAGS GYAS
  { 0x0F3C, 0x0F3D, BPT_O },  // TIBETAN MARK ANG KHANG GYON
  { 0x0F3D, 0x0F3C, BPT_C },  // TIBETAN MARK ANG KHANG GYAS
  { 0x169B, 0x169C, BPT_O },  // OGHAM FEATHER MARK
  { 0x169C, 0x169B, BPT_C },  // OGHAM REVERSED FEATHER MARK
  { 0x2045, 0x2046, BPT_O },  // LEFT SQUARE BRACKET WITH QUILL
  { 0x2046, 0x2045, BPT_C },  // RIGHT SQUARE BRACKET WITH QUILL
  { 0x207D, 0x207E, BPT_O },  // SUPERSCRIPT LEFT PARENTHESIS
  { 0x207E, 0x207D, BPT_C },  // SUPERSCRIPT RIGHT PARENTHESIS
  { 0x208D, 0x208E, BPT_O },  // SUBSCRIPT LEFT PARENTHESIS
  { 0x208E, 0x208D, BPT_C },  // SUBSCRIPT RIGHT PARENTHESIS
  { 0x2308, 0x2309, BPT_O },  // LEFT CEILING
  { 0x2309, 0x2308, BPT_C },  // RIGHT CEILING
  { 0x230A, 0x230B, BPT_O },  // LEFT FLOOR
  { 0x230B, 0x230A, BPT_C },  // RIGHT FLOOR
  { 0x2329, 0x232A, BPT_O },  // LEFT-POINTING ANGLE BRACKET
  { 0x232A, 0x2329, BPT_C },  // RIGHT-POINTING ANGLE BRACKET
  { 0x2768, 0x2769, BPT_O },  // MEDIUM LEFT PARENTHESIS ORNAMENT
  { 0x2769, 0x2768, BPT_C },  // MEDIUM RIGHT PARENTHESIS ORNAMENT
  { 0x276A, 0x276B, BPT_O },  // MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT
  { 0x276B, 0x276A, BPT_C },  // MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT
  { 0x276C, 0x276D, BPT_O },  // MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
  { 0x276D, 0x276C, BPT_C },  // MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
  { 0x276E, 0x276F, BPT_O },  // HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
  { 0x276F, 0x276E, BPT_C },  // HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
  { 0x2770, 0x2771, BPT_O },  // HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
  { 0x2771, 0x2770, BPT_C },  // HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT
  { 0x2772, 0x2773, BPT_O },  // LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT
  { 0x2773, 0x2772, BPT_C },  // LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT
  { 0x2774, 0x2775, BPT_O },  // MEDIUM LEFT CURLY BRACKET ORNAMENT
  { 0x2775, 0x2774, BPT_C },  // MEDIUM RIGHT CURLY BRACKET ORNAMENT
  { 0x27C5, 0x27C6, BPT_O },  // LEFT S-SHAPED BAG DELIMITER
  { 0x27C6, 0x27C5, BPT_C },  // RIGHT S-SHAPED BAG DELIMITER
  { 0x27E6, 0x27E7, BPT_O },  // MATHEMATICAL LEFT WHITE SQUARE BRACKET
  { 0x27E7, 0x27E6, BPT_C },  // MATHEMATICAL RIGHT WHITE SQUARE BRACKET
  { 0x27E8, 0x27E9, BPT_O },  // MATHEMATICAL LEFT ANGLE BRACKET
  { 0x27E9, 0x27E8, BPT_C },  // MATHEMATICAL RIGHT ANGLE BRACKET
  { 0x27EA, 0x27EB, BPT_O },  // MATHEMATICAL LEFT DOUBLE ANGLE BRACKET
  { 0x27EB, 0x27EA, BPT_C },  // MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET
  { 0x27EC, 0x27ED, BPT_O },  // MATHEMATICAL LEFT WHITE TORTOISE SHELL BRACKET
  { 0x27ED, 0x27EC, BPT_C },  // MATHEMATICAL RIGHT WHITE TORTOISE SHELL BRACKET
  { 0x27EE, 0x27EF, BPT_O },  // MATHEMATICAL LEFT FLATTENED PARENTHESIS
  { 0x27EF, 0x27EE, BPT_C },  // MATHEMATICAL RIGHT FLATTENED PARENTHESIS
  { 0x2983, 0x2984, BPT_O },  // LEFT WHITE CURLY BRACKET
  { 0x2984, 0x2983, BPT_C },  // RIGHT WHITE CURLY BRACKET
  { 0x2985, 0x2986, BPT_O },  // LEFT WHITE PARENTHESIS
  { 0x2986, 0x2985, BPT_C },  // RIGHT WHITE PARENTHESIS
  { 0x2987, 0x2988, BPT_O },  // Z NOTATION LEFT IMAGE BRACKET
  { 0x2988, 0x2987, BPT_C },  // Z NOTATION RIGHT IMAGE BRACKET
  { 0x2989, 0x298A, BPT_O },  // Z NOTATION LEFT BINDING BRACKET
  { 0x298A, 0x2989, BPT_C },  // Z NOTATION RIGHT BINDING BRACKET
  { 0x298B, 0x298C, BPT_O },  // LEFT SQUARE BRACKET WITH UNDERBAR
  { 0x298C, 0x298B, BPT_C },  // RIGHT SQUARE BRACKET WITH UNDERBAR
  { 0x298D, 0x2990, BPT_O },  // LEFT SQUARE BRACKET WITH TICK IN TOP CORNER
  { 0x298E, 0x298F, BPT_C },  // RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
  { 0x298F, 0x298E, BPT_O },  // LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
  { 0x2990, 0x298D, BPT_C },  // RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
  { 0x2991, 0x2992, BPT_O },  // LEFT ANGLE BRACKET WITH DOT
  { 0x2992, 0x2991, BPT_C },  // RIGHT ANGLE BRACKET WITH DOT
  { 0x2993, 0x2994, BPT_O },  // LEFT ARC LESS-THAN BRACKET
  { 0x2994, 0x2993, BPT_C },  // RIGHT ARC GREATER-THAN BRACKET
  { 0x2995, 0x2996, BPT_O },  // DOUBLE LEFT ARC GREATER-THAN BRACKET
  { 0x2996, 0x2995, BPT_C },  // DOUBLE RIGHT ARC LESS-THAN BRACKET
  { 0x2997, 0x2998, BPT_O },  // LEFT BLACK TORTOISE SHELL BRACKET
  { 0x2998, 0x2997, BPT_C },  // RIGHT BLACK TORTOISE SHELL BRACKET
  { 0x29D8, 0x29D9, BPT_O },  // LEFT WIGGLY FENCE
  { 0x29D9, 0x29D8, BPT_C },  // RIGHT WIGGLY FENCE
  { 0x29DA, 0x29DB, BPT_O },  // LEFT DOUBLE WIGGLY FENCE
  { 0x29DB, 0x29DA, BPT_C },  // RIGHT DOUBLE WIGGLY FENCE
  { 0x29FC, 0x29FD, BPT_O },  // LEFT-POINTING CURVED ANGLE BRACKET
  { 0x29FD, 0x29FC, BPT_C },  // RIGHT-POINTING CURVED ANGLE BRACKET
  { 0x2E22, 0x2E23, BPT_O },  // TOP LEFT HALF BRACKET
  { 0x2E23, 0x2E22, BPT_C },  // TOP RIGHT HALF BRACKET
  { 0x2E24, 0x2E25, BPT_O },  // BOTTOM LEFT HALF BRACKET
  { 0x2E25, 0x2E24, BPT_C },  // BOTTOM RIGHT HALF BRACKET
  { 0x2E26, 0x2E27, BPT_O },  // LEFT SIDEWAYS U BRACKET
  { 0x2E27, 0x2E26, BPT_C },  // RIGHT SIDEWAYS U BRACKET
  { 0x2E28, 0x2E29, BPT_O },  // LEFT DOUBLE PARENTHESIS
  { 0x2E29, 0x2E28, BPT_C },  // RIGHT DOUBLE PARENTHESIS
  { 0x3008, 0x3009, BPT_O },  // LEFT ANGLE BRACKET
  { 0x3009, 0x3008, BPT_C },  // RIGHT ANGLE BRACKET
  { 0x300A, 0x300B, BPT_O },  // LEFT DOUBLE ANGLE BRACKET
  { 0x300B, 0x300A, BPT_C },  // RIGHT DOUBLE ANGLE BRACKET
  { 0x300C, 0x300D, BPT_O },  // LEFT CORNER BRACKET
  { 0x300D, 0x300C, BPT_C },  // RIGHT CORNER BRACKET
  { 0x300E, 0x300F, BPT_O },  // LEFT WHITE CORNER BRACKET
  { 0x300F, 0x300E, BPT_C },  // RIGHT WHITE CORNER BRACKET
  { 0x3010, 0x3011, BPT_O },  // LEFT BLACK LENTICULAR BRACKET
  { 0x3011, 0x3010, BPT_C },  // RIGHT BLACK LENTICULAR BRACKET
  { 0x3014, 0x3015, BPT_O },  // LEFT TORTOISE SHELL BRACKET
  { 0x3015, 0x3014, BPT_C },  // RIGHT TORTOISE SHELL BRACKET
  { 0x3016, 0x3017, BPT_O },  // LEFT WHITE LENTICULAR BRACKET
  { 0x3017, 0x3016, BPT_C },  // RIGHT WHITE LENTICULAR BRACKET
  { 0x3018, 0x3019, BPT_O },  // LEFT WHITE TORTOISE SHELL BRACKET
  { 0x3019, 0x3018, BPT_C },  // RIGHT WHITE TORTOISE SHELL BRACKET
  { 0x301A, 0x301B, BPT_O },  // LEFT WHITE SQUARE BRACKET
  { 0x301B, 0x301A, BPT_C },  // RIGHT WHITE SQUARE BRACKET
  { 0xFE59, 0xFE5A, BPT_O },  // SMALL LEFT PARENTHESIS
  { 0xFE5A, 0xFE59, BPT_C },  // SMALL RIGHT PARENTHESIS
  { 0xFE5B, 0xFE5C, BPT_O },  // SMALL LEFT CURLY BRACKET
  { 0xFE5C, 0xFE5B, BPT_C },  // SMALL RIGHT CURLY BRACKET
  { 0xFE5D, 0xFE5E, BPT_O },  // SMALL LEFT TORTOISE SHELL BRACKET
  { 0xFE5E, 0xFE5D, BPT_C },  // SMALL RIGHT TORTOISE SHELL BRACKET
  { 0xFF08, 0xFF09, BPT_O },  // FULLWIDTH LEFT PARENTHESIS
  { 0xFF09, 0xFF08, BPT_C },  // FULLWIDTH RIGHT PARENTHESIS
  { 0xFF3B, 0xFF3D, BPT_O },  // FULLWIDTH LEFT SQUARE BRACKET
  { 0xFF3D, 0xFF3B, BPT_C },  // FULLWIDTH RIGHT SQUARE BRACKET
  { 0xFF5B, 0xFF5D, BPT_O },  // FULLWIDTH LEFT CURLY BRACKET
  { 0xFF5D, 0xFF5B, BPT_C },  // FULLWIDTH RIGHT CURLY BRACKET
  { 0xFF5F, 0xFF60, BPT_O },  // FULLWIDTH LEFT WHITE PARENTHESIS
  { 0xFF60, 0xFF5F, BPT_C },  // FULLWIDTH RIGHT WHITE PARENTHESIS
  { 0xFF62, 0xFF63, BPT_O },  // HALFWIDTH LEFT CORNER BRACKET
  { 0xFF63, 0xFF62, BPT_C },  // HALFWIDTH RIGHT CORNER BRACKET
#endif

/*************************** End of file ****************************/

