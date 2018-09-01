/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
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
File        : GUIDEMO_Bargraph.c
Purpose     : Shows a bargraph with alpha effect
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO_Bargraph.c
  * @author  MCD Application Team
  * @brief   Shows a bargraph with alpha effect
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


#include "GUI.h"
#include "GUIDEMO.h"

#if (SHOW_GUIDEMO_BARGRAPH && GUI_SUPPORT_MEMDEV)

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define GRAPH_WIDTH  172
#define GRAPH_HEIGHT 122
#define SHOW_TIME    15000
#define TIME_STEP    31

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       Antialiased digit font for labeling
*/
GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0030[ 36] = { /* code 0030, DIGIT ZERO */
  0x07, 0xEF, 0x91, 0x00,
  0x4F, 0xCA, 0xF7, 0x00,
  0x8F, 0x20, 0xDC, 0x00,
  0xBF, 0x00, 0xBF, 0x00,
  0xBF, 0x00, 0xBF, 0x00,
  0xBF, 0x00, 0xBF, 0x00,
  0x7F, 0x41, 0xEB, 0x00,
  0x2F, 0xED, 0xF6, 0x00,
  0x04, 0xAB, 0x60, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0031[ 27] = { /* code 0031, DIGIT ONE */
  0x00, 0xAF, 0x00,
  0x19, 0xFF, 0x00,
  0xEE, 0xDF, 0x00,
  0x82, 0xBF, 0x00,
  0x00, 0xBF, 0x00,
  0x00, 0xBF, 0x00,
  0x00, 0xBF, 0x00,
  0x00, 0xBF, 0x00,
  0x00, 0x8B, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0032[ 36] = { /* code 0032, DIGIT TWO */
  0x06, 0xEF, 0xC3, 0x00,
  0x3F, 0xC8, 0xFD, 0x00,
  0x5B, 0x30, 0xBF, 0x00,
  0x00, 0x01, 0xEB, 0x00,
  0x00, 0x1C, 0xE3, 0x00,
  0x01, 0xCE, 0x30, 0x00,
  0x0B, 0xE3, 0x00, 0x00,
  0x6F, 0xFF, 0xFF, 0x00,
  0x8B, 0xBB, 0xBB, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0033[ 36] = { /* code 0033, DIGIT THREE */
  0x3B, 0xFD, 0x30, 0x00,
  0xAF, 0x8E, 0xD0, 0x00,
  0x46, 0x0C, 0xE0, 0x00,
  0x00, 0xBE, 0x50, 0x00,
  0x00, 0x7C, 0xC1, 0x00,
  0x00, 0x05, 0xF7, 0x00,
  0xB9, 0x06, 0xF7, 0x00,
  0xAF, 0xCF, 0xE1, 0x00,
  0x18, 0xB9, 0x20, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0034[ 36] = { /* code 0034, DIGIT FOUR */
  0x00, 0x0C, 0xF0, 0x00,
  0x00, 0x7F, 0xF0, 0x00,
  0x04, 0xFE, 0xF0, 0x00,
  0x1D, 0x8B, 0xF0, 0x00,
  0x8D, 0x1B, 0xF0, 0x00,
  0xBA, 0x7D, 0xF7, 0x00,
  0xBF, 0xFF, 0xFF, 0x00,
  0x00, 0x0B, 0xF0, 0x00,
  0x00, 0x08, 0xB0, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0035[ 36] = { /* code 0035, DIGIT FIVE */
  0x05, 0xFF, 0xFB, 0x00,
  0x07, 0xFC, 0xB8, 0x00,
  0x0B, 0xF0, 0x00, 0x00,
  0x0E, 0xFE, 0xD5, 0x00,
  0x2F, 0xD7, 0xEE, 0x10,
  0x02, 0x10, 0x7F, 0x40,
  0x5F, 0x60, 0x9F, 0x30,
  0x2E, 0xFB, 0xFB, 0x00,
  0x03, 0x9B, 0x71, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0036[ 36] = { /* code 0036, DIGIT SIX */
  0x05, 0xDF, 0xB1, 0x00,
  0x2F, 0xCA, 0xF7, 0x00,
  0x8F, 0x10, 0x74, 0x00,
  0xBE, 0x7B, 0x70, 0x00,
  0xBF, 0xDC, 0xF8, 0x00,
  0xBF, 0x10, 0xCF, 0x00,
  0x7F, 0x20, 0xCF, 0x00,
  0x2E, 0xEC, 0xF8, 0x00,
  0x03, 0xAB, 0x70, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0037[ 36] = { /* code 0037, DIGIT SEVEN */
  0xBF, 0xFF, 0xFB, 0x00,
  0x8B, 0xBC, 0xF8, 0x00,
  0x00, 0x0C, 0xB0, 0x00,
  0x00, 0x7F, 0x20, 0x00,
  0x00, 0xE9, 0x00, 0x00,
  0x06, 0xF5, 0x00, 0x00,
  0x0A, 0xF1, 0x00, 0x00,
  0x0D, 0xC0, 0x00, 0x00,
  0x0B, 0x80, 0x00, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0038[ 36] = { /* code 0038, DIGIT EIGHT */
  0x08, 0xEF, 0xA1, 0x00,
  0x6F, 0x97, 0xF9, 0x00,
  0x7F, 0x40, 0xFB, 0x00,
  0x2D, 0xCA, 0xE5, 0x00,
  0x1B, 0xDC, 0xD3, 0x00,
  0x9F, 0x20, 0xDD, 0x00,
  0xBF, 0x10, 0xCF, 0x00,
  0x5F, 0xB8, 0xF9, 0x00,
  0x05, 0xAB, 0x70, 0x00
};

GUI_CONST_STORAGE unsigned char acGUI_FontD9_AA4_0039[ 36] = { /* code 0039, DIGIT NINE */
  0x08, 0xFF, 0x80, 0x00,
  0x7F, 0xA9, 0xF7, 0x00,
  0xBF, 0x00, 0xCC, 0x00,
  0xAF, 0x31, 0xDF, 0x00,
  0x3E, 0xFF, 0xEF, 0x00,
  0x02, 0x76, 0xBF, 0x00,
  0x4B, 0x40, 0xDA, 0x00,
  0x3F, 0xED, 0xF5, 0x00,
  0x05, 0xBA, 0x40, 0x00
};

GUI_CONST_STORAGE GUI_CHARINFO GUI_FontD9_AA4_CharInfo[10] = {
   {   7,   7,  4, acGUI_FontD9_AA4_0030 } /* code 0030 */
  ,{   5,   5,  3, acGUI_FontD9_AA4_0031 } /* code 0031 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0032 } /* code 0032 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0033 } /* code 0033 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0034 } /* code 0034 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0035 } /* code 0035 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0036 } /* code 0036 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0037 } /* code 0037 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0038 } /* code 0038 */
  ,{   7,   7,  4, acGUI_FontD9_AA4_0039 } /* code 0039 */
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontD9_AA4_Prop1 = {
   0x0030 /* first character */
  ,0x0039 /* last character  */
  ,&GUI_FontD9_AA4_CharInfo[  0] /* address of first character */
  ,(GUI_CONST_STORAGE GUI_FONT_PROP *)0 /* pointer to next GUI_FONT_PROP */
};

GUI_CONST_STORAGE GUI_FONT GUI_FontD9_AA4 = {
   GUI_FONTTYPE_PROP_AA4 /* type of font    */
  ,9 /* height of font  */
  ,9 /* space of font y */
  ,1 /* magnification x */
  ,1 /* magnification y */
  ,{&GUI_FontD9_AA4_Prop1}
  ,0 /* Baseline */
  ,0 /* Height of lowercase characters */
  ,0 /* Height of capital characters */
};

/*********************************************************************
*
*       Alpha bitmap for the orange dot at the left of the diagramm
*/
static GUI_CONST_STORAGE unsigned long _acCircleOrange_14x14[] = {
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFA0094F3, 0x860094F3, 0x790094F3, 0x140094F3, 0x140094F3, 0x790094F3, 0x860094F3, 0xFA0094F3, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xB90094F3, 0x450094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x450094F3, 0xB90094F3, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xC10094F3, 0x050094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x050094F3, 0xC10094F3, 0xFFFFFFFF,
  0xF10094F3, 0x3E0094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x3E0094F3, 0xF10094F3,
  0x870094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x870094F3,
  0x760094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x760094F3,
  0x140094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x140094F3,
  0x140094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x140094F3,
  0x760094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x760094F3,
  0x870094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x870094F3,
  0xF10094F3, 0x3E0094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x3E0094F3, 0xF10094F3,
  0xFFFFFFFF, 0xC10094F3, 0x050094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x050094F3, 0xC10094F3, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xB90094F3, 0x450094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x000094F3, 0x450094F3, 0xB90094F3, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFA0094F3, 0x860094F3, 0x790094F3, 0x140094F3, 0x140094F3, 0x790094F3, 0x860094F3, 0xFA0094F3, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

static GUI_CONST_STORAGE GUI_BITMAP _bmCircleOrange_14x14 = {
  14, /* XSize */
  14, /* YSize */
  56, /* BytesPerLine */
  32, /* BitsPerPixel */
  (unsigned char *)_acCircleOrange_14x14,  /* Pointer to picture data */
  NULL  /* Pointer to palette */
 ,GUI_DRAW_BMP8888
};

/*********************************************************************
*
*       Bitmaps for drawing the bars
*/
static GUI_CONST_STORAGE GUI_COLOR ColorsBarOrange_12x1[] = {
     0x2268E2,0x3785EC,0x509FEF,0x4B97F1
    ,0x63B1F3,0x69B5F3,0x71B9F5,0x7BBDF7
    ,0x86C4F8,0x8DC8FA,0xBADEFC,0xD5E9FE
};

static GUI_CONST_STORAGE GUI_COLOR ColorsBarBluegreen_12x1[] = {
     0x746E2D,0x918F3E,0x959041,0x979147
    ,0x9B9650,0xA19C5E,0xA9A56D,0xB2B182
    ,0xBFBE98,0xC3C29A,0xCFCFB3,0xE1E3D4
};

static GUI_CONST_STORAGE GUI_COLOR ColorsBarBlue_12x1[] = {
     0x825100,0xC3942C,0xCE953F,0xCD964A
    ,0xCF9B56,0xD4A465,0xDAAD76,0xE0B986
    ,0xE4C496,0xEBD1A7,0xEAD1A8,0xF4E1BF
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalBarOrange_12x1 = {
  12,	/* number of entries */
  0, 	/* No transparency */
  &ColorsBarOrange_12x1[0]
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalBarBluegreen_12x1 = {
  12,	/* number of entries */
  0, 	/* No transparency */
  &ColorsBarBluegreen_12x1[0]
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalBarBlue_12x1 = {
  12,	/* number of entries */
  0, 	/* No transparency */
  &ColorsBarBlue_12x1[0]
};

static GUI_CONST_STORAGE unsigned char _acBarOrange_12x1[] = {
  0x3B, 0xA9, 0x87, 0x65, 0x42, 0x10
};

static GUI_CONST_STORAGE unsigned char _acBarBluegreen_12x1[] = {
  0x09, 0xBA, 0x87, 0x65, 0x43, 0x21
};

static GUI_CONST_STORAGE unsigned char _acBarBlue_12x1[] = {
  0x01, 0xAB, 0x98, 0x76, 0x54, 0x32
};

static GUI_CONST_STORAGE GUI_BITMAP _bmBarOrange_12x1 = {
  12, /* XSize */
  1, /* YSize */
  6, /* BytesPerLine */
  4, /* BitsPerPixel */
  _acBarOrange_12x1,  /* Pointer to picture data (indices) */
  &PalBarOrange_12x1  /* Pointer to palette */
};

static GUI_CONST_STORAGE GUI_BITMAP _bmBarBluegreen_12x1 = {
  12, /* XSize */
  1, /* YSize */
  6, /* BytesPerLine */
  4, /* BitsPerPixel */
  _acBarBluegreen_12x1,  /* Pointer to picture data (indices) */
  &PalBarBluegreen_12x1  /* Pointer to palette */
};

static GUI_CONST_STORAGE GUI_BITMAP _bmBarBlue_12x1 = {
  12, /* XSize */
  1, /* YSize */
  6, /* BytesPerLine */
  4, /* BitsPerPixel */
  _acBarBlue_12x1,  /* Pointer to picture data (indices) */
  &PalBarBlue_12x1  /* Pointer to palette */
};

static GUI_CONST_STORAGE GUI_BITMAP * _apBmBar[] = {
  &_bmBarBlue_12x1,
  &_bmBarBluegreen_12x1,
  &_bmBarOrange_12x1
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void _DrawLabel(GUI_MEMDEV_Handle hMem, int xPos, int yPos) {
  GUI_MEMDEV_Handle hMemOld;
  int               FontSizeY, i, x0, y0;

  hMemOld = GUI_MEMDEV_Select(hMem);
  GUI_SetFont(&GUI_FontD9_AA4);
  GUI_SetTextMode(GUI_TM_TRANS);
  FontSizeY = GUI_GetFontSizeY();
  for (i = 0; i < 5; i++) {
    x0 = xPos - 7;
    y0 = yPos + 96 - i * 20 - 7;
    //GUI_ClearRect(x0, y0, x0 + 6, y0 + 13);
    GUI_DrawBitmap(&_bmCircleOrange_14x14, x0, y0);
    GUI_GotoXY(x0 + 7, y0 + 7 - FontSizeY / 2);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_SetColor(GUI_BLACK);
    GUI_DispDecMin((i + 1) * 2);
  }
  GUI_MEMDEV_Select(hMemOld);
}


/*********************************************************************
*
*       _DrawDiagramAt
*/
static void _DrawDiagramAt(GUI_MEMDEV_Handle hMem, int xPos, int yPos, int * py, int xBlend) {
  GUI_MEMDEV_Handle hMemOld;
  GUI_RECT          Rect;
  int               i, ySizeBar, IndexBmBar;

  hMemOld = GUI_MEMDEV_Select(hMem);
  //
  // Draw blue background
  //
  GUI_SetColor(0x4a2210);
  GUI_FillRoundedRect(xPos, yPos, xPos + GRAPH_WIDTH, yPos + GRAPH_HEIGHT, 4);
  //
  // Draw grid lines
  //
  GUI_SetColor(0x774830);
  for (i = 0; i < 12; i++) {
    GUI_DrawHLine(yPos + 6 + i * 10, xPos + 2, xPos + GRAPH_WIDTH - 2);
  }
  //
  // Draw bars
  //
  for (i = 0; i < 10; i++) {
    IndexBmBar = (i < 6) ? i / 2 : 4 - (i / 2);
    ySizeBar = *(py + i);
    GUI_DrawBitmapMag(_apBmBar[IndexBmBar], xPos + 8 + i * 16, yPos + GRAPH_HEIGHT - ySizeBar - 6, 1, ySizeBar);
  }
  //
  // Draw alpha effect
  //
  Rect.x0 = xPos;
  Rect.x1 = xPos + 3;
  Rect.y0 = yPos;
  Rect.y1 = yPos + GRAPH_HEIGHT;
  GUI_SetClipRect(&Rect);
  GUI_SetColor(0xd99100);
  GUI_SetAlpha(168);
  GUI_FillRoundedRect(xPos, yPos, xPos + GRAPH_WIDTH, yPos + GRAPH_HEIGHT, 4);
  GUI_SetClipRect(NULL);
  GUI_FillRect(xPos + 4, yPos + 1, xPos + xBlend, yPos + GRAPH_HEIGHT - 1);
  GUI_SetAlpha(0);
  //
  // Draw orange frame
  //
  GUI_SetColor(0x0094f3);
  GUI_DrawRoundedRect(xPos, yPos, xPos + GRAPH_WIDTH, yPos + GRAPH_HEIGHT, 4);
  //
  // Label
  //
  _DrawLabel(hMem, xPos, yPos);
  GUI_MEMDEV_CopyToLCD(hMem);
  GUI_MEMDEV_Select(hMemOld);
}

/*********************************************************************
*
*       _DemoBarGraph
*/
static void _DemoBarGraph(void) {
  GUI_MEMDEV_Handle hMem;
  int xSize, ySize, xPosGraph, yPosGraph, yPosText, i;
  int ayOrg[] = {10, 20, 40, 50, 90, 100, 80, 30, 20, 10};
  int ayCur[] = {10, 20, 40, 50, 90, 100, 80, 30, 20, 10};
  int aAdd[GUI_COUNTOF(ayOrg)];
  int AddBlend = 1;
  int Blend = 64;
  int NumItems = GUI_COUNTOF(ayOrg);
  int TimeStart, TimeDiff, TimeStep;
  int NextState;

  //
  // Calculate positions
  //
  xSize     = LCD_GetXSize();
  ySize     = LCD_GetYSize();
  xPosGraph = (xSize - GRAPH_WIDTH)  >> 1;
  yPosGraph = (ySize - GRAPH_HEIGHT) >> 1;
  //
  // Label demo
  //
//  GUI_SetFont(&GUI_FontRounded22);
//  yPosText = LOGO_DIST_BORDER + ((bmSTLogo70x35.YSize - GUI_GetFontSizeY()) >> 1);
//  GUI_DispStringHCenterAt("Bargraph demo", (xSize + bmSTLogo70x35.XSize) >> 1, yPosText);
  //
  // Initialize values; Create MEMDEV
  //
  hMem = GUI_MEMDEV_Create(xPosGraph, yPosGraph, GRAPH_WIDTH + 7 + 1, GRAPH_HEIGHT + 1);
  if (hMem == 0) {
    return;
  }
  for (i = 0; i < NumItems; i++) {
    aAdd[i] = (i & 1) * 2 - 1;
  }
  _DrawLabel(0, xPosGraph, yPosGraph);
  TimeStart = GUIDEMO_GetTime();
  do {
    TimeDiff = GUIDEMO_GetTime() - TimeStart;
    //
    // Draw diagram
    //
    _DrawDiagramAt(hMem, xPosGraph, yPosGraph, ayCur, Blend);
    //
    // Change blending area
    //
    Blend += AddBlend;
    if ((Blend >= 164) || (Blend <= 8)) {
      AddBlend = -AddBlend;
    }
    //
    // Change values
    //
    for (i = 0; i < NumItems; i++) {
      *(ayCur + i) += *(aAdd + i);
      if ((*(ayCur + i) > (*(ayOrg + i) + 10)) || (*(ayCur + i) < (*(ayOrg + i) - 10))) {
        *(aAdd + i) = -*(aAdd + i);
      }
    }
    TimeStep  = GUIDEMO_GetTime() - TimeStart;
    if ((TimeStep - TimeDiff) < TIME_STEP) {
      GUI_Delay(TIME_STEP - (TimeStep - TimeDiff));
    } else {
      GUI_Exec();
    }
    NextState = GUIDEMO_CheckCancel();
  } while ((TimeDiff < SHOW_TIME) && (NextState == 0));
  GUI_MEMDEV_Delete(hMem);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDEMO_BarGraph
*/
void GUIDEMO_BarGraph(void) {
  GUIDEMO_DrawBk(1);
#if GUI_SUPPORT_CURSOR
  GUI_CURSOR_Hide();
#endif
  _DemoBarGraph();
#if GUI_SUPPORT_CURSOR
  GUI_CURSOR_Show();
#endif
}

#else

void GUIDEMO_BarGraph(void) {}

#endif

/*************************** End of file ****************************/

