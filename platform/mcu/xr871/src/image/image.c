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
#include "sys/image.h"
#include "image_debug.h"

#define image_malloc(l)			malloc(l)
#define image_free(p)			free(p)
#define image_memcpy(d, s, n)	memcpy(d, s, n)
#define image_memset(s, c, n) 	memset(s, c, n)

#define IMAGE_CHECK_SIZE		(1024)
#define IMAGE_EXPAND_SIZE		(5)

typedef struct {
	uint32_t	id;
	uint32_t	addr;
} sec_addr_t;

typedef struct {
	uint8_t		sec_num[IMAGE_SEQ_NUM];
	sec_addr_t *sec_addr[IMAGE_SEQ_NUM];
	uint32_t	flash[IMAGE_SEQ_NUM];
	uint32_t	addr[IMAGE_SEQ_NUM];
	uint32_t	image_size;
	uint32_t	boot_size;
	image_seq_t	seq;
} image_priv_t;

image_priv_t	image_priv;

static void image_clear_sec_addr(void)
{
	image_seq_t seq;

	for (seq = IMAGE_SEQ_1ST; seq < IMAGE_SEQ_NUM; seq++) {
		image_priv.sec_num[seq] = 0;
		if (image_priv.sec_addr[seq]) {
			image_free(image_priv.sec_addr[seq]);
			image_priv.sec_addr[seq] = NULL;
		}
	}
}

/**
 * @brief Initialize the image module
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the image region
 * @param[in] size Size of the image region
 * @return None
 */
