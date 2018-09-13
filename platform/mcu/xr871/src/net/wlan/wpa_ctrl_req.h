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

#ifndef _NET_WLAN_WPA_CTRL_REQ_H_
#define _NET_WLAN_WPA_CTRL_REQ_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief WPA control command definition
 */
typedef enum wpa_ctrl_cmd {
	/* STA */
	WPA_CTRL_CMD_STA_SCAN,
	WPA_CTRL_CMD_STA_SCAN_RESULTS,
	WPA_CTRL_CMD_STA_SCAN_INTERVAL,

	WPA_CTRL_CMD_STA_REASSOCIATE,
	WPA_CTRL_CMD_STA_REATTACH,
	WPA_CTRL_CMD_STA_RECONNECT,
	WPA_CTRL_CMD_STA_TERMINATE,
	WPA_CTRL_CMD_STA_DISCONNECT,

	WPA_CTRL_CMD_STA_ENABLE,
	WPA_CTRL_CMD_STA_DISABLE,

	WPA_CTRL_CMD_STA_SET,
	WPA_CTRL_CMD_STA_GET,
	WPA_CTRL_CMD_STA_AUTOCONNECT,

	WPA_CTRL_CMD_STA_STATE,
	WPA_CTRL_CMD_STA_AP,

	WPA_CTRL_CMD_STA_BSS_EXPIRE_AGE,
	WPA_CTRL_CMD_STA_BSS_EXPIRE_COUNT,
	WPA_CTRL_CMD_STA_BSS_FLUSH,

	WPA_CTRL_CMD_STA_WPS_PBC,
	WPA_CTRL_CMD_STA_WPS_GET_PIN,
	WPA_CTRL_CMD_STA_WPS_SET_PIN,

	/* softAP */
	WPA_CTRL_CMD_AP_ENABLE,
	WPA_CTRL_CMD_AP_RELOAD,
	WPA_CTRL_CMD_AP_DISABLE,
	WPA_CTRL_CMD_AP_TERMINATE,

	WPA_CTRL_CMD_AP_SET,
	WPA_CTRL_CMD_AP_GET,
	WPA_CTRL_CMD_AP_STA_NUM,
	WPA_CTRL_CMD_AP_STA_INFO,
} wpa_ctrl_cmd_t;

int wpa_ctrl_request(wpa_ctrl_cmd_t cmd, void *data);

#ifdef __cplusplus
}
#endif

#endif /* _NET_WLAN_WPA_CTRL_REQ_H_ */
