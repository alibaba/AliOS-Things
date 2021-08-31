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

#ifndef CONFIG_PLATFORM_8721D
#define CONFIG_PLATFORM_8721D
#endif

bool os_sem_create(void **pp_handle, uint32_t init_count, uint32_t max_count);

bool os_sem_delete(void *p_handle);

bool os_sem_take(void *p_handle, uint32_t wait_ms);

bool os_sem_give(void *p_handle);


#endif /* _OSIF_H_ */