void image_init(uint32_t flash, uint32_t addr, uint32_t size)
{
	section_header_t sh;

	IMAGE_DBG("%s(), %d, flash %d, addr %#010x, size %#010x\n",
			  __func__, __LINE__, flash, addr, size);

	if (flash_read(flash, addr, &sh, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE) {
		IMAGE_ERR("read flash failed\n");
		return;
	}

	image_clear_sec_addr();
	image_priv.flash[IMAGE_SEQ_1ST]	= flash;
	image_priv.addr[IMAGE_SEQ_1ST]	= addr;
	if (sh.priv[0] == IMAGE_INVALID_FLASH)
		image_priv.flash[IMAGE_SEQ_2ND] = flash;
	else
		image_priv.flash[IMAGE_SEQ_2ND]	= sh.priv[0];
	image_priv.addr[IMAGE_SEQ_2ND]	= sh.priv[1];
	image_priv.image_size			= size;
	image_priv.boot_size			= sh.next_addr;
	image_priv.seq					= IMAGE_SEQ_NUM;

	IMAGE_DBG("%s(), %d, flash_1st %d, addr_1st %#010x, flash_2nd %d, "
			  "addr_2nd %#010x, image_size %#010x, boot_size %#010x, seq %d\n",
			  __func__, __LINE__,
			  image_priv.flash[IMAGE_SEQ_1ST], image_priv.addr[IMAGE_SEQ_1ST],
			  image_priv.flash[IMAGE_SEQ_2ND], image_priv.addr[IMAGE_SEQ_2ND],
			  image_priv.image_size, image_priv.boot_size, image_priv.seq);
}

/**
 * @brief DeInitialize the image module
 * @return None
 */
void image_deinit(void)
{
	image_clear_sec_addr();
	image_priv.flash[IMAGE_SEQ_1ST]	= 0;
	image_priv.addr[IMAGE_SEQ_1ST]	= IMAGE_INVALID_ADDR;
	image_priv.flash[IMAGE_SEQ_2ND]	= 0;
	image_priv.addr[IMAGE_SEQ_2ND]	= IMAGE_INVALID_ADDR;
	image_priv.image_size			= 0;
	image_priv.boot_size			= 0;
	image_priv.seq					= IMAGE_SEQ_NUM;
}

/**
 * @brief Get OTA parameter
 * @param[in] param Pointer to the OTA parameter
 * @return None
 */
void image_get_ota_param(image_ota_param_t *param)
{
	if (param == NULL) {
		IMAGE_ERR("param %p\n", param);
		return;
	}

	param->flash[IMAGE_SEQ_1ST]	= image_priv.flash[IMAGE_SEQ_1ST];
	param->addr[IMAGE_SEQ_1ST]	= image_priv.addr[IMAGE_SEQ_1ST];
	param->flash[IMAGE_SEQ_2ND]	= image_priv.flash[IMAGE_SEQ_2ND];
	param->addr[IMAGE_SEQ_2ND]	= image_priv.addr[IMAGE_SEQ_2ND];
	param->image_size			= image_priv.image_size;
	param->boot_size			= image_priv.boot_size;

	IMAGE_DBG("%s(), %d, flash_1st %d, addr_1st %#010x, flash_2nd %d, "
			  "addr_2nd %#010x, image_size %#010x, boot_size %#010x\n",
			  __func__, __LINE__,
			  param->flash[IMAGE_SEQ_1ST], param->addr[IMAGE_SEQ_1ST],
			  param->flash[IMAGE_SEQ_2ND], param->addr[IMAGE_SEQ_2ND],
			  param->image_size, param->boot_size);
}

/**
 * @brief Set running image sequence
 * @param[in] seq Image sequence
 * @return None
 */
void image_set_running_seq(image_seq_t seq)
{
	IMAGE_DBG("%s(), %d, seq %d\n", __func__, __LINE__, seq);

	image_priv.seq = seq;
}

/**
 * @brief Get running image sequence
 * @param[in] seq Pointer to image sequence
 * @return None
 */
void image_get_running_seq(image_seq_t *seq)
{
	if (seq == NULL) {
		IMAGE_ERR("seq %p\n", seq);
		return;
	}

	IMAGE_DBG("%s(), %d, seq %d\n", __func__, __LINE__, image_priv.seq);

	*seq = image_priv.seq;
}

static sec_addr_t *image_expand_addr(image_seq_t seq)
{
	sec_addr_t *sec_addr;

	if (image_priv.sec_num[seq] == 0)
		return image_malloc(IMAGE_EXPAND_SIZE * sizeof(sec_addr_t));

	sec_addr = image_malloc((image_priv.sec_num[seq] + IMAGE_EXPAND_SIZE) * sizeof(sec_addr_t));
	if (sec_addr) {
		image_memcpy(sec_addr, image_priv.sec_addr[seq], image_priv.sec_num[seq] * sizeof(sec_addr_t));
		image_free(image_priv.sec_addr[seq]);
	}

	return sec_addr;
}

static uint32_t image_get_addr(image_seq_t seq, uint32_t id)
{
	uint8_t		index;
	uint32_t	addr;
	uint32_t	next_addr;
	sec_addr_t *sec_addr;

	if (seq >= IMAGE_SEQ_NUM) {
		IMAGE_ERR("seq %d\n", seq);
		return IMAGE_INVALID_ADDR;
	}

	for (index = 0; index < image_priv.sec_num[seq]; index++) {
		if (id == image_priv.sec_addr[seq][index].id)
			return image_priv.sec_addr[seq][index].addr;
	}

	if (image_priv.sec_num[seq] == 0) {
		addr = image_priv.addr[IMAGE_SEQ_1ST];
	} else {
		addr = image_priv.sec_addr[seq][image_priv.sec_num[seq] - 1].addr;
		flash_read(image_priv.flash[seq], addr + offsetof(section_header_t, next_addr), &next_addr, sizeof(next_addr));
		addr = image_priv.addr[seq] + next_addr;
	}

	while (next_addr != IMAGE_INVALID_ADDR) {
		if ((image_priv.sec_num[seq] % IMAGE_EXPAND_SIZE) == 0) {
			sec_addr = image_expand_addr(seq);
			if (sec_addr == NULL) {
				IMAGE_ERR("sec_addr %p, seq %d, num %d\n", sec_addr, seq, image_priv.sec_num[seq]);
				return IMAGE_INVALID_ADDR;
			} else {
				image_priv.sec_addr[seq] = sec_addr;
			}
		}

		image_priv.sec_addr[seq][image_priv.sec_num[seq]].addr = addr;
		flash_read(image_priv.flash[seq], addr + offsetof(section_header_t, id),
				   &(image_priv.sec_addr[seq][image_priv.sec_num[seq]].id), sizeof(id));
		image_priv.sec_num[seq]++;

		if (id == image_priv.sec_addr[seq][image_priv.sec_num[seq] - 1].id) {
			return image_priv.sec_addr[seq][image_priv.sec_num[seq] - 1].addr;
		}
		flash_read(image_priv.flash[seq], addr + offsetof(section_header_t, next_addr),
				   &next_addr, sizeof(next_addr));
		addr = image_priv.addr[seq] + next_addr;
	}

	IMAGE_ERR("seq %d, id %#010x\n", seq, id);
	return IMAGE_INVALID_ADDR;
}

/**
 * @brief Get address of the specified section in running image
 * @param[in] id ID of the specified section
 * @return section address
 */
uint32_t image_get_section_addr(uint32_t id)
{
	IMAGE_DBG("%s(), %d, id %#010x\n", __func__, __LINE__, id);

	return image_get_addr(image_priv.seq, id);
}

/**
 * @brief Read an amount of image data from flash
 * @param[in] id Section ID of the image data
 * @param[in] seg Section segment of the image data
 * @param[in] offset Offset into the segment from where to read data
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read
 * @return Number of bytes read
 */
uint32_t image_read(uint32_t id, image_seg_t seg, uint32_t offset, void *buf, uint32_t size)
{
	uint32_t addr;
	uint32_t body_len;

	IMAGE_DBG("%s(), %d, id %#010x, seg %d, offset %d, buf %p, size %d\n",
			  __func__, __LINE__, id, seg, offset, buf, size);

	if (buf == NULL) {
		IMAGE_ERR("buf %p\n", buf);
		return 0;
	}

	addr = image_get_addr(image_priv.seq, id);
	if (addr == IMAGE_INVALID_ADDR) {
		IMAGE_ERR("seq %d, id %#010x, addr %#010x\n", image_priv.seq, id, addr);
		return 0;
	}

	switch (seg) {
	case IMAGE_SEG_HEADER:
		break;
	case IMAGE_SEG_BODY:
		addr += IMAGE_HEADER_SIZE;
		break;
	case IMAGE_SEG_TAILER:
		flash_read(image_priv.flash[image_priv.seq], addr + offsetof(section_header_t, body_len),
				   &body_len, sizeof(body_len));
		addr += IMAGE_HEADER_SIZE + body_len;
		break;
	default :
		IMAGE_ERR("seg %d\n", seg);
		return 0;
	}

	addr += offset;

	return flash_read(image_priv.flash[image_priv.seq], addr, buf, size);

}

/**
 * @brief Write an amount of image data to flash
 * @param[in] id Section ID of the image data
 * @param[in] seg Section segment of the image data
 * @param[in] offset Offset into the segment from where to write data
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be written
 * @return Number of bytes written
 */
uint32_t image_write(uint32_t id, image_seg_t seg, uint32_t offset, void *buf, uint32_t size)
{
	uint32_t addr;
	uint32_t body_len;

	IMAGE_DBG("%s(), %d, id %#010x, seg %d, offset %d, buf %p, size %d\n",
			  __func__, __LINE__, id, seg, offset, buf, size);

	if (buf == NULL) {
		IMAGE_ERR("buf %p\n", buf);
		return 0;
	}

	addr = image_get_addr(image_priv.seq, id);
	if (addr == IMAGE_INVALID_ADDR) {
		IMAGE_ERR("seq %d, id %#010x, addr %#010x\n", image_priv.seq, id, addr);
		return 0;
	}

	switch (seg) {
	case IMAGE_SEG_HEADER:
		image_clear_sec_addr();
		break;
	case IMAGE_SEG_BODY:
		addr += IMAGE_HEADER_SIZE;
		break;
	case IMAGE_SEG_TAILER:
		flash_read(image_priv.flash[image_priv.seq], addr + offsetof(section_header_t, body_len),
				   &body_len, sizeof(body_len));
		addr += IMAGE_HEADER_SIZE + body_len;
		break;
	default :
		IMAGE_ERR("seg %d\n", seg);
		return 0;
	}

	addr += offset;

	return flash_write(image_priv.flash[image_priv.seq], addr, buf, size);
}

static uint16_t image_checksum16(uint8_t *data, uint32_t len)
{
	uint16_t cs = 0;
	uint16_t *p = (uint16_t *)data;

	while (len > 1) {
		cs += *p++;
		len -= 2;
	}
	if (len) {
		cs += *(uint8_t *)p;
	}

	return cs;
}

/**
 * @brief Get 16-bit checksum of the data buffer
 * @param[in] buf Pointer to the data buffer
 * @param[in] len length of the data buffer
 * @return 16-bit checksum
 */
uint16_t image_get_checksum(void *buf, uint32_t len)
{
	if (buf == NULL) {
		IMAGE_ERR("buf %p\n", buf);
		return 0;
	}

	return image_checksum16(buf, len);
}

/**
 * @brief Check vadility of the section header
 * @param[in] sh Pointer to the section header
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
image_val_t image_check_header(section_header_t *sh)
{
	if (sh == NULL) {
		IMAGE_ERR("sh %p\n", sh);
		return IMAGE_INVALID;
	}

	if ((sh->magic_number != IMAGE_MAGIC_NUMBER)
		|| (0xFFFF != image_get_checksum(sh, IMAGE_HEADER_SIZE))) {
		IMAGE_WARN("%s(), %d, magic number %#010x, checksum %#06x\n", __func__, __LINE__,
				   sh->magic_number, image_get_checksum(sh, IMAGE_HEADER_SIZE));
		return IMAGE_INVALID;
	}

	return IMAGE_VALID;
}

/**
 * @brief Check vadility of the section data(body and tailer)
 * @param[in] sh Pointer to the section header
 * @param[in] body Pointer to the section body
 * @param[in] body_len Length of the section body
 * @param[in] tailer Pointer to the section tailer
 * @param[in] tailer_len Length of the section tailer
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
image_val_t image_check_data(section_header_t *sh, void *body, uint32_t body_len,
							 void *tailer, uint32_t tailer_len)
{
	uint8_t joint[2];
	uint16_t data_checksum;

	if ((sh == NULL) || (body == NULL) || (body_len == 0)
		|| ((tailer == NULL) && (tailer_len != 0))) {
		IMAGE_ERR("sh %p, body %p, body len %d, tailer %p, tailer len %d\n",
				  sh, body, body_len, tailer, tailer_len);
		return IMAGE_INVALID;
	}

	if (tailer_len == 0) {
		data_checksum = sh->data_chksum
						+ image_checksum16(body, body_len);
	} else if ((body_len & 0x1) == 0) {
		data_checksum = sh->data_chksum
						+ image_checksum16(body, body_len)
						+ image_checksum16(tailer, tailer_len);
	} else {
		image_memcpy(joint, ((uint8_t *)body) + body_len - 1, 1);
		image_memcpy(joint + 1, tailer, 1);
		data_checksum = sh->data_chksum
						+ image_checksum16(body, body_len - 1)
						+ image_checksum16(joint, 2)
						+ image_checksum16(((uint8_t *)tailer) + 1, tailer_len - 1);
	}

	if (data_checksum != 0xFFFF) {
		IMAGE_WARN("%s(), %d, checksum %#06x\n", __func__, __LINE__, data_checksum);
		return IMAGE_INVALID;
	}

	return IMAGE_VALID;
}

static image_val_t _image_check_section(uint32_t flash, uint32_t addr)
{
	uint16_t			data_chksum;
	uint32_t			data_size;
	uint8_t			   *buf_check;
	section_header_t   *sh;

	sh = (section_header_t *)image_malloc(IMAGE_HEADER_SIZE);
	if (sh == NULL) {
		IMAGE_ERR("sh %p\n", sh);
		return IMAGE_INVALID;
	}

	flash_read(flash, addr, sh, IMAGE_HEADER_SIZE);
	if (image_check_header(sh) == IMAGE_INVALID) {
		IMAGE_ERR("flash %d, addr %#010x\n", flash, addr);
		image_free(sh);
		return IMAGE_INVALID;
	}

	data_chksum = sh->data_chksum;
	data_size = sh->data_size;
	image_free(sh);

	buf_check = (uint8_t *)image_malloc(IMAGE_CHECK_SIZE);
	if (buf_check == NULL) {
		IMAGE_ERR("buf_check %p\n", buf_check);
		return IMAGE_INVALID;
	}

	addr += IMAGE_HEADER_SIZE;
	while (data_size > 0) {
		if (data_size >= IMAGE_CHECK_SIZE) {
			flash_read(flash, addr, buf_check, IMAGE_CHECK_SIZE);
			data_chksum += image_checksum16(buf_check, IMAGE_CHECK_SIZE);
			addr += IMAGE_CHECK_SIZE;
			data_size -= IMAGE_CHECK_SIZE;
		} else {
			flash_read(flash, addr, buf_check, data_size);
			data_chksum += image_checksum16(buf_check, data_size);
			addr += data_size;
			data_size -= data_size;
		}
	}
	image_free(buf_check);

	if (data_chksum != 0xFFFF) {
		IMAGE_WARN("%s(), %d, data checksum %#06x\n", __func__, __LINE__, data_chksum);
		return IMAGE_INVALID;
	}

	return IMAGE_VALID;
}

/**
 * @brief Check vadility of the specified section in specified image
 * @param[in] seq Sequence of the specified image
 * @param[in] id Section ID of the specified section
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
image_val_t image_check_section(image_seq_t seq, uint32_t id)
{
	uint32_t	addr;

	addr = image_get_addr(seq, id);
	if (addr == IMAGE_INVALID_ADDR) {
		IMAGE_ERR("seq %d, id %#010x, addr %#010x\n", seq, id, addr);
		return IMAGE_INVALID;
	}

	IMAGE_DBG("%s(), %d, seq %d, flash %d, id %#010x, addr %#010x\n",
			  __func__, __LINE__, seq, image_priv.flash[seq], id, addr);

	return _image_check_section(image_priv.flash[seq], addr);
}

/**
 * @brief Check vadility of all the sections in specified image
 * @param[in] seq Sequence of the specified image
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
image_val_t image_check_sections(image_seq_t seq)
{
	uint32_t	addr;
	uint32_t	next_addr = image_priv.boot_size;

	IMAGE_DBG("%s(), %d, seq %d, next addr %#010x\n", __func__, __LINE__, seq, next_addr);

	while (next_addr != IMAGE_INVALID_ADDR) {
		addr = image_priv.addr[seq] + next_addr;
		if (_image_check_section(image_priv.flash[seq], addr) == IMAGE_INVALID) {
			IMAGE_WARN("%s(), %d, seq %d, flash %d, addr %#010x\n",
					   __func__, __LINE__, seq, image_priv.flash[seq], addr);
			return IMAGE_INVALID;
		}
		flash_read(image_priv.flash[seq], addr + offsetof(section_header_t, next_addr),
				   &next_addr, sizeof(next_addr));
	}

	return IMAGE_VALID;
}

