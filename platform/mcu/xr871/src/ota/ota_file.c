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

#include "ota_i.h"
#include "ota_debug.h"
#include "ota_file.h"
#include "ff.h"

typedef struct ota_fs_param {
	char   *url;
	FRESULT	res;
	FIL		file;
} ota_fs_param_t;

static ota_fs_param_t g_fs_param;

ota_status_t ota_update_file_init(void *url)
{
	g_fs_param.url = strdup((char *)url + 7);

	g_fs_param.res = f_open(&g_fs_param.file, g_fs_param.url, FA_READ | FA_OPEN_EXISTING);
	if (g_fs_param.res != FR_OK) {
		OTA_ERR("open res %d\n", g_fs_param.res);
		return OTA_STATUS_ERROR;
	}

	OTA_DBG("%s(), %d, open success\n", __func__, __LINE__);

	return OTA_STATUS_OK;
}

ota_status_t ota_update_file_get(uint8_t *buf, uint32_t buf_size, uint32_t *recv_size, uint8_t *eof_flag)
{
	g_fs_param.res = f_read(&g_fs_param.file, buf, buf_size, recv_size);
	if (g_fs_param.res != FR_OK) {
		OTA_ERR("read res %d\n", g_fs_param.res);
		return OTA_STATUS_ERROR;
	}

	if (*recv_size < buf_size) {
		*eof_flag = 1;
		f_close(&g_fs_param.file);
	} else
		*eof_flag = 0;

	return OTA_STATUS_OK;
}

