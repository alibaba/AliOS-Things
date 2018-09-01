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
File        : GUIDEMO_Conf.c
Purpose     : Runtime configurability of the GUIDEMO
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO_Conf.c
  * @author  MCD Application Team
  * @brief   Runtime configurability of the GUIDEMO
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


/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
#include "GUI.h"
#include "GUIDEMO.h"
#if GUIDEMO_USE_VNC
  #include "GUI_VNC.h"
#endif

/*********************************************************************
*
*       Routine table
*/
static void (* _apfTest[])(void) = {
#if (SHOW_GUIDEMO_SPEEDOMETER && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_Speedometer,
#endif
#if (SHOW_GUIDEMO_ZOOMANDROTATE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_ZoomAndRotate,
#endif
#if (SHOW_GUIDEMO_RADIALMENU && GUI_WINSUPPORT)
//  GUIDEMO_RadialMenu,
#endif
#if (SHOW_GUIDEMO_SKINNING && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_Skinning,
#endif
#if (SHOW_GUIDEMO_BARGRAPH && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_BarGraph,
#endif
#if (SHOW_GUIDEMO_FADING && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_Fading,
#endif
#if SHOW_GUIDEMO_AATEXT
//  GUIDEMO_AntialiasedText,
#endif
#if (SHOW_GUIDEMO_TRANSPARENTDIALOG && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_TransparentDialog,
#endif
#if (SHOW_GUIDEMO_WASHINGMACHINE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_WashingMachine,
#endif
#if (SHOW_GUIDEMO_ICONVIEW && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_IconView,
#endif
#if (SHOW_GUIDEMO_IMAGEFLOW && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_ImageFlow,
#endif
#if (SHOW_GUIDEMO_TREEVIEW && GUI_WINSUPPORT)
//  GUIDEMO_Treeview,
#endif
#if (SHOW_GUIDEMO_LISTVIEW && GUI_WINSUPPORT)
//  GUIDEMO_Listview,
#endif
#if SHOW_GUIDEMO_VSCREEN
//  GUIDEMO_VScreen,
#endif
#if (SHOW_GUIDEMO_GRAPH && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_Graph,
#endif
#if SHOW_GUIDEMO_SPEED
//  GUIDEMO_Speed,
#endif
#if SHOW_GUIDEMO_BITMAP
//  GUIDEMO_Bitmap,
#endif
#if (SHOW_GUIDEMO_CURSOR && GUI_SUPPORT_CURSOR && GUI_SUPPORT_TOUCH)
//  GUIDEMO_Cursor,
#endif
#if SHOW_GUIDEMO_COLORBAR
//  GUIDEMO_ColorBar,
#endif
#if (SHOW_GUIDEMO_AUTOMOTIVE && GUI_SUPPORT_MEMDEV)
//  GUIDEMO_Automotive,
#endif
  0
};

/*********************************************************************
*
*       GUIDEMO_Config
*/
void GUIDEMO_Config(GUIDEMO_CONFIG * pConfig) {
  pConfig->apFunc   = _apfTest;
  pConfig->NumDemos = GUI_COUNTOF(_apfTest);
  pConfig->Flags    = GUIDEMO_CF_SHOW_SPRITES | GUIDEMO_CF_USE_VNC | GUIDEMO_CF_USE_AUTO_BK;
  #if GUIDEMO_USE_VNC
    pConfig->pGUI_VNC_X_StartServer = GUI_VNC_X_StartServer;
  #endif
}

/*************************** End of file ****************************/
