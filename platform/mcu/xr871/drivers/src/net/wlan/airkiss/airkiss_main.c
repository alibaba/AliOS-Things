#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "string.h"
#include "sys/mbuf.h"
#include "kernel/os/os.h"
#include "net/wlan/wlan.h"

#include "net/wlan/wlan_airkiss.h"
#include "airkiss.h"

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define g_debuglevel  INFO

#define AIRKISS_DBG(level, fmt, args...) do {		\
					if (level <= g_debuglevel)	\
						printf("[airkiss]"fmt,##args);		\
				 } while (0)

enum {
	AK_END = 0,
	AK_SEARCH_CHAN,
	AK_LOCKED_CHAN,
};

static uint8_t ak_ctrl = 0;
static airkiss_context_t ak_context;
static airkiss_result_t priv_ak_result;
static airkiss_status_t ak_sta = AIRKISS_STATUS_CONTINUE;
static const airkiss_config_t config = {
								 (airkiss_memset_fn)&memset,
								 (airkiss_memcpy_fn)&memcpy,
								 (airkiss_memcmp_fn)&memcmp,
								 (airkiss_printf_fn)&printf
								 };

#define AK_KEY_LEN 16
#define CHTABLESIZE 13
#define MAX_TIME 4294967295

static char ak_key[AK_KEY_LEN];
static struct netif *priv_nif = NULL;

static uint32_t d_time(uint32_t time1, uint32_t time2)
{
	uint32_t d_time = 0;
	if (time1 <= time2)
		d_time = time2 - time1;
	else
		d_time = MAX_TIME - time1 + time2;
	return d_time;
}

static void airkiss_macframe(uint8_t *data, uint32_t len, void *info)
{
	ak_sta = airkiss_recv(&ak_context, data, len);

	if (ak_sta == AIRKISS_STATUS_COMPLETE || ak_ctrl == AK_END) {
		wlan_monitor_set_rx_cb(priv_nif, NULL);
		AIRKISS_DBG(INFO, "%s(), %d, airkiss complete\n", __func__, __LINE__);

		if (ak_ctrl) {
			airkiss_get_result(&ak_context, &priv_ak_result);
			AIRKISS_DBG(INFO, "ssid %s\n", priv_ak_result.ssid);
			AIRKISS_DBG(INFO, "pwd %s\n", priv_ak_result.pwd);
			AIRKISS_DBG(INFO, "random %d\n", priv_ak_result.random);
			AIRKISS_DBG(INFO, "%s, %d airkiss end !!!!! \n", __func__, __LINE__);
		} else
			AIRKISS_DBG(ERROR, "%s, %d airkiss get result error\n", __func__, __LINE__);

		ak_ctrl = AK_END;
	} else if (ak_sta == AIRKISS_STATUS_CHANNEL_LOCKED) {
		AIRKISS_DBG(INFO, "airkiss channel locked\n");
		ak_ctrl = AK_LOCKED_CHAN;
	} else if (ak_sta < 0)
		AIRKISS_DBG(ERROR, "%s, %d airkiss recv error %d\n", __func__, __LINE__, ak_sta);
}

static int airkiss_wifi_set_channel(struct netif *nif, int16_t ch)
{
	int ret = wlan_monitor_set_channel(nif, ch);
	if (ret == -1)
		AIRKISS_DBG(ERROR, "%s, %d airkiss set channel error\n", __func__, __LINE__);

	return ret;
}

static void airkiss_config(void)
{
	int ret = -1;
	AIRKISS_DBG(INFO, "%s\n", airkiss_version());

	ret = airkiss_init(&ak_context, &config);
	if (ret != 0)
		AIRKISS_DBG(ERROR, "%s, %d, airkiss init error\n", __func__, __LINE__);
	if(ak_key[0] != 0) {
		ret = airkiss_set_key(&ak_context, (const unsigned char *)ak_key, AK_KEY_LEN);
		if (ret != 0)
			AIRKISS_DBG(ERROR, "%s, %d, airkiss set key error\n", __func__, __LINE__);
	}
}

static void airkiss_reset(void)
{
	/* release resource of airkiss */
	airkiss_change_channel(&ak_context);
	priv_ak_result.ssid = NULL;
	priv_ak_result.pwd = NULL;
}

