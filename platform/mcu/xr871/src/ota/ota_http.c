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
#include "ota_http.h"
//TODO: #include "net/HTTPClient/HTTPCUsr_api.h"

//TODO: static HTTPParameters	g_http_param;

ota_status_t ota_update_http_init(void *url)
{
//TODO:	ota_memset(&g_http_param, 0, sizeof(g_http_param));
//TODO:	ota_memcpy(g_http_param.Uri, url, strlen(url));

	return OTA_STATUS_OK;
}

ota_status_t ota_update_http_get(uint8_t *buf, uint32_t buf_size, uint32_t *recv_size, uint8_t *eof_flag)
{
//TODO:	int	ret;
//TODO:
//TODO:	ret = HTTPC_get(&g_http_param, (CHAR *)buf, (INT32)buf_size, (INT32 *)recv_size);
//TODO:	if (ret == HTTP_CLIENT_SUCCESS) {
//TODO:		*eof_flag = 0;
//TODO:		return OTA_STATUS_OK;
//TODO:	} else if (ret == HTTP_CLIENT_EOS) {
//TODO:		*eof_flag = 1;
//TODO:		return OTA_STATUS_OK;
//TODO:	} else {
//TODO:		OTA_ERR("ret %d\n", ret);
//TODO:		return OTA_STATUS_ERROR;
//TODO:	}
    return OTA_STATUS_ERROR;
}

