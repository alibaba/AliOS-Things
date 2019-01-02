/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <string.h>
#include <stdio.h>
#include "aws_lib.h"
#include "zconfig_lib.h"
#include "zconfig_utils.h"
#include "zconfig_protocol.h"
#include "zconfig_ieee80211.h"
#include "enrollee.h"
#include "awss_main.h"
#include "awss_timer.h"
#include "awss.h"
#include "os.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
/* aws state machine */
enum {
    /* used by aws_state */
    AWS_SCANNING,
    AWS_CHN_LOCKED,
    AWS_SUCCESS,
    AWS_TIMEOUT,

    /* used by aws_stop */
    AWS_STOPPING,
    AWS_STOPPED
};

struct aws_info {
    u8 state;

    u8 cur_chn; /* current working channel */
    u8 chn_index;

    u8 locked_chn;
    u8 locked_bssid[6];

#define AWS_MAX_CHN_NUMS             (2 * 13 + 5)    /* +5 for safety gap */
    u8 chn_list[AWS_MAX_CHN_NUMS];
    u8  stop;

    u32 chn_timestamp;/* channel start time */
    u32 start_timestamp;/* aws start time */
} *aws_info;

#define aws_state                    (aws_info->state)
#define aws_locked_chn               (aws_info->locked_chn)
#define aws_locked_bssid             (&aws_info->locked_bssid[0])
#define aws_cur_chn                  (aws_info->cur_chn)
#define aws_chn_index                (aws_info->chn_index)
#define aws_chn_list                 (aws_info->chn_list)
#define aws_chn_timestamp            (aws_info->chn_timestamp)
#define aws_start_timestamp          (aws_info->start_timestamp)
#define aws_stop                     (aws_info->stop)

#define aws_channel_lock_timeout_ms  (8 * 1000)

static const u8 aws_fixed_scanning_channels[] = {
    1, 6, 11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
};

static void *rescan_timer = NULL;

static void rescan_monitor();
static void clr_aplist_monitor();
static void aws_try_adjust_chan();

#define RESCAN_MONITOR_TIMEOUT_MS     (5 * 60 * 1000)
#define CLR_APLIST_MONITOR_TIMEOUT_MS (24 * 60 *60 * 1000)
static uint8_t rescan_available = 0;
static uint8_t clr_aplist = 0;

/*
 * sniffer result/storage
 * use global variable/buffer to keep it usable after zconfig_destroy
 */
u8 aws_result_ssid[ZC_MAX_SSID_LEN + 1];
u8 aws_result_passwd[ZC_MAX_PASSWD_LEN + 1];
u8 aws_result_bssid[ETH_ALEN];/* mac addr */
u8 aws_result_auth = ZC_AUTH_TYPE_INVALID;
u8 aws_result_encry = ZC_ENC_TYPE_INVALID;
u8 aws_result_channel = 0;

void zconfig_channel_locked_callback(u8 primary_channel,
                                     u8 secondary_channel, u8 *bssid)
{
    aws_locked_chn = primary_channel;
    if (bssid) {
        memcpy(aws_locked_bssid, bssid, 6);
        //aws_try_adjust_chan();
    }

    if (aws_state == AWS_SCANNING) {
        aws_state = AWS_CHN_LOCKED;
    }

    awss_event_post(AWSS_LOCK_CHAN);
}

