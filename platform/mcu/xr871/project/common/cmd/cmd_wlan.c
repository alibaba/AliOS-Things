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

/*
 * net mode <mode>
 * 		- net mode sta
 * 		- net mode ap
 * 		- net mode mon
 */

/*
 * net sta config <ssid> [psk]
 * 		- net sta config ssid_example
 * 		- net sta config ssid_example psk_example
 *
 * net sta set <field> <value>
 * 		- net sta set ssid ssid_example
 * 		- net sta set psk psk_example
 * 		- net sta set wep_key0 wep_key_example
 * 		- net sta set wep_key1 wep_key_example
 * 		- net sta set wep_key2 wep_key_example
 * 		- net sta set wep_key3 wep_key_example
 * 		- net sta set wep_key_index <0, 1, 2, 3>
 * 		- net sta set key_mgmt {WPA-PSK, NONE}
 * 		- net sta set pairwise {CCMP, TKIP, WEP40, WEP104, NONE}
 * 		- net sta set group {CCMP, TKIP, WEP40, WEP104, NONE}
 * 		- net sta set proto {WPA, RSN}
 * 		- net sta set auth_alg {OPEN, SHARED}
 * 		- net sta set ptk_rekey <seconds>
 * 		- net sta set scan_ssid (0, 1)
 *
 * net sta get <field>
 * 		- net sta get ssid
 * 		- net sta get psk
 * 		- net sta get wep_key0
 * 		- net sta get wep_key1
 * 		- net sta get wep_key2
 * 		- net sta get wep_key3
 * 		- net sta get wep_key_index
 * 		- net sta get key_mgmt
 * 		- net sta get pairwise
 * 		- net sta get group
 * 		- net sta get proto
 * 		- net sta get auth_alg
 * 		- net sta get ptk_rekey
 * 		- net sta get scan_ssid
 *
 * net sta enable
 * net sta disable
 *
 * net sta scan once
 * net sta scan result <num>
 * net sta scan interval <sec>
 * net sta bss flush <age>
 *
 * net sta connect
 * net sta disconnect
 * net sta state
 * net sta ap
 *
 * net sta wps pbc
 * net sta wps pin get
 * net sta wps pin set <pin>
 */

/*
 * net ap config <ssid> [psk]
 * 		- net ap config ssid_example
 * 		- net ap config ssid_example psk_example
 *
 * net ap set <field> <value>
 * 		- net ap set ssid ssid_example
 * 		- net ap set psk psk_example
 * 		- net ap set key_mgmt {WPA-PSK, NONE}
 * 		- net ap set wpa {CCMP, TKIP, NONE}
 * 		- net ap set rsn {CCMP, TKIP, NONE}
 * 		- net ap set proto <NONE, {WPA, RSN}>
 * 		- net ap set auth_alg {OPEN}
 * 		- net ap set group_rekey <seconds>
 * 		- net ap set strict_rekey <0, 1>
 * 		- net ap set gmk_rekey <seconds>
 * 		- net ap set ptk_rekey <seconds>
 * 		- net ap set hw_mode <b, g>
 * 		- net ap set 80211n <0, 1>
 * 		- net ap set channel <1 ~ 13>
 * 		- net ap set beacon_int <15 ~ 65535>
 * 		- net ap set dtim <1 ~ 255>
 * 		- net ap set max_num_sta <num>
 *
 * net ap get <field>
 * 		- net ap get ssid
 * 		- net ap get psk
 * 		- net ap get key_mgmt
 * 		- net ap get wpa
 * 		- net ap get rsn
 * 		- net ap get proto
 * 		- net ap get auth_alg
 * 		- net ap get group_rekey
 * 		- net ap get strict_rekey
 * 		- net ap get gmk_rekey
 * 		- net ap get ptk_rekey
 * 		- net ap get hw_mode
 * 		- net ap get 80211n
 * 		- net ap get channel
 * 		- net ap get beacon_int
 * 		- net ap get dtim
 * 		- net ap get max_num_sta
 *
 * net ap enable
 * net ap reload
 * net ap disable
 *
 * net ap sta num
 * net ap sta info <num>
 */

#include "cmd_util.h"
#include "net/wlan/wlan.h"
#include "net/wlan/wlan_defs.h"
#include "common/framework/net_ctrl.h"

#ifdef __PRJ_CONFIG_WLAN_STA_AP
static const char *g_wlan_mode_str[WLAN_MODE_NUM] = {
	[WLAN_MODE_STA] 	= "station",
	[WLAN_MODE_HOSTAP]	= "hostap",
	[WLAN_MODE_MONITOR] = "monitor",
};

enum cmd_status cmd_wlan_mode_exec(char *cmd)
{
	enum wlan_mode cur_mode, new_mode;
	const char *mode_str;

	if (cmd_strcmp(cmd, "") == 0) {
		cur_mode = wlan_if_get_mode(g_wlan_netif);
		if (cur_mode < WLAN_MODE_NUM) {
			mode_str = g_wlan_mode_str[cur_mode];
		} else {
			mode_str = "invalid";
		}
		cmd_write_respond(CMD_STATUS_OK, "%s", mode_str);
		return CMD_STATUS_ACKED;
	}

