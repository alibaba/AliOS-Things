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

#include "pm/pm.h"
#include "sys/image.h"
#include "net/wlan/wlan.h"
#include "xz/decompress.h"
#include "sys/ducc/ducc_net.h"
#include "sys/ducc/ducc_app.h"
#include "driver/chip/hal_util.h"
#include "driver/chip/hal_ccm.h"
#include "driver/chip/hal_prcm.h"
#include "driver/chip/hal_efuse.h"
#include "kernel/os/os_semaphore.h"

int wpa_ctrl_request(wpa_ctrl_cmd_t cmd, void *data)
{
	struct ducc_param_wlan_wpa_ctrl_req param;
	param.cmd = cmd;
	param.data = data;

	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_WPA_CTRL_REQUEST, &param);
}

static __inline int wpa_ctrl_open(void)
{
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_WPA_CTRL_OPEN, NULL);
}

static __inline void wpa_ctrl_close(void)
{
	ducc_app_ioctl(DUCC_APP_CMD_WLAN_WPA_CTRL_CLOSE, NULL);
}

int wlan_start(struct netif *nif)
{
	if (ducc_app_ioctl(DUCC_APP_CMD_WLAN_START, nif->state) != 0) {
		return -1;
	}

	return wpa_ctrl_open();
}

/* Note: make sure wlan is disconnect before calling wlan_stop() */
int wlan_stop(void)
{
	if (ducc_app_ioctl(DUCC_APP_CMD_WLAN_STOP, NULL) != 0) {
		return -1;
	}
	wpa_ctrl_close();
	return 0;
}

int wlan_set_mac_addr(uint8_t *mac_addr, int mac_len)
{
	struct ducc_param_wlan_set_mac_addr param;
	param.mac_addr = mac_addr;
	param.mac_len = mac_len;
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_SET_MAC_ADDR, &param);
}

int wlan_set_ip_addr(struct netif *nif, uint8_t *ip_addr, int ip_len)
{
	struct ducc_param_wlan_set_ip_addr param;
	param.ifp = nif->state;
	param.ip_addr = ip_addr;
	param.ip_len = ip_len;
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_SET_IP_ADDR, &param);
}

/**
 * @brief Set application-specified IE to specified management frame
 * @param[in] nif Pointer to the network interface
 * @param[in] type Management frame type to be set
 *                 eg. (IEEE80211_FC_TYPE_MGMT | IEEE80211_FC_STYPE_BEACON)
 * @param[in] ie Pointer to application-specified IE data
 * @param[in] ie_len Length of application-specified IE data
 * @return 0 on success
 *
 * @note To delete existing application-specified IE, set ie to NULL, or set
 *       ie_len to 0.
 */
int wlan_set_appie(struct netif *nif, uint8_t type, uint8_t *ie, uint16_t ie_len)
{
	struct ducc_param_wlan_appie param;
	enum wlan_mode mode = ethernetif_get_mode(nif);

	if (mode != WLAN_MODE_STA && mode != WLAN_MODE_HOSTAP) {
		WLAN_DBG("unsupport mode %d\n", mode);
		return -1;
	}

	param.ifp = nif->state;
	param.type = type;
	param.ie = ie;
	param.ie_len = ie_len;
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_SET_APPIE, &param);
}

/* monitor */
static wlan_monitor_rx_cb m_wlan_monitor_rx_cb = NULL;

#define DUMP_MON_DATA 0
#if DUMP_MON_DATA
#define DUMPDLEN 40
uint8_t dump_data[DUMPDLEN];
#endif

void wlan_monitor_input(struct netif *nif, uint8_t *data, uint32_t len, void *info)
{
	#if DUMP_MON_DATA
	{
		uint32_t dump_len = len > DUMPDLEN ? DUMPDLEN : len;
		memset(dump_data, 0, dump_len);
		memcpy(dump_data, data, dump_len);
		if( dump_data[4] == 0x01 && dump_data[5] == 0x00 && dump_data[6] == 0x5e ) {
			for(int i= 4; i < dump_len ;i++) {
				printf("0x%02x ", dump_data[i]);
			}
			printf("\n");
		}
	}
	#endif

	if (m_wlan_monitor_rx_cb) {
		m_wlan_monitor_rx_cb(data, len, info);
	}
}

int wlan_monitor_set_rx_cb(struct netif *nif, wlan_monitor_rx_cb cb)
{
	int enable = cb ? 1 : 0;
	m_wlan_monitor_rx_cb = cb;
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_MONITOR_ENABLE_RX, (void *)enable);
}

