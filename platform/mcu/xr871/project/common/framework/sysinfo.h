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

#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#include <stdint.h>
#include "lwip/netif.h"
#include "net/wlan/wlan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sysinfo MAC address source definition
 */
#define SYSINFO_MAC_ADDR_CODE		(0x0U)
#define SYSINFO_MAC_ADDR_EFUSE		(0x1U)
#define SYSINFO_MAC_ADDR_FLASH		(0x2U)

#define SYSINFO_MAC_ADDR_LEN		(6)

#define SYSINFO_SSID_LEN_MAX		(32)
#define SYSINFO_PSK_LEN_MAX			(65)

/**
 * @brief Sysinfo station wlan parameters definition
 */
struct sysinfo_wlan_sta_param {
	uint8_t ssid[SYSINFO_SSID_LEN_MAX];
	uint8_t ssid_len;

	uint8_t psk[SYSINFO_PSK_LEN_MAX];
};

/**
 * @brief Sysinfo AP wlan parameters definition
 */
struct sysinfo_wlan_ap_param {
	uint8_t ssid[SYSINFO_SSID_LEN_MAX];
	uint8_t ssid_len;

	uint8_t psk[SYSINFO_PSK_LEN_MAX];

	uint8_t channel;
};

/**
 * @brief Sysinfo net interface parameters definition
 */
struct sysinfo_netif_param {
#ifdef __CONFIG_LWIP_V1
	ip_addr_t ip_addr;
	ip_addr_t net_mask;
	ip_addr_t gateway;
#elif LWIP_IPV4 /* now only for IPv4 */
	ip4_addr_t ip_addr;
	ip4_addr_t net_mask;
	ip4_addr_t gateway;
#else
	#error "IPv4 not support!"
#endif
};

/**
 * @brief Sysinfo structure definition
 */
struct sysinfo {
	uint32_t version;

	uint32_t sta_use_dhcp : 1;

	uint8_t mac_addr[SYSINFO_MAC_ADDR_LEN];

	enum wlan_mode wlan_mode;

	struct sysinfo_wlan_sta_param wlan_sta_param;
	struct sysinfo_wlan_ap_param wlan_ap_param;

	struct sysinfo_netif_param netif_sta_param;
	struct sysinfo_netif_param netif_ap_param;
};

#define SYSINFO_SIZE	sizeof(struct sysinfo)

int sysinfo_init(void);
void sysinfo_deinit(void);

int sysinfo_default(void);

int sysinfo_save(void);
int sysinfo_load(void);

struct sysinfo *sysinfo_get(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSINFO_H_ */
