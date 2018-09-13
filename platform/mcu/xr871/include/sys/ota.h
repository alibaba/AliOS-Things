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

#ifndef _SYS_OTA_H_
#define _SYS_OTA_H_

#include "types.h"
#include "sys/image.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OTA status definition
 */
typedef enum ota_status {
	OTA_STATUS_OK		= 0,
	OTA_STATUS_ERROR	= -1,
} ota_status_t;

/**
 * @brief OTA image sequence definition
 */
typedef enum ota_image {
	OTA_IMAGE_1ST = 1,
	OTA_IMAGE_2ND = 2,
} ota_image_t;

/**
 * @brief OTA image verification state definition
 */
typedef enum ota_state {
	OTA_STATE_UNVERIFIED	= 0,
	OTA_STATE_VERIFIED		= 1,
} ota_state_t;

/**
 * @brief OTA configuration definition
 */
typedef struct ota_cfg {
	ota_image_t	image;
	ota_state_t	state;
} ota_cfg_t;

/**
 * @brief OTA protocol definition
 */
typedef enum ota_protocol {
	OTA_PROTOCOL_FILE	= 0,
	OTA_PROTOCOL_HTTP	= 1,
} ota_protocol_t;

/**
 * @brief OTA image verification algorithm definition
 */
typedef enum ota_verify {
	OTA_VERIFY_NONE		= 0,
	OTA_VERIFY_CRC32	= 1,
	OTA_VERIFY_MD5		= 2,
	OTA_VERIFY_SHA1		= 3,
	OTA_VERIFY_SHA256	= 4,
} ota_verify_t;

ota_status_t ota_init(image_ota_param_t *param);
void ota_deinit(void);

ota_status_t ota_read_cfg(ota_cfg_t *cfg);
ota_status_t ota_write_cfg(ota_cfg_t *cfg);

ota_status_t ota_get_image(ota_protocol_t protocol, void *url);
ota_status_t ota_verify_image(ota_verify_t verify, uint32_t *value);
void ota_reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_OTA_H_ */
