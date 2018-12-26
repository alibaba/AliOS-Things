/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_FLASHCACHE_H_
#define _DRIVER_CHIP_HAL_FLASHCACHE_H_

#include <stdbool.h>
#include "driver/chip/hal_def.h"
#include "sys/xr_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	__IO uint32_t COM_CTRL;   			/* ,                                       Address offset:        */
	__IO uint32_t PREFETCH_CTRL;
	__IO uint32_t PREFETCH_START_ADDR;  /* ,                           Address offset:        */
	__IO uint32_t IV_PREFETCH_CTRL;  	/* ,                      Address offset:        */
	__IO uint32_t IV_ADDR[16];   			/* ,                  Address offset:        */
	__IO uint32_t INDIRECT_JUMP_ADDR[16]; /* ,                  Address offset:        */
	__IO uint32_t MIN_ADDR;   			/* ,                  Address offset:        */
	__IO uint32_t MAX_ADDR;   			/* ,                  Address offset:        */
	__IO uint32_t READ_BIAS_ADDR;		/* ,                  Address offset:        */

} FLASH_CACHE_T;


#define FLASH_CACHE ((FLASH_CACHE_T *)FLASH_CACHE_BASE)

/*
 * @brief
 */
#define FLASH_CACHE_COM_CTRL_SIZE_SHIFT				(0)
#define FLASH_CACHE_COM_CTRL_SIZE_MASK					(0x1FFU << FLASH_CACHE_COM_CTRL_SIZE_SHIFT)

#define FLASH_CACHE_PREFETCH_CTRL_SIZE_SHIFT				(8)
#define FLASH_CACHE_PREFETCH_CTRL_SIZE_MASK					(0xFFU << FLASH_CACHE_PREFETCH_CTRL_SIZE_SHIFT)

#define FLASH_CACHE_PREFETCH_CTRL_BRANCH_SHIFT				(4)
#define FLASH_CACHE_PREFETCH_CTRL_BRANCH_MASK					(0x1U << FLASH_CACHE_PREFETCH_CTRL_BRANCH_SHIFT)

#define FLASH_CACHE_PREFETCH_CTRL_STOP_SHIFT				(0)
#define FLASH_CACHE_PREFETCH_CTRL_STOP_MASK					(0x1U << FLASH_CACHE_PREFETCH_CTRL_STOP_SHIFT)

typedef enum {
	FLASH_CACHE_PREFETCH_ENABLE = 0,
	FLASH_CACHE_PREFETCH_DISABLE = 1
} Flash_Cache_Prefetch;

#define FLASH_CACHE_PREFETCH_START_ADDR_SHIFT				(0)
#define FLASH_CACHE_PREFETCH_START_ADDR_MASK					(0xFFFFFFU << FLASH_CACHE_PREFETCH_START_ADDR_SHIFT)

#define FLASH_CACHE_IV_PREFETCH_CTRL_RELEASE_SHIFT				(12)
#define FLASH_CACHE_IV_PREFETCH_CTRL_RELEASE_MASK					(0x1U << FLASH_CACHE_IV_PREFETCH_CTRL_RELEASE_SHIFT)

#define FLASH_CACHE_IV_PREFETCH_CTRL_IV_NUM_SHIFT				(4)
#define FLASH_CACHE_IV_PREFETCH_CTRL_IV_NUM_MASK					(0xFU << FLASH_CACHE_IV_PREFETCH_CTRL_IV_NUM_SHIFT)

#define FLASH_CACHE_IV_PREFETCH_CTRL_ENABLE_SHIFT				(0)
#define FLASH_CACHE_IV_PREFETCH_CTRL_ENABLE_MASK					(0x1U << FLASH_CACHE_IV_PREFETCH_CTRL_ENABLE_SHIFT)

#define FLASH_CACHE_IV_ADDR_SHIFT				(0)
#define FLASH_CACHE_IV_ADDR_MASK					(0xFFFFFFU << FLASH_CACHE_IV_ADDR_SHIFT)

#define FLASH_CACHE_INDIRECT_JUMP_ADDR_SHIFT				(0)
#define FLASH_CACHE_INDIRECT_JUMP_ADDR_MASK					(0xFFFFFFU << FLASH_CACHE_INDIRECT_JUMP_ADDR_SHIFT)

#define FLASH_CACHE_MIN_ADDR_SHIFT				(0)
#define FLASH_CACHE_MIN_ADDR_MASK					(0xFFFFFFFFU << FLASH_CACHE_MIN_ADDR_SHIFT)

#define FLASH_CACHE_MAX_ADDR_SHIFT				(0)
#define FLASH_CACHE_MAX_ADDR_MASK					(0xFFFFFFFFU << FLASH_CACHE_MAX_ADDR_SHIFT)

#define FLASH_CACHE_READ_BIAS_ADDR_SHIFT				(0)
#define FLASH_CACHE_READ_BIAS_ADDR_MASK					(0xFFFFFFU << FLASH_CACHE_READ_BIAS_ADDR_SHIFT)


#define FLASH_ROM_START_ADDR (0x10000000U)
#define FLASH_ROM_END_ADDR (0x101FFFFFU)

#define CACHE_LINE_MAX			(0x1FF)
#define CACHE_START_ADDR	(FLASH_ROM_START_ADDR)
#define CACHE_END_ADDR		(FLASH_ROM_END_ADDR)



typedef struct {
//	uint32_t cache_size;	/* in bytes */
	uint32_t addr_bias;		/* bias from flash */
} FlashCache_Config;

typedef struct {
//	uint32_t prefetch_cache_size;	/* in bytes, recommended size is no more than FlashCache_Config.cache_size */
	bool prefetch_2nd_branch;
	uint32_t addr_prefetch_start;
} FlashCache_PrefetchConfig;

typedef struct {
	struct {
		uint32_t addr_interrupt_vector;
		uint32_t addr_interrupt_cb;
	} prefetch_interrupt[16];
} FlashCache_PrefetchIntConfig;


HAL_Status Hal_FlashCache_Init(FlashCache_Config *cfg);
HAL_Status Hal_FlashCache_Deinit();

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_FLASHCACHE_H_ */
