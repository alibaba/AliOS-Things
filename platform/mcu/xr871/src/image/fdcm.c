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

#include <string.h>
#include <stdlib.h>

#include "flash.h"
#include "sys/fdcm.h"
#include "image_debug.h"

#define fdcm_malloc(l)			malloc(l)
#define fdcm_free(p)			free(p)
#define fdcm_memcpy(d, s, n)	memcpy(d, s, n)
#define fdcm_memset(s, c, n) 	memset(s, c, n)

#define FDCM_ID_CODE		(0xA55A5AA5)
#define FDCM_INDEX_MAX		(1 << 10)
#define FDCM_BYTE_BITS		(8)

typedef struct fdcm_header {
	uint32_t	id_code;
	uint16_t	bitmap_size;
	uint16_t	data_size;
} fdcm_header_t;

#define FDCM_HEADER_SIZE	sizeof(fdcm_header_t)

static uint32_t	fdcm_bit_count(uint8_t *bitmap, uint16_t size)
{
	uint32_t	byte_cnt = 0;
	uint32_t	bit_cnt = 0;
	uint8_t		val;

	while (byte_cnt < size) {
		if (*bitmap == 0) {
			bit_cnt += FDCM_BYTE_BITS;
		}
		else {
			val = ~(*bitmap);
			while (val) {
				val = val >> 1;
				bit_cnt++;
			}
			break;
		}
		bitmap++;
		byte_cnt++;
	}

	return bit_cnt;
}

/**
 * @brief Erase the area and then write a data chunk to the area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes written
 */
static uint32_t fdcm_rewrite(fdcm_handle_t *hdl, void *data, uint16_t data_size)
{
	uint8_t			bitmap_byte = 0xFE;
	fdcm_header_t	header;

	if (flash_erase(hdl->flash, hdl->addr, hdl->size) != hdl->size) {
		FDCM_ERR("flash %d, addr %#010x, size %#010x\n", hdl->flash, hdl->addr, hdl->size);
		return 0;
	}

	header.id_code = FDCM_ID_CODE;
	header.bitmap_size = (hdl->size - FDCM_HEADER_SIZE - 1) / (data_size * FDCM_BYTE_BITS + 1) + 1;
	if (header.bitmap_size > FDCM_INDEX_MAX)
		header.bitmap_size = FDCM_INDEX_MAX;
	header.data_size = data_size;

	FDCM_DBG("%s(), %d, flash %d, addr %#010x, size %#010x, bitmap size %d, data size %d\n",
			 __func__, __LINE__, hdl->flash, hdl->addr, hdl->size, header.bitmap_size, header.data_size);

	if (flash_write(hdl->flash, hdl->addr, &header, FDCM_HEADER_SIZE)
		&& flash_write(hdl->flash, hdl->addr + FDCM_HEADER_SIZE, &bitmap_byte, 1)
		&& flash_write(hdl->flash, hdl->addr + FDCM_HEADER_SIZE + header.bitmap_size, data, data_size)) {
		return data_size;
	} else {
		FDCM_ERR("flash %d, addr %#010x, size %#010x\n", hdl->flash, hdl->addr, hdl->size);
		return 0;
	}
}

/**
 * @brief Open an area in a flash to be managed by FDCM module
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the area
 * @param[in] size Size of the area
 * @retval Pointer to the FDCM handle, NULL on failure
 *
 * @note The area must be aligned to the flash erase block
 */
fdcm_handle_t *fdcm_open(uint32_t flash, uint32_t addr, uint32_t size)
{
	fdcm_handle_t *hdl;

	if (flash_erase_check(flash, addr, size) != 0) {
		FDCM_ERR("flash %d, addr %#010x, size %#010x\n", flash, addr, size);
		return NULL;
	}

	hdl = (fdcm_handle_t *)fdcm_malloc(sizeof(fdcm_handle_t));
	if (hdl == NULL) {
		FDCM_ERR("hdl %p\n", hdl);
		return NULL;
	}

	hdl->flash = flash;
	hdl->addr = addr;
	hdl->size = size;

	FDCM_DBG("%s(), %d, flash %d, addr %#010x, size %#010x\n",
			 __func__, __LINE__, flash, addr, size);

	return hdl;
}

/**
 * @brief Read a data chunk from the specified area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes read
 */
