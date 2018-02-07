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
File        : IP_FS.h
Purpose     : File system abstraction layer
---------------------------END-OF-HEADER------------------------------

Attention : Do not modify this file !
*/

#ifndef  IP_FS_H
#define  IP_FS_H

#include "SEGGER.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Functions
*
**********************************************************************
*/

typedef struct {
  //
  // Read only file operations. These have to be present on ANY file system, even the simplest one.
  //
  void* (*pfOpenFile)             (const char* sFilename);
  int   (*pfCloseFile)            (void* hFile);
  int   (*pfReadAt)               (void* hFile, void* pBuffer, U32 Pos, U32 NumBytes);
  long  (*pfGetLen)               (void* hFile);
  //
  // Directory query operations.
  //
  void  (*pfForEachDirEntry)      (void* pContext, const char* sDir, void (*pf)(void* pContext, void* pFileEntry));
  void  (*pfGetDirEntryFileName)  (void* pFileEntry, char* sFileName, U32 SizeOfBuffer);
  U32   (*pfGetDirEntryFileSize)  (void* pFileEntry, U32* pFileSizeHigh);
  U32   (*pfGetDirEntryFileTime)  (void* pFileEntry);
  int   (*pfGetDirEntryAttributes)(void* pFileEntry);
  //
  // Write file operations.
  //
  void* (*pfCreate)               (const char* sFileName);
  void* (*pfDeleteFile)           (const char* sFilename);
  int   (*pfRenameFile)           (const char* sOldFilename, const char* sNewFilename);
  int   (*pfWriteAt)              (void* hFile, void* pBuffer, U32 Pos, U32 NumBytes);
  //
  // Additional directory operations
  //
  int   (*pfMKDir)                (const char* sDirName);
  int   (*pfRMDir)                (const char* sDirName);
  //
  // Additional operations
  //
  int   (*pfIsFolder)             (const char* sPath);
  int   (*pfMove)                 (const char* sOldFilename, const char* sNewFilename);
} IP_FS_API;

extern const IP_FS_API IP_FS_ReadOnly;              // Read-only file system, typically located in flash memory.
extern const IP_FS_API IP_FS_Win32;                 // File system interface for Win32.
extern const IP_FS_API IP_FS_Linux;                 // File system interface for Linux
extern const IP_FS_API IP_FS_FS;                    // Target file system (emFile), shows and allows access to hidden files.
extern const IP_FS_API IP_FS_FS_AllowHiddenAccess;  // Target file system (emFile), does not show hidden files but allows access to them.
extern const IP_FS_API IP_FS_FS_DenyHiddenAccess;   // Target file system (emFile), does not show hidden files and does not allow access to them.

void IP_FS_WIN32_ConfigBaseDir(const char* sDir);


#if defined(__cplusplus)
  }
#endif


#endif   /* Avoid multiple inclusion */

/*************************** End of file ****************************/
