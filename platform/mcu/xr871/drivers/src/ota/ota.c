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
#include "sys/image.h"
#include "driver/chip/hal_crypto.h"
#include "driver/chip/hal_flash.h"
#include "driver/chip/hal_wdg.h"

#define OTA_IMG_DATA_CORRUPTION_TEST	0 /* make image data corruption, for test only */

#define OTA_UPDATE_DEBUG_SIZE_UNIT		(50 * 1024)

static ota_priv_t	ota_priv;

/* indexed by image_seq_t */
static const image_seq_t ota_update_seq_policy[IMAGE_SEQ_NUM] = {
#if (IMAGE_SEQ_NUM == 2)
	/* update policy: 0 -> 1, 1 -> 0 */
	1, 0
#elif (IMAGE_SEQ_NUM == 3)
	/* update policy: 0 -> 1, 1 -> 2, 2 -> 1 */
	1, 2, 1
#else
	#error "unsupport IMAGE_SEQ_NUM!"
#endif
};

/**
 * @brief Get the image sequence which to be updated by OTA
 * @return Image sequence to be updated
 */
static image_seq_t ota_get_update_seq(void)
{
	image_seq_t seq = IMAGE_SEQ_NUM;
	const image_ota_param_t *iop = ota_priv.iop;

	if (iop == NULL) {
		OTA_ERR("not init");
		return seq;
	}

	if ((iop->img_max_size == 0) || (iop->bl_size == 0) ||
		(iop->running_seq >= IMAGE_SEQ_NUM)) {
		OTA_ERR("not init, img_max_size %#x, bl_size %#x, running seq %u\n",
				iop->img_max_size, iop->bl_size, iop->running_seq);
		return seq;
	}

	seq = ota_update_seq_policy[iop->running_seq]; /* update sequence */
	OTA_DBG("%s(), update seq %d\n", __func__, seq);
	return seq;
}

/**
 * @brief Initialize the OTA service
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_init(void)
{
	ota_memset(&ota_priv, 0, sizeof(ota_priv));
	ota_priv.iop = image_get_ota_param();
	return OTA_STATUS_OK;
}

/**
 * @brief DeInitialize the OTA service
 * @return None
 */
void ota_deinit(void)
{
	ota_memset(&ota_priv, 0, sizeof(ota_priv));
}

/* FIXME: Ugly! Used internal APIs from image module to save code size. */
extern int flash_erase(uint32_t flash, uint32_t addr, uint32_t size);