int wlan_monitor_set_channel(struct netif *nif, int16_t channel)
{
	struct ducc_param_wlan_mon_set_chan param;
	enum wlan_mode mode = ethernetif_get_mode(nif);

	if (mode != WLAN_MODE_MONITOR) {
		WLAN_DBG("NOT in monitor mode, current mode %d\n", mode);
		return -1;
	}

	param.ifp = nif->state;
	param.channel = channel;
	return ducc_app_ioctl(DUCC_APP_CMD_WLAN_MONITOR_SET_CHAN, &param);
}

/* PM */
#ifdef CONFIG_PM
static int m_wlan_suspending;

static int wlan_power_notify(enum suspend_state_t state)
{
	return ducc_app_ioctl(DUCC_APP_CMD_POWER_NOTIFY, (void *)state);
}

int wlan_wakeup_net(void)
{
	return wlan_power_notify(PM_MODE_ON);
}

static int wlan_power_callback(int state)
{
	m_wlan_suspending = 0;

	return 0;
}

static int wlan_sys_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	int err = 0;
	uint32_t _timeout = OS_GetTicks() + OS_SecsToJiffies(3);

	switch (state) {
	case PM_MODE_STANDBY:
		m_wlan_suspending = 1;
		wlan_power_notify(PM_MODE_ON);
		wlan_power_notify(state);
		while (!HAL_PRCM_IsCPUNDeepSleep() && m_wlan_suspending &&
		       OS_TimeBefore(OS_GetTicks(), _timeout)) {
			OS_MSleep(5);
		}
		if (OS_TimeAfterEqual(OS_GetTicks(), _timeout) || !m_wlan_suspending) {
			err = -1;
			break;
		}
		OS_MSleep(2);
		break;
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		/* step1: notify net cpu to switch to HOSC, turn off SYSCLK2 and enter WFI state. */
		m_wlan_suspending = 1;
		wlan_power_notify(PM_MODE_POWEROFF);
		while (!HAL_PRCM_IsCPUNSleep() && m_wlan_suspending) {
			OS_MSleep(5);
		}
		if (!m_wlan_suspending)
			WLAN_WARN("wlan poweroff faild!\n");
		OS_MSleep(5); /* wait net cpu enter wfi */

		/* step2: writel(0x00, GPRCM_SYS2_CRTL) to reset and isolation network system. */
		HAL_PRCM_EnableSys2Isolation();
		HAL_PRCM_ForceCPUNReset();
		HAL_PRCM_ForceSys2Reset();
		OS_MSleep(5);
		HAL_PRCM_DisableSys2Power();
		WLAN_DBG("%s okay\n", __func__);
		break;
	default:
		break;
	}

	return err;
}

static int wlan_sys_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_STANDBY:
		/* maybe wakeup net at this time better than later by other cmds */
		pm_set_sync_magic();
		wlan_power_notify(PM_MODE_ON);
		m_wlan_suspending = 0;
		WLAN_DBG("%s okay\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static struct soc_device_driver m_wlan_sys_drv = {
	.name = "wlan_sys",
	.suspend = wlan_sys_suspend,
	.resume = wlan_sys_resume,
};

static struct soc_device m_wlan_sys_dev = {
	.name = "wlan_sys",
	.driver = &m_wlan_sys_drv,
};

#define WLAN_SYS_DEV (&m_wlan_sys_dev)

#else /* CONFIG_PM */

static int wlan_power_callback(int state)
{
	return 0;
}

#define WLAN_SYS_DEV NULL

#endif /* CONFIG_PM */

static OS_Semaphore_t m_ducc_sync_sem; /* use to sync with net system */
static ducc_cb_func m_wlan_net_sys_cb = NULL;

#define COMP_BUF_SIZE (4 * 1024)
#define STREAN_FOOT_LEN 12

static uint32_t wlan_net_uncompress_size(uint32_t image_id, section_header_t *sh, uint8_t *buf, uint32_t buf_size)
{
	uint32_t read_len = 0;

	if (buf_size < 1204) {
		WLAN_ERR("%s: %d The buf size minimum 1KB, buf_size %d\n", __func__, __LINE__, buf_size);
		goto error;
	}

	read_len = image_read(image_id, IMAGE_SEG_BODY, sh->body_len - STREAN_FOOT_LEN, buf, STREAN_FOOT_LEN);
	if (read_len != STREAN_FOOT_LEN) {
		WLAN_ERR("%s: %d read image error, len %d\n", __func__, __LINE__, read_len);
		goto error;
	}

	uint32_t  index_len = xz_index_len(buf);

	read_len = image_read(image_id, IMAGE_SEG_BODY, sh->body_len - STREAN_FOOT_LEN - index_len, buf, index_len);
	if (read_len != index_len) {
		WLAN_ERR("%s: %d read image error, len %d\n", __func__, __LINE__, read_len);
		goto error;
	}

	return xz_file_uncompress_size(buf, read_len);

	error:
	return 0;
}

