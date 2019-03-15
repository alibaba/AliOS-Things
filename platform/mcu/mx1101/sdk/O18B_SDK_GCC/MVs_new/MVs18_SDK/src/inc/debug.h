////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:debug.h
//
//            maintainer Halley
 //
//		Description	:
//					Define debug ordinary print & debug routine
//
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include <stdio.h>

#define	DBG(format, ...)		mvprintf(format, ##__VA_ARGS__)

#define	APP_DBG(format, ...)	mvprintf(format, ##__VA_ARGS__)

#define	FS_DBG(format, ...)		mvprintf(format, ##__VA_ARGS__)

#define ASSERT(x)

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__DBG_H__ 