static ota_status_t ota_update_image_process(image_seq_t seq, void *url,
											 ota_update_init_t init_cb,
											 ota_update_get_t get_cb)
{
	ota_status_t	status;
	uint32_t		flash;
	uint32_t		addr;
	uint32_t		bl_size;
	uint32_t		recv_size;
	uint32_t		img_max_size;
	uint8_t		   *ota_buf;
	uint8_t			eof_flag;
	uint32_t		debug_size;
	ota_status_t	ret = OTA_STATUS_ERROR;
	const image_ota_param_t *iop = ota_priv.iop;

	flash = iop->flash[seq];
	addr = iop->addr[seq];
	img_max_size = iop->img_max_size;

	OTA_DBG("%s(), seq %d, flash %u, addr %#x\n", __func__, seq, flash, addr);
	OTA_SYSLOG("OTA: erase flash...\n");

	if (flash_erase(flash, addr, img_max_size) != 0) {
		return ret;
	}

	OTA_DBG("%s(), erase flash success\n", __func__);

	ota_buf = ota_malloc(OTA_BUF_SIZE);
	if (ota_buf == NULL) {
		OTA_ERR("no mem\n");
		return ret;
	}

	if (init_cb(url) != OTA_STATUS_OK) {
		OTA_ERR("ota update init failed\n");
		goto ota_err;
	}

	OTA_SYSLOG("OTA: start loading image...\n");
	debug_size = OTA_UPDATE_DEBUG_SIZE_UNIT;
	ota_priv.get_size = 0;

	/* skip bootloader */
	bl_size = iop->bl_size;
	while (bl_size > 0) {
		status = get_cb(ota_buf,
		                (bl_size > OTA_BUF_SIZE) ? OTA_BUF_SIZE : bl_size,
		                &recv_size, &eof_flag);
		if ((status != OTA_STATUS_OK) || eof_flag) {
			OTA_ERR("status %d, eof %d\n", status, eof_flag);
			goto ota_err;
		}
		bl_size -= recv_size;
		ota_priv.get_size += recv_size;

		if (ota_priv.get_size >= debug_size) {
			OTA_SYSLOG("OTA: loading image (%u KB)...\n",
			           ota_priv.get_size / 1024);
			debug_size += OTA_UPDATE_DEBUG_SIZE_UNIT;
		}
	}

	OTA_DBG("%s(), skip bootloader success\n", __func__);

	if (HAL_Flash_Open(flash, OTA_FLASH_TIMEOUT) != HAL_OK) {
		OTA_ERR("open flash %u fail\n", flash);
		goto ota_err;
	}

	OTA_DBG("image max size %u\n", img_max_size);
#if OTA_IMG_DATA_CORRUPTION_TEST
	OTA_SYSLOG("ota img data corruption test start, pls power down the device\n");
#endif
	while (img_max_size > 0) {
		status = get_cb(ota_buf, (img_max_size > OTA_BUF_SIZE) ? OTA_BUF_SIZE : img_max_size,
		                &recv_size, &eof_flag);
		if (status != OTA_STATUS_OK) {
			OTA_ERR("status %d\n", status);
			break;
		}
		img_max_size -= recv_size;
		ota_priv.get_size += recv_size;

		if (HAL_Flash_Write(flash, addr, ota_buf, recv_size) != HAL_OK) {
			OTA_ERR("write flash fail, flash %u, addr %#x, size %#x\n",
			        flash, addr, recv_size);
			break;
		}
		addr += recv_size;
		if (eof_flag) {
			ret = OTA_STATUS_OK;
			break;
		}

		if (ota_priv.get_size >= debug_size) {
			OTA_SYSLOG("OTA: loading image (%u KB)...\n",
			           ota_priv.get_size / 1024);
			debug_size += OTA_UPDATE_DEBUG_SIZE_UNIT;
		}
	}
#if OTA_IMG_DATA_CORRUPTION_TEST
	OTA_SYSLOG("ota img data corruption test end\n");
#endif

	HAL_Flash_Close(flash);

ota_err:
	if (ota_buf)
		ota_free(ota_buf);

	if (ret != OTA_STATUS_OK) {
		if (img_max_size == 0) {
			/* reach max size, but not end, continue trying to check sections */
			OTA_ERR("download img size %u == %u, but not end\n",
			        ota_priv.get_size - iop->bl_size, iop->img_max_size);
		} else {
			return ret;
		}
	}

	OTA_SYSLOG("OTA: finish loading image(%#010x)\n", ota_priv.get_size);

	if (image_check_sections(seq) == IMAGE_INVALID) {
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
	image_seq_t		seq;

	seq = ota_get_update_seq();
	if (seq < IMAGE_SEQ_NUM) {
		return ota_update_image_process(seq, url, init_cb, get_cb);
	} else {
		return OTA_STATUS_ERROR;
	}
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

	ota_init();

	switch (protocol) {
#if OTA_OPT_PROTOCOL_FILE
	case OTA_PROTOCOL_FILE:
		return ota_update_image(url, ota_update_file_init, ota_update_file_get);
#endif
#if OTA_OPT_PROTOCOL_HTTP
	case OTA_PROTOCOL_HTTP:
		return ota_update_image(url, ota_update_http_init, ota_update_http_get);
#endif
	default:
		OTA_ERR("invalid protocol %d\n", protocol);
		return OTA_STATUS_ERROR;
	}
}

#if (OTA_OPT_EXTRA_VERIFY_CRC32 || OTA_OPT_EXTRA_VERIFY_MD5 || \
     OTA_OPT_EXTRA_VERIFY_SHA1 || OTA_OPT_EXTRA_VERIFY_SHA256)
