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
File        : GUI_VNC.h
Purpose     : Publics for the VNC server
---------------------------END-OF-HEADER------------------------------

Attention : Do not modify this file ! If you do, you will not
            be able do update to a later GUI version !

*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
#ifndef  GUI_VNC_H
#define  GUI_VNC_H

#include "GUI_Private.h"
#include "GUI_Type.h"
#include "IP_FS.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define GUI_VNC_NO_ERROR            0
#define GUI_VNC_ERROR_MISC          1
#define GUI_VNC_ERROR_WRONGFORMAT   2

#define GUI_DES_ENCRYPT 0
#define GUI_DES_DECRYPT 1

//
// File transfer
//
#define GUI_VNC_SUPPORT_FILETRANSFER 0x10

//
// Extension message (used for file transfer)
//
#define RFB_EXTENSION 0xFCul

//
// File transfer protocol
//
#define RFBX_FILE_LIST_REQUEST      (0x000102ul | (RFB_EXTENSION << 24))
#define RFBX_FILE_LIST_REPLY        (0x000103ul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_START_REQUEST   (0x000106ul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_START_REPLY     (0x000107ul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_DATA_REQUEST    (0x000108ul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_DATA_REPLY      (0x000109ul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_END_REQUEST     (0x00010Aul | (RFB_EXTENSION << 24))
#define RFBX_UPLOAD_END_REPLY       (0x00010Bul | (RFB_EXTENSION << 24))

#define RFBX_DOWNLOAD_START_REQUEST (0x00010Cul | (RFB_EXTENSION << 24))
#define RFBX_DOWNLOAD_START_REPLY   (0x00010Dul | (RFB_EXTENSION << 24))
#define RFBX_DOWNLOAD_DATA_REQUEST  (0x00010Eul | (RFB_EXTENSION << 24))
#define RFBX_DOWNLOAD_DATA_REPLY    (0x00010Ful | (RFB_EXTENSION << 24))
#define RFBX_DOWNLOAD_END_REPLY     (0x000110ul | (RFB_EXTENSION << 24))
#define RFBX_MKDIR_REQUEST          (0x000111ul | (RFB_EXTENSION << 24))
#define RFBX_MKDIR_REPLY            (0x000112ul | (RFB_EXTENSION << 24))
#define RFBX_REMOVE_REQUEST         (0x000113ul | (RFB_EXTENSION << 24))
#define RFBX_REMOVE_REPLY           (0x000114ul | (RFB_EXTENSION << 24))
#define RFBX_RENAME_REQUEST         (0x000115ul | (RFB_EXTENSION << 24))
#define RFBX_RENAME_REPLY           (0x000116ul | (RFB_EXTENSION << 24))

  /*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  U8 * pBuffer;
  int  BufferSize;
  int  NumBytesInBuffer;
} BUFFER_CB;

typedef struct GUI_VNC_CONTEXT {
  GUI_DEVICE * pDevice;
  struct GUI_VNC_CONTEXT * pNext;
  int LayerIndex;
  int BytesPerPixel;
  int BitsPerPixel;  // Note, that from within the VNC server the function LCD_GetBitsBerPixel() can not be used because the VNC server runs in a separate thread and the device chain can change during the function call
  //
  // Connection related data
  //
  GUI_tSend    pfSend;
  GUI_tRecv    pfReceive;
  void       * pConnectInfo;
  U16          ServerIndex;
  //
  // Display related info
  //
  int x0Dirty, y0Dirty, x1Dirty, y1Dirty;
  int xSize, ySize;
  int xOrg, yOrg, xOrgNew, yOrgNew;
  //
  // Status
  //
  char ClientSupportsHextile;
  char IsBigEndian;
  char OrgLock;
  char BkFlag;
  //
  // Pointer to buffer
  //
  U8 * pBuffer;
  unsigned SizeOfBuffer;
  int (* pfStoreU8)  (struct GUI_VNC_CONTEXT * pContext, BUFFER_CB * pBCB, U8 Data);
  int (* pfStoreU16) (struct GUI_VNC_CONTEXT * pContext, BUFFER_CB * pBCB, U16 Data);
  int (* pfStoreU32) (struct GUI_VNC_CONTEXT * pContext, BUFFER_CB * pBCB, U32 Data);
  int (* pfStoreData)(struct GUI_VNC_CONTEXT * pContext, BUFFER_CB * pBCB, const U8 * pData, int NumBytes);
  int (* pfFlush)    (struct GUI_VNC_CONTEXT * pContext, BUFFER_CB * pBCB);
  int (* pfRead)     (struct GUI_VNC_CONTEXT * pContext, U8 * pBuffer, int Len);
} GUI_VNC_CONTEXT;

typedef struct {
  void (* pfGetChallenge)(U8 * pChallenge);
  void (* pfGetResponse )(U8 * pResponse );
} GUI_VNC_AUTHENTICATION;

/*********************************************************************
*
*       Private Functions
*
**********************************************************************
*/
void GUI_VNC_SetDESKey(U8 * pKey, int Mode);
void GUI_VNC_DoDES    (U8 * pInblock, U8 * pOutblock);

/*********************************************************************
*
*       Public Functions
*
**********************************************************************
*/
void GUI_VNC_AttachToLayer      (GUI_VNC_CONTEXT * pContext, int LayerIndex);
void GUI_VNC_EnableKeyboardInput(int OnOff);
void GUI_VNC_EnableMouseInput   (int OnOff);
void GUI_VNC_EnableFileTransfer (unsigned OnOff);
int  GUI_VNC_GetNumConnections  (void);
int  GUI_VNC_Process            (GUI_VNC_CONTEXT * pContext, GUI_tSend pfSend, GUI_tRecv pfReceive, void * pConnectInfo);
void GUI_VNC_RingBell           (void);
void GUI_VNC_SetAuthentication  (GUI_VNC_AUTHENTICATION * pAuthentication);
void GUI_VNC_SetPassword        (U8 * sPassword);
void GUI_VNC_SetProgName        (const char * sProgName);
void GUI_VNC_SetSize            (unsigned xSize, unsigned ySize);
void GUI_VNC_SetLockFrame       (unsigned OnOff);
void GUI_VNC_SetRetryCount      (unsigned Cnt);
void GUI_VNC_SetFS_API          (const IP_FS_API * pFS_API);

//
// Private function for setting file transfer handler
//
void GUI_VNC__SetRFBExtensionHandler(int (* pFunc)(U32, GUI_VNC_CONTEXT *, BUFFER_CB *));

//
// External routine to link the server to the system ... USER defined !
//
int  GUI_VNC_X_StartServer  (int LayerIndex, int ServerIndex);
int  GUI_VNC_X_StartServerFT(int LayerIndex, int ServerIndex);
void GUI_VNC_X_getpeername  (U32 * Addr);

#if defined(__cplusplus)
  }
#endif

#endif   /* Avoid multiple inclusion */

/*************************** End of file ****************************/
