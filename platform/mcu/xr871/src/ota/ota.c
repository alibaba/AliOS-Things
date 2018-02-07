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

#include "ota_i.h"
#include "ota_debug.h"
#include "ota_file.h"
#include "ota_http.h"
#include "sys/ota.h"
#include "sys/fdcm.h"
#include "sys/image.h"
#include "driver/chip/hal_crypto.h"
#include "driver/chip/hal_flash.h"
#include "driver/chip/hal_wdg.h"

static ota_priv_t	ota_priv;

/**
 * @brief Initialize the OTA service according to the specified parameters
 * @param[in] param Pointer to image_ota_param_t structure
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_init(image_ota_param_t *param)
{
	if (param == NULL) {
		OTA_ERR("param %p\n", param);
		return OTA_STATUS_ERROR;
	}

	ota_priv.flash[IMAGE_SEQ_1ST]	= param->flash[IMAGE_SEQ_1ST];
	ota_priv.addr[IMAGE_SEQ_1ST]	= param->addr[IMAGE_SEQ_1ST];
	ota_priv.flash[IMAGE_SEQ_2ND]	= param->flash[IMAGE_SEQ_2ND];
	ota_priv.addr[IMAGE_SEQ_2ND]	= param->addr[IMAGE_SEQ_2ND];
	ota_priv.image_size				= param->image_size;
	ota_priv.boot_size				= param->boot_size;
	ota_priv.get_size				= 0;

	OTA_DBG("%s(), %d, flash_1st %d, addr_1st %#010x, flash_2nd %d, "
			"addr_2nd %#010x, image_size %#010x, boot_size %#010x\n",
			__func__, __LINE__,
			ota_priv.flash[IMAGE_SEQ_1ST], ota_priv.addr[IMAGE_SEQ_1ST],
			ota_priv.flash[IMAGE_SEQ_2ND], ota_priv.addr[IMAGE_SEQ_2ND],
			ota_priv.image_size, ota_priv.boot_size);

	return OTA_STATUS_OK;
}

/**
 * @brief DeInitialize the OTA service
 * @return None
 */
void ota_deinit(void)
{
	ota_priv.flash[IMAGE_SEQ_1ST]	= 0;
	ota_priv.addr[IMAGE_SEQ_1ST]	= IMAGE_INVALID_ADDR;
	ota_priv.flash[IMAGE_SEQ_2ND]	= 0;
	ota_priv.addr[IMAGE_SEQ_2ND]	= IMAGE_INVALID_ADDR;
	ota_priv.image_size				= 0;
	ota_priv.boot_size				= 0;
	ota_priv.get_size				= 0;
}

static ota_status_t ota_read_boot_cfg(ota_boot_cfg_t *boot_cfg)
{
	fdcm_handle_t *fdcm_hdl;

	fdcm_hdl = fdcm_open(ota_priv.flash[IMAGE_SEQ_2ND], ota_priv.addr[IMAGE_SEQ_2ND], ota_priv.boot_size);
	if (fdcm_hdl == NULL) {
		OTA_ERR("fdcm hdl %p\n", fdcm_hdl);
		return OTA_STATUS_ERROR;
	}

	if (fdcm_read(fdcm_hdl, boot_cfg, OTA_BOOT_CFG_SIZE) != OTA_BOOT_CFG_SIZE) {
		OTA_ERR("fdcm read failed\n");
		fdcm_close(fdcm_hdl);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, boot image %#06x, boot state %#06x\n",
			__func__, __LINE__, boot_cfg->boot_image, boot_cfg->boot_state);

	fdcm_close(fdcm_hdl);
	return OTA_STATUS_OK;
}

static ota_status_t ota_write_boot_cfg(ota_boot_cfg_t * boot_cfg)
{
	fdcm_handle_t *fdcm_hdl;

	fdcm_hdl = fdcm_open(ota_priv.flash[IMAGE_SEQ_2ND], ota_priv.addr[IMAGE_SEQ_2ND], ota_priv.boot_size);
	if (fdcm_hdl == NULL) {
		OTA_ERR("fdcm hdl %p\n", fdcm_hdl);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, boot image %#06x, boot state %#06x\n",
			__func__, __LINE__, boot_cfg->boot_image, boot_cfg->boot_state);

	if (fdcm_write(fdcm_hdl, boot_cfg, OTA_BOOT_CFG_SIZE) != OTA_BOOT_CFG_SIZE) {
		OTA_ERR("fdcm write failed\n");
		fdcm_close(fdcm_hdl);
		return OTA_STATUS_ERROR;
	}

	fdcm_close(fdcm_hdl);
	return OTA_STATUS_OK;
}

