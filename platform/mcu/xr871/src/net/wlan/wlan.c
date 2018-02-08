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

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE))

#include "wlan_debug.h"
#include "wpa_ctrl_req.h"
#include "lwip/netif.h"
#include "sys/ducc/ducc_net.h"
#include "sys/ducc/ducc_app.h"

#include "net/wlan/wlan.h"
#include "net/wlan/wlan_defs.h"

#include "kernel/os/os.h"

#define WLAN_ASSERT_POINTER(p)						\
	do {								\
		if (p == NULL) {					\
			WLAN_ERR("invalid parameter\n");	\
			return -1;					\
		}							\
	} while (0)

/**
 * @brief Configure station in a convenient way to join a specified network
 * @param[in] ssid Network name, length is [1, 32]
 * @param[in] ssid_len The length of network name
 * @param[in] psk Network password, in one of the optional formats:
 *            - NULL, to join an OPEN network
 *            - an ASCII string, length is [8, 63]
 *            - a hex string (two characters per octet of PSK), length is 64
 * @return 0 on success, -1 on failure
 *
 * @note This way is only adapted to join an OPEN, WPA-PSK, WPA2-PSK or
 *       WPA-PSK/WPA2-PSK network.
 */
int wlan_sta_set(uint8_t *ssid, uint8_t ssid_len, uint8_t *psk)
{
	WLAN_ASSERT_POINTER(ssid);

	if ((ssid_len == 0) || (ssid_len > 32)) {
		WLAN_ERR("invalid ssid len %d\n", ssid_len);
		return -1;
	}

	wlan_sta_config_t config;
	wlan_memset(&config, 0, sizeof(config));

	/* ssid */
	config.field = WLAN_STA_FIELD_SSID;
	wlan_memcpy(config.u.ssid.ssid, ssid, ssid_len);
	config.u.ssid.ssid_len = ssid_len;
	if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
		return -1;

	/* auth_alg: OPEN */
	config.field = WLAN_STA_FIELD_AUTH_ALG;
	config.u.auth_alg = WPA_AUTH_ALG_OPEN;
	if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
		return -1;

	if (psk == NULL) {
		/* key_mgmt: NONE */
		config.field = WLAN_STA_FIELD_KEY_MGMT;
		config.u.key_mgmt = WPA_KEY_MGMT_NONE;
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;
	} else {
		/* psk */
		config.field = WLAN_STA_FIELD_PSK;
		wlan_strlcpy((char *)config.u.psk, (char *)psk, sizeof(config.u.psk));
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;

		/* proto: WPA | RSN */
		config.field = WLAN_STA_FIELD_PROTO;
		config.u.proto = WPA_PROTO_WPA | WPA_PROTO_RSN;
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;

		/* key_mgmt: PSK */
		config.field = WLAN_STA_FIELD_KEY_MGMT;
		config.u.key_mgmt = WPA_KEY_MGMT_PSK;
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;

		/* pairwise: CCMP | TKIP */
		config.field = WLAN_STA_FIELD_PAIRWISE_CIPHER;
		config.u.pairwise_cipher = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP;
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;

		/* group: CCMP | TKIP | WEP40 | WEP104 */
		config.field = WLAN_STA_FIELD_GROUP_CIPHER;
		config.u.pairwise_cipher = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP
					   | WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104;
		if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
			return -1;
	}

	/* scan_ssid: 1 */
	config.field = WLAN_STA_FIELD_SCAN_SSID;
	config.u.scan_ssid = 1;
	if (wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, &config) != 0)
		return -1;

	return 0;
}

/**
 * @brief Set station specified field configuration
 * @param[in] config Pointer to the configuration
 * @return 0 on success, -1 on failure
 */
int wlan_sta_set_config(wlan_sta_config_t *config)
{
	WLAN_ASSERT_POINTER(config);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_SET, config);
}

/**
 * @brief Get station specified field configuration
 * @param[in] config Pointer to the configuration
 * @return 0 on success, -1 on failure
 */
int wlan_sta_get_config(wlan_sta_config_t *config)
{
	WLAN_ASSERT_POINTER(config);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_GET, config);
}

