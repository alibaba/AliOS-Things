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
File        : GUIDEMO.c
Purpose     : Several GUIDEMO routines
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO.c
  * @author  MCD Application Team
  * @brief   Several GUIDEMO routines
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

#include <stdio.h>
#include "GUIDEMO.h"
/*********************************************************************
*
*       Dialog resources
*
**********************************************************************
*/

/*********************************************************************
*
*       Static variables
*
**********************************************************************
*/
static GUIDEMO_CONFIG _GUIDemoConfig;
static WM_HWIN        _hDialogControl;
static WM_HWIN        _hDialogInfo;

static void (* _pfDrawBk)(int DrawLogo);
static int     _iDemo;
static int     _iDemoMinor;
static int     _HaltTime;
static int     _HaltTimeStart;
static int     _Halt;
int             _Next;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _ClearHalt
*
*   This function is called if the next button is pressed after
*   the demo was halted
*/
static void _ClearHalt(void) {
  _Halt          = 0;
  _HaltTime      = 0;
  _HaltTimeStart = 0;
}

/*********************************************************************
*
*       _DrawBkSimple
*/
static void _DrawBkSimple(int DrawLogo) {
  GUI_SetBkColor(BK_COLOR_1);
  GUI_Clear();
  if (DrawLogo) {
//    GUI_DrawBitmap(&bmSTLogo70x35, 5, 5);
  }
}


/*********************************************************************
*
*       _HideProgress
*/
static void _HideProgress(void) {
  PROGBAR_Handle hProg;

  hProg = WM_GetDialogItem(_hDialogControl, GUI_ID_PROGBAR0);
  WM_HideWindow(hProg);
}

/*********************************************************************
*
*       _ShowProgress
*/
static void _ShowProgress(void) {
  PROGBAR_Handle hProg;

  hProg = WM_GetDialogItem(_hDialogControl, GUI_ID_PROGBAR0);
  WM_ShowWindow(hProg);
}

/*********************************************************************
*
*       _Main
*/
/*********************************************************************
*
*       GUIDEMO_SetDrawLogo
*/
void GUIDEMO_AddIntToString(char * pText, unsigned int Number) {
  int TextLen;
  int LenNum;
  int i;

  TextLen = 0;
  while (*(pText + TextLen)) {
    TextLen++;
  }
  i       = 1;
  LenNum  = 1;
  while ((Number / i) >= 10) {
    i *= 10;
    LenNum++;
  }
  *(pText + TextLen + LenNum) = '\0';
  while (LenNum) {
    *(pText + TextLen + LenNum - 1) = '0' + Number % 10;
    Number /= 10;
    LenNum--;
  }
}

/*********************************************************************
*
*       GUIDEMO_AddStringToString
*/
void GUIDEMO_AddStringToString(char * pText, const char * acAdd) {
  int i;
  int j;

  i = 0;
  j = 0;
  while (*(pText + i)) {
    i++;
  }
  while (*(acAdd + j)) {
    *(pText + i) = *(acAdd + j);
    i++;
    j++;
  }
  *(pText + i) = '\0';
}

/*********************************************************************
*
*       GUIDEMO_CheckCancel
*/
int GUIDEMO_CheckCancel(void) {
  //
  // Do not return until the button is released
  //
  while (_Halt == 1) {
    GUI_Delay(10);
  }
  //
  // Check Next Button
  //
  if (_Next == 1) {
    _Next = 0;
    return 1;
  }
  return 0;
}

/*********************************************************************
*
*       GUIDEMO_ClearText
*
*/
void GUIDEMO_ClearText(char * pText) {
  *pText = 0;
}

/*********************************************************************
*
*       GUIDEMO_Delay
*
*   This function has to be called if the current step of the sample
*   is the last one and consists of a single frame
*/
void GUIDEMO_Delay(int TimeDelay) {
  PROGBAR_Handle hProg;
  int            NextState;
  U32            TimeStart;
  U32            TimeDiff;

  hProg = WM_GetDialogItem(_hDialogControl, GUI_ID_PROGBAR0);
  if (TimeDelay > SHOW_PROGBAR_AT) {
    _ShowProgress();
  }
  PROGBAR_SetValue(hProg, 0);
  PROGBAR_SetMinMax(hProg, 0, TimeDelay);
  TimeStart     = GUI_GetTime();
  do {
    
     
    TimeDiff = GUIDEMO_GetTime() - TimeStart;
    if (TimeDelay > SHOW_PROGBAR_AT) {
      PROGBAR_SetValue(hProg, TimeDiff);
    }
    GUI_Delay(5);
    NextState = GUIDEMO_CheckCancel();
  } while (TimeDiff < (U32)TimeDelay && !NextState);
  if (TimeDelay > SHOW_PROGBAR_AT) {
    _HideProgress();
  }
  GUI_Exec();
}