static ota_status_t ota_verify_image_append_process(uint32_t flash,
													uint32_t addr,
													uint32_t size,
													ota_verify_append_t append,
													void *hdl)
{
	uint32_t read_size;
	uint8_t *buf = NULL;
	ota_status_t status = OTA_STATUS_ERROR;

	if (HAL_Flash_Open(flash, OTA_FLASH_TIMEOUT) != HAL_OK) {
		OTA_ERR("open flash %u fail\n", flash);
		return status;
	}

	buf = ota_malloc(OTA_BUF_SIZE);
	if (buf == NULL) {
		OTA_ERR("no mem\n");
		goto out;
	}

	while (size > 0) {
		read_size = size > OTA_BUF_SIZE ? OTA_BUF_SIZE : size;
		if (HAL_Flash_Read(flash, addr, buf, read_size) != HAL_OK) {
			OTA_ERR("read flash %u fail, addr %#x, size %#x\n",
			        flash, addr, read_size);
			break;
		}
		if (append(hdl, buf, read_size) != HAL_OK) {
			break;
		}
		size -= read_size;
		addr += read_size;
	}

	if (size == 0) {
		status = OTA_STATUS_OK;
	}

out:
	if (buf) {
		ota_free(buf);
	}
	HAL_Flash_Close(flash);
	return status;
}

static ota_status_t ota_verify_image_append(image_seq_t seq,
											ota_verify_append_t append,
											void *hdl)
{
	ota_priv_t *ota = &ota_priv;
	const image_ota_param_t *iop = ota->iop;

	OTA_DBG("%s(), seq %d\n", __func__, seq);

	if (ota_verify_image_append_process(IMG_BL_FLASH(iop),
	                                    IMG_BL_ADDR(iop),
	                                    iop->bl_size,
	                                    append,
	                                    hdl) != OTA_STATUS_OK) {
		OTA_ERR("append bl fail, seq %d\n", seq);
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append_process(iop->flash[seq],
	                                    iop->addr[seq],
	                                    ota->get_size - iop->bl_size,
	                                    append,
	                                    hdl) != OTA_STATUS_OK) {
		OTA_ERR("append image fail, seq %d\n", seq);
		return OTA_STATUS_ERROR;
	}

	return OTA_STATUS_OK;
}
#endif /* (OTA_OPT_EXTRA_VERIFY_CRC32 || OTA_OPT_EXTRA_VERIFY_MD5 || ...) */

static ota_status_t ota_verify_image_none(image_seq_t seq, uint32_t *value)
{
	return OTA_STATUS_OK;
}

