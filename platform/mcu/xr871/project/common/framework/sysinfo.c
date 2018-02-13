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
#include <stdlib.h>

#include "efpg/efpg.h"
#include "sys/fdcm.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "sysinfo.h"
#include "sysinfo_debug.h"

static struct sysinfo g_sysinfo;
static fdcm_handle_t *g_fdcm_hdl;

static uint8_t m_sysinfo_mac_addr[] = { 0x00, 0x80, 0xE1, 0x29, 0xE8, 0xD1 };

static void sysinfo_init_mac_addr(void)
{
	struct sysinfo *info;

	switch (PRJCONF_MAC_ADDR_SOURCE) {
	case SYSINFO_MAC_ADDR_CODE:
		SYSINFO_DBG("mac addr source: code\n");
		memcpy(g_sysinfo.mac_addr, m_sysinfo_mac_addr, SYSINFO_MAC_ADDR_LEN);
		return;
	case SYSINFO_MAC_ADDR_EFUSE:
		SYSINFO_DBG("mac addr source: eFuse\n");
		if (efpg_read(EFPG_FIELD_MAC, g_sysinfo.mac_addr) != 0) {
			SYSINFO_WRN("failed to read MAC address from eFuse\n");
			goto random_mac_addr;
		}
		return;
	case SYSINFO_MAC_ADDR_FLASH:
		SYSINFO_DBG("mac addr source: flash\n");
		info = malloc(SYSINFO_SIZE);
		if (info == NULL) {
			SYSINFO_ERR("malloc failed\n");
			goto random_mac_addr;
		}
		if (fdcm_read(g_fdcm_hdl, info, SYSINFO_SIZE) != SYSINFO_SIZE) {
			SYSINFO_WRN("failed to read MAC address from flash\n");
			free(info);
			goto random_mac_addr;
		}
		memcpy(g_sysinfo.mac_addr, info->mac_addr, SYSINFO_MAC_ADDR_LEN);
		free(info);
		return;
	default:
		SYSINFO_ERR("invalid mac addr source\n");
		goto random_mac_addr;
	}

random_mac_addr:
	SYSINFO_DBG("random mac address\n");

	int i;
	uint32_t chipid[4] = {0};

	g_sysinfo.mac_addr[0] = 0x00;
	g_sysinfo.mac_addr[1] = 0x54;
	efpg_read(EFPG_FIELD_CHIPID, chipid);

	for (i = 2; i < SYSINFO_MAC_ADDR_LEN; i++) {
		srand(chipid[i - 2]);
		g_sysinfo.mac_addr[i] = rand() % 256;
	}
}

static void sysinfo_init_value(void)
{
	if (sysinfo_load() != 0) {
		sysinfo_default();
		return;
	}

	if (PRJCONF_MAC_ADDR_SOURCE != SYSINFO_MAC_ADDR_FLASH) {
		sysinfo_init_mac_addr();
	}
}

/**
 * @brief Initialize the sysinfo module
 * @return 0 on success, -1 on failure
 */
int sysinfo_init(void)
{
	g_fdcm_hdl = fdcm_open(PRJCONF_SYSINFO_FLASH, PRJCONF_SYSINFO_ADDR, PRJCONF_SYSINFO_SIZE);
	if (g_fdcm_hdl == NULL) {
		SYSINFO_ERR("fdcm open failed, hdl %p\n", g_fdcm_hdl);
		return -1;
	}

	sysinfo_init_value();

	return 0;
}

/**
 * @brief DeInitialize the sysinfo module
 * @return None
 */
void sysinfo_deinit(void)
{
	fdcm_close(g_fdcm_hdl);
}

/**
 * @brief Set default value to sysinfo
 * @return 0 on success, -1 on failure
 */
int sysinfo_default(void)
{
	if (g_fdcm_hdl == NULL) {
		SYSINFO_ERR("uninitialized, hdl %p\n", g_fdcm_hdl);
		return -1;
	}

	memset(&g_sysinfo, 0, SYSINFO_SIZE);

	/* MAC address */
	sysinfo_init_mac_addr();

	/* wlan mode */
	g_sysinfo.wlan_mode = WLAN_MODE_STA;

	/* netif STA */
	g_sysinfo.sta_use_dhcp = 1;

	/* netif AP */
	IP4_ADDR(&g_sysinfo.netif_ap_param.ip_addr, 192, 168, 51, 1);
	IP4_ADDR(&g_sysinfo.netif_ap_param.net_mask, 255, 255, 255, 0);
	IP4_ADDR(&g_sysinfo.netif_ap_param.gateway, 192, 168, 51, 1);

	SYSINFO_DBG("set default value\n");

	return 0;
}

/**
 * @brief Save sysinfo to flash
 * @return 0 on success, -1 on failure
 */
int sysinfo_save(void)
{
	if (g_fdcm_hdl == NULL) {
		SYSINFO_ERR("uninitialized, hdl %p\n", g_fdcm_hdl);
		return -1;
	}

	if (fdcm_write(g_fdcm_hdl, &g_sysinfo, SYSINFO_SIZE) != SYSINFO_SIZE) {
		SYSINFO_ERR("fdcm write failed\n");
		return -1;
	}

	SYSINFO_DBG("save sysinfo to flash\n");

	return 0;
}

/**
 * @brief Load sysinfo from flash
 * @return 0 on success, -1 on failure
 */
int sysinfo_load(void)
{
	if (g_fdcm_hdl == NULL) {
		SYSINFO_ERR("uninitialized, hdl %p\n", g_fdcm_hdl);
		return -1;
	}

	if (fdcm_read(g_fdcm_hdl, &g_sysinfo, SYSINFO_SIZE) != SYSINFO_SIZE) {
		SYSINFO_WRN("fdcm read failed\n");
		return -1;
	}

	SYSINFO_DBG("load sysinfo from flash\n");

	return 0;
}

/**
 * @brief Get the pointer of the sysinfo
 * @return Pointer to the sysinfo, NULL on failure
 */
struct sysinfo *sysinfo_get(void)
{
	if (g_fdcm_hdl == NULL) {
		SYSINFO_ERR("uninitialized, hdl %p\n", g_fdcm_hdl);
		return NULL;
	}

	return &g_sysinfo;
}

