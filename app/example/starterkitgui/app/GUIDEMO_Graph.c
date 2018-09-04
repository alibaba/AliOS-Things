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
File        : GUIDEMO_Graph.c
Purpose     : Several GUIDEMO routines
----------------------------------------------------------------------
*/

/**
  ******************************************************************************
  * @file    GUIDEMO_Graph.c
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


#include "GUIDEMO.h"

#if (SHOW_GUIDEMO_GRAPH && GUI_SUPPORT_MEMDEV)

/*********************************************************************
*
*       Configuration defines
*
**********************************************************************
*/
#define MAX_GRAPH_SIZE_X 440
#define MAX_GRAPH_SIZE_Y (MAX_GRAPH_SIZE_X * 11) / 20

#define DIST_TO_BORDER   10
#define DIST_TO_WIN      5

#define BORDER_TOP       0
#define BORDER_BOTTOM    9
#define BORDER_LEFT      19
#define BORDER_RIGHT     0

#define COLOR_BK         GUI_DARKGRAY
#define COLOR_BORDER     BK_COLOR_1
#define COLOR_FRAME      GUI_BLACK
#define COLOR_GRID       GUI_GRAY

#define SCALE_H_HEIGHT   4

#define TICK_DIST_H      25
#define TICK_DIST_V      20

#define TIME_RUN         5000
#define TIME_STEP        15

#define HEARTBEAT_TIME   1000

#define MAX_NUM_DATA_OBJ 5

#define GRAPH_DIV        9   // (2^9 = 512) If this value is changed _aWaves[] need to be changed too!
#define GRID_DIST_X      25
#define GRID_DIST_Y      10
#define GRID_OFF_Y       25

/*********************************************************************
*
*       Typedef / Data
*
**********************************************************************
*/
typedef struct {
  char  * Name;
  int     ScaleVOff;
  int     DataVOff;
  int     GridVOff;
  void (* pfAddData)(GRAPH_DATA_Handle hData, int DataID);
  int     NumWaves;
} GRAPH_WAVE;

