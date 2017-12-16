/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */


#ifndef __PlatformLogging_h__
#define __PlatformLogging_h__

#include "common.h"
#include "debug.h"

#ifndef MICO_PLATFORM_LOG_INFO_DISABLE
	#define platform_log(M, ...) custom_log("Platform", M, ##__VA_ARGS__)
	#define platform_log_trace() custom_log_trace("Platform")
#else
	#define platform_log(M, ...) 
	#define platform_log_trace() 
#endif

#endif // __PlatformLogging_h__

