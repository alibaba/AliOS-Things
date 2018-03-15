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

/**
 * @brief Read an amount of data from flash
 * @param[in] flash Flash device number
 * @param[in] src_addr The source address of reading
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read
 * @return Number of bytes read
 */
uint32_t flash_read(uint32_t flash, uint32_t src_addr, void *buf, uint32_t size)
{
	if (HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT) != HAL_OK) {
		FLASH_ERR("flash %d\n", flash);
		return 0;
	}

	if (HAL_Flash_Read(flash, src_addr, buf, size) != HAL_OK) {
		FLASH_ERR("flash %d, src addr %#010x, buf %p, size %#010x\n",
				  flash, src_addr, buf, size);
		HAL_Flash_Close(flash);
		return 0;
	}

	HAL_Flash_Close(flash);
	return size;
}

/**
 * @brief Write an amount of data to flash
 * @param[in] flash Flash device number
 * @param[in] dst_addr The destination address of writing
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be written
 * @return Number of bytes written
 */
uint32_t flash_write(uint32_t flash, uint32_t dst_addr, void *buf, uint32_t size)
{
	if (HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT) != HAL_OK) {
		FLASH_ERR("flash %d\n", flash);
		return 0;
	}

	if (HAL_Flash_Write(flash, dst_addr, buf, size) != HAL_OK) {
		FLASH_ERR("flash %d, dst addr %#010x, buf %p, size %#010x\n",
				  flash, dst_addr, buf, size);
		HAL_Flash_Close(flash);
		return 0;
	}

	HAL_Flash_Close(flash);
	return size;
}

/**
 * @brief Check whether the specified area is aligned to the flash erase block
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return 0 on aligned, -1 on misaligned
 */
int32_t flash_erase_check(uint32_t flash, uint32_t addr, uint32_t size)
{
	uint32_t	start;

	if ((size >= (64 << 10))
		&& (HAL_Flash_MemoryOf(flash, FLASH_ERASE_64KB, addr, &start) == HAL_OK)
		&& (addr == start)
		&& ((size & 0xFFFF) == 0)) {
	} else if ((size >= (32 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_32KB, addr, &start) == HAL_OK)
			   && (addr == start)
			   && ((size & 0x7FFF) == 0)) {
	} else if ((size >= (4 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_4KB, addr, &start) == HAL_OK)
			   && (addr == start)
			   && ((size & 0xFFF) == 0)) {
	} else {
		FLASH_WARN("%s(), %d, flash %d, addr %#010x, size %#010x\n",
				   __func__, __LINE__, flash, addr, size);
		return -1;
	}

	return 0;
}

/**
 * @brief Erase a specified area in flash
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return Number of bytes erased
 */
uint32_t flash_erase(uint32_t flash, uint32_t addr, uint32_t size)
{
	uint32_t		start;
	uint32_t		multiples;
	FlashEraseMode	erase_size;
	HAL_Status		status;

	if ((size >= (64 << 10))
		&& (HAL_Flash_MemoryOf(flash, FLASH_ERASE_64KB, addr, &start) == HAL_OK)) {
		multiples = size / (64 << 10);
		erase_size = FLASH_ERASE_64KB;
	} else if ((size >= (32 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_32KB, addr, &start) == HAL_OK)) {
		multiples = size / (32 << 10);
		erase_size = FLASH_ERASE_32KB;
	} else if ((size >= (4 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_4KB, addr, &start) == HAL_OK)) {
		multiples = size / (4 << 10);
		erase_size = FLASH_ERASE_4KB;
	} else {
		FLASH_ERR("flash %d, addr %#010x, size %#010x\n", flash, addr, size);
		return 0;
	}

	if (HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT) != HAL_OK) {
		FLASH_ERR("flash %d\n", flash);
		return 0;
	}

	status = HAL_Flash_Erase(flash, erase_size, addr, multiples);
	if (status != HAL_OK) {
		FLASH_ERR("flash %d, erase size %#010x, addr %#010x, mul %d\n",
				  flash, erase_size, addr, multiples);
		HAL_Flash_Close(flash);
		return 0;
	}

	HAL_Flash_Close(flash);
	return size;
}

