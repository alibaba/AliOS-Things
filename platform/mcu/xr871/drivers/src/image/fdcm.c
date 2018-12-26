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
	uint16_t	bitmap_size; 	/* in byte */
	uint16_t	data_size;		/* in byte */
} fdcm_header_t;

#define FDCM_HEADER_SIZE	sizeof(fdcm_header_t)

/* calc the number of used bits, bit 0 means used */
static uint32_t	fdcm_bit_count(uint8_t *bitmap, uint16_t size)
{
	uint32_t	byte_cnt = 0;
	uint32_t	bit_cnt = 0;
	uint8_t		val;

	while (byte_cnt < size) {
		if (*bitmap == 0) {
			bit_cnt += FDCM_BYTE_BITS;
		} else {
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
static int32_t fdcm_rewrite(fdcm_handle_t *hdl, const void *data,
                            uint16_t data_size)
{
	uint8_t			bitmap_byte = 0xFE;
	uint32_t		addr;
	fdcm_header_t	header;

	FDCM_DBG("%s(), hdl %p, data (%p, %u)\n", __func__, hdl, data, data_size);

	header.id_code = FDCM_ID_CODE;
	header.bitmap_size = (hdl->size - FDCM_HEADER_SIZE - 1) /
	                     (data_size * FDCM_BYTE_BITS + 1) + 1;
	if (header.bitmap_size > FDCM_INDEX_MAX)
		header.bitmap_size = FDCM_INDEX_MAX;
	header.data_size = data_size;

	addr = hdl->size - FDCM_HEADER_SIZE - header.bitmap_size; /* temp value */
	if (data_size > addr) {
		FDCM_ERR("data size %u > %u\n", data_size, addr);
		return 0;
	}

	FDCM_DBG("%s(), area (%u, %#x, %u), bitmap_size %u, data_size %u\n",
			 __func__, hdl->flash, hdl->addr, hdl->size,
			 header.bitmap_size, header.data_size);

	if (flash_erase(hdl->flash, hdl->addr, hdl->size) != 0) {
		return 0;
	}

	addr = hdl->addr;
	if (flash_write(hdl->flash, addr, &header,
	                FDCM_HEADER_SIZE) != FDCM_HEADER_SIZE) {
		return 0;
	}

	addr += FDCM_HEADER_SIZE;
	if (flash_write(hdl->flash, addr, &bitmap_byte, 1) != 1) {
		return 0;
	}

	addr += header.bitmap_size;
	if (flash_write(hdl->flash, addr, data, data_size) != data_size) {
		return 0;
	}
	return data_size;
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

	if (flash_get_erase_block(flash, addr, size) < 0) {
		return NULL;
	}

	hdl = (fdcm_handle_t *)fdcm_malloc(sizeof(fdcm_handle_t));
	if (hdl == NULL) {
		FDCM_ERR("no mem\n");
		return NULL;
	}

	hdl->flash = flash;
	hdl->addr = addr;
	hdl->size = size;
	FDCM_DBG("%s(), flash %d, addr %#x, size %u, hdl %p\n",
			 __func__, flash, addr, size, hdl);
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
	uint32_t		ret = 0;

	FDCM_DBG("%s(), hdl %p, data (%p, %u)\n", __func__, hdl, data, data_size);

	if ((hdl == NULL) || (data == NULL)) {
		FDCM_ERR("hdl %p, data %p\n", hdl, data);
		return ret;
	}

	if (flash_read(hdl->flash, hdl->addr, &header,
	               FDCM_HEADER_SIZE) != FDCM_HEADER_SIZE) {
		return ret;
	}

	FDCM_DBG("%s(), header id_code %#x, bitmap_size %u, data_size %u\n",
			 __func__, header.id_code, header.bitmap_size, header.data_size);

	if ((header.id_code != FDCM_ID_CODE) ||
	    (header.data_size != data_size) ||
	    (header.bitmap_size == 0) ||
	    (header.bitmap_size + FDCM_HEADER_SIZE >= hdl->size)) {
		FDCM_WRN("%s(), invalid head, id_code %#x, bitmap_size %u, "
		         "data_size %u, read size %u\n", __func__, header.id_code,
		         header.bitmap_size, header.data_size, data_size);
		return ret;
	}

	bitmap = (uint8_t *)fdcm_malloc(header.bitmap_size);
	if (bitmap == NULL) {
		FDCM_ERR("no mem\n");
		return ret;
	}

	do {
		addr = hdl->addr + FDCM_HEADER_SIZE;
		if (flash_read(hdl->flash, addr, bitmap,
					   header.bitmap_size) != header.bitmap_size) {
			break;
		}

		bit_cnt = fdcm_bit_count(bitmap, header.bitmap_size);
		addr += header.bitmap_size + data_size * (bit_cnt - 1);
		ret = flash_read(hdl->flash, addr, data, data_size);
	} while (0);

	fdcm_free(bitmap);
	return ret;
}

/**
 * @brief Write a data chunk to the specified area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes written
 */
uint32_t fdcm_write(fdcm_handle_t *hdl, const void *data, uint16_t data_size)
{
	fdcm_header_t	header;
	uint8_t		   *bitmap;
	uint32_t		bit_cnt;
	uint32_t		index;
	uint32_t		addr;
	uint32_t		ret = 0;

	FDCM_DBG("%s(), hdl %p, data (%p, %u)\n", __func__, hdl, data, data_size);

	if ((hdl == NULL) || (data == NULL)) {
		FDCM_ERR("hdl %p, data %p\n", hdl, data);
		return ret;
	}

	if (flash_read(hdl->flash, hdl->addr, &header,
	               FDCM_HEADER_SIZE) != FDCM_HEADER_SIZE) {
		return ret;
	}

	FDCM_DBG("%s(), header id_code %#x, bitmap_size %u, data_size %u\n",
			 __func__, header.id_code, header.bitmap_size, header.data_size);

	if ((header.id_code != FDCM_ID_CODE) ||
	    (header.data_size != data_size) ||
	    (header.bitmap_size == 0) ||
	    (header.bitmap_size + FDCM_HEADER_SIZE >= hdl->size)) {
		return fdcm_rewrite(hdl, data, data_size);
	}

	addr = hdl->size - FDCM_HEADER_SIZE - header.bitmap_size; /* temp value */
	if (data_size > addr) {
		FDCM_ERR("data size %u > %u\n", data_size, addr);
		return ret;
	}

	bitmap = (uint8_t *)fdcm_malloc(header.bitmap_size);
	if (bitmap == NULL) {
		FDCM_ERR("no mem\n");
		return ret;
	}

	do {
		/* read bitmap */
		addr = hdl->addr + FDCM_HEADER_SIZE;
		if (flash_read(hdl->flash, addr, bitmap,
		               header.bitmap_size) != header.bitmap_size) {
			break;
		}

		bit_cnt = fdcm_bit_count(bitmap, header.bitmap_size);
		if ((bit_cnt >= FDCM_BYTE_BITS * header.bitmap_size) ||
		    (FDCM_HEADER_SIZE + header.bitmap_size +
		     data_size * (bit_cnt + 1) > hdl->size)) {
			ret = fdcm_rewrite(hdl, data, data_size);
			break;
		}

		/* write bitmap */
		index = bit_cnt / FDCM_BYTE_BITS;
		addr += index;
		bitmap[index] &= ~(0x1 << (bit_cnt % FDCM_BYTE_BITS));
		if (flash_write(hdl->flash, addr, &bitmap[index], 1) != 1) {
			break;
		}

		addr = hdl->addr + FDCM_HEADER_SIZE + header.bitmap_size +
		       data_size * bit_cnt;
		ret = flash_write(hdl->flash, addr, data, data_size);
	} while (0);

	fdcm_free(bitmap);
	return ret;
}

/**
 * @brief Erase the whole FDCM area
 * @param[in] hdl Pointer to the FDCM handle
 * @return 0 on success, -1 on failure
 */
int fdcm_erase(fdcm_handle_t *hdl)
{
	if (hdl == NULL) {
		FDCM_ERR("hdl %p\n", hdl);
		return -1;
	}

	FDCM_DBG("%s(), hdl %p, (%u, %#x, %u)\n", __func__, hdl,
	         hdl->flash, hdl->addr, hdl->size);

	if (flash_erase(hdl->flash, hdl->addr, hdl->size) != 0) {
		FDCM_ERR("erase fail, (%u, %#x, %u)\n",
		         hdl->flash, hdl->addr, hdl->size);
		return -1;
	}
	return 0;
}

/**
 * @brief Close the area managed by FDCM module
 * @param[in] hdl Pointer to the FDCM handle
 * @return None
 */
void fdcm_close(fdcm_handle_t *hdl)
{
	FDCM_DBG("%s(), hdl %p\n", __func__, hdl);

	if (hdl != NULL) {
		fdcm_free(hdl);
	}
}
