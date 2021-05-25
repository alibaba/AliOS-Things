/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
  ******************************************************************************
  * @file    bt_config_wifi.h
  * @author
  * @version
  * @brief   This file provides user interface for Wi-Fi station and AP mode configuration 
  *             base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  */
#ifndef __BT_CONFIG_WIFI_H_
#define __BT_CONFIG_WIFI_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "platform_stdlib.h"
#if defined(CONFIG_PLATFORM_8721D)
#include "ameba_soc.h"
#endif
#include "wifi_structures.h"

#define DEBUG 0

#if DEBUG==1
#define BC_DBG_PREFIX	"\n\r[BT Config Wifi][DBG] "
#define	BC_DBG(...) printf(BC_DBG_PREFIX __VA_ARGS__);
#else
#define BC_DBG(...)
#endif

#define BC_PREFIX		"\n\r[BT Config Wifi] "
#define	BC_printf(...) printf(BC_PREFIX __VA_ARGS__);

#define BC_BSSID_LEN					(6)
#define BC_MAX_SSID_LEN					(32)
#define BC_MAX_WIFI_SCAN_AP_NUM 		(64)

typedef enum {
	BC_DEV_DISABLED            = 0x0,
	BC_DEV_INIT                = 0x1,
	BC_DEV_IDLE                = 0x2,
	BC_DEV_BT_CONNECTED        = 0x3,
	BC_DEV_DEINIT              = 0x4,
} BC_device_state_t;

typedef enum {
	BC_BAND_UNKNOWN       = 0x00,
	BC_BAND_2G            = 0x01,
	BC_BAND_5G            = 0x10,
	BC_BAND_2G_5G         = BC_BAND_2G | BC_BAND_5G,
} BC_band_t;

typedef enum {
	BC_STATE_DISABLED            = 0x0,
	BC_STATE_IDLE                = 0x1,
	BC_STATE_CONNECTED           = 0x2,
	BC_STATE_WRONG_PASSWORD      = 0x3,
} BC_status_t;

struct BC_wifi_scan_result {
	rtw_scan_result_t ap_info[BC_MAX_WIFI_SCAN_AP_NUM];
	uint32_t          ap_num;
};

/* API defined in bt_config_wifi_lib */
extern void BC_cmd_task_init(void);
extern void BC_cmd_task_deinit(void);
extern void BC_handle_read_request(uint8_t **pRead_buf, uint32_t *pRead_buf_len);
extern void BC_send_cmd(uint8_t *cmd, uint32_t len);

/* API used in bt_config_wifi_lib */
extern BC_band_t BC_req_band_hdl(void);
extern int BC_req_scan_hdl(BC_band_t band, struct BC_wifi_scan_result* BC_scan_result);
extern int BC_req_connect_hdl(uint8_t *ssid, uint8_t *password, uint8_t *bssid, rtw_security_t security, BC_band_t band);
extern void BC_req_status_hdl(BC_status_t *status, uint8_t *SSID, uint8_t *BSSID, rtw_security_t *security, uint8_t *channel, int *rssi);

void bt_config_wifi_init(void);
void bt_config_wifi_deinit(void);
#endif // __BT_CONFIG_WIFI_H_