/*********************************************************************
*
*       GUIDEMO_DrawBk
*/
void GUIDEMO_DrawBk(int DrawLogo) {
  _pfDrawBk(DrawLogo);
}

/*********************************************************************
*
*       GUIDEMO_GetConfFlag
*/
U16 GUIDEMO_GetConfFlag(U16 Flag) {
  return (_GUIDemoConfig.Flags & Flag) ? 1 : 0;
}

/*********************************************************************
*
*       GUIDEMO_GetTime
*/
int GUIDEMO_GetTime(void) {
  return _Halt ? _HaltTimeStart : GUI_GetTime() - _HaltTime;
}

/*********************************************************************
*
*       GUIDEMO_HideControlWin
*/
void GUIDEMO_HideControlWin(void) {
  WM_HideWindow(_hDialogControl);
  WM_ValidateWindow(WM_HBKWIN);
}

/*********************************************************************
*
*       GUIDEMO_HideInfoWin
*/
void GUIDEMO_HideInfoWin(void) {
  WM_HideWindow(_hDialogInfo);
  WM_ValidateWindow(WM_HBKWIN);
}

/*********************************************************************
*
*       GUIDEMO_NotifyStartNext
*
*   Use this function if the next step of the current sample will be
*   shown immediately
*/
void GUIDEMO_NotifyStartNext(void) {
  _iDemoMinor++;
  _ClearHalt();
  GUIDEMO_UpdateControlText();
}

/*********************************************************************
*
*       GUIDEMO_ShowControlWin
*/
void GUIDEMO_ShowControlWin(void) {
  WM_ShowWindow(_hDialogControl);
  GUI_Exec();
}

/*********************************************************************
*
*       GUIDEMO_ShowInfo
*/
void GUIDEMO_ShowInfo(const char * acInfo) {
  TEXT_Handle hText;

  if (WM_IsVisible(_hDialogInfo)) {
    hText = WM_GetDialogItem(_hDialogInfo, GUI_ID_TEXT0);
    TEXT_SetWrapMode(hText, GUI_WRAPMODE_WORD);    
    TEXT_SetText(hText, acInfo);
  }
}

/*********************************************************************
*
*       GUIDEMO_ShowInfoWin
*/
void GUIDEMO_ShowInfoWin(void) {
  WM_ShowWindow(_hDialogInfo);
}

/*********************************************************************
*
*       GUIDEMO_UpdateControlText
*/
void GUIDEMO_UpdateControlText(void) {
  TEXT_Handle hText;
  char        acText[20] = { 0 };

  hText = WM_GetDialogItem(_hDialogControl, GUI_ID_TEXT0);
  GUIDEMO_AddStringToString(acText, "Demo ");
  GUIDEMO_AddIntToString   (acText, _iDemo + 1);
  GUIDEMO_AddStringToString(acText, ".");
  GUIDEMO_AddIntToString   (acText, _iDemoMinor);
  GUIDEMO_AddStringToString(acText, "/");
  GUIDEMO_AddIntToString   (acText, _GUIDemoConfig.NumDemos - 1);
  TEXT_SetText             (hText,  acText);
}

/*********************************************************************
*
*       GUIDEMO_Wait
*
*   This function has to be called if the current step is a static
*   frame and another step will follow
*/
void GUIDEMO_Wait(int TimeWait) {
  GUIDEMO_Delay(TimeWait);
  GUIDEMO_NotifyStartNext();
}

/*********************************************************************
*
*       GUIDEMO_Main
*/

static void _Main(void) {
  WM_SelectWindow(WM_HBKWIN);
  GUI_Clear();

  WM_InvalidateWindow(_hDialogControl);
  WM_DisableMemdev(WM_HBKWIN);
  GUI_Exec();
  WM_EnableMemdev(WM_HBKWIN);
	
  GUIDEMO_Intro();
  GUIDEMO_TransparentDialog();
  GUIDEMO_Graph();	
  GUIDEMO_ColorBar();
			
  _iDemo = 0;

  WM_DeleteWindow(_hDialogControl);
}

void GUIDEMO_Main(void) {
  _pfDrawBk = _DrawBkSimple;

  while (1) {
		 printf("hello world! \n");
    _Main();
  }
}

/*************************** End of file ****************************/