	if (cmd_strcmp(cmd, "sta") == 0) {
		new_mode = WLAN_MODE_STA;
	} else if (cmd_strcmp(cmd, "ap") == 0) {
		new_mode = WLAN_MODE_HOSTAP;
	} else if (cmd_strcmp(cmd, "mon") == 0) {
		new_mode = WLAN_MODE_MONITOR;
	} else {
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");
	net_switch_mode(new_mode);

	return CMD_STATUS_ACKED;
}
#endif /* __PRJ_CONFIG_WLAN_STA_AP */

/* wpas parse */

static int cmd_wpas_parse_int(const char *value, int min, int max, int *dst)
{
	int val;
	char *end;

	val = cmd_strtol(value, &end, 0);
	if (*end) {
		CMD_ERR("Invalid number '%s'", value);
		return -1;
	}

	if (val < min || val > max) {
		CMD_ERR("out of range value %d (%s), range is [%d, %d]\n",
			 val, value, min, max);
		return -1;
	}

	*dst = val;
	return 0;
}

static int cmd_wpas_parse_key_mgmt(const char *value)
{
	int val = 0, last, errors = 0;
	char *start, *end, *buf;

	buf = cmd_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		if (cmd_strcmp(start, "WPA-PSK") == 0)
			val |= WPA_KEY_MGMT_PSK;
		else if (cmd_strcmp(start, "WPA-EAP") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X;
		else if (cmd_strcmp(start, "IEEE8021X") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X_NO_WPA;
		else if (cmd_strcmp(start, "NONE") == 0)
			val |= WPA_KEY_MGMT_NONE;
		else if (cmd_strcmp(start, "WPA-NONE") == 0)
			val |= WPA_KEY_MGMT_WPA_NONE;
		else {
			CMD_DBG("Invalid key_mgmt '%s'", start);
			errors++;
		}

		if (last)
			break;
		start = end + 1;
	}
	cmd_free(buf);

	if (val == 0) {
		CMD_DBG("No key_mgmt values configured\n");
		errors++;
	}

	CMD_DBG("key_mgmt: 0x%x\n", val);
	return errors ? -1 : val;
}

static int cmd_wpas_parse_cipher(const char *value)
{
	int val = 0, last;
	char *start, *end, *buf;

	buf = cmd_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		if (cmd_strcmp(start, "CCMP-256") == 0)
			val |= WPA_CIPHER_CCMP_256;
		else if (cmd_strcmp(start, "GCMP-256") == 0)
			val |= WPA_CIPHER_GCMP_256;
		else if (cmd_strcmp(start, "CCMP") == 0)
			val |= WPA_CIPHER_CCMP;
		else if (cmd_strcmp(start, "GCMP") == 0)
			val |= WPA_CIPHER_GCMP;
		else if (cmd_strcmp(start, "TKIP") == 0)
			val |= WPA_CIPHER_TKIP;
		else if (cmd_strcmp(start, "WEP104") == 0)
			val |= WPA_CIPHER_WEP104;
		else if (cmd_strcmp(start, "WEP40") == 0)
			val |= WPA_CIPHER_WEP40;
		else if (cmd_strcmp(start, "NONE") == 0)
			val |= WPA_CIPHER_NONE;
		else if (cmd_strcmp(start, "GTK_NOT_USED") == 0)
			val |= WPA_CIPHER_GTK_NOT_USED;
		else {
			cmd_free(buf);
			return -1;
		}

		if (last)
			break;
		start = end + 1;
	}
	cmd_free(buf);

	return val;
}

static int cmd_wpas_parse_proto(const char *value)
{
	int val = 0, last, errors = 0;
	char *start, *end, *buf;

	buf = cmd_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';

		/* softAP work on open mode. */
		if (cmd_strcmp(start, "NONE") == 0) {
			val = 0;
			break;
		}

		if (cmd_strcmp(start, "WPA") == 0)
			val |= WPA_PROTO_WPA;
		else if (cmd_strcmp(start, "RSN") == 0 ||
			 cmd_strcmp(start, "WPA2") == 0)
			val |= WPA_PROTO_RSN;
		else if (cmd_strcmp(start, "OSEN") == 0)
			val |= WPA_PROTO_OSEN;
		else {
			CMD_DBG("Invalid proto '%s'\n", start);
			errors++;
		}

		if (last)
			break;
		start = end + 1;
	}
	cmd_free(buf);

/* softAP work on open mode. */
#if 0
	if (val == 0) {
		CMD_DBG("No proto values configured\n");
		errors++;
	}
#endif

	CMD_DBG("proto: 0x%x\n", val);
	return errors ? -1 : val;
}

static int cmd_wpas_parse_auth_alg(const char *value)
{
	int val = 0, last, errors = 0;
	char *start, *end, *buf;

	buf = cmd_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		if (cmd_strcmp(start, "OPEN") == 0)
			val |= WPA_AUTH_ALG_OPEN;
		else if (cmd_strcmp(start, "SHARED") == 0)
			val |= WPA_AUTH_ALG_SHARED;
		else if (cmd_strcmp(start, "LEAP") == 0)
			val |= WPA_AUTH_ALG_LEAP;
		else {
			CMD_DBG("Invalid auth_alg '%s'\n", start);
			errors++;
		}

		if (last)
			break;
		start = end + 1;
	}
	cmd_free(buf);

	if (val == 0) {
		CMD_DBG("No auth_alg values configured\n");
		errors++;
	}

	CMD_DBG("auth_alg: 0x%x\n", val);
	return errors ? -1 : val;
}