/**
 * @brief Enable the station
 * @return 0 on success, -1 on failure
 */
int wlan_sta_enable(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_ENABLE, NULL);
}

/**
 * @brief Disable the station
 * @return 0 on success, -1 on failure
 */
int wlan_sta_disable(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_DISABLE, NULL);
}

/**
 * @brief Station scan once
 * @return 0 on success, -1 on failure
 */
int wlan_sta_scan_once(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_SCAN, NULL);
}

/**
 * @brief Get station scan results
 * @param[in] results Pointer to the scan results
 * @return 0 on success, -1 on failure
 */
int wlan_sta_scan_result(wlan_sta_scan_results_t *results)
{
	WLAN_ASSERT_POINTER(results);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_SCAN_RESULTS, results);
}

/**
 * @brief Set station scan interval
 * @param[in] sec Scan interval in Seconds
 * @return 0 on success, -1 on failure
 */
int wlan_sta_scan_interval(int sec)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_SCAN_INTERVAL, (void *)sec);
}

/**
 * @brief Flush station old BSS entries
 * @param[in] age Maximum entry age in seconds
 * @return 0 on success, -1 on failure
 *
 * @note Remove BSS entries that have not been updated during the last @age
 * seconds.
 */
int wlan_sta_bss_flush(int age)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_BSS_FLUSH, (void *)age);
}

/**
 * @brief Request a new connection
 * @return 0 on success, -1 on failure
 */
int wlan_sta_connect(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_REASSOCIATE, NULL);
}

/**
 * @brief Disconnect the current connection
 * @return 0 on success, -1 on failure
 */
int wlan_sta_disconnect(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_DISCONNECT, NULL);
}

/**
 * @brief Get station connection state
 * @param[in] state Pointer to the connection state
 * @return 0 on success, -1 on failure
 */
int wlan_sta_state(wlan_sta_states_t *state)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_STATE, state);
}

/**
 * @brief Get the information of connected AP
 * @param[in] ap Pointer to the AP information
 * @return 0 on success, -1 on failure
 */
int wlan_sta_ap_info(wlan_sta_ap_t *ap)
{
	WLAN_ASSERT_POINTER(ap);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_AP, ap);
}

/**
 * @brief Start the WPS negotiation with PBC method
 * @return 0 on success, -1 on failure
 *
 * @note WPS will be turned off automatically after two minutes.
 */
int wlan_sta_wps_pbc(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_STA_WPS_PBC, NULL);
}

/**
 * @brief Get a random valid WPS PIN
 * @param[in] wps Pointer to the WPS pin
 * @return 0 on success, -1 on failure
 */
int wlan_sta_wps_pin_get(wlan_sta_wps_pin_t *wps)
{
	WLAN_ASSERT_POINTER(wps);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_WPS_GET_PIN, wps);
}

/**
 * @brief Start the WPS negotiation with PIN method
 * @param[in] wps Pointer to the WPS pin
 * @return 0 on success, -1 on failure
 *
 * @note WPS will be turned off automatically after two minutes.
 */
int wlan_sta_wps_pin_set(wlan_sta_wps_pin_t *wps)
{
	WLAN_ASSERT_POINTER(wps);

	return wpa_ctrl_request(WPA_CTRL_CMD_STA_WPS_SET_PIN, wps);
}

/**
 * @brief Configure AP in a convenient way to build a specified network
 * @param[in] ssid Network name, length is [1, 32]
 * @param[in] ssid_len The length of network name
 * @param[in] psk Network password, in one of the optional formats:
 *            - NULL, to build an OPEN network
 *            - an ASCII string, length is [8, 63]
 *            - a hex string (two characters per octet of PSK), length is 64
 * @return 0 on success, -1 on failure
 *
 * @note This way is only adapted to build an OPEN or WPA-PSK/WPA2-PSK network.
 */
