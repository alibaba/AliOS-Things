/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "enrollee.h"
#include "awss_cmp.h"
#include "awss_notify.h"
#include "awss_timer.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

extern int switch_ap_done;
static uint8_t adha_switch = 0;
static uint8_t awss_stopped = 0;

static void *adha_timer = NULL;
static void *aha_timer = NULL;

static void adha_monitor(void)
{
    adha_switch = 1;
}

#define ADHA_WORK_CYCLE      (5 * 1000)

static void aha_monitor(void);
#define AHA_MONITOR_TIMEOUT_MS  (1 * 60 * 1000)
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
    HAL_Timer_Stop(aha_timer);
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
    if (aha_timer == NULL)
        aha_timer = HAL_Timer_Create("aha", (void (*)(void *))aha_monitor, NULL);
    HAL_Timer_Stop(aha_timer);
    HAL_Timer_Start(aha_timer, AHA_MONITOR_TIMEOUT_MS);
}

int awss_report_cloud()
{
    awss_cmp_online_init();
    awss_check_reset();
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
    awss_cmp_local_init();
    awss_suc_notify_stop();
    awss_suc_notify();
    return 0;
}

int awss_event_post(int event)
{
    extern int iotx_event_post(int);
    return iotx_event_post(event);
}

int awss_start()
{
    char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};

    awss_event_post(AWSS_START);
    produce_random(aes_random, sizeof(aes_random));

    do {
        awss_stopped = 0;
        __awss_start();

        do {
            while (1) {
                memset(ssid, 0, sizeof(ssid));
                os_wifi_get_ap_info(ssid , NULL, NULL);
                awss_debug("start, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
                if (strlen(ssid) > 0 && strcmp(ssid, ADHA_SSID))  // not adha AP
                    break;

                if (os_sys_net_is_ready()) { // skip the adha failed
                    awss_cmp_local_init();

                    adha_switch = 0;
                    if (adha_timer == NULL)
                        adha_timer = HAL_Timer_Create("adha", (void (*)(void *))adha_monitor, NULL);
                    HAL_Timer_Stop(adha_timer);
                    HAL_Timer_Start(adha_timer, ADHA_WORK_CYCLE);
                    while (!adha_switch)
                        os_msleep(50);
                    adha_switch = 0;

                    awss_cmp_local_deinit();
                }

                if (switch_ap_done || awss_stopped)
                    break;
                __awss_start();
            }
            if (switch_ap_done || awss_stopped)
                break;
            if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID))  // not AHA
                break;

            if (os_sys_net_is_ready()) {
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
                    os_msleep(50);
                }

                awss_cmp_local_deinit();

                if (switch_ap_done || awss_stopped)
                    break;

                if (dest_ap == 1)
                    break;
            }
            __awss_start();
        } while (1);

        if (os_sys_net_is_ready())
            break;
    } while (1);

    awss_stop_timer(aha_timer);
    aha_timer = NULL;
    awss_stop_timer(adha_timer);
    adha_timer = NULL;

    awss_success_notify();

    return 0;
}

int awss_stop()
{
    awss_stop_timer(aha_timer);
    aha_timer = NULL;
    awss_stop_timer(adha_timer);
    adha_timer = NULL;
    __awss_stop();
    awss_cmp_local_deinit();
    awss_stopped = 1;
    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
