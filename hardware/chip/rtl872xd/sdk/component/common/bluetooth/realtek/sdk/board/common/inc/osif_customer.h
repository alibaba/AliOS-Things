/*
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _OSIF_H_
#define _OSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef false
	#define false 0
#endif

#ifndef true
	#define true (!false)
#endif

#ifndef bool
	#define bool unsigned char
#endif

#define CONFIG_PLATFORM_8721D

#endif /* _OSIF_H_ */
