/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#ifdef AWSS_SUPPORT_DEV_AP

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define TIMEOUT_CNT       10
#define GET_GBK_TIMEOUT   6000
static void *g_awss_dev_ap_mutex = NULL;
static char awss_dev_ap_switchap_done = 0;
static char awss_dev_ap_switchap_resp_suc = 0;
static char awss_dev_ap_ongoing = 0;
static void start_connect_ap();

typedef struct {
    char ssid[PLATFORM_MAX_SSID_LEN + 1];
    char passwd[PLATFORM_MAX_PASSWD_LEN + 1];
    uint8_t bssid[ETH_ALEN];
    uint8_t token[RANDOM_MAX_LEN + 1];
    uint16_t msgid;
    uint8_t cnt;
    uint8_t token_found;
} ap_info_t;
ap_info_t *g_dev_ap_info_ptr = NULL;

struct ssid_config {
    uint8_t bssid[ETH_ALEN];
    char ssid[33];
    uint8_t got_ssid;
};

static struct ssid_config config;

static int awss_80211_frame_handler(char *buf, int length, enum AWSS_LINK_TYPE link_type, int with_fcs,
                                    signed char rssi)
{
    uint8_t ssid[PLATFORM_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    struct ieee80211_hdr *hdr;
    int fc;
    int ret = -1;

    if (link_type != AWSS_LINK_TYPE_NONE) {
        return -1;
    }
    /* remove FCS filed */
    if (with_fcs) {
        length -= 4;
    }

    hdr = (struct ieee80211_hdr *)buf;

    fc = hdr->frame_control;
    if (!ieee80211_is_beacon(fc) && !ieee80211_is_probe_resp(fc)) {
        return -1;
    }

    ret = ieee80211_get_bssid((uint8_t *)hdr, bssid);
    if (ret < 0) {
        return -1;
    }

    awss_trace("bssid = %02x %02x %02x %02x %02x %02x", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    if (memcmp(config.bssid, bssid, ETH_ALEN) != 0) {
        return -1;
    }

    ret = ieee80211_get_ssid((uint8_t *)hdr, length, ssid);
    if (ret < 0) {
        return -1;
    }

    memcpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    config.got_ssid = 1;
    awss_info("get ssid in RAW FRAME :%s\n", ssid);
    HAL_Awss_Close_Monitor();
    return 0;
}

static char is_str_asii(char *str)
{
    int i;
    for (i = 0; str[i] != '\0'; ++i) {
        if ((uint8_t)str[i] > 0x7F) {
            return 0;
        }
    }
    return 1;
}

static char bssid_is_valid(uint8_t *bssid)
{
    int i;
    for (i = 0; i < ETH_ALEN; ++i) {
        if (bssid[i] != 0) {
            return 1;
        }
    }
    return 0;
}

static uint8_t get_next_channel(void)
{
    static int aws_chn_index = 0;

    aws_chn_index ++;
    if (aws_chn_index > 13) {
        aws_chn_index = 1;    /* rollback to start */
    }

    return aws_chn_index;
}

static int try_get_real_ssid(uint32_t timeout, uint8_t *bissd_in, char *ssid_out)
{

    uint64_t pre_time = HAL_UptimeMs();
    uint64_t start_time = pre_time;
    uint64_t cur_time = 0;
    int ret = -1;
    memset(&config, 0, sizeof(struct ssid_config));

    memcpy(config.bssid, bissd_in, ETH_ALEN);
    HAL_Awss_Open_Monitor(awss_80211_frame_handler);

    do {
        HAL_SleepMs(50);
        if (config.got_ssid) {
            strncpy(ssid_out, config.ssid, strlen(config.ssid));
            ssid_out[strlen(config.ssid)] = 0;
            ret = 0;
            break;
        }
        cur_time = HAL_UptimeMs();
        if (cur_time - pre_time > 250) {
            int channel = get_next_channel();
            HAL_Awss_Switch_Channel(channel, 0, NULL);
            pre_time = cur_time;
        }
    } while (cur_time - start_time < timeout);
    return ret;
}
static int awss_dev_ap_setup()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    char passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int ret = -1;
    int count = 0;
    do {  /* reduce stack used */
        char pk[OS_PRODUCT_KEY_LEN + 1] = {0};
        char mac_str[OS_MAC_LEN + 1] = {0};

        HAL_GetProductKey(pk);
        os_wifi_get_mac_str(mac_str);
        memmove(mac_str + 11, mac_str + 12, 2);
        memmove(mac_str + 13, mac_str + 15, 2);
        mac_str[15] = '\0';
        HAL_Snprintf(ssid, PLATFORM_MAX_SSID_LEN, "adh_%s_%s", pk, &mac_str[9]);
    } while (0);

    awss_trace("ssid:%s\n", ssid);
    for (count = 0; count < 3; count++) {
        ret = HAL_Awss_Open_Ap(ssid, passwd, 100, 0);
        if (0 == ret) {
            break;
        }
        HAL_SleepMs(2000);
    }
    return ret;
}

static void try_to_do_connect_ap()
{
    int ret;
    ap_info_t *info = g_dev_ap_info_ptr;
    if (NULL == info) {
        return;
    }
    if (awss_dev_ap_ongoing == 0) {
        awss_cmp_coap_cancel_packet(info->msgid);
        return;
    }
    if ((strlen(info->ssid) == 0) && (strlen(info->passwd) == 0)) {
        return;
    }

    if (awss_dev_ap_switchap_resp_suc || ++info->cnt == TIMEOUT_CNT) {
        start_connect_ap();
        /* reset ssid/passwd data to 0 */
        memset(info, 0, sizeof(ap_info_t));
    }
}

extern int awss_success_notify(void);
int awss_dev_ap_start(void)
{
    int ret = -1;
    ap_info_t dev_ap_info = {0};

    if (g_awss_dev_ap_mutex || awss_dev_ap_ongoing) {
        awss_trace("dev ap already running");
        return -1;
    }

    if (g_awss_dev_ap_mutex == NULL) {
        g_awss_dev_ap_mutex = HAL_MutexCreate();
    }
    if (g_awss_dev_ap_mutex == NULL) {
        awss_trace("awss dev ap start fail");
        goto AWSS_DEV_AP_FAIL;
    }

    HAL_MutexLock(g_awss_dev_ap_mutex);

    awss_dev_ap_ongoing = 1;
    awss_dev_ap_switchap_done = 0;
    awss_dev_ap_switchap_resp_suc = 0;
    g_dev_ap_info_ptr = &dev_ap_info;

    HAL_MutexUnlock(g_awss_dev_ap_mutex);
    ret = awss_dev_ap_setup();
    if (0 != ret) {
        awss_trace("awss dev ap setup fail");
        goto AWSS_DEV_AP_FAIL;
    }

    HAL_SleepMs(1000);  /* wait for dev ap to work well */
    HAL_MutexLock(g_awss_dev_ap_mutex);
    if (awss_dev_ap_ongoing) {
        awss_cmp_local_init(AWSS_LC_INIT_DEV_AP);
    }
    while (awss_dev_ap_ongoing) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_SleepMs(200);
        HAL_MutexLock(g_awss_dev_ap_mutex);
        if (awss_dev_ap_switchap_done) {
            break;
        }
        try_to_do_connect_ap();
    }
    g_dev_ap_info_ptr = NULL;
    HAL_MutexUnlock(g_awss_dev_ap_mutex);

    ret = awss_dev_ap_switchap_done == 0 ? -1 : 0;

    if (awss_dev_ap_ongoing == 0) {  /* interrupt by user */
        HAL_SleepMs(1000);
        return -1;
    }

    awss_dev_ap_ongoing = 0;
    awss_success_notify();

AWSS_DEV_AP_FAIL:
    if (g_awss_dev_ap_mutex) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_MutexDestroy(g_awss_dev_ap_mutex);
    }
    g_awss_dev_ap_mutex = NULL;
    g_dev_ap_info_ptr = NULL;
    return ret;
}

