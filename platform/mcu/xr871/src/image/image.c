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
#include "sys/image.h"
#include "image_debug.h"

#define OTA_IMG_CFG_SEQ_CORRUPTION_TEST     0 /* make image cfg seq corruption, for test only */
#define OTA_IMG_CFG_STATE_CORRUPTION_TEST   0 /* make image cfg state corruption, for test only */

#define image_malloc(l)			malloc(l)
#define image_free(p)			free(p)
#define image_memcpy(d, s, n)	memcpy(d, s, n)
#define image_memset(s, c, n) 	memset(s, c, n)
#define image_memcmp(a, b, l)	memcmp(a, b, l)

#define IMAGE_CHECK_SIZE		(1024)
#define IMAGE_EXPAND_SIZE		(5)

#define IMAGE_INVALID_FLASH 	(0xFF)
#define IMAGE_INVALID_OTA_SIZE	(0xFFFFFF)

#define IMAGE_INVALID_BL_VER  	(0)

typedef struct {
	uint32_t	id;
	uint32_t	addr;
} sec_addr_t;

typedef struct {
	image_ota_param_t	iop;
	uint8_t				sec_num[IMAGE_SEQ_NUM];
	sec_addr_t		   *sec_addr[IMAGE_SEQ_NUM];
} image_priv_t;

static image_priv_t	image_priv;

/* Definition of OTA area parameters in bootloader's section_header_t::priv[] */
#define BLSH_OTA_FLASH(sh)	((sh)->priv[0] & 0xFF)			 	/* flash ID of OTA area */
#define BLSH_OTA_SIZE(sh)	(((sh)->priv[0] >> 8) & 0xFFFFFF) 	/* size of OTA area */
#define BLSH_OTA_ADDR(sh)	((sh)->priv[1])						/* start addr of OTA area */

#define IMG_SEC_ADDR(iop, seq, off)	((iop)->addr[(seq)] + (off) - (iop)->bl_size)


static void image_clear_sec_addr(image_priv_t *img)
{
	image_seq_t seq;

	for (seq = 0; seq < IMAGE_SEQ_NUM; seq++) {
		img->sec_num[seq] = 0;
		if (img->sec_addr[seq]) {
			image_free(img->sec_addr[seq]);
			img->sec_addr[seq] = NULL;
		}
	}
}

#ifndef __CONFIG_BOOTLOADER

#if IMAGE_OPT_BL_COMPATIBILITY
/*
 * @brief Get the version of bootloader
 * @return Bootloader's version
 */
static uint32_t image_get_bl_ver(void)
{
	uint32_t ver;
	image_ota_param_t *iop = &image_priv.iop;
	uint32_t addr = IMG_BL_ADDR(iop) + offsetof(section_header_t, version);

	if (flash_read(IMG_BL_FLASH(iop), addr, &ver, sizeof(ver)) != sizeof(ver)) {
		return IMAGE_INVALID_BL_VER;
	}
	IMAGE_DBG("bl ver %u\n", ver);
	return ver;
}
#endif /* IMAGE_OPT_BL_COMPATIBILITY */