void zconfig_got_ssid_passwd_callback(u8 *ssid, u8 *passwd,
                                      u8 *bssid, u8 auth, u8 encry, u8 channel)
{
    if (bssid) {
        awss_debug("ssid:%s, bssid:%02x%02x%02x%02x%02x%02x, %s, %s, %d\r\n",
                   ssid,
                   bssid[0], bssid[1], bssid[2],
                   bssid[3], bssid[4], bssid[5],
                   zconfig_auth_str(auth), zconfig_encry_str(encry),
                   channel);
    } else {
        awss_debug("ssid:%s, bssid:--, %s, %s, %d\r\n",
                   ssid,
                   zconfig_auth_str(auth), zconfig_encry_str(encry),
                   channel);
    }

    memset(aws_result_ssid, 0, sizeof(aws_result_ssid));
    memset(aws_result_passwd, 0, sizeof(aws_result_passwd));
    strncpy((char *)aws_result_ssid, (const char *)ssid, ZC_MAX_SSID_LEN - 1);
    strncpy((char *)aws_result_passwd, (const char *)passwd, ZC_MAX_PASSWD_LEN - 1);

    if (bssid) {
        memcpy(aws_result_bssid, bssid, ETH_ALEN);
    }
    aws_result_auth = auth;
    aws_result_encry = encry;
    aws_result_channel = channel;

    aws_state = AWS_SUCCESS;

    awss_event_post(AWSS_GOT_SSID_PASSWD);
}

u8 aws_next_channel(void)
{
    /* aws_chn_index start from -1 */
    while (1) {
        aws_chn_index++;
        if (aws_chn_index >= AWS_MAX_CHN_NUMS) {
            aws_chn_index = 0;    //rollback to start
        }

        if (aws_chn_list[aws_chn_index]) {  //valid channel
            break;
        }
    }

    aws_cur_chn = aws_chn_list[aws_chn_index];

    return aws_cur_chn;
}

#define SA_OFFSET            (10)
#define ADHA_PROBE_PKT_LEN   (50)
#define AHA_PROBE_PKT_LEN    (49)
const uint8_t adha_probe_req_frame[ADHA_PROBE_PKT_LEN] = {
    0x40, 0x00,  // mgnt type, frame control
    0x00, 0x00,  // duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // DA
    0x28, 0xC2, 0xDD, 0x61, 0x68, 0x83,  // SA, to be replaced with wifi mac
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // BSSID
    0xC0, 0x79,  // seq
    0x00, 0x04, 0x61, 0x64, 0x68, 0x61,  // ssid, adha
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  // supported rates
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  // extended supported rates
    0x3F, 0x84, 0x10, 0x9E  // FCS
};
const uint8_t aha_probe_req_frame[AHA_PROBE_PKT_LEN] = {
    0x40, 0x00,  // mgnt type, frame control
    0x00, 0x00,  // duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // DA
    0x28, 0xC2, 0xDD, 0x61, 0x68, 0x83,  // SA, to be replaced with wifi mac
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // BSSID
    0xC0, 0x79,  // seq
    0x00, 0x03, 0x61, 0x68, 0x61,        // ssid, aha
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  // supported rates
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  // extended supported rates
    0x3F, 0x84, 0x10, 0x9E  // FCS
};

void aws_send_adha_probe_req()
{
    uint8_t probe[ADHA_PROBE_PKT_LEN];
    memcpy(probe, adha_probe_req_frame, sizeof(probe));
    os_wifi_get_mac(&probe[SA_OFFSET]);
    os_wifi_send_80211_raw_frame(FRAME_PROBE_REQ, probe, sizeof(probe));
}

void aws_send_aha_probe_req()
{
    uint8_t probe[AHA_PROBE_PKT_LEN];
    memcpy(probe, aha_probe_req_frame, sizeof(probe));
    os_wifi_get_mac(&probe[SA_OFFSET]);
    os_wifi_send_80211_raw_frame(FRAME_PROBE_REQ, probe, sizeof(probe));
}

static void aws_switch_dst_chan(int channel);
static int aws_amend_dst_chan = 0;
void aws_switch_channel(void)
{
    if (aws_amend_dst_chan != 0) {
        aws_switch_dst_chan(aws_amend_dst_chan);
        aws_amend_dst_chan = 0;
        return;
    }

    int channel = aws_next_channel();
    aws_chn_timestamp = os_get_time_ms();
    os_awss_switch_channel(channel, 0, NULL);
    os_printf("chan %d\r\n", channel);
}

void aws_set_dst_chan(int channel)
{
    aws_amend_dst_chan = channel;
}

