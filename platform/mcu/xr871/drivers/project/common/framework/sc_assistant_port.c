#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/os/os.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"

#include "net/wlan/wlan_defs.h"
#include "smartlink/sc_assistant.h"

#include "sysinfo.h"
#include "net_ctrl.h"

#define SCA_DEBUG_LEVEL 1

#if (SCA_DEBUG_LEVEL >= 3)
#define SCA_LOGD(format, args...) printf(format, ##args)
#else
#define SCA_LOGD(x...)
#endif
#if (SCA_DEBUG_LEVEL >= 2)
#define SCA_LOGN(format, args...) printf(format, ##args)
#else
#define SCA_LOGN(x...)
#endif

#if (SCA_DEBUG_LEVEL >= 1)
#define SCA_LOGW(format, args...) printf("SCA_WAR:"format, ##args)
#define SCA_LOGE(format, args...) printf("SCA_ERR:"format, ##args)
#else
#define SCA_LOGW(x...)
#define SCA_LOGE(x...)
#endif

#define SCA_BUG_ON(v) do {if(v) {printf("BUG at %s:%d!\n", __func__, __LINE__); \
                                 while (1);}} while (0)
#define SCA_WARN_ON(v) do {if(v) {printf("WARN at %s:%d!\n", __func__, __LINE__);} \
                          } while (0)

#define SCA_CONNECTING_AP               (1 << 0)
#define SCA_STOP_CONNECTING_AP          (1 << 1)

static uint8_t stop_connect_ap = 0;

static uint8_t *__sc_assistant_get_mac(uint8_t *mac_hex)
{
	struct sysinfo *sysinfo;

	SCA_BUG_ON(!mac_hex);

	sysinfo = sysinfo_get();
	memcpy(mac_hex, sysinfo->mac_addr, IEEE80211_ADDR_LEN);

	return mac_hex;
}

static void __sc_assistant_open_monitor(void)
{
	enum wlan_mode mode = wlan_if_get_mode(g_wlan_netif);

	if (mode == WLAN_MODE_HOSTAP) {
		wlan_ap_disable();
	}

	net_switch_mode(WLAN_MODE_MONITOR);
}

/* exit monitor mode, enter station mode */
static void __sc_assistant_close_monitor(void)
{
	net_switch_mode(WLAN_MODE_STA);
}

/* channel:1-13 */
static void __sc_assistant_switch_channel(char channel)
{
	wlan_monitor_set_channel(g_wlan_netif, channel);
}

static int __sc_assistant_send_raw_frame(int type, uint8_t *buffer, int len)
{
	return wlan_send_raw_frame(g_wlan_netif, type, buffer, len);
}

static struct netif *__sc_assistant_open_sta(void)
{
	net_switch_mode(WLAN_MODE_STA);

	return g_wlan_netif;
}

/* set psk to NULL if no psk.
 */
static int __sc_assistant_connect_ap(uint8_t *ssid, int ssid_len, uint8_t *psk,
                                     unsigned int timeout_ms)
{
	struct netif *nif = g_wlan_netif;
	uint32_t timeout = OS_GetTicks() + OS_TicksToMSecs(timeout_ms);

	net_switch_mode(WLAN_MODE_STA);
	wlan_sta_disable();

	SCA_LOGD("connect AP:%s psk:%s\n", ssid, psk ? psk : (uint8_t *)(""));
	if (wlan_sta_set((uint8_t *)ssid, ssid_len, psk)) {
		goto err;
	}

	OS_ThreadSuspendScheduler();
	stop_connect_ap |= SCA_CONNECTING_AP;
	OS_ThreadResumeScheduler();

	wlan_sta_enable();
	while (!(stop_connect_ap & SCA_STOP_CONNECTING_AP) &&
	       !(nif && netif_is_up(nif) && netif_is_link_up(nif)) &&
	       OS_TimeBeforeEqual(OS_GetTicks(), timeout)) {
		OS_MSleep(20);
	}

	OS_ThreadSuspendScheduler();
	stop_connect_ap = 0;
	OS_ThreadResumeScheduler();

	if (OS_TimeBeforeEqual(OS_GetTicks(), timeout)) {
		return 0;
	}

err:
	SCA_LOGE("connect ap failed\n");

	return -1;
}

static void __sc_assistant_stop_connect_ap(void)
{
	OS_ThreadSuspendScheduler();
	stop_connect_ap |= SCA_STOP_CONNECTING_AP;
	OS_ThreadResumeScheduler();
	while (stop_connect_ap & SCA_CONNECTING_AP)
		OS_MSleep(5);
	OS_ThreadSuspendScheduler();
	stop_connect_ap = 0;
	OS_ThreadResumeScheduler();
}

static int32_t __sc_assistant_get_ip(char *ip_str, const char *ifname)
{
	struct netif *nif = g_wlan_netif;

	if (nif && netif_is_up(nif) && netif_is_link_up(nif)) {
		strncpy(ip_str, inet_ntoa(nif->ip_addr), 16); /* add '\0' */
		return 0;
	}

	return -1;
}

void sc_assistant_get_fun(sc_assistant_fun_t *fun)
{
	if (!fun)
		return ;

	fun->get_mac = __sc_assistant_get_mac;
	fun->open_monitor = __sc_assistant_open_monitor;
	fun->close_monitor = __sc_assistant_close_monitor;
	fun->switch_channel = __sc_assistant_switch_channel;
	fun->send_raw_frame = __sc_assistant_send_raw_frame;
	fun->open_sta = __sc_assistant_open_sta;
	fun->connect_ap = __sc_assistant_connect_ap;
	fun->stop_connect_ap = __sc_assistant_stop_connect_ap;
	fun->get_ip = __sc_assistant_get_ip;
}