static image_seq_t image_init_running_seq(const image_ota_param_t *iop)
{
	image_cfg_t cfg;
	image_seq_t	seq = 0; /* default to the first image seq when error occur */

	if (iop->ota_addr == IMAGE_INVALID_ADDR) {
		IMAGE_DBG("invalid ota addr %#x\n", iop->ota_addr);
		return seq;
	}

	if (image_get_cfg(&cfg) != 0) {
		IMAGE_ERR("read img cfg failed\n");
		return seq;
	}
	IMAGE_DBG("img seq %d, state %d\n", cfg.seq, cfg.state);

#if IMAGE_OPT_BL_COMPATIBILITY
	uint32_t bl_ver = image_get_bl_ver();
	if (bl_ver == IMAGE_INVALID_BL_VER) {
		IMAGE_ERR("invalid bl ver %u\n", bl_ver);
		return seq;
	}

	/* for bl version < 0.3, image sequence MUST < 2 */
	if (bl_ver < 3 && cfg.seq > 1) {
		IMAGE_ERR("invalid img seq %u, bl ver %u\n", cfg.seq, bl_ver);
		return seq;
	}

	if (bl_ver < 2) {
		/* for bl version < 0.2, the verfied state is set by app.bin only */
		seq = cfg.seq;
//		cfg.state = IMAGE_STATE_VERIFIED;
	} else if (bl_ver == 2) {
		/* for bl version == 0.2, only support image sequence 0 and 1 */
		if (cfg.state == IMAGE_STATE_VERIFIED) {
			seq = cfg.seq;
		} else {
			seq = (cfg.seq + 1) % 2;
		}
	} else
#endif /* IMAGE_OPT_BL_COMPATIBILITY */
	{
		/* for bl version >= 0.3, only IMAGE_STATE_VERIFIED will be saved */
		if (cfg.state == IMAGE_STATE_VERIFIED) {
			seq = cfg.seq;
		} else {
			IMAGE_ERR("invalid state %d, seq %d\n", cfg.state, cfg.seq);
		}
	}

	return seq;
}

#endif /* __CONFIG_BOOTLOADER */

/**
 * @brief Initialize the image module
 * @param[in] flash Flash device number of the 1st image region
 * @param[in] addr Start address of the 1st image region (including bootloader)
 * @param[in] size Max size of the image region (including bootloader)
 * @retval 0 on success, -1 on failure
 */