static void aws_switch_dst_chan(int channel)
{
    int i = aws_chn_index;
    for (; i < AWS_MAX_CHN_NUMS; i ++) {
        if (aws_chn_list[i] == 0) {
            continue;
        }
        if (aws_chn_list[i] == channel) {
            break;
        }
    }

    if (i >= AWS_MAX_CHN_NUMS) {
        for (i = 0; i < aws_chn_index; i ++) {
            if (aws_chn_list[i] == 0) {
                continue;
            }
            if (aws_chn_list[i] == channel) {
                break;
            }
        }
    }
    if (i == aws_chn_index) { // no need to switch channel.
        return;
    }

    aws_chn_index = i;
    aws_locked_chn = channel;
    aws_cur_chn = channel;
    aws_chn_timestamp = os_get_time_ms();
    if (aws_state == AWS_SCANNING)
        aws_state = AWS_CHN_LOCKED;
    os_awss_switch_channel(channel, 0, NULL);

    os_printf("adjust chan %d\r\n", channel);
}

enum {
    CHNSCAN_ONGOING, /* no timeout, continue */
    CHNSCAN_NEXT_CHN, /* should swith to next channel */
    CHNSCAN_TIMEOUT /* aws timeout */
};

int aws_is_chnscan_timeout(void)
{
    if (aws_stop == AWS_STOPPING) {
        awss_debug("aws will stop...\r\n");
        return CHNSCAN_TIMEOUT;
    }

    if (time_elapsed_ms_since(aws_chn_timestamp) > os_awss_get_channelscan_interval_ms()) {
        if ((0 != os_awss_get_timeout_interval_ms()) &&
            (time_elapsed_ms_since(aws_start_timestamp) > os_awss_get_timeout_interval_ms())) {
            return CHNSCAN_TIMEOUT;
        } else {
            return CHNSCAN_NEXT_CHN;
        }
    }

    return CHNSCAN_ONGOING;
}

int zconfig_add_active_channel(int channel)
{
    if (zconfig_is_valid_channel(channel)) {
        int fixed_channel_nums = sizeof(aws_fixed_scanning_channels);
        aws_chn_list[fixed_channel_nums + channel] = channel;
        return 0;
    }
    return -1;
}

/*
 * platform like mc300, 雄迈 depend on auth & encry type
 * so keep scanning if auth & encry is incomplete
 */
int aws_force_scanning(void)
{
#ifdef WITH_AUTH_ENCRY
    int timeout = sizeof(aws_fixed_scanning_channels) / sizeof(u8)
                  * os_awss_get_timeout_interval_ms() * 2; /* 2 round */

    /* force scanning useful only when aws is success */
    if (aws_state != AWS_SUCCESS) {
        return 0;
    }

    //channel scanning at most 2 round
    if (time_elapsed_ms_since(aws_start_timestamp) >= timeout) {
        return 0;//timeout
    } else {
        /*
         * for platform which need auth & encry, retry to get auth info
         */
        int found = zconfig_get_auth_info(aws_result_ssid, aws_result_bssid,
                                          &aws_result_auth, &aws_result_encry,
                                          &aws_result_channel);
        if (found) {
            return 0;
        } else {
            return 1;    /* keep scanning */
        }
    }
#else
    //no need scanning a round
    return 0;
#endif
}

static void aws_try_adjust_chan()
{
    struct ap_info *ap = NULL;
    char ssid[ZC_MAX_SSID_LEN] = {0};
    ap = zconfig_get_apinfo(zc_bssid);
    if (ap == NULL)
        return;
    if (aws_locked_chn == ap->channel)
        return;
    if (!zconfig_is_valid_channel(ap->channel))
        return;
    strncpy(ssid, (const char *)ap->ssid, ZC_MAX_SSID_LEN - 1);

    if (strlen(ssid) == strlen(zc_default_ssid) &&
        strncmp(ap->ssid, zc_default_ssid, strlen(zc_default_ssid)) == 0)
        return;
    if (strlen(ssid) == strlen(zc_adha_ssid) &&
        strncmp(ap->ssid, zc_adha_ssid, strlen(zc_adha_ssid)) == 0)
        return;

    aws_set_dst_chan(ap->channel);
    aws_switch_channel();
}