static __inline void cmd_wlan_sta_print_ap(wlan_sta_ap_t *ap)
{
	CMD_LOG(1, "%02x:%02x:%02x:%02x:%02x:%02x  ssid=%-32.32s  "
		"beacon_int=%d  freq=%d  channel=%u  rssi=%d  level=%d  "
		"flags=%#010x  wpa_key_mgmt=%#010x  wpa_cipher=%#010x  "
		"wpa2_key_mgmt=%#010x  wpa2_cipher=%#010x\n",
		ap->bssid[0], ap->bssid[1],
		ap->bssid[2], ap->bssid[3],
		ap->bssid[4], ap->bssid[5],
		ap->ssid.ssid,
		ap->beacon_int,
		ap->freq,
		ap->channel,
		ap->rssi,
		ap->level,
		ap->wpa_flags,
		ap->wpa_key_mgmt,
		ap->wpa_cipher,
		ap->wpa2_key_mgmt,
		ap->wpa2_cipher);
}


static void cmd_wlan_sta_print_scan_results(wlan_sta_scan_results_t *results)
{
	int i;

	for (i = 0; i < results->num; ++i) {
		CMD_LOG(1, "\n%02d:  ", i + 1);
		cmd_wlan_sta_print_ap(&results->ap[i]);
	}
}

/* @return
 *   -2: CMD_STATUS_INVALID_ARG
 *   -1: CMD_STATUS_FAIL
 *    0: CMD_STATUS_OK
 */
static int cmd_wlan_sta_set(char *cmd)
{
	char *value;
	wlan_sta_config_t config;

	value = cmd_strchr(cmd, ' ');
	if (value == NULL)
		return -2;
	*value++ = '\0';

	config.field = WLAN_STA_FIELD_NUM;

	if (cmd_strcmp(cmd, "ssid") == 0) {
		uint8_t ssid_len = cmd_strlen(value);
		if ((ssid_len >= 1) && (ssid_len <= 32)) {
			config.field = WLAN_STA_FIELD_SSID;
			cmd_memcpy(config.u.ssid.ssid, value, ssid_len);
			config.u.ssid.ssid_len = ssid_len;
		}
	} else if (cmd_strcmp(cmd, "psk") == 0) {
		config.field = WLAN_STA_FIELD_PSK;
		cmd_strlcpy((char *)config.u.psk, value, sizeof(config.u.psk));
	} else if (cmd_strcmp(cmd, "wep_key0") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY0;
		cmd_strlcpy((char *)config.u.wep_key, value, sizeof(config.u.wep_key));
	} else if (cmd_strcmp(cmd, "wep_key1") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY1;
		cmd_strlcpy((char *)config.u.wep_key, value, sizeof(config.u.wep_key));
	} else if (cmd_strcmp(cmd, "wep_key2") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY2;
		cmd_strlcpy((char *)config.u.wep_key, value, sizeof(config.u.wep_key));
	} else if (cmd_strcmp(cmd, "wep_key3") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY3;
		cmd_strlcpy((char *)config.u.wep_key, value, sizeof(config.u.wep_key));
	} else if (cmd_strcmp(cmd, "wep_key_index") == 0) {
		int index;
		if (cmd_wpas_parse_int(value, 0, 3, &index) == 0) {
			config.field = WLAN_STA_FIELD_WEP_KEY_INDEX;
			config.u.wep_tx_keyidx = index;
		}
	} else if (cmd_strcmp(cmd, "key_mgmt") == 0) {
		int key_mgmt = cmd_wpas_parse_key_mgmt(value);
		if (key_mgmt > 0) {
			config.field = WLAN_STA_FIELD_KEY_MGMT;
			config.u.key_mgmt = key_mgmt;
		}
	} else if (cmd_strcmp(cmd, "pairwise") == 0) {
		int pairwise_cipher = cmd_wpas_parse_cipher(value);
		if (pairwise_cipher > 0) {
			config.field = WLAN_STA_FIELD_PAIRWISE_CIPHER;
			config.u.pairwise_cipher = pairwise_cipher;
		}
	} else if (cmd_strcmp(cmd, "group") == 0) {
		int group_cipher = cmd_wpas_parse_cipher(value);
		if (group_cipher > 0) {
			config.field = WLAN_STA_FIELD_GROUP_CIPHER;
			config.u.group_cipher = group_cipher;
		}
	} else if (cmd_strcmp(cmd, "proto") == 0) {
		int proto = cmd_wpas_parse_proto(value);
		if (proto >= 0) {
			config.field = WLAN_STA_FIELD_PROTO;
			config.u.proto = proto;
		}
	} else if (cmd_strcmp(cmd, "auth_alg") == 0) {
		int auth_alg = cmd_wpas_parse_auth_alg(value);
		if (auth_alg > 0) {
			config.field = WLAN_STA_FIELD_AUTH_ALG;
			config.u.auth_alg = auth_alg;
		}
	} else if (cmd_strcmp(cmd, "ptk_rekey") == 0) {
		int sec;
		if (cmd_wpas_parse_int(value, 0, INT32_MAX, &sec) == 0) {
			config.field = WLAN_STA_FIELD_WPA_PTK_REKEY;
			config.u.wpa_ptk_rekey = sec;
		}
	} else if (cmd_strcmp(cmd, "scan_ssid") == 0) {
		int enable;
		if (cmd_wpas_parse_int(value, 0, 1, &enable) == 0) {
			config.field = WLAN_STA_FIELD_SCAN_SSID;
			config.u.scan_ssid = enable;
		}
	}

	if (config.field < WLAN_STA_FIELD_NUM)
		return wlan_sta_set_config(&config);

	CMD_ERR("%s: invalid arg '%s %s'\n", __func__, cmd, value);
	return -2;
}

