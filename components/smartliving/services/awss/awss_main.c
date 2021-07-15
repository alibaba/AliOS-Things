/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "aws_lib.h"
#include "zconfig_lib.h"
#include "zconfig_utils.h"
#include "awss_enrollee.h"
#include "awss_packet.h"
#include "awss_notify.h"
#include "awss_statis.h"
#include "awss_event.h"
#include "connect_ap.h"
#include "awss_main.h"
#include "awss_cmp.h"
#include "passwd.h"
#include "awss.h"
#include "os.h"
#include "sl_config.h"

char awss_finished = 2;
char awss_stop_connecting = 0;

int __awss_start(pair_success_callback pair_cb)
{
    char ssid[OS_MAX_SSID_LEN + 1] = {0}, passwd[OS_MAX_PASSWD_LEN + 1] = {0};
    enum AWSS_AUTH_TYPE auth = AWSS_AUTH_TYPE_INVALID;
    enum AWSS_ENC_TYPE encry = AWSS_ENC_TYPE_INVALID;
    uint8_t token[ZC_MAX_TOKEN_LEN] = {0};
    uint8_t bssid[OS_ETH_ALEN] = {0};
    uint8_t channel = 0;
    uint8_t find_token = 0;
    uint8_t i;
    int ret;
    int pair_success = -1;
    
    printf("%s \r\n", __FUNCTION__);
    awss_stop_connecting = 0;
    awss_finished = 0;
    /* these params is useless, keep it for compatible reason */
    aws_start(NULL, NULL, NULL, NULL);

    ret = aws_get_ssid_passwd(&ssid[0], &passwd[0], &bssid[0], &token[0],
                              (char *)&auth, (char *)&encry, &channel);
    if (!ret) {
        awss_warn("awss timeout!");
    }
    printf("%s-%d ssid:%s passwd:%s\r\n", __FUNCTION__, __LINE__, ssid, passwd);
    if (awss_stop_connecting) {
        awss_finished = 1;
        return -1;
    }
    for (i = 0; i < ZC_MAX_TOKEN_LEN; i++) {
        if (token[i] != 0) {
            find_token = 1;
             awss_debug("found token!");
            break;
        }
    }
    aws_destroy();

    do {
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        char awss_notify_needed = 1;
        int adha = 0;
#endif

        if (awss_stop_connecting || strlen(ssid) == 0) {
            break;
        }
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        if ((adha = strcmp(ssid, ADHA_SSID)) == 0 || strcmp(ssid, DEFAULT_SSID) == 0) {
            awss_notify_needed = 0;
            awss_event_post(adha != 0 ? AWSS_CONNECT_AHA : AWSS_CONNECT_ADHA);
        } else
#endif
        {
            awss_event_post(AWSS_CONNECT_ROUTER);
            AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);
        }

        /*ret = os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT_MS, ssid, passwd,
                                 auth, encry, bssid, channel);*/

        // ret = awss_connect(ssid, passwd, bssid, ETH_ALEN, find_token != 0 ? token : NULL,
        //                    find_token != 0 ? ZC_MAX_TOKEN_LEN : 0);
        ret = pair_cb(1, ssid, passwd, WLAN_CONNECTION_TIMEOUT_MS);
        if (!ret) {
            awss_debug("awss connect ssid:%s success", ssid);
            pair_success = 0;
            awss_event_post(AWSS_GOT_IP);

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
            if (awss_notify_needed == 0) {
                awss_dev_bind_notify_stop();
                awss_suc_notify_stop();
                awss_cmp_local_init(adha == 0 ? AWSS_LC_INIT_ROUTER : AWSS_LC_INIT_PAP);
                awss_devinfo_notify();
                if (adha == 0) {
                    AWSS_UPDATE_STATIS(AWSS_STATIS_ROUTE_IDX, AWSS_STATIS_TYPE_TIME_SUC);
				}
                awss_event_post(AWSS_SETUP_NOTIFY);
            } else
#endif
            {
                AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_SUC);
                awss_devinfo_notify_stop();
            }
        } else {
            awss_debug("awss connect ssid:%s fail", ssid);
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
            if (awss_notify_needed == 0) {
                awss_event_post(adha != 0 ? AWSS_CONNECT_AHA_FAIL : AWSS_CONNECT_ADHA_FAIL);
            } else
#endif
            {
                awss_event_post(AWSS_CONNECT_ROUTER_FAIL);
            }
        }
    } while (0);

    AWSS_DISP_STATIS();
    awss_finished = 1;
    return pair_success;
}

int __awss_stop(void)
{
    awss_stop_connecting = 1;
    aws_destroy();
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    awss_devinfo_notify_stop();
#endif
    awss_suc_notify_stop();
#ifndef AWSS_DISABLE_REGISTRAR
    awss_registrar_deinit();
#endif
    if (awss_finished < 2) {
        awss_cmp_local_deinit(1);
    } else {
        awss_cmp_local_deinit(0);
    }

    while (1) {
        if (awss_finished) {
            break;
        }
        os_msleep(300);
    }
    aws_release_mutex();
    awss_finished = 2;
    return 0;
}


