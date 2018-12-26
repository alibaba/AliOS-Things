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

#include "flash.h"
#include "image_debug.h"

#include "driver/chip/hal_flash.h"
#include "sys/param.h"

#define FLASH_OPEN_TIMEOUT	(5000)

typedef struct flash_erase_param {
	int32_t         block_size;
	FlashEraseMode	erase_mode;
} flash_erase_param_t;

static const flash_erase_param_t s_flash_erase_param[] = {
	{ (64 * 1024), FLASH_ERASE_64KB },
	{ (32 * 1024), FLASH_ERASE_32KB },
	{ ( 4 * 1024), FLASH_ERASE_4KB  },
};

#define FLASH_ERASE_PARAM_CNT	nitems(s_flash_erase_param)

/**
 * @brief Read/write an amount of data from/to flash
 * @param[in] flash Flash device number
 * @param[in/out] addr flash address to be read/written
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read/written
 * @param[in] do_write Read or write
 * @return Number of bytes read/written
 */
uint32_t flash_rw(uint32_t flash, uint32_t addr,
                  void *buf, uint32_t size, int do_write)
{
	HAL_Status status;

	status = HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT);
	if (status != HAL_OK) {
		FLASH_ERR("open %u fail\n", flash);
		return 0;
	}

	if (do_write) {
		status = HAL_Flash_Write(flash, addr, buf, size);
	} else {
		status = HAL_Flash_Read(flash, addr, buf, size);
	}

	HAL_Flash_Close(flash);
	if (status != HAL_OK) {
		FLASH_ERR("%s fail, (%u, %#x, %u), %p\n", do_write ? "write" : "read",
				  flash, addr, size, buf);
		return 0;
	}
	return size;
}

/**
 * @brief Get erase block size for the specified area
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return Erase block size, -1 on misaligned
 */
int32_t flash_get_erase_block(uint32_t flash, uint32_t addr, uint32_t size)
{
	int i;
	int32_t block_size;
	FlashEraseMode erase_mode;
	uint32_t start;

	for (i = 0; i < FLASH_ERASE_PARAM_CNT; ++i) {
		block_size = s_flash_erase_param[i].block_size;
		erase_mode = s_flash_erase_param[i].erase_mode;
		if ((size >= block_size) &&
		    ((size & (block_size - 1)) == 0) &&
		    (HAL_Flash_MemoryOf(flash, erase_mode, addr, &start) == HAL_OK) &&
		    (addr == start)) {
		    break;
		}
	}

	if (i >= FLASH_ERASE_PARAM_CNT) {
		FLASH_ERR("(%u, %#x, %u) misaligned\n", flash, addr, size);
		return -1;
	}
	return block_size;
}

/**
 * @brief Erase a specified area in flash
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return 0 on success, -1 on failure
 */
int flash_erase(uint32_t flash, uint32_t addr, uint32_t size)
{
	int i;
	int32_t block_size;
	FlashEraseMode erase_mode;
	uint32_t start;
	HAL_Status status;

	if (HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT) != HAL_OK) {
		FLASH_ERR("open %d fail\n", flash);
		return -1;
	}

	for (i = 0; i < FLASH_ERASE_PARAM_CNT; ++i) {
		block_size = s_flash_erase_param[i].block_size;
		erase_mode = s_flash_erase_param[i].erase_mode;
		if ((size >= block_size) &&
		    ((size & (block_size - 1)) == 0) &&
		    (HAL_Flash_MemoryOf(flash, erase_mode, addr, &start) == HAL_OK) &&
		    (addr == start)) {
			FLASH_DBG("%s() (%u, %#x, %u), block_size %d, erase_mode %#x\n",
			          __func__, flash, addr, size, block_size, erase_mode);
			status = HAL_Flash_Erase(flash, erase_mode, addr, size / block_size);
			if (status == HAL_OK) {
				FLASH_DBG("%s() success\n", __func__);
				break;
			} else {
				FLASH_WRN("%s() fail, (%u, %#x, %u), block_size %d, "
				          "erase_mode %#x\n", __func__, flash, addr, size,
				          block_size, erase_mode);
			}
		}
	}

	HAL_Flash_Close(flash);

	if (i >= FLASH_ERASE_PARAM_CNT) {
		FLASH_ERR("fail, (%u, %#x, %u)\n", flash, addr, size);
		return -1;
	}
	return 0;
}