static int wlan_compress_bin(uint32_t image_id, section_header_t *sh)
{
	struct xz_buf stream;
	uint32_t read_len = 0;
	uint32_t ret;
	uint32_t d_len = 0;
	uint32_t compress_len = 0;
	uint32_t read_count = 0;
	uint32_t last_len = 0;
	uint16_t checksum = 0;
	uint8_t *read_buf = NULL;
	int umcompress_sta = 0;
	int i = 0;

	read_buf = (uint8_t *)malloc(COMP_BUF_SIZE);
	if (!read_buf) {
		WLAN_ERR("%s: %d malloc error\n", __func__, __LINE__);
		goto error;
	}

	checksum += sh->data_chksum;
	last_len = sh->body_len % COMP_BUF_SIZE;
	read_count = (sh->body_len + COMP_BUF_SIZE - 1 ) & (~(COMP_BUF_SIZE - 1));
 	read_count /= COMP_BUF_SIZE;
	if (last_len)
		read_count -= 1;

	d_len = wlan_net_uncompress_size(image_id, sh, read_buf, COMP_BUF_SIZE);

	if (!xz_uncompress_init(&stream)) {
		WLAN_ERR("%s: %d xz uncompress init error\n", __func__, __LINE__);
		goto error;
	}

	read_len = COMP_BUF_SIZE;
	for (i = 0; i <= read_count; i ++) {
		if (i == read_count)
			read_len = last_len;

		ret = image_read(image_id, IMAGE_SEG_BODY, i * COMP_BUF_SIZE, read_buf, read_len);
		if (ret != read_len) {
			WLAN_ERR("%s: %d read image error, len %d\n", __func__, __LINE__, ret);
			goto error;
		}

		checksum += image_get_checksum(read_buf, read_len);

		umcompress_sta = xz_uncompress_stream(&stream, read_buf, read_len,
			             (uint8_t *)sh->load_addr, d_len, &compress_len);
		if (umcompress_sta != XZ_OK && umcompress_sta != XZ_STREAM_END) {
			WLAN_ERR("%s: %d uncompress error %d\n", __func__, __LINE__, umcompress_sta);
			goto error;
		}
		sh->load_addr += compress_len;
		d_len -= compress_len;
	}

	if (checksum != 0xFFFF) {
		WLAN_ERR("%s: checksum error error, checksum %d\n", __func__, checksum);
		goto error;
	}

	xz_uncompress_end();
	free(read_buf);
	return 0;
error:
	free(read_buf);
	return -1;
}

