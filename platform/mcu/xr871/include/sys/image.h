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

#ifndef _SYS_IMAGE_H_
#define _SYS_IMAGE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* compatibility for old bootloader (version < 0.3) due to OTA upgrade */
#define IMAGE_OPT_BL_COMPATIBILITY	(1)

#define IMAGE_SEQ_NUM	(2)	/* max number of image sequence */

/**
 * @brief Image sequence index, starting from 0
 */
typedef uint8_t image_seq_t;

/**
 * @brief Image verification state definition
 */
typedef enum image_state {
    IMAGE_STATE_UNVERIFIED = 0,
    IMAGE_STATE_VERIFIED   = 1,
    IMAGE_STATE_UNDEFINED  = 2,
} image_state_t;

/**
 * @brief Image configuration definition
 */
typedef struct image_cfg {
    image_seq_t   seq;
    image_state_t state;
} image_cfg_t;

/**
 * @brief Image segment definition
 */
typedef enum image_segment {
	IMAGE_SEG_HEADER	= 0,
	IMAGE_SEG_BODY		= 1,
	IMAGE_SEG_TAILER	= 2,
} image_seg_t;

/**
 * @brief Image validity definition
 */
typedef enum image_validity {
	IMAGE_INVALID		= 0,
	IMAGE_VALID			= 1,
} image_val_t;

/**
 * @brief Section header magic number definition (AWIH)
 */
#define IMAGE_MAGIC_NUMBER	(0x48495741)

/**
 * @brief Section ID definition
 */
#define IMAGE_BOOT_ID		(0xA5FF5A00)
#define IMAGE_APP_ID		(0xA5FE5A01)
#define IMAGE_APP_XIP_ID	(0xA5FD5A02)
#define IMAGE_NET_ID		(0xA5FC5A03)
#define IMAGE_NET_AP_ID		(0xA5FB5A04)
#define IMAGE_WLAN_BL_ID	(0xA5FA5A05)
#define IMAGE_WLAN_FW_ID	(0xA5F95A06)
#define IMAGE_WLAN_SDD_ID	(0xA5F85A07)

/**
 * @brief Section header definition (64 Bytes)
 */
typedef struct section_header {
	uint32_t magic_number;  /* magic number */
	uint32_t version;       /* version */
	uint16_t header_chksum; /* header checksum */
	uint16_t data_chksum;   /* data checksum */
	uint32_t data_size;     /* data size */
	uint32_t load_addr;     /* load address */
	uint32_t entry;         /* entry point */
	uint32_t body_len;      /* body length */
	uint32_t attribute;     /* attribute */
	uint32_t next_addr;     /* next section address */
	uint32_t id;            /* section ID */
	uint32_t priv[6];       /* private data */
} section_header_t;

#define IMAGE_HEADER_SIZE	sizeof(section_header_t)

/**
 * @brief OTA parameter definition
 */
typedef struct image_ota_param {
	uint32_t	ota_flash : 8;	/* flash ID of OTA area */
	uint32_t	ota_size  : 24; /* size of OTA area */
	uint32_t	ota_addr;		/* start addr of OTA area */
	uint32_t	img_max_size;	/* image max size (excluding bootloader) */
	uint32_t	bl_size;		/* bootloader size */
	image_seq_t running_seq;	/* running image sequence */
	uint8_t		flash[IMAGE_SEQ_NUM];	/* flash ID which the image on */
	uint32_t	addr[IMAGE_SEQ_NUM];	/* image start addr, excluding bootloader */
} image_ota_param_t;

#define IMG_BL_FLASH(iop) ((iop)->flash[0]) /* bootloader flash */
#define IMG_BL_ADDR(iop)  ((iop)->addr[0] - (iop)->bl_size) /* bootloader addr */

#define IMAGE_INVALID_ADDR	(0xFFFFFFFF)

int image_init(uint32_t flash, uint32_t addr, uint32_t max_size);
void image_deinit(void);

const image_ota_param_t *image_get_ota_param(void);

void image_set_running_seq(image_seq_t seq);
image_seq_t image_get_running_seq(void);

uint32_t image_get_section_addr(uint32_t id);

uint32_t image_rw(uint32_t id, image_seg_t seg, uint32_t offset,
                  void *buf, uint32_t size, int do_write);

/**
 * @brief Read an amount of image data from flash
 * @param[in] id Section ID of the image data
 * @param[in] seg Section segment of the image data
 * @param[in] offset Offset into the segment from where to read data
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read
 * @return Number of bytes read
 */
static __inline uint32_t image_read(uint32_t id, image_seg_t seg,
                                    uint32_t offset, void *buf, uint32_t size)
{
	return image_rw(id, seg, offset, buf, size, 0);
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
static __inline uint32_t image_write(uint32_t id, image_seg_t seg,
                                     uint32_t offset, void *buf, uint32_t size)
{
	return image_rw(id, seg, offset, buf, size, 1);
}

uint16_t image_get_checksum(void *buf, uint32_t len);

image_val_t image_check_header(section_header_t *sh);
image_val_t image_check_data(section_header_t *sh, void *body, uint32_t body_len,
							 void *tailer, uint32_t tailer_len);

image_val_t image_check_section(image_seq_t seq, uint32_t id);
image_val_t image_check_sections(image_seq_t seq);

uint32_t image_get_size(void);

int image_get_cfg(image_cfg_t *cfg);
int image_set_cfg(image_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IMAGE_H_ */
