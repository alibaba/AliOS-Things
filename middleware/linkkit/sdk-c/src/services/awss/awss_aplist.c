#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include "awss_adha.h"
#include "awss_timer.h"
#include "awss_aplist.h"
#include "zconfig_protocol.h"
#include "zconfig_ieee80211.h"

#define CLR_APLIST_MONITOR_TIMEOUT_MS    (24 * 60 *60 * 1000)
/* storage to store apinfo */
struct ap_info *zconfig_aplist = NULL;
/* aplist num, less than MAX_APLIST_NUM */
uint8_t zconfig_aplist_num = 0;

static uint8_t clr_aplist = 0;
static void *clr_aplist_timer = NULL;

static void awss_clr_aplist_monitor()
{
    clr_aplist = 1;
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);
}

int awss_is_ready_clr_aplist(void)
{
    return clr_aplist;
}

int awss_clear_aplist(void)
{
    memset(zconfig_aplist, 0, sizeof(struct ap_info) * MAX_APLIST_NUM);
    memset(adha_aplist, 0, sizeof(*adha_aplist));
    zconfig_aplist_num = 0;
    clr_aplist = 0;

    return 0;
}

int awss_open_aplist_monitor(void)
{
    if (clr_aplist_timer == NULL)
        clr_aplist_timer = HAL_Timer_Create("clr_aplist", (void (*)(void *))awss_clr_aplist_monitor, (void *)NULL);
    if (clr_aplist_timer == NULL)
        return -1;

    HAL_Timer_Stop(clr_aplist_timer);
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);
    return 0;
}

int awss_close_aplist_monitor(void)
{
    if (clr_aplist_timer == NULL)
        return 0;
    awss_stop_timer(clr_aplist_timer);
    clr_aplist_timer = NULL;
    return 0;
}

int awss_init_ieee80211_aplist(void)
{
    if (zconfig_aplist)
        return 0;
    zconfig_aplist = (struct ap_info *)os_zalloc(sizeof(struct ap_info) * MAX_APLIST_NUM);
    if (zconfig_aplist == NULL)
        return -1;
    zconfig_aplist_num = 0;
    return 0;
}

int awss_deinit_ieee80211_aplist(void)
{
    if (zconfig_aplist == NULL)
        return 0;
    os_free(zconfig_aplist);
    zconfig_aplist = NULL;
    zconfig_aplist_num = 0;
    return 0;
}

int awss_ieee80211_aplist_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    uint8_t ssid[ZC_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    uint8_t auth, pairwise_cipher, group_cipher;
    struct ieee80211_hdr *hdr;
    int fc, ret, channel;

    if (mgmt_header == NULL)
        return ALINK_INVALID;

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    /*
     * just for save ap in aplist for ssid amend.
     */
    if (!ieee80211_is_beacon(fc) && !ieee80211_is_probe_resp(fc))
        return ALINK_INVALID;

    ret = ieee80211_get_bssid(mgmt_header, bssid);
    if (ret < 0)
        return ALINK_INVALID; 

    ret = ieee80211_get_ssid(mgmt_header, len, ssid);
    if (ret < 0)
        return ALINK_INVALID; 

    /*
     * skip all the adha and aha
     */
    if (strcmp((const char *)ssid, zc_default_ssid) == 0 ||
        strcmp((const char *)ssid, zc_adha_ssid) == 0)
        return ALINK_INVALID; 

    channel = cfg80211_get_bss_channel(mgmt_header, len);
    rssi = rssi > 0 ? rssi - 256 : rssi;

    cfg80211_get_cipher_info(mgmt_header, len, &auth,
                             &pairwise_cipher, &group_cipher);
    __zconfig_save_apinfo(ssid, bssid, channel, auth,
                          pairwise_cipher, group_cipher, rssi);
    return ALINK_INVALID;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