/* @return
 *   -2: CMD_STATUS_INVALID_ARG
 *   -1: CMD_STATUS_FAIL
 *    0: CMD_STATUS_OK
 */
static int cmd_wlan_sta_get(char *cmd)
{
	wlan_sta_config_t config;
	cmd_memset(&config, 0, sizeof(config));

	if (cmd_strcmp(cmd, "ssid") == 0) {
		config.field = WLAN_STA_FIELD_SSID;
	} else if (cmd_strcmp(cmd, "psk") == 0) {
		config.field = WLAN_STA_FIELD_PSK;
	} else if (cmd_strcmp(cmd, "wep_key0") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY0;
	} else if (cmd_strcmp(cmd, "wep_key1") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY1;
	} else if (cmd_strcmp(cmd, "wep_key2") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY2;
	} else if (cmd_strcmp(cmd, "wep_key3") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY3;
	} else if (cmd_strcmp(cmd, "wep_key_index") == 0) {
		config.field = WLAN_STA_FIELD_WEP_KEY_INDEX;
	} else if (cmd_strcmp(cmd, "key_mgmt") == 0) {
		config.field = WLAN_STA_FIELD_KEY_MGMT;
	} else if (cmd_strcmp(cmd, "pairwise") == 0) {
		config.field = WLAN_STA_FIELD_PAIRWISE_CIPHER;
	} else if (cmd_strcmp(cmd, "group") == 0) {
		config.field = WLAN_STA_FIELD_GROUP_CIPHER;
	} else if (cmd_strcmp(cmd, "proto") == 0) {
		config.field = WLAN_STA_FIELD_PROTO;
	} else if (cmd_strcmp(cmd, "auth_alg") == 0) {
		config.field = WLAN_STA_FIELD_AUTH_ALG;
	} else if (cmd_strcmp(cmd, "ptk_rekey") == 0) {
		config.field = WLAN_STA_FIELD_WPA_PTK_REKEY;
	} else if (cmd_strcmp(cmd, "scan_ssid") == 0) {
		config.field = WLAN_STA_FIELD_SCAN_SSID;
	} else {
		CMD_ERR("%s: invalid arg '%s'\n", __func__, cmd);
		return -2;
	}

	if (wlan_sta_get_config(&config) != 0) {
		CMD_ERR("%s: get config failed\n", __func__);
		return -1;
	}

	if (config.field == WLAN_STA_FIELD_SSID) {
		CMD_LOG(1, "ssid: %.32s\n", config.u.ssid.ssid);
	} else if (config.field == WLAN_STA_FIELD_PSK) {
		CMD_LOG(1, "psk: %s\n", config.u.psk);
	} else if (config.field == WLAN_STA_FIELD_WEP_KEY0) {
		CMD_LOG(1, "wep_key0: %s\n", config.u.wep_key);
	} else if (config.field == WLAN_STA_FIELD_WEP_KEY1) {
		CMD_LOG(1, "wep_key1: %s\n", config.u.wep_key);
	} else if (config.field == WLAN_STA_FIELD_WEP_KEY2) {
		CMD_LOG(1, "wep_key2: %s\n", config.u.wep_key);
	} else if (config.field == WLAN_STA_FIELD_WEP_KEY3) {
		CMD_LOG(1, "wep_key3: %s\n", config.u.wep_key);
	} else if (config.field == WLAN_STA_FIELD_WEP_KEY_INDEX) {
		CMD_LOG(1, "wep_key_index: %d\n", config.u.wep_tx_keyidx);
	} else if (config.field == WLAN_STA_FIELD_KEY_MGMT) {
		CMD_LOG(1, "key_mgmt: %#06x\n", config.u.key_mgmt);
	} else if (config.field == WLAN_STA_FIELD_PAIRWISE_CIPHER) {
		CMD_LOG(1, "pairwise_cipher: %#06x\n", config.u.pairwise_cipher);
	} else if (config.field == WLAN_STA_FIELD_GROUP_CIPHER) {
		CMD_LOG(1, "group_cipher: %#06x\n", config.u.group_cipher);
	} else if (config.field == WLAN_STA_FIELD_PROTO) {
		CMD_LOG(1, "proto: %#06x\n", config.u.proto);
	} else if (config.field == WLAN_STA_FIELD_AUTH_ALG) {
		CMD_LOG(1, "auth_alg: %#06x\n", config.u.auth_alg);
	} else if (config.field == WLAN_STA_FIELD_WPA_PTK_REKEY) {
		CMD_LOG(1, "ptk_rekey: %d\n", config.u.wpa_ptk_rekey);
	} else if (config.field == WLAN_STA_FIELD_SCAN_SSID) {
		CMD_LOG(1, "scan_ssid: %d\n", config.u.scan_ssid);
	}

	return 0;
}

