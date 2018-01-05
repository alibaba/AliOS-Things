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
File        : GUIDEMO.h
Purpose     : Configuration file of GUIDemo
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO.c
  * @author  MCD Application Team
  * @brief   Configuration file of GUIDemo
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


#ifndef GUIDEMO_H
#define GUIDEMO_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "GUI.h"

#if GUI_WINSUPPORT
  #include "WM.h"

//  #include "CHECKBOX.h"
  #include "FRAMEWIN.h"
  #include "PROGBAR.h"
  #include "TEXT.h"
//  #include "BUTTON.h"
  #include "SLIDER.h"
//  #include "HEADER.h"
  #include "GRAPH.h"
//  #include "ICONVIEW.h"
//  #include "LISTVIEW.h"
//  #include "TREEVIEW.h"
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define CONTROL_SIZE_X    80
#define CONTROL_SIZE_Y    48
#define INFO_SIZE_Y       65
#define BUTTON_SIZE_X     32
#define BUTTON_SIZE_Y     20
#define PROGBAR_SIZE_X    66
#define PROGBAR_SIZE_Y    12
#define TEXT_SIZE_X       69
#define TEXT_SIZE_Y       7
#define SHOW_PROGBAR_AT   100
#define GUI_ID_HALT       (GUI_ID_USER + 0)
#define GUI_ID_NEXT       (GUI_ID_USER + 1)

#define BK_COLOR_0        0xFF5555
#define BK_COLOR_1        0x880000

#define NUMBYTES_NEEDED   0x200000

#define CIRCLE_RADIUS     100

#define LOGO_DIST_BORDER  5

#define CHAR_READING_TIME 80

/*********************************************************************
*
*       Configuration of modules to be used
*
**********************************************************************
*/

#ifndef   SHOW_GUIDEMO_BITMAP
  #define SHOW_GUIDEMO_BITMAP            (1)
#endif

#ifndef   SHOW_GUIDEMO_COLORBAR
  #define SHOW_GUIDEMO_COLORBAR          (1)
#endif

#ifndef   SHOW_GUIDEMO_CURSOR
  #define SHOW_GUIDEMO_CURSOR            (1)
#endif

#ifndef   SHOW_GUIDEMO_GRAPH
  #define SHOW_GUIDEMO_GRAPH             (1)
#endif

#ifndef   SHOW_GUIDEMO_LISTVIEW
  #define SHOW_GUIDEMO_LISTVIEW          (1)
#endif

#ifndef   SHOW_GUIDEMO_SPEED
  #define SHOW_GUIDEMO_SPEED             (1)
#endif

#ifndef   SHOW_GUIDEMO_TREEVIEW
  #define SHOW_GUIDEMO_TREEVIEW          (1)
#endif

#ifndef   SHOW_GUIDEMO_ICONVIEW
  #define SHOW_GUIDEMO_ICONVIEW          (1)
#endif

#ifndef   SHOW_GUIDEMO_RADIALMENU
  #define SHOW_GUIDEMO_RADIALMENU        (0)
#endif

#ifndef   SHOW_GUIDEMO_VSCREEN
  #define SHOW_GUIDEMO_VSCREEN           (0)
#endif

#ifndef   SHOW_GUIDEMO_AUTOMOTIVE
  #define SHOW_GUIDEMO_AUTOMOTIVE        (1)
#endif

#ifndef   SHOW_GUIDEMO_TRANSPARENTDIALOG
  #define SHOW_GUIDEMO_TRANSPARENTDIALOG (1)
#endif

#ifndef   SHOW_GUIDEMO_AATEXT
  #define SHOW_GUIDEMO_AATEXT            (0)
#endif

#ifndef   SHOW_GUIDEMO_BARGRAPH
  #define SHOW_GUIDEMO_BARGRAPH          (1)
#endif

#ifndef   SHOW_GUIDEMO_FADING
  #define SHOW_GUIDEMO_FADING            (0)
#endif

#ifndef   SHOW_GUIDEMO_SKINNING
  #define SHOW_GUIDEMO_SKINNING          (0)
#endif

#ifndef   SHOW_GUIDEMO_SPEEDOMETER
  #define SHOW_GUIDEMO_SPEEDOMETER       (0)
#endif

#ifndef   SHOW_GUIDEMO_IMAGEFLOW
  #define SHOW_GUIDEMO_IMAGEFLOW         (0)
#endif