int awss_dev_ap_stop(void)
{
    if (awss_dev_ap_ongoing == 0) {
        return 0;
    }

    awss_dev_ap_ongoing = 0;

    awss_trace("%s", __func__);

    if (g_awss_dev_ap_mutex) {
        HAL_MutexLock(g_awss_dev_ap_mutex);
    }

    HAL_Awss_Close_Ap();

    awss_cmp_local_deinit(1);

    if (g_awss_dev_ap_mutex) {
        HAL_MutexUnlock(g_awss_dev_ap_mutex);
        HAL_MutexDestroy(g_awss_dev_ap_mutex);
        g_awss_dev_ap_mutex = NULL;
    }

    awss_dev_ap_switchap_done = 0;
    awss_dev_ap_switchap_resp_suc = 0;
    g_dev_ap_info_ptr = NULL;

    awss_trace("%s exit", __func__);

    return 0;
}

static int awss_dev_ap_switchap_resp(void *context, int result,
                                     void *userdata, void *remote,
                                     void *message)
{
    if (result == 2) { /* success */
        awss_dev_ap_switchap_resp_suc = 1;
    }
    return 0;
}


static void start_connect_ap()
{
    int ret = 0;
    ap_info_t *info = g_dev_ap_info_ptr;
    if (NULL == info) {
        return;
    }
    awss_cmp_coap_cancel_packet(info->msgid);
    AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);
    if (awss_dev_ap_ongoing == 0) {  /* interrupt by user */
        ret = -1;
        return;
    }
    HAL_Awss_Close_Ap();

    if (is_str_asii(info->ssid) == 0 &&  bssid_is_valid(info->bssid)) { /*try to get real ssid from mngmt frame */
        HAL_SleepMs(200); /*wait ap close */
        try_get_real_ssid(GET_GBK_TIMEOUT, info->bssid, info->ssid);
    }
    ret = awss_connect(info->ssid, info->passwd, info->bssid, ETH_ALEN,
                       info->token_found == 1 ? info->token : NULL,
                       info->token_found == 1 ? RANDOM_MAX_LEN : 0);
    if (ret == 0) {
        AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        awss_dev_ap_switchap_done = 1;
        AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_TIME_SUC);
    } else {
        int ret = awss_dev_ap_setup();
        if (0 != ret) {
            awss_trace("retry, but awss dev ap setup fail");
        }
    }
    awss_trace("connect '%s' %s\r\n", info->ssid, ret == 0 ? "success" : "fail");
}