static int wlan_load_net_bin(enum wlan_mode mode)
{
	section_header_t section_header;
	section_header_t *sh = &section_header;
	uint32_t image_id;

	image_id = (mode == WLAN_MODE_HOSTAP) ? IMAGE_NET_AP_ID : IMAGE_NET_ID;

	if (image_read(image_id, IMAGE_SEG_HEADER, 0, sh, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE
		|| (image_check_header(sh) == IMAGE_INVALID)) {
		WLAN_ERR("%s: failed to load net section\n", __func__);
		return -1;
	}

	if (sh->attribute & (1 << 4)) {
		if (wlan_compress_bin(image_id, sh) == -1)
			return -1;
	} else {
		if ((image_read(image_id, IMAGE_SEG_BODY, 0, (void *)sh->load_addr, sh->body_len) != sh->body_len)
	   		|| (image_check_data(sh, (void *)sh->load_addr, sh->data_size, NULL, 0) == IMAGE_INVALID)) {
			WLAN_ERR("%s: failed to load net section\n", __func__);
			return -1;
		}
	}

	return 0;
}

static int wlan_get_wlan_bin(int type, int offset, uint8_t *buf, int len)
{
	uint32_t id;
	section_header_t section_header;
	section_header_t *sh = &section_header;

	switch (type) {
	case DUCC_WLAN_BIN_TYPE_BL:
		id = IMAGE_WLAN_BL_ID;
		break;
	case DUCC_WLAN_BIN_TYPE_FW:
		id = IMAGE_WLAN_FW_ID;
		break;
	case DUCC_WLAN_BIN_TYPE_SDD:
		id = IMAGE_WLAN_SDD_ID;
		break;
	default:
		WLAN_ERR("invalid wlan bin type\n");
		return 0;
	}

	if (offset == 0) {
		if (image_read(id, IMAGE_SEG_HEADER, 0, sh, IMAGE_HEADER_SIZE) != IMAGE_HEADER_SIZE) {
			WLAN_ERR("load section (id: %#08x) header failed\n", id);
			return 0;
		}
		if (image_check_header(sh) == IMAGE_INVALID) {
			WLAN_ERR("check section (id: %#08x) header failed\n", id);
			return 0;
		}

		if (len > sh->body_len)
			len = sh->body_len;
	}

	if (image_read(id, IMAGE_SEG_BODY, offset, buf, len) != len) {
		WLAN_ERR("load section (id: %#010x) body failed\n", id);
		return 0;
	}

	if (offset == 0)
		return sh->body_len;
	else
		return len;
}

static int wlan_sys_callback(uint32_t param0, uint32_t param1)
{
	struct ducc_param_wlan_bin *p;
	struct ducc_param_efuse *efuse;

	switch (param0) {
	case DUCC_NET_CMD_SYS_EVENT:
		switch (param1) {
		case DUCC_NET_SYS_READY:
			OS_SemaphoreRelease(&m_ducc_sync_sem);
			break;
		default:
			break;
		}
		break;
	case DUCC_NET_CMD_POWER_NOTIFY:
		wlan_power_callback(param1);
		break;
	case DUCC_NET_CMD_BIN_READ:
		p = (struct ducc_param_wlan_bin *)param1;
		return wlan_get_wlan_bin(p->type, p->index, p->buf, p->len);
	case DUCC_NET_CMD_EFUSE_READ:
		efuse = (struct ducc_param_efuse *)param1;
		if (HAL_EFUSE_Read(efuse->start_bit, efuse->bit_num, efuse->data) != HAL_OK)
			return -1;
	default:
		if (m_wlan_net_sys_cb) {
			return m_wlan_net_sys_cb(param0, param1);
		}
		break;
	}
	return 0;
}

int wlan_sys_init(enum wlan_mode mode, ducc_cb_func cb)
{
#ifndef __CONFIG_ARCH_MEM_PATCH
	HAL_PRCM_DisableSys2();
	HAL_PRCM_DisableSys2Power();
	HAL_PRCM_EnableSys2Power();
#endif

	m_wlan_net_sys_cb = cb;
	OS_SemaphoreCreateBinary(&m_ducc_sync_sem);

#ifndef __CONFIG_ARCH_MEM_PATCH
	HAL_PRCM_DisableSys2Isolation();
	HAL_PRCM_ReleaseSys2Reset();
	HAL_UDelay(500);
#endif

	if (wlan_load_net_bin(mode) != 0) {
		WLAN_ERR("%s: wlan load net bin failed\n", __func__);

#ifndef __CONFIG_ARCH_MEM_PATCH
		HAL_PRCM_ForceSys2Reset();
		HAL_PRCM_EnableSys2Isolation();
		HAL_PRCM_DisableSys2Power();
		HAL_PRCM_DisableSys2();
#endif

		return -1;
	}

	struct ducc_app_param param = { wlan_sys_callback };
	ducc_app_start(&param);

	HAL_PRCM_ReleaseCPUNReset();
	OS_SemaphoreWait(&m_ducc_sync_sem, OS_WAIT_FOREVER);
	OS_SemaphoreDelete(&m_ducc_sync_sem);
	WLAN_DBG("wlan sys init done\n");

#ifdef CONFIG_PM
	pm_register_ops(WLAN_SYS_DEV);
#endif

	return 0;
}

int wlan_sys_deinit(void)
{
#ifdef CONFIG_PM
	pm_unregister_ops(WLAN_SYS_DEV);
#endif

	HAL_PRCM_ForceCPUNReset();
	WLAN_DBG("wlan sys deinit done\n");

	ducc_app_stop();
	m_wlan_net_sys_cb = NULL;

#ifndef __CONFIG_ARCH_MEM_PATCH
	HAL_PRCM_ForceSys2Reset();
	HAL_PRCM_EnableSys2Isolation();
	HAL_PRCM_DisableSys2Power();
	HAL_PRCM_DisableSys2();
#endif

	return 0;
}

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */
