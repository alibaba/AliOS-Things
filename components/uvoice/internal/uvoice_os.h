/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_OS_H__
#define __UVOICE_OS_H__


#define __os_alios_things__

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a):(b))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define UINT32_BIG_2_LIT(val) \
    ((val&0xFF)<<24) | ((val&0xFF00)<<8) | ((val&0xFF0000)>>8) | ((val&0xFF000000)>>24)

#ifndef bool
#define bool	char
#endif

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

#define snd_memcpy(dest, src, length) 		\
	{										\
		if ((dest) != (src) && length > 0)	\
			memcpy(dest, src, length);		\
	}

#define snd_memmove(dest, src, length) 		\
	{										\
		if ((dest) != (src))				\
			memmove(dest, src, length);		\
	}

#ifdef __os_alios_things__

#include "uvoice_aos.h"

#elif defined(__os_linux__)

#include "uvoice_linux.h"

#else

#error OS type not specified !

#endif

#endif /* __UVOICE_OS_H__ */