#if OTA_OPT_EXTRA_VERIFY_CRC32
static ota_status_t ota_verify_image_crc32(image_seq_t seq, uint32_t *value)
{
	CE_CRC_Handler	hdl;
	uint32_t		crc;

	if (HAL_CRC_Init(&hdl, CE_CRC32, ota_priv.get_size) != HAL_OK) {
		OTA_ERR("CRC init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(seq,
								(ota_verify_append_t)HAL_CRC_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("CRC append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_CRC_Finish(&hdl, &crc) != HAL_OK) {
		OTA_ERR("CRC finish failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), value %#x, crc %#x\n", __func__, *value, crc);

	if (ota_memcmp(value, &crc, sizeof(crc)) == 0) {
		return OTA_STATUS_OK;
	} else {
		return OTA_STATUS_ERROR;
	}
}
#endif /* OTA_OPT_EXTRA_VERIFY_CRC32 */

#if OTA_OPT_EXTRA_VERIFY_MD5
static ota_status_t ota_verify_image_md5(image_seq_t seq, uint32_t *value)
{
	CE_MD5_Handler	hdl;
	uint32_t		digest[4];

	if (HAL_MD5_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("MD5 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(seq,
								(ota_verify_append_t)HAL_MD5_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("MD5 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_MD5_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("MD5 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), value[0] %#x, digest[0] %#x\n", __func__, value[0], digest[0]);

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		return OTA_STATUS_OK;
	} else {
		return OTA_STATUS_ERROR;
	}
}
#endif /* OTA_OPT_EXTRA_VERIFY_MD5 */

#if OTA_OPT_EXTRA_VERIFY_SHA1
static ota_status_t ota_verify_image_sha1(image_seq_t seq, uint32_t *value)
{
	CE_SHA1_Handler	hdl;
	uint32_t		digest[5];

	if (HAL_SHA1_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("SHA1 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(seq,
								(ota_verify_append_t)HAL_SHA1_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("SHA1 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_SHA1_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("SHA1 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), value[0] %#x, digest[0] %#x\n", __func__, value[0], digest[0]);

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		return OTA_STATUS_OK;
	} else {
		return OTA_STATUS_ERROR;
	}
}
#endif /* OTA_OPT_EXTRA_VERIFY_SHA1 */

#if OTA_OPT_EXTRA_VERIFY_SHA256
static ota_status_t ota_verify_image_sha256(image_seq_t seq, uint32_t *value)
{
	CE_SHA256_Handler	hdl;
	uint32_t			digest[8];

	if (HAL_SHA256_Init(&hdl, CE_CTL_IVMODE_SHA_MD5_FIPS180, NULL) != HAL_OK) {
		OTA_ERR("SHA256 init failed\n");
		return OTA_STATUS_ERROR;
	}

	if (ota_verify_image_append(seq,
								(ota_verify_append_t)HAL_SHA256_Append,
								(void *)&hdl) != OTA_STATUS_OK) {
		OTA_ERR("SHA256 append failed\n");
		return OTA_STATUS_ERROR;
	}

	if (HAL_SHA256_Finish(&hdl, digest) != HAL_OK) {
		OTA_ERR("SHA256 finish failed\n");
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), value[0] %#x, digest[0] %#x\n", __func__, value[0], digest[0]);

	if (ota_memcmp(value, digest, sizeof(digest)) == 0) {
		return OTA_STATUS_OK;
	} else {
		return OTA_STATUS_ERROR;
	}
}
#endif /* OTA_OPT_EXTRA_VERIFY_SHA256 */

/**
 * @brief Verify the image file and modify OTA configuration correspondingly
 * @param[in] verify Verification algorithm
 * @param[in] value Pointer to standard value of the verification algorithm
 * @retval ota_status_t, OTA_STATUS_OK on success
 */
ota_status_t ota_verify_image(ota_verify_t verify, uint32_t *value)
{
	ota_status_t	status;
	image_cfg_t		cfg;
	image_seq_t		seq;

	if ((verify != OTA_VERIFY_NONE) && (value == NULL)) {
		OTA_ERR("invalid args, verify %d, res %p\n", verify, value);
		return OTA_STATUS_ERROR;
	}

	seq = ota_get_update_seq();
	if (seq >= IMAGE_SEQ_NUM) {
		return OTA_STATUS_ERROR;
	}

	if (ota_priv.get_size == 0) {
		OTA_ERR("need to get image, get_size %#x\n", ota_priv.get_size);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), verify %d, size %#x\n", __func__, verify, ota_priv.get_size);

	switch (verify) {
	case OTA_VERIFY_NONE:
		status = ota_verify_image_none(seq, value);
		break;
#if OTA_OPT_EXTRA_VERIFY_CRC32
	case OTA_VERIFY_CRC32:
		status = ota_verify_image_crc32(seq, value);
		break;
#endif
#if OTA_OPT_EXTRA_VERIFY_MD5
	case OTA_VERIFY_MD5:
		status = ota_verify_image_md5(seq, value);
		break;
#endif
#if OTA_OPT_EXTRA_VERIFY_SHA1
	case OTA_VERIFY_SHA1:
		status = ota_verify_image_sha1(seq, value);
		break;
#endif
#if OTA_OPT_EXTRA_VERIFY_SHA256
	case OTA_VERIFY_SHA256:
		status = ota_verify_image_sha256(seq, value);
		break;
#endif
	default:
		OTA_ERR("invalid verify %d\n", verify);
		return OTA_STATUS_ERROR;
	}

	if (status != OTA_STATUS_OK) {
		OTA_ERR("verify fail, status %d, verify %d\n", status, verify);
		return OTA_STATUS_ERROR;
	}

	cfg.seq = seq;
	cfg.state = IMAGE_STATE_VERIFIED;
	return (image_set_cfg(&cfg) == 0 ? OTA_STATUS_OK : OTA_STATUS_ERROR);
}

/**
 * @brief Reboot system
 * @return None
 */
#if 0
void ota_reboot(void)
{
	OTA_DBG("OTA reboot.\n");
	HAL_WDG_Reboot();
}
#endif

 /**
  * @brief set ota get size
  * @return None
  */
 void ota_set_get_size(uint32_t get_size)
 {
	ota_priv.get_size = get_size;
 }