static void airkiss_search(int16_t * ch_buff)
{
	int i = 0;

	while (1) {
		if (ak_ctrl == AK_SEARCH_CHAN) {
			if (ak_sta == AIRKISS_STATUS_CONTINUE) {
				airkiss_wifi_set_channel(priv_nif,  ch_buff[i++]);
				airkiss_reset();
			}

			if (i >= CHTABLESIZE)
				i = 0;
		} else if (ak_ctrl == AK_LOCKED_CHAN)
			break;
		else if (ak_ctrl == AK_END)
			break;

		OS_MSleep(100);
	}
}

static int airkiss_read_result(wlan_airkiss_result_t *result)
{
	if (priv_ak_result.ssid != NULL) {
		if (priv_ak_result.ssid_length > WLAN_SSID_MAX_LEN) {
			AIRKISS_DBG(ERROR, "Invalid ssid len %d\n", priv_ak_result.ssid_length);
			return -1;
		}

		if (priv_ak_result.pwd_length > WLAN_PASSPHRASE_MAX_LEN) {
			AIRKISS_DBG(ERROR, "Invalid passphrase len %d\n", priv_ak_result.pwd_length);
			return -1;
		}

		memcpy(result->ssid, priv_ak_result.ssid, priv_ak_result.ssid_length);
		result->ssid_len = priv_ak_result.ssid_length;

		if (priv_ak_result.pwd_length > 0 && priv_ak_result.pwd != NULL) {
			memcpy(result->passphrase, priv_ak_result.pwd, priv_ak_result.pwd_length);
			result->passphrase[priv_ak_result.pwd_length] = '\0';
		} else {
			result->passphrase[0] = '\0';
		}

		result->random_num = priv_ak_result.random;
		return 0;
	}

	return -1;
}

static int airkiss_set_aeskey(const char *key, uint32_t length)
{
	if (length != AK_KEY_LEN)
		return -1;

	memcpy(ak_key, key, AK_KEY_LEN);

	return 0;
}

static void airkiss_close(void)
{
	ak_ctrl = AK_END;

	if (wlan_monitor_set_rx_cb(priv_nif, NULL) != 0) {
		AIRKISS_DBG(ERROR, "%s(),%d monitor set rx cb error\n", __func__, __LINE__);
	}

	airkiss_reset();
}

static wlan_airkiss_status_t airkiss_start(struct netif *nif,
                                           uint32_t timeout_ms,
                                           wlan_airkiss_result_t *result)
{
	int ret = -1;
	wlan_airkiss_status_t status = WLAN_AIRKISS_FAIL;
	int16_t ak_ch_buffer[CHTABLESIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

	AIRKISS_DBG(INFO, "start\n");

	if (nif == NULL || result == NULL) {
		AIRKISS_DBG(ERROR, "%s(),%d, nif %p, result %p\n", __func__, __LINE__, nif, result);
		return WLAN_AIRKISS_INVALID;
	}

	priv_nif = nif;
	airkiss_config();

	AIRKISS_DBG(INFO, "monitor_register\n");

	ret = wlan_monitor_set_rx_cb(priv_nif, airkiss_macframe);
	if (ret != 0) {
		AIRKISS_DBG(ERROR, "%s(),%d monitor set rx cb fail\n", __func__, __LINE__);
		goto out;
	}

	uint32_t start_time = OS_JiffiesToMSecs(OS_GetJiffies());

	ak_ctrl = AK_SEARCH_CHAN;
	airkiss_search(ak_ch_buffer);

	while (ak_ctrl) {
		uint32_t d_t = d_time(start_time, OS_JiffiesToMSecs(OS_GetJiffies()));
		if (d_t >= timeout_ms) {
			airkiss_close();
			status = WLAN_AIRKISS_TIMEOUT;
			goto out;
		}

		OS_MSleep(100);
	}

	ret = airkiss_read_result(result);
	if (ret == 0) {
		status = WLAN_AIRKISS_SUCCESS;
	}

out:
	airkiss_reset();
	return status;
}

int wlan_airkiss_set_key(const char *key, uint32_t len)
{
	if (key == NULL || len != WLAN_AIRKISS_KEY_LEN) {
		AIRKISS_DBG(ERROR, "invalid airkiss key (%p, %d)\n", key, len);
		return -1;
	}

	return airkiss_set_aeskey(key, len);
}

wlan_airkiss_status_t wlan_airkiss_start(struct netif *nif, uint32_t timeout_ms,
                                         wlan_airkiss_result_t *result)
{
	return airkiss_start(nif, timeout_ms, result);
}

int wlan_airkiss_stop(void)
{
	airkiss_close();
	return 0;
}