enum cmd_status cmd_wlan_sta_exec(char *cmd)
{
	int ret;

	cmd_write_respond(CMD_STATUS_OK, "OK");

	if (cmd_strncmp(cmd, "config ", 7) == 0) {
		char *argv[2];
		if (cmd_parse_argv(cmd + 7, argv, cmd_nitems(argv)) == 0) {
			ret = -2;
			goto out;
		}
		ret = wlan_sta_set((uint8_t *)argv[0], cmd_strlen(argv[0]), (uint8_t *)argv[1]);
	} else if (cmd_strncmp(cmd, "set ", 4) == 0) {
		ret = cmd_wlan_sta_set(cmd + 4);
	} else if (cmd_strncmp(cmd, "get ", 4) == 0) {
		ret = cmd_wlan_sta_get(cmd + 4);
	} else if (cmd_strcmp(cmd, "enable") == 0) {
		ret = wlan_sta_enable();
	} else if (cmd_strcmp(cmd, "disable") == 0) {
		ret = wlan_sta_disable();
	} else if (cmd_strcmp(cmd, "scan once") == 0) {
		ret = wlan_sta_scan_once();
	} else if (cmd_strncmp(cmd, "scan result ", 12) == 0) {
		int size;
		if (cmd_wpas_parse_int(cmd + 12, 1, 30, &size) != 0) {
			ret = -2;
			goto out;
		}
		wlan_sta_scan_results_t results;
		results.ap = cmd_malloc(size * sizeof(wlan_sta_ap_t));
		if (results.ap == NULL) {
			CMD_ERR("%s: malloc failed\n", __func__);
			ret = -1;
			goto out;
		}
		results.size = size;
		ret = wlan_sta_scan_result(&results);
		if (ret == 0)
			cmd_wlan_sta_print_scan_results(&results);
		cmd_free(results.ap);
	} else if (cmd_strncmp(cmd, "scan interval ", 14) == 0) {
		int sec;
		if (cmd_wpas_parse_int(cmd + 14, 0, INT32_MAX, &sec) != 0) {
			ret = -2;
			goto out;
		}
		ret = wlan_sta_scan_interval(sec);
	} else if (cmd_strncmp(cmd, "bss flush ", 10) == 0) {
		int age;
		if (cmd_wpas_parse_int(cmd + 10, 0, INT32_MAX, &age) != 0) {
			ret = -2;
			goto out;
		}
		ret = wlan_sta_bss_flush(age);
	} else if (cmd_strcmp(cmd, "connect") == 0) {
		ret = wlan_sta_connect();
	} else if (cmd_strcmp(cmd, "disconnect") == 0) {
		ret = wlan_sta_disconnect();
	} else if (cmd_strcmp(cmd, "state") == 0) {
		wlan_sta_states_t state;
		ret = wlan_sta_state(&state);
		if (ret == 0)
			CMD_LOG(1, "sta state: %d\n", state);
	} else if (cmd_strcmp(cmd, "ap") == 0) {
		wlan_sta_ap_t *ap = cmd_malloc(sizeof(wlan_sta_ap_t));
		if (ap == NULL) {
			CMD_ERR("%s: malloc failed\n", __func__);
			ret = -1;
			goto out;
		}
		ret = wlan_sta_ap_info(ap);
		if (ret == 0)
			cmd_wlan_sta_print_ap(ap);
		cmd_free(ap);
	} else if (cmd_strcmp(cmd, "wps pbc") == 0) {
		ret = wlan_sta_wps_pbc();
	} else if ((cmd_strlen(cmd) == 7) || (cmd_strcmp(cmd, "wps pin") == 0)) {
		wlan_sta_wps_pin_t wps;
		ret = wlan_sta_wps_pin_get(&wps);
		if (ret == 0)
			CMD_LOG(1, "WPS pin: %s\n", wps.pin);
	} else if (cmd_strncmp(cmd, "wps pin ", 8) == 0) {
		if (cmd_strlen(cmd + 8) != 8) {
			ret = -2;
			goto out;
		}
		wlan_sta_wps_pin_t wps;
		cmd_memcpy(wps.pin, cmd + 8, 8);
		wps.pin[8] = '\0';
		ret = wlan_sta_wps_pin_set(&wps);
	} else {
		CMD_ERR("%s: unknown command '%s'\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

out:
	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_ACKED;
}

static void cmd_wlan_ap_print_sta_info(wlan_ap_stas_t *stas)
{
	int i;

	CMD_LOG(1, "sta_num: %d\n", stas->num);

	for (i = 0; i < stas->num; i++) {
		CMD_LOG(1, "[%02d]Mac addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			i + 1, stas->sta[i].addr[0], stas->sta[i].addr[1],
			stas->sta[i].addr[2], stas->sta[i].addr[3],
			stas->sta[i].addr[4], stas->sta[i].addr[5]);
	}
}

/* @return
 *   -2: CMD_STATUS_INVALID_ARG
 *   -1: CMD_STATUS_FAIL
 *    0: CMD_STATUS_OK
 */
static int cmd_wlan_ap_set(char *cmd)
{
	char *value;
	wlan_ap_config_t config;

	value = cmd_strchr(cmd, ' ');
	if (value == NULL)
		return -2;
	*value++ = '\0';

	config.field = WLAN_AP_FIELD_NUM;

	if (cmd_strcmp(cmd, "ssid") == 0) {
		uint8_t ssid_len = cmd_strlen(value);
		if ((ssid_len >= 1) && (ssid_len <= 32)) {
			config.field = WLAN_STA_FIELD_SSID;
			cmd_memcpy(config.u.ssid.ssid, value, ssid_len);
			config.u.ssid.ssid_len = ssid_len;
		}
	} else if (cmd_strcmp(cmd, "psk") == 0) {
		config.field = WLAN_AP_FIELD_PSK;
		cmd_strlcpy((char *)config.u.psk, value, sizeof(config.u.psk));
	} else if (cmd_strcmp(cmd, "key_mgmt") == 0) {
		int key_mgmt = cmd_wpas_parse_key_mgmt(value);
		if (key_mgmt > 0) {
			config.field = WLAN_AP_FIELD_KEY_MGMT;
			config.u.key_mgmt = key_mgmt;
		}
	} else if (cmd_strcmp(cmd, "wpa") == 0) {
		int wpa_cipher = cmd_wpas_parse_cipher(value);
		if (wpa_cipher > 0) {
			config.field = WLAN_AP_FIELD_WPA_CIPHER;
			config.u.wpa_cipher = wpa_cipher;
		}
	} else if (cmd_strcmp(cmd, "rsn") == 0) {
		int rsn_cipher = cmd_wpas_parse_cipher(value);
		if (rsn_cipher > 0) {
			config.field = WLAN_AP_FIELD_RSN_CIPHER;
			config.u.rsn_cipher = rsn_cipher;
		}
	} else if (cmd_strcmp(cmd, "proto") == 0) {
		int proto = cmd_wpas_parse_proto(value);
		if (proto >= 0) {
			config.field = WLAN_AP_FIELD_PROTO;
			config.u.proto = proto;
		}
	} else if (cmd_strcmp(cmd, "auth_alg") == 0) {
		int auth_alg = cmd_wpas_parse_auth_alg(value);
		if (auth_alg > 0) {
			config.field = WLAN_AP_FIELD_AUTH_ALG;
			config.u.auth_alg = auth_alg;
		}
	} else if (cmd_strcmp(cmd, "group_rekey") == 0) {
		int group_rekey;
		if (cmd_wpas_parse_int(value, 0, INT32_MAX, &group_rekey) == 0) {
			config.field = WLAN_AP_FIELD_GROUP_REKEY;
			config.u.group_rekey = group_rekey;
		}
	} else if (cmd_strcmp(cmd, "strict_rekey") == 0) {
		int strict_rekey;
		if (cmd_wpas_parse_int(value, 0, 1, &strict_rekey) == 0) {
			config.field = WLAN_AP_FIELD_STRICT_REKEY;
			config.u.strict_rekey = strict_rekey;
		}
	} else if (cmd_strcmp(cmd, "gmk_rekey") == 0) {
		int gmk_rekey;
		if (cmd_wpas_parse_int(value, 0, INT32_MAX, &gmk_rekey) == 0) {
			config.field = WLAN_AP_FIELD_GMK_REKEY;
			config.u.gmk_rekey = gmk_rekey;
		}
	} else if (cmd_strcmp(cmd, "ptk_rekey") == 0) {
		int ptk_rekey;
		if (cmd_wpas_parse_int(value, 0, INT32_MAX, &ptk_rekey) == 0) {
			config.field = WLAN_AP_FIELD_PTK_REKEY;
			config.u.ptk_rekey = ptk_rekey;
		}
	} else if (cmd_strcmp(cmd, "hw_mode") == 0) {
		if ((value[0] == 'b') && (value[1] == '\0')) {
			config.field = WLAN_AP_FIELD_HW_MODE;
			config.u.hw_mode = WLAN_AP_HW_MODE_IEEE80211B;
		} else if ((value[0] == 'g') && (value[1] == '\0')) {
			config.field = WLAN_AP_FIELD_HW_MODE;
			config.u.hw_mode = WLAN_AP_HW_MODE_IEEE80211G;
		}
	} else if (cmd_strcmp(cmd, "80211n") == 0) {
		int ieee80211n;
		if (cmd_wpas_parse_int(value, 0, 1, &ieee80211n) == 0) {
			config.field = WLAN_AP_FIELD_IEEE80211N;
			config.u.ieee80211n = ieee80211n;
		}
	} else if (cmd_strcmp(cmd, "channel") == 0) {
		int channel;
		if (cmd_wpas_parse_int(value, 1, 14, &channel) == 0) {
			config.field = WLAN_AP_FIELD_CHANNEL;
			config.u.channel = channel;
		}
	} else if (cmd_strcmp(cmd, "beacon_int") == 0) {
		int beacon_int;
		if (cmd_wpas_parse_int(value, 15, 65535, &beacon_int) == 0) {
			config.field = WLAN_AP_FIELD_BEACON_INT;
			config.u.beacon_int = beacon_int;
		}
	} else if (cmd_strcmp(cmd, "dtim") == 0) {
		int dtim;
		if (cmd_wpas_parse_int(value, 1, 255, &dtim) == 0) {
			config.field = WLAN_AP_FIELD_DTIM;
			config.u.dtim = dtim;
		}
	} else if (cmd_strcmp(cmd, "max_num_sta") == 0) {
		int max_num_sta;
		if (cmd_wpas_parse_int(value, 0, INT32_MAX, &max_num_sta) == 0) {
			config.field = WLAN_AP_FIELD_MAX_NUM_STA;
			config.u.max_num_sta = max_num_sta;
		}
	}

	if (config.field < WLAN_AP_FIELD_NUM)
		return wlan_ap_set_config(&config);

	CMD_ERR("%s: invalid arg '%s %s'\n", __func__, cmd, value);
	return -2;
}

/* @return
 *   -2: CMD_STATUS_INVALID_ARG
 *   -1: CMD_STATUS_FAIL
 *    0: CMD_STATUS_OK
 */
static int cmd_wlan_ap_get(char *cmd)
{
	wlan_ap_config_t config;
	cmd_memset(&config, 0, sizeof(config));

	if (cmd_strcmp(cmd, "ssid") == 0) {
		config.field = WLAN_AP_FIELD_SSID;
	} else if (cmd_strcmp(cmd, "psk") == 0) {
		config.field = WLAN_AP_FIELD_PSK;
	} else if (cmd_strcmp(cmd, "key_mgmt") == 0) {
		config.field = WLAN_AP_FIELD_KEY_MGMT;
	} else if (cmd_strcmp(cmd, "wpa") == 0) {
		config.field = WLAN_AP_FIELD_WPA_CIPHER;
	} else if (cmd_strcmp(cmd, "rsn") == 0) {
		config.field = WLAN_AP_FIELD_RSN_CIPHER;
	} else if (cmd_strcmp(cmd, "proto") == 0) {
		config.field = WLAN_AP_FIELD_PROTO;
	} else if (cmd_strcmp(cmd, "auth_alg") == 0) {
		config.field = WLAN_AP_FIELD_AUTH_ALG;
	} else if (cmd_strcmp(cmd, "group_rekey") == 0) {
		config.field = WLAN_AP_FIELD_GROUP_REKEY;
	} else if (cmd_strcmp(cmd, "strict_rekey") == 0) {
		config.field = WLAN_AP_FIELD_STRICT_REKEY;
	} else if (cmd_strcmp(cmd, "gmk_rekey") == 0) {
		config.field = WLAN_AP_FIELD_GMK_REKEY;
	} else if (cmd_strcmp(cmd, "ptk_rekey") == 0) {
		config.field = WLAN_AP_FIELD_PTK_REKEY;
	} else if (cmd_strcmp(cmd, "hw_mode") == 0) {
		config.field = WLAN_AP_FIELD_HW_MODE;
	} else if (cmd_strcmp(cmd, "80211n") == 0) {
		config.field = WLAN_AP_FIELD_IEEE80211N;
	} else if (cmd_strcmp(cmd, "channel") == 0) {
		config.field = WLAN_AP_FIELD_CHANNEL;
	} else if (cmd_strcmp(cmd, "beacon_int") == 0) {
		config.field = WLAN_AP_FIELD_BEACON_INT;
	} else if (cmd_strcmp(cmd, "dtim") == 0) {
		config.field = WLAN_AP_FIELD_DTIM;
	} else if (cmd_strcmp(cmd, "max_num_sta") == 0) {
		config.field = WLAN_AP_FIELD_MAX_NUM_STA;
	} else {
		CMD_ERR("%s: invalid arg '%s'\n", __func__, cmd);
		return -2;
	}

	if (wlan_ap_get_config(&config) != 0) {
		CMD_ERR("%s: get config failed\n", __func__);
		return -1;
	}

	if (config.field == WLAN_AP_FIELD_SSID) {
		CMD_LOG(1, "ssid: %.32s\n", config.u.ssid.ssid);
	} else if (config.field == WLAN_AP_FIELD_PSK) {
		CMD_LOG(1, "psk: %s\n", config.u.psk);
	} else if (config.field == WLAN_AP_FIELD_KEY_MGMT) {
		CMD_LOG(1, "key_mgmt: %#06x\n", config.u.key_mgmt);
	} else if (config.field == WLAN_AP_FIELD_WPA_CIPHER) {
		CMD_LOG(1, "wpa_cipher: %#06x\n", config.u.wpa_cipher);
	} else if (config.field == WLAN_AP_FIELD_RSN_CIPHER) {
		CMD_LOG(1, "rsn_cipher: %#06x\n", config.u.rsn_cipher);
	} else if (config.field == WLAN_AP_FIELD_PROTO) {
		CMD_LOG(1, "proto: %#06x\n", config.u.proto);
	} else if (config.field == WLAN_AP_FIELD_AUTH_ALG) {
		CMD_LOG(1, "auth_alg: %#06x\n", config.u.auth_alg);
	} else if (config.field == WLAN_AP_FIELD_GROUP_REKEY) {
		CMD_LOG(1, "group_rekey: %d\n", config.u.group_rekey);
	} else if (config.field == WLAN_AP_FIELD_STRICT_REKEY) {
		CMD_LOG(1, "strict_rekey: %d\n", config.u.strict_rekey);
	} else if (config.field == WLAN_AP_FIELD_GMK_REKEY) {
		CMD_LOG(1, "gmk_rekey: %d\n", config.u.gmk_rekey);
	} else if (config.field == WLAN_AP_FIELD_PTK_REKEY) {
		CMD_LOG(1, "ptk_rekey: %d\n", config.u.ptk_rekey);
	} else if (config.field == WLAN_AP_FIELD_HW_MODE) {
		if (config.u.hw_mode == WLAN_AP_HW_MODE_IEEE80211B) {
			CMD_LOG(1, "hw_mode: b\n");
		} else if (config.u.hw_mode == WLAN_AP_HW_MODE_IEEE80211G) {
			CMD_LOG(1, "hw_mode: g\n");
		} else if (config.u.hw_mode == WLAN_AP_HW_MODE_IEEE80211A) {
			CMD_LOG(1, "hw_mode: a\n");
		} else if (config.u.hw_mode == WLAN_AP_HW_MODE_IEEE80211AD) {
			CMD_LOG(1, "hw_mode: ad\n");
		} else {
			CMD_ERR("%s: invalid hw_mode\n", __func__);
		}
	} else if (config.field == WLAN_AP_FIELD_IEEE80211N) {
		CMD_LOG(1, "ieee80211n: %d\n", config.u.ieee80211n);
	} else if (config.field == WLAN_AP_FIELD_CHANNEL) {
		CMD_LOG(1, "channel: %d\n", config.u.channel);
	} else if (config.field == WLAN_AP_FIELD_BEACON_INT) {
		CMD_LOG(1, "beacon_int: %d\n", config.u.beacon_int);
	} else if (config.field == WLAN_AP_FIELD_DTIM) {
		CMD_LOG(1, "dtim: %d\n", config.u.dtim);
	} else if (config.field == WLAN_AP_FIELD_MAX_NUM_STA) {
		CMD_LOG(1, "max_num_sta: %d\n", config.u.max_num_sta);
	}

	return 0;
}

enum cmd_status cmd_wlan_ap_exec(char *cmd)
{
	int ret;

	cmd_write_respond(CMD_STATUS_OK, "OK");

	if (cmd_strncmp(cmd, "config ", 7) == 0) {
		char *argv[2];
		if (cmd_parse_argv(cmd + 7, argv, cmd_nitems(argv)) == 0) {
			ret = -2;
			goto out;
		}
		ret = wlan_ap_set((uint8_t *)argv[0], cmd_strlen(argv[0]), (uint8_t *)argv[1]);
	} else if (cmd_strncmp(cmd, "set ", 4) == 0) {
		ret = cmd_wlan_ap_set(cmd + 4);
	} else if (cmd_strncmp(cmd, "get ", 4) == 0) {
		ret = cmd_wlan_ap_get(cmd + 4);
	} else if (cmd_strcmp(cmd, "enable") == 0) {
		ret = wlan_ap_enable();
	} else if (cmd_strcmp(cmd, "reload") == 0) {
		ret = wlan_ap_reload();
	} else if (cmd_strcmp(cmd, "disable") == 0) {
		ret = wlan_ap_disable();
	} else if (cmd_strcmp(cmd, "sta num") == 0) {
		int num;
		ret = wlan_ap_sta_num(&num);
		if (ret == 0)
			CMD_LOG(1, "sta num: %d\n", num);
	} else if (cmd_strncmp(cmd, "sta info ", 9) == 0) {
		int size;
		if (cmd_wpas_parse_int(cmd + 9, 1, 30, &size) != 0) {
			ret = -2;
			goto out;
		}
		wlan_ap_stas_t stas;
		stas.sta = (wlan_ap_sta_t *)cmd_malloc(size * sizeof(wlan_ap_sta_t));
		if (stas.sta == NULL) {
			CMD_ERR("%s: malloc failed\n", __func__);
			ret = -1;
			goto out;
		}
		stas.size = size;
		ret = wlan_ap_sta_info(&stas);
		if (ret == 0)
			cmd_wlan_ap_print_sta_info(&stas);
		cmd_free(stas.sta);
	} else {
		CMD_ERR("%s: unknown command '%s'\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

out:
	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_ACKED;
}