/*
 * channel scanning/re-scanning control
 * Note: 修改该函数时，需考虑到各平台差异
 * 庆科平台：
 * --aws_switch_channel() 为空
 * --zconfig_destroy()会被调用两次，一次被aws_main_thread_fun()，一次被庆科驱动
 * linux/rtos平台差异
 * --vendor_recv_80211_frame()有实现，rtos平台该函数通常为空，通过注册callback方式收包
 */
void aws_main_thread_func(void)
{
    aws_start_timestamp = os_get_time_ms();

    /* channel switch init */
    aws_switch_channel();

rescanning:
    //start scaning channel
    memset(zc_bssid, 0, ETH_ALEN);
    while (aws_amend_dst_chan != 0 || aws_state == AWS_SCANNING || aws_force_scanning()) {
        switch (aws_is_chnscan_timeout()) {
            case CHNSCAN_ONGOING:
                break;
            case CHNSCAN_NEXT_CHN:
                aws_switch_channel();
                break;
            case CHNSCAN_TIMEOUT:
                goto timeout_scanning;
            default:
                break;
        }

        if (aws_state != AWS_SCANNING) {
            break;
        }

        int interval = (os_awss_get_channelscan_interval_ms() + 2) / 3;
        if (interval < 1) {
            interval = 1;
        }

#ifndef AWSS_DISABLE_ENROLLEE
        /* 80211 frame handled by callback */
        os_msleep(interval);
        awss_broadcast_enrollee_info();
#endif
        os_msleep(interval);
        aws_send_adha_probe_req();

        os_msleep(interval);
        aws_send_aha_probe_req();
    }

    //channel lock
    awss_debug("[channel scanning] %d ms\r\n",
               time_elapsed_ms_since(aws_start_timestamp));

    /*
     * make sure switch to locked channel,
     * in case of inconsistent with aws_cur_chn
     */
    aws_try_adjust_chan();
    awss_debug("final channel %d\r\n", aws_locked_chn);

    while (aws_state != AWS_SUCCESS) {
        /* 80211 frame handled by callback */
        os_msleep(300);

        if (aws_is_chnscan_timeout() == CHNSCAN_TIMEOUT) {
            goto timeout_recving;
        }

        if (aws_state == AWS_SCANNING) {
            awss_debug("channel rescanning...\n");
            goto rescanning;
        }
    }

    awss_debug("[channel recving] %d ms\r\n",
               time_elapsed_ms_since(aws_start_timestamp));

    goto success;

timeout_scanning:
    awss_debug("aws timeout scanning!\r\n");
timeout_recving:
    awss_debug("aws timeout recving!\r\n");
    if (rescan_timer == NULL) {
        rescan_timer = HAL_Timer_Create("rescan", (void(*)(void *))rescan_monitor, NULL);
    }
    HAL_Timer_Stop(rescan_timer);
    HAL_Timer_Start(rescan_timer, RESCAN_MONITOR_TIMEOUT_MS);
    while (rescan_available == 0) {
        if (zconfig_get_press_status()) {
            HAL_Timer_Stop(rescan_timer);
            break;
        }
        os_msleep(200);
    }
    rescan_available = 0;
    aws_stop = AWS_SCANNING;
    aws_state = AWS_SCANNING;
    if (clr_aplist) {
        memset(zconfig_aplist, 0, sizeof(struct ap_info) * MAX_APLIST_NUM);
        zconfig_aplist_num = 0;
        memset(adha_aplist, 0, sizeof(*adha_aplist));
        clr_aplist = 0;
    }
    aws_start_timestamp = os_get_time_ms();
    goto rescanning;

success:
    awss_stop_timer(rescan_timer);
    rescan_timer = NULL;
    /*
     * zconfig_destroy() after os_awss_monitor_close() beacause
     * zconfig_destroy will release mem/buffer that
     * zconfig_recv_callback will use
     *
     * Note: hiflying will reboot after calling this func, so
     *    aws_get_ssid_passwd() was called in os_awss_monitor_close()
     */
    if (strcmp((const char *)aws_result_ssid, (const char *)zc_adha_ssid) == 0 ||
        strcmp((const char *)aws_result_ssid, (const char *)zc_default_ssid) == 0) {
        zconfig_destroy();
    } else {
        zconfig_force_destroy();
    }
}