static int _HeartBeat[] = {
    2,   4,   6,   8,  10,   6,   2,   0,   0,   0,
   -8,  16,  40,  64,  88,  58,  28,  -2, -32, -30,
  -20, -10,   0,   2,   2,   4,   4,   6,   6,   8,
    8,  10,  12,  14,  16,  18,  20,  16,  12,   8,
    4,   2,   2,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static GUI_COLOR _aColorData[MAX_NUM_DATA_OBJ] = {
  0x50C0FF,
  0xFFC050,
  0x50FFC0,
  0x800000,
  0x000080
};

GRAPH_SCALE_Handle _hScaleH, _hScaleV;
static int         _DataAdjust;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _AddData_Sine
*/
static void _AddData_Sine(GRAPH_DATA_Handle hData, int DataID) {
  static int x1000[MAX_NUM_DATA_OBJ];
  I32        SinHQ;
  int        Multi, Step;

  switch (DataID) {
  case 0:
    Multi = 70;
    Step  = 3;
    break;
  case 1:
    Multi = 50;
    Step  = 1;
    break;
  case 2:
    Multi = 30;
    Step  = 7;
    break;
  default:
    return;
  }
  SinHQ          = GUI__SinHQ(x1000[DataID]);
  x1000[DataID] += 1000 * Step;
  GRAPH_DATA_YT_AddValue(hData, ((SinHQ * Multi) >> 16) + _DataAdjust);
}

/*********************************************************************
*
*       _AddData_Heartbeat
*/
static void _AddData_Heartbeat(GRAPH_DATA_Handle hData, int DataID) {
  static int Index;

  GUI_USE_PARA(DataID);
  GRAPH_DATA_YT_AddValue(hData, (_HeartBeat[Index]) + _DataAdjust);
  if (++Index == GUI_COUNTOF(_HeartBeat)) {
    Index = 0;
  }
}

/*********************************************************************
*
*       DATA _aWave - Keep below _AddData-functions
*/
GRAPH_WAVE _aWave[] = {
  { "Heartbeat",        // Name
    157,                // Vertical scale offset in relation to GRAPH_DIV
    152,                // Vertical data  offset in relation to GRAPH_DIV
    21,                 // Vertical grid  offset in relation to GRAPH_DIV
    _AddData_Heartbeat, // Pointer to specific AddData function
    1                   // Number of waves
  },
  { "Sine Waves",
    265,
    253,
    23,
    _AddData_Sine,
    3
  },
  0
};

/*********************************************************************
*
*       _ShowGraph
*/
static void _ShowGraph(GRAPH_Handle hGraph, GRAPH_DATA_Handle hData[], int DataCount, void (* pfAddData)(GRAPH_DATA_Handle hData, int DataID)) {
  int Count, Data_xSize, xSize;
  int TimeStart, TimeDiff, TimeStep;
  int NextState, i, Flag;

  xSize      = LCD_GetXSize();
  Data_xSize = xSize - (DIST_TO_BORDER << 1) - (BORDER_LEFT + BORDER_RIGHT);
  Count      = 0;
  //
  // Attach data objects
  //
  for (i = 0; i < DataCount; i++) {
    GRAPH_AttachData(hGraph, hData[i]);
  }
  //
  // Add values before GRAPH is displayed
  //
  while (Count < Data_xSize) {
    for (i = 0; i < DataCount; i++) {
      pfAddData(hData[i], i);
    }
    Count++;
  }
  //
  // Add values depending on time
  //

  TimeStart = GUIDEMO_GetTime();
  Flag = 1;
  do {
    TimeDiff = GUIDEMO_GetTime() - TimeStart;
    for (i = 0; i < DataCount; i++) {
      pfAddData(hData[i], i);
    }
    if (Flag) {
      Flag = 0;
      GUI_Exec();
      GRAPH_DetachScale(hGraph, _hScaleH);
      GRAPH_DetachScale(hGraph, _hScaleV);
      WM_ValidateWindow(hGraph);
    }
    NextState = GUIDEMO_CheckCancel();
    TimeStep  = GUIDEMO_GetTime() - TimeStart;
    if ((TimeStep - TimeDiff) < TIME_STEP) {
      GUI_Delay(TIME_STEP - (TimeStep - TimeDiff));
    }
  } while ((TimeDiff < TIME_RUN) && (NextState == 0));
  for (i = 0; i < DataCount; i++) {
    GRAPH_DetachData(hGraph, hData[i]);
  }
  GUIDEMO_NotifyStartNext();
}

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUIDEMO_DrawBk(1);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _GraphDemo
*/
static void _GraphDemo(void) {
  const WIDGET_EFFECT * pEffectOld;
  GRAPH_Handle          hGraph;
  GRAPH_DATA_Handle     hData[MAX_NUM_DATA_OBJ];
  int                   xSize, ySize, i;
  int                   Graph_xSize, Graph_ySize, Data_ySize;
  int                   Graph_xPos, Graph_yPos;

  xSize      = LCD_GetXSize();
  ySize      = LCD_GetYSize();
  pEffectOld = WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple);
  //
  // Set Callback function for background window
  //
  WM_SetCallback(WM_HBKWIN, _cbBk);
  //
  // Determine size of GRAPH
  //
  Graph_xSize = xSize - (DIST_TO_BORDER << 1);            // xsize = Screen size subtracting twice the distance to the border of the screen
  Graph_ySize = ySize - INFO_SIZE_Y - (DIST_TO_WIN << 1); // ysize = Screen size subtracting the window sizes and twice the distance to the windows
  if (Graph_ySize > MAX_GRAPH_SIZE_Y) {
    Graph_ySize = MAX_GRAPH_SIZE_Y;
    if (Graph_xSize > (Graph_ySize * 20) / 11) {
      Graph_xSize = (Graph_ySize * 20) / 11;
    }
  }
  //
  // Create and configure GRAPH_WIDGET
  //
  Graph_xPos = (xSize - Graph_xSize) >> 1;
  Graph_yPos = (ySize - Graph_ySize) >> 1;
  if (Graph_yPos < INFO_SIZE_Y + DIST_TO_WIN) {
    Graph_yPos = INFO_SIZE_Y + DIST_TO_WIN;
  }
  hGraph = GRAPH_CreateEx(Graph_xPos, Graph_yPos, Graph_xSize, Graph_ySize, WM_HBKWIN, WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, 0);
  GRAPH_SetBorder(hGraph, BORDER_LEFT, BORDER_TOP, BORDER_RIGHT, BORDER_BOTTOM);
  WM_SetHasTrans (hGraph);
  GRAPH_SetColor (hGraph, COLOR_BK,     GRAPH_CI_BK);
  GRAPH_SetColor (hGraph, COLOR_BORDER, GRAPH_CI_BORDER);
  GRAPH_SetColor (hGraph, COLOR_FRAME,  GRAPH_CI_FRAME);
  GRAPH_SetColor (hGraph, COLOR_GRID,   GRAPH_CI_GRID);
  //
  // Adjust grid
  //
  GRAPH_SetGridVis  (hGraph, 1);
  GRAPH_SetGridDistX(hGraph, GRID_DIST_X);
  GRAPH_SetGridDistY(hGraph, GRID_DIST_Y);
  WM_BringToBottom  (hGraph);
  //
  // Create and configure GRAPH_DATA_YT object
  //
  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
    hData[i] = GRAPH_DATA_YT_Create(_aColorData[i], xSize - (DIST_TO_BORDER << 1) - BORDER_LEFT, 0, 0);
  }
  Data_ySize = Graph_ySize - BORDER_BOTTOM;
  //
  // Create and configure GRAPH_SCALE objects
  //
  _hScaleH = GRAPH_SCALE_Create(BORDER_BOTTOM >> 1, GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, TICK_DIST_H);
  _hScaleV = GRAPH_SCALE_Create(BORDER_LEFT   >> 1, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL,   TICK_DIST_V);
  GRAPH_SCALE_SetPos(_hScaleH, Graph_ySize - SCALE_H_HEIGHT);
  GRAPH_SCALE_SetOff(_hScaleH, -5);
  //
  // Show some graphs
  //
  i = 0;
  while (_aWave[i].pfAddData != 0) {
    GUIDEMO_ShowInfo(_aWave[i].Name);
    GRAPH_AttachScale(hGraph, _hScaleH);
    GRAPH_AttachScale(hGraph, _hScaleV);
    _DataAdjust = (Data_ySize * _aWave[i].DataVOff) >> GRAPH_DIV;
    GRAPH_SetGridOffY (hGraph, (Data_ySize * _aWave[i].GridVOff) >> GRAPH_DIV);
    GRAPH_SCALE_SetOff(_hScaleV, (((Data_ySize - BORDER_BOTTOM) * _aWave[i].ScaleVOff) >> GRAPH_DIV));
    _ShowGraph(hGraph, hData, _aWave[i].NumWaves, _aWave[i].pfAddData);
    i++;
  }
  //
  // Clean up
  //
  GRAPH_DetachScale(hGraph, _hScaleH);
  GRAPH_DetachScale(hGraph, _hScaleV);
  GRAPH_SCALE_Delete(_hScaleH);
  GRAPH_SCALE_Delete(_hScaleV);
  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
    GRAPH_DATA_YT_Delete(hData[i]);
  }
  WM_DeleteWindow(hGraph);
  WIDGET_SetDefaultEffect(pEffectOld);
}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
void GUIDEMO_Graph(void) {
  GUIDEMO_ShowInfoWin();
  _GraphDemo();
  GUIDEMO_NotifyStartNext();
}

#else

void GUIDEMO_Graph(void) {}

#endif

/*************************** End of file ****************************/