/*********************************************************************
*
*       Configuration macros
*
**********************************************************************
*/
#ifndef   GUIDEMO_SHOW_SPRITES
  #define GUIDEMO_SHOW_SPRITES    (1)
#endif
#ifndef   GUIDEMO_USE_VNC
  #define GUIDEMO_USE_VNC         (0)
#endif
#ifndef   GUIDEMO_USE_AUTO_BK
  #define GUIDEMO_USE_AUTO_BK     (1)
#endif

#define GUIDEMO_CF_SHOW_SPRITES (GUIDEMO_SHOW_SPRITES <<  0)
#define GUIDEMO_CF_USE_VNC      (GUIDEMO_USE_VNC      <<  1)
#define GUIDEMO_CF_USE_AUTO_BK  (GUIDEMO_USE_AUTO_BK  <<  2)

/*********************************************************************
*
*       GUIDEMO_CONFIG
*/
typedef struct GUIDEMO_CONFIG {
  void (* * apFunc)(void);
  int       NumDemos;
  U16       Flags;
  #if GUIDEMO_USE_VNC
    int  (* pGUI_VNC_X_StartServer)(int LayerIndex, int ServerIndex);
  #endif
} GUIDEMO_CONFIG;

/*********************************************************************
*
*       Internal functions
*
**********************************************************************
*/
void GUIDEMO_AddIntToString   (char * acText, unsigned int Number);
void GUIDEMO_AddStringToString(char * acText, const char * acAdd);
int  GUIDEMO_CheckCancel      (void);
void GUIDEMO_ClearText        (char * acText);
void GUIDEMO_Config           (GUIDEMO_CONFIG * pConfig);
void GUIDEMO_Delay            (int t);
void GUIDEMO_DrawBk           (int DrawLogo);
U16  GUIDEMO_GetConfFlag      (U16 Flag);
int  GUIDEMO_GetTime          (void);
void GUIDEMO_HideControlWin   (void);
void GUIDEMO_HideInfoWin      (void);
void GUIDEMO_NotifyStartNext  (void);
void GUIDEMO_SetDrawLogo      (U8 OnOff);
void GUIDEMO_ShowControlWin   (void);
void GUIDEMO_ShowInfo         (const char * acInfo);
void GUIDEMO_ShowInfoWin      (void);
void GUIDEMO_ShowIntro        (const char * acText, const char * acDescription);
void GUIDEMO_UpdateControlText(void);
void GUIDEMO_Wait             (int TimeWait);
void GUIDEMO_Main             (void);

/*********************************************************************
*
*       Demo modules
*
**********************************************************************
*/
void GUIDEMO_AntialiasedText  (void);
void GUIDEMO_Automotive       (void);
void GUIDEMO_BarGraph         (void);
void GUIDEMO_Bitmap           (void);
void GUIDEMO_ColorBar         (void);
void GUIDEMO_Cursor           (void);
void GUIDEMO_Fading           (void);
void GUIDEMO_Graph            (void);
void GUIDEMO_IconView         (void);
void GUIDEMO_ImageFlow        (void);
void GUIDEMO_Intro            (void);
void GUIDEMO_Intro1           (void);
void GUIDEMO_Listview         (void);
void GUIDEMO_RadialMenu       (void);
void GUIDEMO_Skinning         (void);
void GUIDEMO_Speed            (void);
void GUIDEMO_Speedometer      (void);
void GUIDEMO_TransparentDialog(void);
void GUIDEMO_Treeview         (void);
void GUIDEMO_VScreen          (void);
void GUIDEMO_WashingMachine   (void);
void GUIDEMO_ZoomAndRotate    (void);

/*********************************************************************
*
*       Externs
*
**********************************************************************
*/
//extern GUI_CONST_STORAGE GUI_BITMAP bmSTLogo;
//extern GUI_CONST_STORAGE GUI_BITMAP bmSeggerLogo;
//extern GUI_CONST_STORAGE GUI_BITMAP bmSeggerLogo70x35;
extern GUI_CONST_STORAGE GUI_BITMAP bmSTLogo70x35;

extern GUI_CONST_STORAGE GUI_FONT   GUI_FontRounded16;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontRounded22;
//extern GUI_CONST_STORAGE GUI_FONT   GUI_FontSouvenir18;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontD6x8;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontAA2_32;
extern GUI_CONST_STORAGE GUI_FONT   GUI_FontAA4_32;


#if defined(__cplusplus)
  }
#endif

#endif // avoid multiple inclusion

/*************************** End of file ****************************/