static void rescan_monitor()
{
    rescan_available = 1;
}

static void clr_aplist_monitor()
{
    clr_aplist = 1;
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);
}

int aws_80211_frame_handler(char *buf, int length, enum AWSS_LINK_TYPE link_type, int with_fcs, signed char rssi)
{
    static unsigned int lock_start;

    int ret = zconfig_recv_callback(buf, length, aws_cur_chn, link_type, with_fcs, rssi);

    if (aws_state == AWS_CHN_LOCKED) {
        switch (ret) {
            case PKG_START_FRAME:
            case PKG_DATA_FRAME:
            case PKG_GROUP_FRAME:
                lock_start = os_get_time_ms();
                break;
            default:
                if (time_elapsed_ms_since(lock_start) > aws_channel_lock_timeout_ms)
                    /* set to rescanning */
                {
                    aws_state = AWS_SCANNING;
                }
                break;
        }
    }

    return ret;
}

void aws_start(char *pk, char *dn, char *ds, char *ps)
{
    aws_info = os_zalloc(sizeof(struct aws_info));
    if (!aws_info) {
        return;
    }

    aws_state = AWS_SCANNING;

    /* start from -1 */
    aws_chn_index = 0xff;
    memcpy(aws_chn_list, aws_fixed_scanning_channels,
           sizeof(aws_fixed_scanning_channels));

    memset(aws_result_ssid, 0, sizeof(aws_result_ssid));
    memset(aws_result_passwd, 0, sizeof(aws_result_passwd));
    memset(aws_result_bssid, 0, sizeof(aws_result_bssid));
    aws_result_auth = ZC_AUTH_TYPE_INVALID;
    aws_result_encry = ZC_ENC_TYPE_INVALID;
    aws_result_channel = 0;

    zconfig_init();

    if (clr_aplist_timer == NULL) {
        clr_aplist_timer = HAL_Timer_Create("clr_aplist", (void (*)(void *))clr_aplist_monitor, (void *)NULL);
    }
    HAL_Timer_Stop(clr_aplist_timer);
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);

    os_awss_open_monitor(aws_80211_frame_handler);

#ifndef AWSS_DISABLE_ENROLLEE
    awss_init_enrollee_info();
#endif

    aws_main_thread_func();
}

void aws_destroy(void)
{
    if (aws_info) {
        aws_stop = AWS_STOPPING;
        while (aws_state != AWS_SUCCESS && aws_state != AWS_TIMEOUT) {
            os_msleep(100);
        }

        os_free(aws_info);
        aws_info = NULL;

#ifndef AWSS_DISABLE_ENROLLEE
        awss_destroy_enrollee_info();
#endif
    }
}

int aws_get_ssid_passwd(char *ssid, char *passwd, unsigned char *bssid,
                        char *auth, char *encry, unsigned char *channel)
{
    if (aws_state != AWS_SUCCESS) {
        return 0;
    }
    if (ssid) {
        strncpy((char *)ssid, (const char *)aws_result_ssid, ZC_MAX_SSID_LEN - 1);
    }
    if (passwd) {
        strncpy((char *)passwd, (const char *)aws_result_passwd, ZC_MAX_PASSWD_LEN - 1);
    }
    if (bssid) {
        memcpy(bssid, aws_result_bssid, ETH_ALEN);
    }
    if (auth) {
        *auth = aws_result_auth;
    }
    if (encry) {
        *encry = aws_result_encry;
    }
    if (channel) {
        *channel = aws_result_channel;
    }
    return 1;
}

const char *aws_version(void)
{
    return zconfig_lib_version();
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
