/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGDefine.h
 * @brief
 * @author zhaojian.lzj@alibaba-inc.com
 * @data 2019/01/07
 * @version 1.0
 */

#ifndef _AG_DEFINE_H_
#define _AG_DEFINE_H_
#include <stdlib.h>

#ifndef VOID
typedef void           VOID;
#endif
#ifndef CHAR
typedef char           CHAR;
#endif
#ifndef BOOL
typedef unsigned char  BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char  BOOLEAN;
#endif
#ifndef INT32
typedef int            INT32;
#endif
#ifndef UINT32
typedef unsigned int   UINT32;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
#endif
#ifndef INT16
typedef short          INT16;
#endif
#ifndef INT8
typedef signed char    INT8;
#endif
#ifndef UINT8
typedef unsigned char  UINT8;
#endif

#define STATUS_SUCCESS   0
#define STATUS_ERROR    -1
#define DELETEIF(p) do{if(p != NULL) {delete p;p=NULL;}}while(0)
#define FREEIF(p) do{if(p != NULL) {free(p);p=NULL;}}while(0)
#define FCLOSEIF(p) do{if(p != NULL) {fclose(p);p=NULL;}}while(0)
#define BYTE_L1(word) ((UINT8)(word & 0xFF))
#define BYTE_L2(word) ((UINT8)((word>>8) & 0xFF))
#define BYTE_L3(dword) ((UINT8)((dword>>16) & 0xFF))
#define BYTE_L4(dword) ((UINT8)((dword>>24) & 0xFF))
#define MAKE_U16(byte2,byte1) ((INT16U)(((byte2)<<8) | (byte1)))
#define MAKE_U32_4(byte4,byte3,byte2,byte1) ((UINT32)(((byte4)<<24) | ((byte3)<<16) | ((byte2)<<8) | (byte1)))
#define MAKE_U32_2(u16_high,u16_low) ((UINT32)(((u16_high)<<16) | (u16_low)))
#define MAKE_I32_4(byte4,byte3,byte2,byte1) ((INT32)(((byte4)<<24) | ((byte3)<<16) | ((byte2)<<8) | (byte1)))
#define MAKE_I32_2(u16_high,u16_low) ((INT32)(((u16_high)<<16) | (u16_low)))

#if defined (_T_HEAD_)
#define SMARTBOX_DEFAULT_PRIORITY 40
#define SMARTBOX_HIGH_PRIORITY 39
#elif defined (_AMLOGIC_)
#define SMARTBOX_DEFAULT_PRIORITY 3
#define SMARTBOX_HIGH_PRIORITY 6
#endif

#define SMARTBOX_NORMAL_STACKSIZE 4096
#define SMARTBOX_HIGH_STACKSIZE 8192 * 2
#endif //_AG_DEFINE_H_




