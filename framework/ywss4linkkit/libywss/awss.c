/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "work_queue.h"
#include "enrollee.h"
#include "awss_cmp.h"
#include "awss_notify.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

extern int switch_ap_done;
static uint8_t awss_stopped = 0;
static uint8_t adha_switch = 0;
void adha_monitor(void)
{
    adha_switch = 1;
}

#define ADHA_WORK_CYCLE      (5 * 1000)
static struct work_struct adha_work = {
    .func = (work_func_t) &adha_monitor,
    .prio = DEFAULT_WORK_PRIO,
    .name = "adha",
};

static void aha_monitor(void);
#define AHA_MONITOR_TIMEOUT_MS  (1 * 60 * 1000)
static struct work_struct aha_work = {
    .func = (work_func_t) &aha_monitor,
    .prio = 1, /* smaller digit means higher priority */
    .name = "aha",
};
static volatile char aha_timeout;

static void aha_monitor(void)
{
    aha_timeout = 1;
}

int aha_is_timeout()
{
    return aha_timeout > 0;
}

int awss_cancel_aha_monitor()
{
    cancel_work(&aha_work);
    aha_timeout = 0;
    return 0;
}

static void awss_open_aha_monitor()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    os_wifi_get_ap_info(ssid , NULL, NULL);
    awss_debug("aha monitor, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
    if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID)) { // not adha AP
        return;
    }
    aha_timeout = 0;
    queue_delayed_work(&aha_work, AHA_MONITOR_TIMEOUT_MS);
}

int awss_report_cloud()
{
    work_queue_init();
    awss_cmp_online_init();
    awss_report_token();
    awss_cmp_local_init();
    awss_connectap_notify_stop();
    awss_connectap_notify();
#ifndef AWSS_DISABLE_REGISTRAR
    awss_registrar_init();
#endif
    return 0;
}

int awss_success_notify()
{
    work_queue_init();
    awss_cmp_local_init();
    awss_suc_notify_stop();
    awss_suc_notify();
    return 0;
}

int awss_start()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
    work_queue_init();
    produce_random(aes_random, sizeof(aes_random));

    awss_stopped = 0;
    __awss_start();

    do {
        while (1) {
            memset(ssid, 0, sizeof(ssid));
            os_wifi_get_ap_info(ssid , NULL, NULL);
            awss_debug("start, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
            if (strlen(ssid) > 0 && strcmp(ssid, ADHA_SSID)) { // not adha AP
                break;
            }

            if (os_sys_net_is_ready()) { // skip the adha failed
                awss_cmp_local_init();

                adha_switch = 0;
                queue_delayed_work(&adha_work, ADHA_WORK_CYCLE);
                while (!adha_switch) {
                    os_msleep(200);
                }
                adha_switch = 0;

                awss_cmp_local_deinit();
            }

            if (switch_ap_done || awss_stopped) {
                break;
            }
            netmgr_clear_ap_config();
            __awss_start();
        }
        if (switch_ap_done || awss_stopped) {
            break;
        }
        if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID)) { // not AHA
            break;
        }

        awss_open_aha_monitor();

        awss_cmp_local_init();
        char dest_ap = 0;
        while (!aha_is_timeout()) {
            memset(ssid, 0, sizeof(ssid));
            os_wifi_get_ap_info(ssid , NULL, NULL);
            if (os_sys_net_is_ready() &&
                strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID)) {  // not AHA
                dest_ap = 1;
                break;
            }
            os_msleep(200);
        }

        awss_cmp_local_deinit();
        if (switch_ap_done || awss_stopped) {
            break;
        }

        if (dest_ap == 1) {
            break;
        }

        __awss_start();
    } while (1);

    if (os_sys_net_is_ready() == 0) {
        return -1;
    }

    awss_success_notify();

    return 0;
}

int awss_stop()
{
    cancel_work(&adha_work);
    cancel_work(&aha_work);
    __awss_stop();
    awss_cmp_local_deinit();
    awss_stopped = 1;
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