int image_init(uint32_t flash, uint32_t addr, uint32_t max_size)
{
	section_header_t sh;
	image_ota_param_t *iop;
	int i;

	IMAGE_DBG("init: flash %u, addr %#x (%u KB), max_size %u KB\n",
	          flash, addr, addr / 1024, max_size / 1024);

	if (flash_read(flash, addr, &sh, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE) {
		return -1;
	}

	iop = &image_priv.iop;
	iop->bl_size = sh.next_addr;
	iop->img_max_size = max_size - iop->bl_size;
	iop->ota_flash = (BLSH_OTA_FLASH(&sh) == IMAGE_INVALID_FLASH) ?
	                 flash : BLSH_OTA_FLASH(&sh);
	iop->ota_size = (BLSH_OTA_SIZE(&sh) == IMAGE_INVALID_OTA_SIZE) ?
	                iop->bl_size : BLSH_OTA_SIZE(&sh);
	iop->ota_addr = BLSH_OTA_ADDR(&sh);

	/* no info in section_header_t::priv[] if version < 0.2 */
	if (sh.version < 2) { /* for compatibility only */
		iop->ota_flash = flash;
		iop->ota_size = iop->bl_size;
		iop->ota_addr = (1 << 20); /* default to 1MB */
	}
	IMAGE_DBG("bl size %u KB, image (no bl) max size %u KB\n",
	          iop->bl_size / 1024, iop->img_max_size / 1024);
	IMAGE_DBG("ota cfg: flash %u, size %u KB, addr %#x (%u KB)\n",
	          iop->ota_flash, iop->ota_size / 1024, iop->ota_addr,
	          iop->ota_addr / 1024);

	iop->flash[0] = flash;
	iop->addr[0] = addr + iop->bl_size;
	IMAGE_DBG("image 0: flash %u, addr %#010x (%u KB)\n",
	          iop->flash[0], iop->addr[0], iop->addr[0] / 1024);
	for (i = 1; i < IMAGE_SEQ_NUM; ++i) {
		iop->flash[i] = iop->ota_flash;
		iop->addr[i] = iop->ota_addr + iop->ota_size + iop->img_max_size * (i - 1);
		IMAGE_DBG("image %d: flash %u, addr %#010x (%u KB)\n",
		          i, iop->flash[i], iop->addr[i], iop->addr[i] / 1024);
	}
	image_clear_sec_addr(&image_priv);

#ifndef __CONFIG_BOOTLOADER
	iop->running_seq = image_init_running_seq(iop); /* init running sequence */
#else
	iop->running_seq = IMAGE_SEQ_NUM; /* set to invalid running sequence for bl*/
#endif
	IMAGE_DBG("running seq %u\n", iop->running_seq);

	return 0;
}

/**
 * @brief DeInitialize the image module
 * @return None
 */
void image_deinit(void)
{
	image_priv_t *img = &image_priv;
	image_clear_sec_addr(img);
	image_memset(img, 0, sizeof(image_priv_t));
}

/**
 * @brief Get OTA parameter
 * @return Pointer to OTA parameter (read only)
 */
const image_ota_param_t *image_get_ota_param(void)
{
	return &image_priv.iop;
}

/**
 * @brief Set running image sequence
 * @param[in] seq Image sequence
 * @return None
 */
void image_set_running_seq(image_seq_t seq)
{
	IMAGE_DBG("set running seq %u\n", seq);
	image_priv.iop.running_seq = seq;
}

/**
 * @brief Get running image sequence
 * @return running image sequence
 */
image_seq_t image_get_running_seq(void)
{
	IMAGE_DBG("get running seq %u\n", image_priv.iop.running_seq);
	return image_priv.iop.running_seq;
}

static int image_expand_addr(image_priv_t *img, image_seq_t seq)
{
	sec_addr_t *sec_addr;

	sec_addr = image_malloc((img->sec_num[seq] + IMAGE_EXPAND_SIZE) * sizeof(sec_addr_t));
	if (sec_addr == NULL) {
		IMAGE_ERR("no mem\n");
		return -1;
	}

	if (img->sec_addr[seq] && img->sec_num[seq] > 0) {
		image_memcpy(sec_addr, img->sec_addr[seq], img->sec_num[seq] * sizeof(sec_addr_t));
	}

	if (img->sec_addr[seq]) {
		image_free(img->sec_addr[seq]);
	}

	img->sec_addr[seq] = sec_addr;
	return 0;
}

static uint32_t image_get_addr(image_priv_t *img, image_seq_t seq, uint32_t id)
{
	struct sh_parts {
		uint32_t next_addr;     /* next section address */
		uint32_t id;            /* section ID */
	} sh_part; /* parts of section_header_t, used to reduce stack size */
	uint8_t		index;
	uint32_t	flash;
	uint32_t	addr;
	uint32_t	next_addr;
	image_ota_param_t *iop;

	if (seq >= IMAGE_SEQ_NUM) {
		IMAGE_ERR("seq %u\n", seq);
		return IMAGE_INVALID_ADDR;
	}

	for (index = 0; index < img->sec_num[seq]; index++) {
		if (id == img->sec_addr[seq][index].id)
			return img->sec_addr[seq][index].addr;
	}

	iop = &img->iop;
	if (img->sec_num[seq] == 0) {
		/* search from bootloader */
		flash = IMG_BL_FLASH(iop);
		addr = IMG_BL_ADDR(iop);
		next_addr = addr; /* dummy, != IMAGE_INVALID_ADDR */
	} else {
		/* search from the last section */
		flash = iop->flash[seq];
		addr = img->sec_addr[seq][img->sec_num[seq] - 1].addr;
		if (flash_read(iop->flash[seq],
		               addr + offsetof(section_header_t, next_addr),
		               &next_addr, sizeof(next_addr)) != sizeof(next_addr)) {
			return IMAGE_INVALID_ADDR;
		}
		addr = IMG_SEC_ADDR(iop, seq, next_addr);
	}

	while (next_addr != IMAGE_INVALID_ADDR) {
		if ((img->sec_num[seq] % IMAGE_EXPAND_SIZE) == 0) {
			if (image_expand_addr(img, seq) != 0) {
				return IMAGE_INVALID_ADDR;
			}
		}

		/* get parts of section header */
		if (flash_read(flash, addr + offsetof(section_header_t, next_addr),
		               &sh_part, sizeof(sh_part)) != sizeof(sh_part)) {
			return IMAGE_INVALID_ADDR;
		}

		img->sec_addr[seq][img->sec_num[seq]].addr = addr;
		img->sec_addr[seq][img->sec_num[seq]].id = sh_part.id;
		img->sec_num[seq]++;
		if (id == sh_part.id) {
			return addr;
		}
		flash = iop->flash[seq];
		next_addr = sh_part.next_addr;
		addr = IMG_SEC_ADDR(iop, seq, next_addr);
	}

	IMAGE_ERR("no section id %#x, seq %u\n", id, seq);
	return IMAGE_INVALID_ADDR;
}

/**
 * @brief Get address of the specified section in running image
 * @param[in] id ID of the specified section
 * @return section address
 */
uint32_t image_get_section_addr(uint32_t id)
{
	IMAGE_DBG("%s(), seq %u, id %#x\n", __func__, image_priv.iop.running_seq, id);
	return image_get_addr(&image_priv, image_priv.iop.running_seq, id);
}

/**
 * @brief Read/Write an amount of image data from/to flash
 * @param[in] id Section ID of the image data
 * @param[in] seg Section segment of the image data
 * @param[in] offset Offset into the segment from where to read/write data
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read/written
 * @param[in] do_write Read or write
 * @return Number of bytes read/written
 */
uint32_t image_rw(uint32_t id, image_seg_t seg, uint32_t offset,
                  void *buf, uint32_t size, int do_write)
{
	uint32_t addr;
	uint32_t body_len;
	image_priv_t *img;
	image_ota_param_t *iop;
	image_seq_t seq;

//	IMAGE_DBG("%s() id %#x, seg %u, offset %u, buf %p, size %u, do_write %d\n",
//	          __func__, id, seg, offset, buf, size, do_write);

	if (buf == NULL) {
		IMAGE_ERR("buf %p\n", buf);
		return 0;
	}

	img = &image_priv;
	iop = &img->iop;
	seq = iop->running_seq;
	addr = image_get_addr(img, iop->running_seq, id);
	if (addr == IMAGE_INVALID_ADDR) {
		IMAGE_ERR("get section fail, seq %u, id %#x\n", seq, id);
		return 0;
	}

	switch (seg) {
	case IMAGE_SEG_HEADER:
		if (do_write) {
			image_clear_sec_addr(img);
		}
		break;
	case IMAGE_SEG_BODY:
		addr += IMAGE_HEADER_SIZE;
		break;
	case IMAGE_SEG_TAILER:
		if (flash_read(iop->flash[seq],
		               addr + offsetof(section_header_t, body_len),
		       	       &body_len, sizeof(body_len)) != sizeof(body_len)) {
			return 0;
		}
		addr += IMAGE_HEADER_SIZE + body_len;
		break;
	default :
		IMAGE_ERR("invalid seg %d\n", seg);
		return 0;
	}

	if (do_write) {
		return flash_write(iop->flash[seq], addr + offset, buf, size);
	} else {
		return flash_read(iop->flash[seq], addr + offset, buf, size);
	}
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
	uint16_t checksum;

	if (sh == NULL) {
		return IMAGE_INVALID;
	}

	if (sh->magic_number != IMAGE_MAGIC_NUMBER) {
		IMAGE_WRN("%s() invalid head magic %#x\n", __func__, sh->magic_number);
		return IMAGE_INVALID;
	}

	checksum = image_get_checksum(sh, IMAGE_HEADER_SIZE);
	if (checksum != 0xFFFF) {
		IMAGE_WRN("%s() invalid head checksum %#x\n", __func__, checksum);
		return IMAGE_INVALID;
	}

	return IMAGE_VALID;
}

/**
 * @brief Check vadility of the section data (body and tailer)
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

	if ((sh == NULL) || (body == NULL) || (body_len == 0) ||
		((tailer == NULL) && (tailer_len != 0))) {
		IMAGE_ERR("sh %p, body %p, body len %u, tailer %p, tailer len %u\n",
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
		IMAGE_WRN("%s(), invalid checksum %#06x\n", __func__, data_checksum);
		return IMAGE_INVALID;
	}

	return IMAGE_VALID;
}

/**
 * @brief Check vadility of the section placed at the specified address
 * @param[in] flash Flash device number of the section
 * @param[in] flash Address of the section
 * @param[in] buf Temporary buffer helping to read data
 * @param[in] buf_len Length of the temporary buffer, MUST >= IMAGE_HEADER_SIZE
 * @param[out] next_addr Pointer to the the next section offset
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
static image_val_t _image_check_section(uint32_t flash, uint32_t addr,
                                        uint8_t *buf, uint32_t buf_len,
                                        uint32_t *next_addr)
{
	uint32_t			offset;
	uint16_t			data_chksum;
	uint32_t			data_size;
	section_header_t   *sh;

	if (flash_read(flash, addr, buf, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE) {
		return IMAGE_INVALID;
	}

	sh = (section_header_t *)buf;
	if (image_check_header(sh) == IMAGE_INVALID) {
		return IMAGE_INVALID;
	}
	data_chksum = sh->data_chksum;
	data_size = sh->data_size;
	offset = sh->next_addr;

	addr += IMAGE_HEADER_SIZE;
	while (data_size > 0) {
		if (data_size >= buf_len) {
			if (flash_read(flash, addr, buf, buf_len) != buf_len) {
				return IMAGE_INVALID;
			}
			data_chksum += image_checksum16(buf, buf_len);
			addr += buf_len;
			data_size -= buf_len;
		} else {
			if (flash_read(flash, addr, buf, data_size) != data_size) {
				return IMAGE_INVALID;
			}
			data_chksum += image_checksum16(buf, data_size);
			addr += data_size;
			data_size -= data_size;
		}
	}

	if (data_chksum != 0xFFFF) {
		IMAGE_WRN("%s() fail, data checksum %#x\n", __func__, data_chksum);
		return IMAGE_INVALID;
	}

	if (next_addr) {
		*next_addr = offset;
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
	uint32_t addr;
	uint8_t *buf;
	image_val_t ret;
	image_priv_t *img = &image_priv;

	addr = image_get_addr(img, seq, id);
	if (addr == IMAGE_INVALID_ADDR) {
		return IMAGE_INVALID;
	}

	IMAGE_DBG("%s(), seq %d, flash %d, id %#x, addr %#x\n",
			  __func__, seq, img->iop.flash[seq], id, addr);

	buf = image_malloc(IMAGE_CHECK_SIZE);
	if (buf == NULL) {
		IMAGE_ERR("no mem\n");
		return IMAGE_INVALID;
	}

	ret = _image_check_section(img->iop.flash[seq], addr,
	                           buf, IMAGE_CHECK_SIZE, NULL);
	image_free(buf);
	return ret;
}

/**
 * @brief Check vadility of all sections in the specified image
 * @param[in] seq Sequence of the specified image
 * @retval image_val_t, IMAGE_VALID on valid, IMAGE_INVALID on invalid
 */
image_val_t image_check_sections(image_seq_t seq)
{
	uint32_t flash;
	uint32_t addr;
	uint32_t next_addr;
	uint8_t *buf;
	image_ota_param_t *iop = &image_priv.iop;

	IMAGE_DBG("%s(), seq %d\n", __func__, seq);

	buf = image_malloc(IMAGE_CHECK_SIZE);
	if (buf == NULL) {
		IMAGE_ERR("no mem\n");
		return IMAGE_INVALID;
	}

	flash = IMG_BL_FLASH(iop);
	addr = IMG_BL_ADDR(iop);

	while (1) {
		if (_image_check_section(flash, addr, buf, IMAGE_CHECK_SIZE,
		                         &next_addr) == IMAGE_INVALID) {
			IMAGE_WRN("%s(), invalid section, seq %d, flash %d, addr %#x\n",
					  __func__, seq, flash, addr);
			image_free(buf);
			return IMAGE_INVALID;
		}
		if (next_addr == IMAGE_INVALID_ADDR)
			break;

		flash = iop->flash[seq];
		addr = IMG_SEC_ADDR(iop, seq, next_addr);
	}

	image_free(buf);
	return IMAGE_VALID;
}

/**
 * @brief Get the size of the running image (including bootloader)
 * @return the size of the running image, 0 on bad image
 */
uint32_t image_get_size(void)
{
	section_header_t sh;
	uint32_t flash;
	uint32_t addr;
	uint32_t next_addr;
	uint32_t size = 0;
	image_ota_param_t *iop = &image_priv.iop;
	image_seq_t seq = iop->running_seq;

	if (seq >= IMAGE_SEQ_NUM) {
		IMAGE_ERR("seq %u\n", seq);
		return size;
	}

	/* iterate from bootloader */
	flash = IMG_BL_FLASH(iop);
	addr = IMG_BL_ADDR(iop);

	while (1) {
		if (flash_read(flash, addr, &sh, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE) {
			break;
		}
		if (image_check_header(&sh) == IMAGE_INVALID) {
			IMAGE_ERR("bad header, flash %u, seq %u, addr %#x, id %#x\n",
					  flash, seq, addr, sh.id);
			break;
		}
		next_addr = sh.next_addr;
		if (next_addr == IMAGE_INVALID_ADDR) {
			size = addr + IMAGE_HEADER_SIZE + sh.data_size - iop->addr[seq] +
			       iop->bl_size;
			IMAGE_DBG("%s(), seq %d, image size %#x\n", __func__, seq, size);
			break;
		}
		flash = iop->flash[seq];
		addr = IMG_SEC_ADDR(iop, seq, next_addr);
	};

	return size;
}

/* try counts for writing image raw config */
#define IMAGE_WRITE_RAW_CFG_TRY_CNT   2

typedef struct image_raw_cfg {
	uint16_t raw_seq;	/* magic number for image_seq_t */
	uint16_t raw_state;	/* magic number for image_state_t */
} image_raw_cfg_t;

/* magic numbers of image_raw_cfg_t::raw_seq, indexed by image_seq_t */
static const uint16_t s_image_raw_seq[IMAGE_SEQ_NUM] = {
	0x5555,
	0xAAAA,
#if (IMAGE_SEQ_NUM >= 3)
	0x3333,
#endif
};

#define IMAGE_RAW_STATE_UNVERIFIED	(0x6996)
#define IMAGE_RAW_STATE_VERIFIED	(0x9669)


static int image_get_raw_cfg(const image_ota_param_t *iop,
                             image_raw_cfg_t *raw_cfg)
{
	fdcm_handle_t *fdcm_hdl;
	int ret = -1;

	fdcm_hdl = fdcm_open(iop->ota_flash, iop->ota_addr, iop->ota_size);
	if (fdcm_hdl == NULL) {
		IMAGE_ERR("fdcm_open() failed\n");
		return ret;
	}

	if (fdcm_read(fdcm_hdl, raw_cfg, sizeof(*raw_cfg)) == sizeof(*raw_cfg)) {
		IMAGE_DBG("%s(), raw_seq %#x, raw_state %#x\n", __func__,
		          raw_cfg->raw_seq, raw_cfg->raw_state);
		ret = 0;
	} else {
		IMAGE_ERR("fdcm read failed\n");
	}

	fdcm_close(fdcm_hdl);
	return ret;
}

static int image_set_raw_cfg(const image_ota_param_t *iop,
                             image_raw_cfg_t *raw_cfg)
{
	image_raw_cfg_t read_cfg;
	fdcm_handle_t *fdcm_hdl;
	uint8_t i;
	int ret = -1;

	fdcm_hdl = fdcm_open(iop->ota_flash, iop->ota_addr, iop->ota_size);
	if (fdcm_hdl == NULL) {
		IMAGE_ERR("fdcm_open() failed\n");
		return ret;
	}

	IMAGE_DBG("%s(), raw_seq %#x, raw_state %#x\n", __func__,
	          raw_cfg->raw_seq, raw_cfg->raw_state);

	for (i = 0; i < IMAGE_WRITE_RAW_CFG_TRY_CNT; ++i) {
		if (fdcm_write(fdcm_hdl, raw_cfg, sizeof(*raw_cfg)) != sizeof(*raw_cfg)) {
			IMAGE_ERR("fdcm write failed\n");
			break;
		}
		if (fdcm_read(fdcm_hdl, &read_cfg, sizeof(*raw_cfg)) != sizeof(*raw_cfg)) {
			IMAGE_ERR("fdcm read back failed\n");
			break;
		}
		if (image_memcmp(&read_cfg, raw_cfg, sizeof(*raw_cfg)) == 0) {
			ret = 0;
			break;
		}
	}

	fdcm_close(fdcm_hdl);
	return ret;
}

/**
 * @brief Get the image configuration
 * @param[in] cfg Pointer to image configuration
 * @return 0 on success, -1 on failure
 */
int image_get_cfg(image_cfg_t *cfg)
{
	image_seq_t seq;
	image_raw_cfg_t raw_cfg;
	const image_ota_param_t *iop = &image_priv.iop;

	if ((cfg == NULL) || (iop->ota_size == 0) ||
		(iop->ota_addr == IMAGE_INVALID_ADDR)) {
		IMAGE_ERR("invalid param, cfg %p, ota_size %#x, ota_addr %#x\n",
				  cfg, iop->ota_size, iop->ota_addr);
		return -1;
	}

	if (image_get_raw_cfg(iop, &raw_cfg) != 0) {
		return -1;
	}

	for (seq = 0; seq < IMAGE_SEQ_NUM; ++seq) {
		if (raw_cfg.raw_seq == s_image_raw_seq[seq]) {
			cfg->seq = seq;
			break;
		}
	}
	if (seq >= IMAGE_SEQ_NUM) {
		IMAGE_ERR("invalid raw_seq %#x\n", raw_cfg.raw_seq);
		return -1;
	}

	if (raw_cfg.raw_state == IMAGE_RAW_STATE_VERIFIED) {
		cfg->state = IMAGE_STATE_VERIFIED;
	} else if (raw_cfg.raw_state == IMAGE_RAW_STATE_UNVERIFIED) {
		cfg->state = IMAGE_STATE_UNVERIFIED;
	} else {
		cfg->state = IMAGE_STATE_UNDEFINED;
		IMAGE_ERR("raw_state %#x\n", raw_cfg.raw_state);
	}

	IMAGE_DBG("%s(), seq %d, state %d\n", __func__, cfg->seq, cfg->state);
	return 0;
}

/**
 * @brief Set the image configuration
 * @param[in] cfg Pointer to image configuration
 * @return 0 on success, -1 on failure
 */
int image_set_cfg(image_cfg_t *cfg)
{
	image_raw_cfg_t raw_cfg;
	const image_ota_param_t *iop = &image_priv.iop;

	if ((cfg == NULL) || (iop->ota_size == 0) ||
		(iop->ota_addr == IMAGE_INVALID_ADDR)) {
		IMAGE_ERR("invalid param, cfg %p, ota_size %#x, ota_addr %#x\n",
				  cfg, iop->ota_size, iop->ota_addr);
		return -1;
	}

	if (cfg->seq >= IMAGE_SEQ_NUM || cfg->state != IMAGE_STATE_VERIFIED) {
		IMAGE_ERR("invalid cfg, seq %d, state %d\n", cfg->seq, cfg->state);
		return -1;
	}

	raw_cfg.raw_seq = s_image_raw_seq[cfg->seq];
	raw_cfg.raw_state = IMAGE_RAW_STATE_VERIFIED;
#if OTA_IMG_CFG_SEQ_CORRUPTION_TEST
	raw_cfg.raw_seq = 0xFFFF;
	IMAGE_LOG(1, "ota cfg img raw seq (%#x) corruption test\n", raw_cfg.raw_seq);
#endif
#if OTA_IMG_CFG_STATE_CORRUPTION_TEST
	raw_cfg.raw_state = 0xFFFF;
	IMAGE_LOG(1, "ota cfg img raw state (%#x) corruption test\n", raw_cfg.raw_state);
#endif

	IMAGE_DBG("%s(), raw_seq %#x, raw_state %#x\n", __func__,
	          raw_cfg.raw_seq, raw_cfg.raw_state);

	if (image_set_raw_cfg(iop, &raw_cfg) != 0) {
		IMAGE_ERR("write boot cfg failed\n");
		return -1;
	}

	return 0;
}
