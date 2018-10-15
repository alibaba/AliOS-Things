/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "awss.h"
#include "awss_main.h"
#include "zconfig_utils.h"
#include "awss_enrollee.h"
#include "awss_cmp.h"
#include "awss_notify.h"
#include "awss_timer.h"
#include "awss_packet.h"
#include "awss_event.h"
#include "awss_adha.h"
#include "awss_aha.h"
#include "passwd.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define AWSS_PRESS_TIMEOUT_MS  (60000)

extern int switch_ap_done;
static uint8_t awss_stopped = 0;
static uint8_t g_user_press = 0;
static void *press_timer = NULL;

static void awss_press_timeout(void);

int awss_success_notify(void)
{
    g_user_press = 0;
    awss_press_timeout();

    awss_cmp_local_init();
    awss_suc_notify_stop();
    awss_suc_notify();
    return 0;
}

int awss_start(void)
{
    awss_event_post(AWSS_START);
    produce_random(aes_random, sizeof(aes_random));

    do {
        awss_stopped = 0;
        __awss_start();
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        do {
            char ssid[PLATFORM_MAX_SSID_LEN + 1] = {0};
#ifdef AWSS_SUPPORT_ADHA
            while (1) {
                memset(ssid, 0, sizeof(ssid));
                os_wifi_get_ap_info(ssid , NULL, NULL);
                awss_debug("start, ssid:%s, strlen:%d\n", ssid, strlen(ssid));
                if (strlen(ssid) > 0 && strcmp(ssid, ADHA_SSID))  // not adha AP
                    break;

                if (os_sys_net_is_ready()) { // skip the adha failed
                    awss_cmp_local_init();

                    awss_open_adha_monitor();
                    while (!awss_is_ready_switch_next_adha())
                        os_msleep(50);
                    awss_cmp_local_deinit();
                }

                if (switch_ap_done || awss_stopped)
                    break;
                __awss_start();
            }
#endif
            if (awss_stopped)
                break;

            if (switch_ap_done)
                break;

            if (strlen(ssid) > 0 && strcmp(ssid, DEFAULT_SSID))  // not AHA
                break;

            if (os_sys_net_is_ready()) {
                awss_open_aha_monitor();

                awss_cmp_local_init();
                char dest_ap = 0;
                while (!awss_aha_monitor_is_timeout()) {
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
            awss_event_post(AWSS_ENABLE_TIMEOUT);
            __awss_start();
        } while (1);
#endif
        if (awss_stopped)
            break;

        if (os_sys_net_is_ready())
            break;
    } while (1);

#ifdef AWSS_SUPPORT_AHA
    awss_close_aha_monitor();
#endif
#ifdef AWSS_SUPPORT_ADHA
    awss_close_adha_monitor();
#endif

    awss_success_notify();

    return 0;
}

int awss_stop(void)
{
#ifdef AWSS_SUPPORT_AHA
    awss_close_aha_monitor();
#endif
#ifdef AWSS_SUPPORT_ADHA
    awss_close_adha_monitor();
#endif
    __awss_stop();
    awss_cmp_local_deinit();
    awss_stopped = 1;
    return 0;
}

static void awss_press_timeout(void)
{
    awss_stop_timer(press_timer);
    press_timer = NULL;
    if (g_user_press)
        awss_event_post(AWSS_ENABLE_TIMEOUT);
    g_user_press = 0;
}

int awss_config_press(void)
{
    int timeout = os_awss_get_timeout_interval_ms();

    awss_trace("enable awss\r\n");

    g_user_press = 1;

    awss_event_post(AWSS_ENABLE);

    if (press_timer == NULL)
        press_timer = HAL_Timer_Create("press", (void (*)(void *))awss_press_timeout, NULL);
    if (press_timer == NULL)
        return -1;

    HAL_Timer_Stop(press_timer);

    if (timeout < AWSS_PRESS_TIMEOUT_MS)
        timeout = AWSS_PRESS_TIMEOUT_MS;
    HAL_Timer_Start(press_timer, timeout);

    awss_debug("%s exit", __func__);

    return 0;
}

uint8_t awss_get_config_press(void)
{
    return g_user_press;
}

void awss_set_config_press(uint8_t press)
{
    g_user_press = press;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
