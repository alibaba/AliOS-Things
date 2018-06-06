/**
 ****************************************************************************************
 *
 * @file co_bool.h
 *
 * @brief This file replaces the need to include stdint or stdbool typical headers,
 *        which may not be available in all toolchains, and adds new types
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 * $Rev: $
 *
 ****************************************************************************************
 */

#ifndef _CO_BOOL_H_
#define _CO_BOOL_H_
#include <stdbool.h>

#if 0
typedef unsigned char bool;

#ifndef true
	#define true     1
#endif

#ifndef false
	#define false    0
#endif

#ifndef TRUE
	#define TRUE     1
#endif

#ifndef FALSE
	#define FALSE    0
#endif
#endif
/// @} CO_BOOL
#endif // _CO_BOOL_H_