int wlan_ap_set(uint8_t *ssid, uint8_t ssid_len, uint8_t *psk)
{
	WLAN_ASSERT_POINTER(ssid);

	if ((ssid_len == 0) || (ssid_len > 32)) {
		WLAN_ERR("invalid ssid len %d\n", ssid_len);
		return -1;
	}

	wlan_ap_config_t config;
	wlan_memset(&config, 0, sizeof(config));

	/* ssid */
	config.field = WLAN_AP_FIELD_SSID;
	wlan_memcpy(config.u.ssid.ssid, ssid, ssid_len);
	config.u.ssid.ssid_len = ssid_len;
	if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
		return -1;

	/* auth_alg: OPEN */
	config.field = WLAN_AP_FIELD_AUTH_ALG;
	config.u.auth_alg = WPA_AUTH_ALG_OPEN;
	if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
		return -1;

	if (psk == NULL) {
		/* proto: 0 */
		config.field = WLAN_AP_FIELD_PROTO;
		config.u.proto = 0;
		if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
			return -1;

		/* key_mgmt: NONE */
		config.field = WLAN_AP_FIELD_KEY_MGMT;
		config.u.key_mgmt = WPA_KEY_MGMT_NONE;
		if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
			return -1;
	} else {
		/* psk */
		config.field = WLAN_AP_FIELD_PSK;
		wlan_strlcpy((char *)config.u.psk, (char *)psk, sizeof(config.u.psk));
		if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
			return -1;

		/* proto: WPA | RSN */
		config.field = WLAN_AP_FIELD_PROTO;
		config.u.proto = WPA_PROTO_WPA | WPA_PROTO_RSN;
		if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
			return -1;

		/* key_mgmt: PSK */
		config.field = WLAN_AP_FIELD_KEY_MGMT;
		config.u.key_mgmt = WPA_KEY_MGMT_PSK;
		if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
			return -1;
	}

	/* wpa_cipher: TKIP */
	config.field = WLAN_AP_FIELD_WPA_CIPHER;
	config.u.wpa_cipher = WPA_CIPHER_TKIP;
	if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
		return -1;

	/* rsn_cipher: CCMP */
	config.field = WLAN_AP_FIELD_RSN_CIPHER;
	config.u.rsn_cipher = WPA_CIPHER_CCMP;
	if (wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, &config) != 0)
		return -1;

	return 0;
}

/**
 * @brief Set AP specified field configuration
 * @param[in] config Pointer to the configuration
 * @return 0 on success, -1 on failure
 */
int wlan_ap_set_config(wlan_ap_config_t *config)
{
	WLAN_ASSERT_POINTER(config);

	return wpa_ctrl_request(WPA_CTRL_CMD_AP_SET, config);
}

/**
 * @brief Get AP specified field configuration
 * @param[in] config Pointer to the configuration
 * @return 0 on success, -1 on failure
 */
int wlan_ap_get_config(wlan_ap_config_t *config)
{
	WLAN_ASSERT_POINTER(config);

	return wpa_ctrl_request(WPA_CTRL_CMD_AP_GET, config);
}

/**
 * @brief Enable the AP
 * @return 0 on success, -1 on failure
 */
int wlan_ap_enable(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_AP_ENABLE, NULL);
}

/**
 * @brief Reload AP configuration
 * @return 0 on success, -1 on failure
 */
int wlan_ap_reload(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_AP_RELOAD, NULL);
}

/**
 * @brief Disable the AP
 * @return 0 on success, -1 on failure
 */
int wlan_ap_disable(void)
{
	return wpa_ctrl_request(WPA_CTRL_CMD_AP_DISABLE, NULL);
}

/**
 * @brief Get the number of connected stations
 * @param[in] num Pointer to the number
 * @return 0 on success, -1 on failure
 */
int wlan_ap_sta_num(int *num)
{
	WLAN_ASSERT_POINTER(num);

	return wpa_ctrl_request(WPA_CTRL_CMD_AP_STA_NUM, num);
}

/**
 * @brief Get the information of connected stations
 * @param[in] stas Pointer to the stations information
 * @return 0 on success, -1 on failure
 */
int wlan_ap_sta_info(wlan_ap_stas_t *stas)
{
	WLAN_ASSERT_POINTER(stas);

	return wpa_ctrl_request(WPA_CTRL_CMD_AP_STA_INFO, stas);
}

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */
