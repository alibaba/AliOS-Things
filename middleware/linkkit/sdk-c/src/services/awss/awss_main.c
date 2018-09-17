/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdlib.h>
#include "log.h"
#include "aws_lib.h"
#include "zconfig_utils.h"
#include "zconfig_lib.h"
#include "awss_cmp.h"
#include "awss_main.h"
#include "awss_notify.h"
#include "os.h"
#include "passwd.h"
#include "utils.h"
#include "enrollee.h"
#include "awss.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

char awss_finished = 0;
char awss_stop_connecting = 0;

int __awss_start(void)
{
    char ssid[OS_MAX_SSID_LEN + 1] = {0}, passwd[OS_MAX_PASSWD_LEN + 1] = {0};
    enum AWSS_AUTH_TYPE auth = AWSS_AUTH_TYPE_INVALID;
    enum AWSS_ENC_TYPE encry = AWSS_ENC_TYPE_INVALID;
    uint8_t bssid[OS_ETH_ALEN] = {0};
    uint8_t channel = 0;
    int ret;

    os_printf("awss version: %s", zconfig_lib_version());
    awss_stop_connecting = 0;
    awss_finished = 0;
    /* these params is useless, keep it for compatible reason */
    aws_start(NULL, NULL, NULL, NULL);

    ret = aws_get_ssid_passwd(&ssid[0], &passwd[0], &bssid[0],
            (char *)&auth, (char *)&encry, &channel);
    if (!ret)
	    log_warn("awss timeout!");

    aws_destroy();

    char awss_notify_needed = 1;
    char adha = 0;
    do {
        if (awss_stop_connecting)
            break;

        if ((adha = strcmp(ssid, ADHA_SSID)) == 0 || strcmp(ssid, DEFAULT_SSID) == 0) {
            awss_notify_needed = 0;
            awss_event_post(adha != 0 ? AWSS_CONNECT_AHA : AWSS_CONNECT_ADHA);
        } else {
            awss_event_post(AWSS_CONNECT_ROUTER);
        }

        ret = os_awss_connect_ap(WLAN_CONNECTION_TIMEOUT_MS, ssid, passwd,
                                 auth, encry, bssid, channel);
        if (!ret) {
            awss_debug("awss connect ssid:%s success", ssid);
            awss_event_post(AWSS_GOT_IP);

            if (awss_notify_needed == 0) {
                awss_connectap_notify_stop();
                awss_suc_notify_stop();
                awss_cmp_local_init();
                awss_devinfo_notify();
                awss_event_post(AWSS_SETUP_NOTIFY);
            } else {
                awss_devinfo_notify_stop();
                produce_random(aes_random, sizeof(aes_random));
            }
        } else {
            awss_debug("awss connect ssid:%s fail", ssid);
            if (awss_notify_needed == 0) {
                awss_event_post(adha != 0 ? AWSS_CONNECT_AHA_FAIL : AWSS_CONNECT_ADHA_FAIL);
            } else {
                awss_event_post(AWSS_CONNECT_ROUTER_FAIL);
            }
        }
    } while (0);

    awss_finished = 1;
    /* never reach here */
    return 0;
}

int __awss_stop(void)
{
    awss_stop_connecting = 1;
    aws_destroy();
    awss_connectap_notify_stop();
    awss_devinfo_notify_stop();
    awss_suc_notify_stop();

    awss_registrar_exit();

    while (1) {
        if (awss_finished) break;
        os_msleep(100);
    }
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
