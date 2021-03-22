/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Name:    GLCD_Config.h
 * Purpose: Graphic LCD interface configuration file for MCB1800 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#ifndef __GLCD_CONFIG_H
#define __GLCD_CONFIG_H

/*---------------------- Graphic LCD orientation configuration ---------------*/
#ifndef GLCD_MIRROR_X
#define GLCD_MIRROR_X   0               /* Mirror X axis = 1:yes, 0:no */
#endif
#ifndef GLCD_MIRROR_Y
#define GLCD_MIRROR_Y   1               /* Mirror Y axis = 1:yes, 0:no */
#endif
#ifndef GLCD_SWAP_XY
#define GLCD_SWAP_XY    1               /* Swap X&Y axis = 1:yes, 0:no */
#endif

/*---------------------- Graphic LCD physical definitions --------------------*/
#define GLCD_SIZE_X     240             /* Screen size X (in pixels) */
#define GLCD_SIZE_Y     320             /* Screen size Y (in pixels) */
#define GLCD_BPP        16              /* Bits per pixel            */

#if    (GLCD_SWAP_XY)
#define GLCD_WIDTH      GLCD_SIZE_Y     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_X     /* Screen Height (in pixels) */
#else
#define GLCD_WIDTH      GLCD_SIZE_X     /* Screen Width  (in pixels) */
#define GLCD_HEIGHT     GLCD_SIZE_Y     /* Screen Height (in pixels) */
#endif

/*---------------------- Graphic LCD color definitions -----------------------*/
/* Color coding (16-bit): 
     15..11 = R4..0 (Red)
     10..5  = G5..0 (Green)
      4..0  = B4..0 (Blue)
*/

/* GLCD RGB color definitions                            */
#define GLCD_COLOR_BLACK        0x0000  /*   0,   0,   0 */
#define GLCD_COLOR_NAVY         0x000F  /*   0,   0, 128 */
#define GLCD_COLOR_DARK_GREEN   0x03E0  /*   0, 128,   0 */
#define GLCD_COLOR_DARK_CYAN    0x03EF  /*   0, 128, 128 */
#define GLCD_COLOR_MAROON       0x7800  /* 128,   0,   0 */
#define GLCD_COLOR_PURPLE       0x780F  /* 128,   0, 128 */
#define GLCD_COLOR_OLIVE        0x7BE0  /* 128, 128,   0 */
#define GLCD_COLOR_LIGHT_GREY   0xC618  /* 192, 192, 192 */
#define GLCD_COLOR_DARK_GREY    0x7BEF  /* 128, 128, 128 */
#define GLCD_COLOR_BLUE         0x001F  /*   0,   0, 255 */
#define GLCD_COLOR_GREEN        0x07E0  /*   0, 255,   0 */
#define GLCD_COLOR_CYAN         0x07FF  /*   0, 255, 255 */
#define GLCD_COLOR_RED          0xF800  /* 255,   0,   0 */
#define GLCD_COLOR_MAGENTA      0xF81F  /* 255,   0, 255 */
#define GLCD_COLOR_YELLOW       0xFFE0  /* 255, 255, 0   */
#define GLCD_COLOR_WHITE        0xFFFF  /* 255, 255, 255 */

#endif /* __GLCD_CONFIG_H */