uint32_t fdcm_read(fdcm_handle_t *hdl, void *data, uint16_t data_size)
{
	fdcm_header_t	header;
	uint8_t		   *bitmap;
	uint32_t		bit_cnt;
	uint32_t		addr;

	if ((hdl == NULL) || (data == NULL)) {
		FDCM_ERR("hdl %p, data %p\n", hdl, data);
		return 0;
	}

	flash_read(hdl->flash, hdl->addr, &header, FDCM_HEADER_SIZE);

	FDCM_DBG("%s(), %d, id code %#010x, bitmap size %#06x, data size %#06x\n",
			 __func__, __LINE__, header.id_code, header.bitmap_size, header.data_size);

	if ((header.id_code != FDCM_ID_CODE)
		|| (header.data_size != data_size)
		|| (header.bitmap_size == 0)
		|| (header.bitmap_size + FDCM_HEADER_SIZE >= hdl->size)) {
		FDCM_WARN("id code %#010x, bitmap size %#06x, data size %#06x\n",
				  header.id_code, header.bitmap_size, header.data_size);
		return 0;
	}

	bitmap = (uint8_t *)fdcm_malloc(header.bitmap_size);
	if (bitmap == NULL) {
		FDCM_ERR("bitmap %p\n", bitmap);
		return 0;
	}

	flash_read(hdl->flash, hdl->addr + FDCM_HEADER_SIZE, bitmap, header.bitmap_size);
	bit_cnt = fdcm_bit_count(bitmap, header.bitmap_size);
	fdcm_free(bitmap);

	addr = hdl->addr + FDCM_HEADER_SIZE + header.bitmap_size + data_size * (bit_cnt - 1);
	return flash_read(hdl->flash, addr, data, data_size);
}

/**
 * @brief Write a data chunk to the specified area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes written
 */
uint32_t fdcm_write(fdcm_handle_t *hdl, void *data, uint16_t data_size)
{
	fdcm_header_t	header;
	uint8_t		   *bitmap;
	uint32_t		bit_cnt;
	uint32_t		addr;

	if ((hdl == NULL) || (data == NULL)) {
		FDCM_ERR("hdl %p, data %p\n", hdl, data);
		return 0;
	}

	flash_read(hdl->flash, hdl->addr, &header, FDCM_HEADER_SIZE);

	FDCM_DBG("%s(), %d, id code %#010x, bitmap size %#06x, data size %#06x\n",
			 __func__, __LINE__, header.id_code, header.bitmap_size, header.data_size);

	if ((header.id_code != FDCM_ID_CODE)
		|| (header.data_size != data_size)
		|| (header.bitmap_size == 0)
		|| (header.bitmap_size + FDCM_HEADER_SIZE >= hdl->size)) {
		return fdcm_rewrite(hdl, data, data_size);
	}

	bitmap = (uint8_t *)fdcm_malloc(header.bitmap_size);
	if (bitmap == NULL) {
		FDCM_ERR("bitmap %p\n", bitmap);
		return 0;
	}

	flash_read(hdl->flash, hdl->addr + FDCM_HEADER_SIZE, bitmap, header.bitmap_size);
	bit_cnt = fdcm_bit_count(bitmap, header.bitmap_size);
	if ((bit_cnt == FDCM_BYTE_BITS * header.bitmap_size)
		|| (FDCM_HEADER_SIZE + header.bitmap_size + data_size * (bit_cnt + 1) > hdl->size)) {
		fdcm_free(bitmap);
		return fdcm_rewrite(hdl, data, data_size);
	}

	addr = hdl->addr + FDCM_HEADER_SIZE + bit_cnt / FDCM_BYTE_BITS;
	bitmap[bit_cnt / FDCM_BYTE_BITS] &= ~(0x1 << (bit_cnt % FDCM_BYTE_BITS));
	flash_write(hdl->flash, addr, bitmap + bit_cnt / FDCM_BYTE_BITS, 1);
	fdcm_free(bitmap);

	addr = hdl->addr + FDCM_HEADER_SIZE + header.bitmap_size + data_size * bit_cnt;
	return flash_write(hdl->flash, addr, data, data_size);
}

/**
 * @brief Close the area managed by FDCM module
 * @param[in] hdl Pointer to the FDCM handle
 * @return None
 */
void fdcm_close(fdcm_handle_t *hdl)
{
	if (hdl != NULL) {
		fdcm_free(hdl);
		hdl = NULL;
	}
}

