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

#ifndef _NET_WLAN_WLAN_EXT_REQ_H_
#define _NET_WLAN_WLAN_EXT_REQ_H_

#include <stdint.h>
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Wlan extended command definition
 */
typedef enum wlan_ext_cmd {
    WLAN_EXT_CMD_SET_PM_DTIM = 0,
    WLAN_EXT_CMD_SET_PS_CFG,
    WLAN_EXT_CMD_SET_AMPDU_TXNUM,
    WLAN_EXT_CMD_SET_TX_RETRY_CNT,
    WLAN_EXT_CMD_SET_PM_TX_NULL_PERIOD,
    WLAN_EXT_CMD_SET_BCN_WIN_US,

    WLAN_EXT_CMD_GET_BCN_STATUS = 50,
} wlan_ext_cmd_t;

/**
 * @brief Parameter for WLAN_EXT_CMD_SET_PS_CFG
 */
typedef struct wlan_ext_ps_cfg {
	int ps_mode;
	int ps_idle_period;
	int ps_change_period;
} wlan_ext_ps_cfg_t;

/**
 * @brief Parameter for WLAN_EXT_CMD_GET_BCN_STATUS
 */
typedef struct wlan_ext_bcn_status {
	uint32_t bcn_duration;
	int32_t  bcn_delay_max;
	int32_t  bcn_delay_sum;
	uint16_t bcn_delay_cnt[8];
	uint16_t bcn_rx_cnt;
	uint16_t bcn_miss_cnt;
} wlan_ext_bcn_status_t;

int wlan_ext_request(struct netif *nif, wlan_ext_cmd_t cmd, uint32_t param);

#ifdef __cplusplus
}
#endif

#endif /* _NET_WLAN_WLAN_EXT_REQ_H_ */
