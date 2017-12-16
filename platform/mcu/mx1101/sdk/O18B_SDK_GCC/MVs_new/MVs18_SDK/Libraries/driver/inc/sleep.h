/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			Sleep.c
* Description:		Sleep header file
*
* maintainer lujiangang
*
* Change History:
*			Lance	- 11/xx/2012 - V0.1
*					- create
******************************************************************************
*/

#ifndef __SLEEP_H__
#define __SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

void Sleep(void);

void DeepSleep(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__SLEEP_H__
