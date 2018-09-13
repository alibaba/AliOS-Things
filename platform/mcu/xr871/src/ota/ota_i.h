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

#ifndef _OTA_I_H_
#define _OTA_I_H_

#include <string.h>
#include "types.h"
#include "sys/ota.h"
#include "driver/chip/hal_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ota_malloc(l)				malloc(l)
#define ota_free(p)					free(p)
#define ota_memcpy(d, s, n)			memcpy(d, s, n)
#define ota_memset(s, c, n) 		memset(s, c, n)
#define ota_memcmp(a, b, l)			memcmp(a, b, l)

#define OTA_BUF_SIZE				(2 << 10)
#define OTA_FLASH_TIMEOUT			(5000)

#define OTA_BOOT_IMAGE_1ST			(0x5555)
#define OTA_BOOT_IMAGE_2ND			(0xAAAA)
#define OTA_BOOT_STATE_UNVERIFIED	(0x6996)
#define OTA_BOOT_STATE_VERIFIED		(0x9669)

typedef struct {
	uint16_t	boot_image;
	uint16_t	boot_state;
} ota_boot_cfg_t;

#define OTA_BOOT_CFG_SIZE	sizeof(ota_boot_cfg_t)

typedef struct {
	uint32_t	flash[IMAGE_SEQ_NUM];
	uint32_t	addr[IMAGE_SEQ_NUM];
	uint32_t	image_size;
	uint32_t	boot_size;
	uint32_t	get_size;
} ota_priv_t;

typedef ota_status_t (*ota_update_init_t)(void *url);
typedef ota_status_t (*ota_update_get_t)(uint8_t *buf, uint32_t buf_size, uint32_t *recv_size, uint8_t *eof_flag);

typedef HAL_Status (*ota_verify_append_t)(void *hdl, uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_I_H_ */
