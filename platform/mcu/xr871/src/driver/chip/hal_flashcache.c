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

#include <stdbool.h>
#include "hal_base.h"
#include "driver/chip/hal_flashcache.h"

#include "sys/xr_debug.h"

#define FC_DEBUG(msg, arg...) XR_DEBUG((DBG_OFF | XR_LEVEL_ALL), NOEXPAND, "[FC Debug] " msg, ##arg)

#define FC_REG(reg) FC_DEBUG("register " #reg ": 0x%x.\n", reg);

#define FC_REG_ALL() { \
		FC_DEBUG("flash cache register:\n"); \
		FC_REG(FLASH_CACHE->COM_CTRL); \
		FC_REG(FLASH_CACHE->PREFETCH_CTRL); \
		FC_REG(FLASH_CACHE->PREFETCH_START_ADDR); \
		FC_REG(FLASH_CACHE->IV_PREFETCH_CTRL); \
		FC_REG(FLASH_CACHE->MIN_ADDR); \
		FC_REG(FLASH_CACHE->MAX_ADDR); \
		FC_REG(FLASH_CACHE->READ_BIAS_ADDR); \
	}






HAL_Status Hal_FlashCache_EnablePrefetch(FlashCache_PrefetchConfig *cfg)
{
/*	uint32_t cache_line = (cfg->prefetch_cache_size * 8 + 127) / 128;
	if (cache_line >= CACHE_LINE_MAX)
		return HAL_INVALID;
*/

	HAL_MODIFY_REG(FLASH_CACHE->PREFETCH_START_ADDR,
				   FLASH_CACHE_PREFETCH_START_ADDR_MASK,
				   cfg->addr_prefetch_start << FLASH_CACHE_PREFETCH_START_ADDR_SHIFT);
	HAL_MODIFY_REG(FLASH_CACHE->PREFETCH_CTRL,
//				   FLASH_CACHE_PREFETCH_CTRL_SIZE_MASK |
				   FLASH_CACHE_PREFETCH_CTRL_BRANCH_MASK |
				   FLASH_CACHE_PREFETCH_CTRL_STOP_MASK,
//				   (cache_line << FLASH_CACHE_PREFETCH_CTRL_SIZE_SHIFT) |
				   (cfg->prefetch_2nd_branch << FLASH_CACHE_PREFETCH_CTRL_BRANCH_SHIFT) |
				   (FLASH_CACHE_PREFETCH_ENABLE << FLASH_CACHE_PREFETCH_CTRL_STOP_SHIFT));

	return HAL_OK;
}

HAL_Status Hal_FlashCache_DisablePrefetch()
{
	HAL_MODIFY_REG(FLASH_CACHE->PREFETCH_CTRL,
				   FLASH_CACHE_PREFETCH_CTRL_STOP_MASK,
				   (FLASH_CACHE_PREFETCH_DISABLE << FLASH_CACHE_PREFETCH_CTRL_STOP_SHIFT));
	return HAL_OK;
}

/*
HAL_Status Hal_FlashCache_PrefetchInt(FlashCache_PrefetchIntConfig * cfg)
{
	uint32_t cnt = 0;



	HAL_MODIFY_REG(FLASH_CACHE->IV_PREFETCH_CTRL,
				   FLASH_CACHE_IV_PREFETCH_CTRL_IV_NUM_MASK,
				   cnt << FLASH_CACHE_IV_PREFETCH_CTRL_IV_NUM_SHIFT);

	if (cnt != 0)
		HAL_SET_BIT(FLASH_CACHE->IV_PREFETCH_CTRL,
					   FLASH_CACHE_IV_PREFETCH_CTRL_RELEASE_MASK |
					   FLASH_CACHE_IV_PREFETCH_CTRL_ENABLE_MASK);
	else
		HAL_CLR_BIT(FLASH_CACHE->IV_PREFETCH_CTRL,
					   FLASH_CACHE_IV_PREFETCH_CTRL_RELEASE_MASK |
					   FLASH_CACHE_IV_PREFETCH_CTRL_ENABLE_MASK);
}
*/


HAL_Status Hal_FlashCache_Init(FlashCache_Config *cfg)
{
//	uint32_t cache_line = (cfg->cache_size * 8 + 127) / 128;
//	uint32_t cache_line = (CACHE_SIZE * 8 + 127) / 128;

	/* check param */
	if (/*(cache_line > 0x1FF) || */(cfg->addr_bias & (~FLASH_CACHE_READ_BIAS_ADDR_MASK))) {

		return HAL_INVALID;
	}

	/* CCMU Enable */

//	HAL_MODIFY_REG(FLASH_CACHE, FLASH_CACHE_COM_CTRL_SIZE_MASK, cache_line << FLASH_CACHE_COM_CTRL_SIZE_SHIFT);
	HAL_MODIFY_REG(FLASH_CACHE->MIN_ADDR,
				   FLASH_CACHE_MIN_ADDR_MASK,
				   CACHE_START_ADDR << FLASH_CACHE_MIN_ADDR_SHIFT);
	HAL_MODIFY_REG(FLASH_CACHE->MAX_ADDR,
				   FLASH_CACHE_MAX_ADDR_MASK,
				   CACHE_END_ADDR << FLASH_CACHE_MAX_ADDR_SHIFT);
	HAL_MODIFY_REG(FLASH_CACHE->READ_BIAS_ADDR,
				   FLASH_CACHE_READ_BIAS_ADDR_MASK,
				   cfg->addr_bias << FLASH_CACHE_READ_BIAS_ADDR_SHIFT);

	FC_REG_ALL();
	return HAL_OK;
}

HAL_Status Hal_FlashCache_Deinit()
{
	return HAL_OK;
}