int wifimgr_process_dev_ap_switchap_request(void *ctx, void *resource, void *remote, void *request)
{
#define AWSS_DEV_AP_SWITCHA_RSP_LEN (512)
    int str_len = 0, success = 1, len = 0;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char random[RANDOM_MAX_LEN + 1] = {0};
    char *msg = NULL, *dev_info = NULL;
    char *str = NULL, *buf = NULL;
    char ssid_found = 0;
    int ret = -1;
    char *ssid;
    char *passwd;
    uint8_t *bssid;
    uint8_t *token_found;
    uint8_t *token;
    static char dev_ap_switchap_parsed = 0;
    char topic[TOPIC_LEN_MAX] = {0};
    uint16_t msgid = -1;
    int result = 0;

    ap_info_t *info = g_dev_ap_info_ptr;
    if (NULL == info) {
        return -1;
    }

    if (awss_dev_ap_switchap_done != 0) {
        return -1;
    }

    ssid = info->ssid;
    passwd = info->passwd;
    bssid = info->bssid;
    token_found = &(info->token_found);
    token = info->token;

    if (dev_ap_switchap_parsed != 0) {
        goto DEV_AP_SWITCHAP_END;
    }
    dev_ap_switchap_parsed = 1;

    AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_TIME_START);

    msg = awss_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN);
    if (msg == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }
    dev_info = awss_zalloc(AWSS_DEV_AP_SWITCHA_RSP_LEN);
    if (dev_info == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_trace("dev ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);
    if (buf == NULL) {
        goto DEV_AP_SWITCHAP_END;
    }

    do {

        str_len = 0;
        str = json_get_value_by_name(buf, len, "ssid", &str_len, 0);
        awss_trace("ssid, len:%u, %s\r\n", str_len, str != NULL ? str : "NULL");
        if (str && (str_len < PLATFORM_MAX_SSID_LEN)) {
            memcpy(ssid, str, str_len);
            ssid_found = 1;
        }

        if (!ssid_found) {
            str_len = 0;
            str = json_get_value_by_name(buf, len, "xssid", &str_len, 0);
            if (str && (str_len < PLATFORM_MAX_SSID_LEN * 2 - 1)) {
                uint8_t decoded[OS_MAX_SSID_LEN] = {0};
                int len = str_len / 2;
                memcpy(ssid, str, str_len);
                utils_str_to_hex(ssid, str_len, decoded, OS_MAX_SSID_LEN);
                memcpy(ssid, (const char *)decoded, len);
                ssid[len] = '\0';
            } else {
                HAL_Snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -1, "\"ssid error\"");
                success = 0;
                break;
            }
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "random", &str_len, 0);
        if (str && str_len ==  RANDOM_MAX_LEN * 2) {
            utils_str_to_hex(str, str_len, (unsigned char *)random, RANDOM_MAX_LEN);
        } else {
            HAL_Snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -4, "\"random len error\"");
            success = 0;
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "token", &str_len, 0);
        if (str && str_len ==  RANDOM_MAX_LEN * 2) {  /* token len equal to random len */
            utils_str_to_hex(str, str_len, (unsigned char *)token, RANDOM_MAX_LEN);
            *token_found = 1;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "bssid", &str_len, 0);
        if (str) {
            os_wifi_str2mac(str, (char *)bssid);
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);

        if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
            char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
            memcpy(encoded, str, str_len);
            aes_decrypt_string(encoded, passwd, str_len,
                               0, awss_get_encrypt_type(), 1, random); /* 64bytes=2x32bytes */
        } else {
            HAL_Snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
            success = 0;
            AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            HAL_Snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd content error\"");
            success = 0;
            AWSS_UPDATE_STATIS(AWSS_STATIS_DAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
        }
    } while (0);

    awss_trace("Sending message to app: %s", msg);
    awss_trace("switch to ap: '%s'", ssid);
    if (success == 1) {
        if (*token_found == 0) {
            produce_random(aes_random, sizeof(aes_random));
        }
        dev_info[0] = '{';
        awss_build_dev_info(*token_found == 1 ? AWSS_NOTIFY_TYPE_MAX : AWSS_NOTIFY_DEV_BIND_TOKEN, dev_info + 1,
                            AWSS_DEV_AP_SWITCHA_RSP_LEN - 1);
        dev_info[strlen(dev_info)] = '}';
        dev_info[AWSS_DEV_AP_SWITCHA_RSP_LEN - 1] = '\0';
        HAL_Snprintf(msg, AWSS_DEV_AP_SWITCHA_RSP_LEN, AWSS_ACK_FMT, req_msg_id, 200, dev_info);
    }
    awss_build_topic((const char *)TOPIC_AWSS_DEV_AP_SWITCHAP, topic, TOPIC_LEN_MAX);
    result = awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request, awss_dev_ap_switchap_resp,
                                     &(info->msgid), 1);
    (void)result;  /* remove complier warnings */
    awss_trace("sending %s.", result == 0 ? "success" : "fail");

DEV_AP_SWITCHAP_END:
    dev_ap_switchap_parsed = 0;
    if (dev_info) {
        HAL_Free(dev_info);
    }
    if (msg) {
        HAL_Free(msg);
    }
    return ret;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