/**
 * @brief Get the OTA configuration
 * @param[in] cfg Pointer to OTA configuration
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_read_cfg(ota_cfg_t *cfg)
{
	ota_boot_cfg_t	boot_cfg;

	if ((cfg == NULL)
		|| (ota_priv.boot_size == 0)
		|| (ota_priv.addr[IMAGE_SEQ_2ND] == IMAGE_INVALID_ADDR)) {
		OTA_ERR("cfg %p, boot size %#010x, addr 2nd %#010x\n",
				cfg, ota_priv.boot_size, ota_priv.addr[IMAGE_SEQ_2ND]);
		return OTA_STATUS_ERROR;
	}

	if (ota_read_boot_cfg(&boot_cfg) != OTA_STATUS_OK) {
		OTA_ERR("read boot cfg failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, boot image %#06x, boot state %#06x\n",
			__func__, __LINE__, boot_cfg.boot_image, boot_cfg.boot_state);

	if (boot_cfg.boot_image == OTA_BOOT_IMAGE_1ST) {
		cfg->image = OTA_IMAGE_1ST;
	} else if (boot_cfg.boot_image == OTA_BOOT_IMAGE_2ND) {
		cfg->image = OTA_IMAGE_2ND;
	} else {
		OTA_ERR("boot image %#06x\n", boot_cfg.boot_image);
		return OTA_STATUS_ERROR;
	}

	if (boot_cfg.boot_state == OTA_BOOT_STATE_UNVERIFIED) {
		cfg->state = OTA_STATE_UNVERIFIED;
	} else if (boot_cfg.boot_state == OTA_BOOT_STATE_VERIFIED) {
		cfg->state = OTA_STATE_VERIFIED;
	} else {
		OTA_ERR("boot state %#06x\n", boot_cfg.boot_state);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, image %d, state %d\n", __func__, __LINE__, cfg->image, cfg->state);

	return OTA_STATUS_OK;
}

/**
 * @brief Set the OTA configuration
 * @param[in] cfg Pointer to OTA configuration
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_write_cfg(ota_cfg_t *cfg)
{
	ota_boot_cfg_t	boot_cfg;

	if ((cfg == NULL)
		|| (ota_priv.boot_size == 0)
		|| (ota_priv.addr[IMAGE_SEQ_2ND] == IMAGE_INVALID_ADDR)) {
		OTA_ERR("cfg %p, boot size %#010x, addr 2nd %#010x\n",
				cfg, ota_priv.boot_size, ota_priv.addr[IMAGE_SEQ_2ND]);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, image %d, state %d\n", __func__, __LINE__, cfg->image, cfg->state);

	if (cfg->image == OTA_IMAGE_1ST) {
		boot_cfg.boot_image = OTA_BOOT_IMAGE_1ST;
	} else if (cfg->image == OTA_IMAGE_2ND) {
		boot_cfg.boot_image = OTA_BOOT_IMAGE_2ND;
	} else {
		OTA_ERR("image %d\n", cfg->image);
		return OTA_STATUS_ERROR;
	}

	if (cfg->state == OTA_STATE_UNVERIFIED) {
		boot_cfg.boot_state= OTA_BOOT_STATE_UNVERIFIED;
	} else if (cfg->state == OTA_STATE_VERIFIED) {
		boot_cfg.boot_state = OTA_BOOT_STATE_VERIFIED;
	} else {
		OTA_ERR("tate %d\n", cfg->state);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, boot image %#06x, boot state %#06x\n",
			__func__, __LINE__, boot_cfg.boot_image, boot_cfg.boot_state);

	if (ota_write_boot_cfg(&boot_cfg) != OTA_STATUS_OK) {
		OTA_ERR("write boot cfg failed\n");
		return OTA_STATUS_ERROR;
	}

	return OTA_STATUS_OK;
}

static ota_status_t ota_erase_flash(uint32_t flash, uint32_t addr, uint32_t size)
{
	uint32_t		start;
	uint32_t		multiples;
	FlashEraseMode	erase_size;
	HAL_Status		status;

	OTA_DBG("%s(), %d, flash %d, addr %#010x, size %#010x\n",
			__func__, __LINE__, flash, addr, size);

	if ((size >= (64 << 10))
		&& (HAL_Flash_MemoryOf(flash, FLASH_ERASE_64KB, addr, &start) == HAL_OK)
		&& (addr == start)
		&& ((size & 0xFFFF) == 0)) {
		multiples = size / (64 << 10);
		erase_size = FLASH_ERASE_64KB;
	} else if ((size >= (32 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_32KB, addr, &start) == HAL_OK)
			   && (addr == start)
			   && ((size & 0x7FFF) == 0)) {
		multiples = size / (32 << 10);
		erase_size = FLASH_ERASE_32KB;
	} else if ((size >= (4 << 10))
			   && (HAL_Flash_MemoryOf(flash, FLASH_ERASE_4KB, addr, &start) == HAL_OK)
			   && (addr == start)
			   && ((size & 0xFFF) == 0)) {
		multiples = size / (4 << 10);
		erase_size = FLASH_ERASE_4KB;
	} else {
		OTA_ERR("flash %d, addr %#010x, size %#010x\n", flash, addr, size);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, erase size %#010x, multiples %d\n",
			__func__, __LINE__, erase_size, multiples);

	if (HAL_Flash_Open(flash, OTA_FLASH_TIMEOUT) != HAL_OK) {
		OTA_ERR("flash %d\n", flash);
		return OTA_STATUS_ERROR;
	}

	status = HAL_Flash_Erase(flash, erase_size, addr, multiples);
	if (status != HAL_OK) {
		OTA_ERR("status %d\n", status);
		HAL_Flash_Close(flash);
		return OTA_STATUS_ERROR;
	}

	HAL_Flash_Close(flash);
	return OTA_STATUS_OK;
}

static ota_status_t ota_update_image_process(image_seq_t seq, void *url,
											 ota_update_init_t init_cb,
											 ota_update_get_t get_cb)
{
	ota_status_t	status;
	uint32_t		flash;
	uint32_t		addr;
	uint32_t		boot_size;
	uint32_t		recv_size;
	uint32_t		image_size;
	uint8_t		   *ota_buf;
	uint8_t			eof_flag;
	uint8_t			cnt;

	flash = ota_priv.flash[seq];
	addr = ota_priv.addr[seq] + ota_priv.boot_size;

	OTA_DBG("%s(), %d, seq %d, flash %d, addr %#010x\n", __func__, __LINE__, seq, flash, addr);
	OTA_SYSLOG("OTA: erase flash...\n");

	if (ota_erase_flash(flash, addr, ota_priv.image_size - ota_priv.boot_size) != OTA_STATUS_OK) {
		OTA_ERR("flash %d, addr %#010x, image size %#010x\n", flash, addr, ota_priv.image_size);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, erase flash success\n", __func__, __LINE__);

	if (init_cb(url) != OTA_STATUS_OK) {
		OTA_ERR("ota update init failed\n");
		return OTA_STATUS_ERROR;
	}

	ota_buf = ota_malloc(OTA_BUF_SIZE);
	if (ota_buf == NULL) {
		OTA_ERR("ota buf %p\n", ota_buf);
		return OTA_STATUS_ERROR;
	}

	OTA_SYSLOG("OTA: start loading image...\n");
	cnt = 0;

	ota_priv.get_size = 0;
	boot_size = ota_priv.boot_size;
	while (boot_size > 0) {
		if (boot_size > OTA_BUF_SIZE)
			status = get_cb(ota_buf, OTA_BUF_SIZE, &recv_size, &eof_flag);
		else
			status = get_cb(ota_buf, boot_size, &recv_size, &eof_flag);

		if ((status != OTA_STATUS_OK) || eof_flag) {
			OTA_ERR("status %d, eof %d\n", status, eof_flag);
			ota_free(ota_buf);
			return OTA_STATUS_ERROR;
		}
		boot_size -= recv_size;
		ota_priv.get_size += recv_size;

		cnt++;
		if (cnt == 100) {
			OTA_SYSLOG("OTA: loading image(%#010x)...\n", ota_priv.get_size);
			cnt = 0;
		}
	}

	OTA_DBG("%s(), %d, load bootloader success\n", __func__, __LINE__);

	if (HAL_Flash_Open(flash, OTA_FLASH_TIMEOUT) != HAL_OK) {
		OTA_ERR("flash %d\n", flash);
		ota_free(ota_buf);
		return OTA_STATUS_ERROR;
	}

	image_size = ota_priv.image_size;
	while (image_size > 0) {
		if (image_size > OTA_BUF_SIZE)
			status = get_cb(ota_buf, OTA_BUF_SIZE, &recv_size, &eof_flag);
		else
			status = get_cb(ota_buf, image_size, &recv_size, &eof_flag);

		if (status != OTA_STATUS_OK) {
			OTA_ERR("status %d\n", status);
			HAL_Flash_Close(flash);
			ota_free(ota_buf);
			return OTA_STATUS_ERROR;
		}
		image_size -= recv_size;
		ota_priv.get_size += recv_size;

		if (HAL_Flash_Write(flash, addr, ota_buf, recv_size) != HAL_OK) {
			OTA_ERR("flash %d, addr %#010x, size %#010x\n", flash, addr, recv_size);
			HAL_Flash_Close(flash);
			ota_free(ota_buf);
			return OTA_STATUS_ERROR;
		}
		addr += recv_size;
		if (eof_flag)
			break;

		cnt++;
		if (cnt == 100) {
			OTA_SYSLOG("OTA: loading image(%#010x)...\n", ota_priv.get_size);
			cnt = 0;
		}
	}

	HAL_Flash_Close(flash);
	ota_free(ota_buf);

	OTA_SYSLOG("OTA: finish loading image(%#010x).\n", ota_priv.get_size);

	if (image_check_sections(seq) != IMAGE_VALID) {
		OTA_ERR("ota check image failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_SYSLOG("OTA: finish checking image.\n");
	return OTA_STATUS_OK;
}

static ota_status_t ota_update_image(void *url,
									 ota_update_init_t init_cb,
									 ota_update_get_t get_cb)
{
	ota_boot_cfg_t	boot_cfg;
	image_seq_t		seq;

	if ((ota_priv.image_size == 0) || (ota_priv.boot_size == 0)) {
		OTA_ERR("need to init, image size %#010x, boot size %#010x\n",
				ota_priv.image_size, ota_priv.boot_size);
		return OTA_STATUS_ERROR;
	}

	if (ota_read_boot_cfg(&boot_cfg) != OTA_STATUS_OK) {
		OTA_ERR("read boot cfg failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, boot image %#06x, boot state %#06x\n",
			__func__, __LINE__, boot_cfg.boot_image, boot_cfg.boot_state);

	if (((boot_cfg.boot_image == OTA_BOOT_IMAGE_1ST) && (boot_cfg.boot_state == OTA_BOOT_STATE_UNVERIFIED))
		|| ((boot_cfg.boot_image == OTA_BOOT_IMAGE_2ND) && (boot_cfg.boot_state == OTA_BOOT_STATE_VERIFIED))) {
		seq = IMAGE_SEQ_1ST;
		boot_cfg.boot_image = OTA_BOOT_IMAGE_1ST;
	} else if (((boot_cfg.boot_image == OTA_BOOT_IMAGE_1ST) && (boot_cfg.boot_state == OTA_BOOT_STATE_VERIFIED))
			   || ((boot_cfg.boot_image == OTA_BOOT_IMAGE_2ND) && (boot_cfg.boot_state == OTA_BOOT_STATE_UNVERIFIED))) {
		seq = IMAGE_SEQ_2ND;
		boot_cfg.boot_image = OTA_BOOT_IMAGE_2ND;
	} else {
		OTA_ERR("image %#06x, state %#06x\n", boot_cfg.boot_image, boot_cfg.boot_state);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, seq %d\n", __func__, __LINE__, seq);

	if (ota_update_image_process(seq, url, init_cb, get_cb) != OTA_STATUS_OK) {
		OTA_ERR("ota update image failed\n");
		return OTA_STATUS_ERROR;
	}

	boot_cfg.boot_state = OTA_BOOT_STATE_UNVERIFIED;
	return ota_write_boot_cfg(&boot_cfg);
}

/**
 * @brief Get the image file with the specified protocol and write to flash
 * @param[in] protocol Pointer to the protocol of getting image file
 * @param[in] url URL of the image file
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_get_image(ota_protocol_t protocol, void *url)
{
	if (url == NULL) {
		OTA_ERR("url %p\n", url);
		return OTA_STATUS_ERROR;
	}

	switch (protocol) {
	case OTA_PROTOCOL_FILE:
		return ota_update_image(url, ota_update_file_init, ota_update_file_get);
	case OTA_PROTOCOL_HTTP:
		return ota_update_image(url, ota_update_http_init, ota_update_http_get);
	default:
		OTA_ERR("invalid protocol %d\n", protocol);
		return OTA_STATUS_ERROR;
	}
}

static ota_status_t ota_verify_image_append_process(uint32_t flash,
													uint32_t addr,
													uint32_t size,
													ota_verify_append_t append,
													void *hdl)
{
	uint32_t read_size;
	uint8_t *buf;

	buf = ota_malloc(OTA_BUF_SIZE);
	if (buf == NULL) {
		OTA_ERR("buf %p\n", buf);
		return OTA_STATUS_ERROR;
	}

	if (HAL_Flash_Open(flash, OTA_FLASH_TIMEOUT) != HAL_OK) {
		OTA_ERR("flash %d\n", flash);
		ota_free(buf);
		return OTA_STATUS_ERROR;
	}

	while (size > 0) {
		read_size = size > OTA_BUF_SIZE ? OTA_BUF_SIZE : size;
		if ((HAL_Flash_Read(flash, addr, buf, read_size) != HAL_OK)
			|| (append(hdl, buf, read_size) != HAL_OK)) {
			OTA_ERR("flash %d, addr %#010x, size %#010x\n", flash, addr, read_size);
			HAL_Flash_Close(flash);
			ota_free(buf);
			return OTA_STATUS_ERROR;
		}
		size -= read_size;
		addr += read_size;
	}

	HAL_Flash_Close(flash);
	ota_free(buf);
	return OTA_STATUS_OK;
}

static ota_status_t ota_verify_image_append(ota_boot_cfg_t *cfg,
											ota_verify_append_t append,
											void *hdl)
{
	image_seq_t seq;

	if (cfg->boot_image == OTA_BOOT_IMAGE_1ST) {
		seq = IMAGE_SEQ_1ST;
	} else if (cfg->boot_image == OTA_BOOT_IMAGE_2ND) {
		seq = IMAGE_SEQ_2ND;
	} else {
		OTA_ERR("invalid boot image %#06x\n", cfg->boot_image);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, image %#06x, seq %d\n", __func__, __LINE__, cfg->boot_image, seq);

	if ((ota_verify_image_append_process(ota_priv.flash[IMAGE_SEQ_1ST],
										 ota_priv.addr[IMAGE_SEQ_1ST],
										 ota_priv.boot_size,
										 append,
										 hdl) != OTA_STATUS_OK)
		|| (ota_verify_image_append_process(ota_priv.flash[seq],
											ota_priv.addr[seq] + ota_priv.boot_size,
											ota_priv.get_size - ota_priv.boot_size,
											append,
											hdl) != OTA_STATUS_OK)) {
		OTA_ERR("append failed, seq %d\n", seq);
		return OTA_STATUS_ERROR;
	}

	return OTA_STATUS_OK;
}

static inline ota_status_t ota_verify_image_none(ota_boot_cfg_t *cfg, uint32_t *value)
{
	cfg->boot_state = OTA_BOOT_STATE_VERIFIED;
	return OTA_STATUS_OK;
}

static ota_status_t ota_verify_image_crc32(ota_boot_cfg_t *cfg, uint32_t *value)
{
	CE_CRC_Handler	hdl;
	uint32_t		crc;

	if (HAL_CRC_Init(&hdl, CE_CRC32, ota_priv.get_size) != HAL_OK) {
		OTA_ERR("CRC init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(cfg,
								(ota_verify_append_t)HAL_CRC_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("CRC append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_CRC_Finish(&hdl, &crc) != HAL_OK) {
		OTA_ERR("CRC finish failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_memcmp(value, &crc, sizeof(crc)) == 0) {
		cfg->boot_state = OTA_BOOT_STATE_VERIFIED;
	} else {
		OTA_WARN("%s(), %d, value %#010x, crc %#010x\n",
				 __func__, __LINE__, *value, crc);
		cfg->boot_state = OTA_BOOT_STATE_UNVERIFIED;
	}

	OTA_DBG("%s(), %d, state %#06x, value %#010x, crc %#010x\n",
			__func__, __LINE__, cfg->boot_state, *value, crc);

	return OTA_STATUS_OK;
}

static ota_status_t ota_verify_image_md5(ota_boot_cfg_t *cfg, uint32_t *value)
{
	CE_MD5_Handler	hdl;
	uint32_t		digest[4];

	if (HAL_MD5_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("MD5 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(cfg,
								(ota_verify_append_t)HAL_MD5_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("MD5 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_MD5_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("MD5 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		cfg->boot_state = OTA_BOOT_STATE_VERIFIED;
	} else {
		OTA_WARN("%s(), %d, value[0] %#010x, digest[0] %#010x\n",
				 __func__, __LINE__, value[0], digest[0]);
		cfg->boot_state = OTA_BOOT_STATE_UNVERIFIED;
	}

	OTA_DBG("%s(), %d, state %#06x, value[0] %#010x, digest[0] %#010x\n",
			__func__, __LINE__, cfg->boot_state, value[0], digest[0]);

	return OTA_STATUS_OK;
}

static ota_status_t ota_verify_image_sha1(ota_boot_cfg_t *cfg, uint32_t *value)
{
	CE_SHA1_Handler	hdl;
	uint32_t		digest[5];

	if (HAL_SHA1_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("SHA1 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(cfg,
								(ota_verify_append_t)HAL_SHA1_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("SHA1 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_SHA1_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("SHA1 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		cfg->boot_state = OTA_BOOT_STATE_VERIFIED;
	} else {
		OTA_WARN("%s(), %d, value[0] %#010x, digest[0] %#010x\n",
				 __func__, __LINE__, value[0], digest[0]);
		cfg->boot_state = OTA_BOOT_STATE_UNVERIFIED;
	}

	OTA_DBG("%s(), %d, state %#06x, value[0] %#010x, digest[0] %#010x\n",
			__func__, __LINE__, cfg->boot_state, value[0], digest[0]);

	return OTA_STATUS_OK;
}

static ota_status_t ota_verify_image_sha256(ota_boot_cfg_t *cfg, uint32_t *value)
{
	CE_SHA256_Handler	hdl;
	uint32_t			digest[8];

	if (HAL_SHA256_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("SHA256 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(cfg,
								(ota_verify_append_t)HAL_SHA256_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("SHA256 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_SHA256_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("SHA256 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		cfg->boot_state = OTA_BOOT_STATE_VERIFIED;
	} else {
		OTA_WARN("%s(), %d, value[0] %#010x, digest[0] %#010x\n",
				 __func__, __LINE__, value[0], digest[0]);
		cfg->boot_state = OTA_BOOT_STATE_UNVERIFIED;
	}

	OTA_DBG("%s(), %d, state %#06x, value[0] %#010x, digest[0] %#010x\n",
			__func__, __LINE__, cfg->boot_state, value[0], digest[0]);

	return OTA_STATUS_OK;
}

/**
 * @brief Verify the image file and modify OTA configuration correspondingly
 * @param[in] verify Verification algorithm
 * @param[in] value Pointer to standard value of the verification algorithm
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_verify_image(ota_verify_t verify, uint32_t *value)
{
	ota_boot_cfg_t	boot_cfg;
	ota_status_t	status;

	if ((verify != OTA_VERIFY_NONE) && (value == NULL)) {
		OTA_ERR("verify %d, res %p\n", verify, value);
		return OTA_STATUS_ERROR;
	}

	if (ota_priv.get_size == 0) {
		OTA_ERR("need to get image, get size %#010x\n", ota_priv.get_size);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, verify %d, get size %#010x\n",
			__func__, __LINE__, verify, ota_priv.get_size);

	if (ota_read_boot_cfg(&boot_cfg) != OTA_STATUS_OK) {
		OTA_ERR("read boot cfg failed\n");
		return OTA_STATUS_ERROR;
	}

	switch (verify) {
	case OTA_VERIFY_NONE:
		status = ota_verify_image_none(&boot_cfg, value);
		break;
	case OTA_VERIFY_CRC32:
		status = ota_verify_image_crc32(&boot_cfg, value);
		break;
	case OTA_VERIFY_MD5:
		status = ota_verify_image_md5(&boot_cfg, value);
		break;
	case OTA_VERIFY_SHA1:
		status = ota_verify_image_sha1(&boot_cfg, value);
		break;
	case OTA_VERIFY_SHA256:
		status = ota_verify_image_sha256(&boot_cfg, value);
		break;
	default:
		OTA_ERR("invalid verify %d\n", verify);
		return OTA_STATUS_ERROR;
	}

	if (status != OTA_STATUS_OK) {
		OTA_ERR("status %d, verify %d\n", status, verify);
		return OTA_STATUS_ERROR;
	}

	return ota_write_boot_cfg(&boot_cfg);
}

/**
 * @brief Reboot system
 * @return None
 */
void ota_reboot_system(void)
{
	OTA_DBG("%s(), %d, OTA: reboot.\n", __func__, __LINE__);
	HAL_WDG_Reboot();
}

void ota_set_size(uint32_t size)
{
	ota_priv.get_size = size;

}
