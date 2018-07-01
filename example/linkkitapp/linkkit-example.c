/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "iot_export_event.h"
#include "linkkit_app.h"
#include "linkkit_export.h"
#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

static char linkkit_started = 0;
static char awss_running = 0;

void start_linkkitapp(void *parms);
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0) {
        //clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
        aos_post_delayed_action(50, start_linkkitapp, NULL);
        linkkit_started = 1;
    }
}

void start_linkkitapp(void *parms)
{
    LOG("linkkit app");
    linkkit_app();
}


static void cloud_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_YUNIO) {
        return;
    }

    LOG("cloud_service_event %d", event->code);

    if (event->code == CODE_YUNIO_ON_CONNECTED) {
        LOG("user sub and pub here");
        return;
    }

    if (event->code == CODE_YUNIO_ON_DISCONNECTED) {
    }
}

/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in linkkit_event_monitor
 */
static void linkkit_event_monitor(int event)
{
    switch (event) {
    case IOTX_AWSS_START:                // AWSS start without enbale, just supports device discover
        // operate led to indicate user
        LOG("IOTX_AWSS_START");
        break;
    case IOTX_AWSS_ENABLE:               // AWSS enable
        LOG("IOTX_AWSS_ENABLE");
        // operate led to indicate user
        break;
    case IOTX_AWSS_LOCK_CHAN:            // AWSS lock channel(Got AWSS sync packet)
        LOG("IOTX_AWSS_LOCK_CHAN");
        // operate led to indicate user
        break;
    case IOTX_AWSS_PASSWD_ERR:           // AWSS decrypt passwd error
        LOG("IOTX_AWSS_PASSWD_ERR");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_SSID_PASSWD:
        LOG("IOTX_AWSS_GOT_SSID_PASSWD");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA:         // AWSS try to connnect adha (device discover, router solution)
        LOG("IOTX_AWSS_CONNECT_ADHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA_FAIL:    // AWSS fails to connect adha
        LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA:          // AWSS try to connect aha (AP solution)
        LOG("IOTX_AWSS_CONNECT_AHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA_FAIL:     // AWSS fails to connect aha
        LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SETUP_NOTIFY:         // AWSS sends out device setup information (AP and router solution)
        LOG("IOTX_AWSS_SETUP_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER:       // AWSS try to connect destination router
        LOG("IOTX_AWSS_CONNECT_ROUTER");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER_FAIL:  // AWSS fails to connect destination router.
        LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_IP:               // AWSS connects destination successfully and got ip address
        LOG("IOTX_AWSS_GOT_IP");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SUC_NOTIFY:           // AWSS sends out success notify (AWSS sucess)
        LOG("IOTX_AWSS_SUC_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_BIND_NOTIFY:          // AWSS sends out bind notify information to support bind between user and device
        LOG("IOTX_AWSS_BIND_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD:                // Device try to connect cloud
        LOG("IOTX_CONN_CLOUD");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_FAIL:           // Device fails to connect cloud, refer to net_sockets.h for error code
        LOG("IOTX_CONN_CLOUD_FAIL");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_SUC:            // Device connects cloud successfully
        LOG("IOTX_CONN_CLOUD_SUC");
        // operate led to indicate user
        break;
    case IOTX_RESET:                     // Linkkit reset success (just got reset response from cloud without any other operation)
        LOG("IOTX_RESET");
        // operate led to indicate user
        break;
    default:
        break;
    }
}

static void start_netmgr(void *p)
{
    iotx_event_regist_cb(linkkit_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
}

void do_awss_active()
{
    LOG("do_awss_active %d\n", awss_running);
    awss_running = 1;
    extern int awss_config_press();
    awss_config_press();
}

static void linkkit_reset(void *p)
{
    netmgr_clear_ap_config();
    HAL_Sys_reboot();
}

extern int awss_report_reset();
static void do_awss_reset()
{
    aos_task_new("reset", (void (*)(void *))awss_report_reset, NULL, 2048);
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

void linkkit_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }
    LOG("awss config press %d\n", eventinfo->value);

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {
            do_awss_active();
        } else if (eventinfo->value == VALUE_KEY_LTCLICK) {
            do_awss_reset();
        }
    }
}

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
}

static struct cli_command resetcmd = {
    .name = "reset",
    .help = "factory reset",
    .function = handle_reset_cmd
};

static struct cli_command ncmd = {
    .name = "active_awss",
    .help = "active_awss [start]",
    .function = handle_active_cmd
};
#endif

int application_start(int argc, char **argv)
{
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif


#ifdef WITH_SAL
    sal_init();
#endif
    aos_set_log_level(AOS_LL_DEBUG);

    netmgr_init();
    aos_register_event_filter(EV_KEY, linkkit_key_process, NULL);
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
    aos_register_event_filter(EV_YUNIO, cloud_service_event, NULL);

#ifdef CONFIG_AOS_CLI
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&ncmd);
#endif
    aos_task_new("netmgr", start_netmgr, NULL, 4096);

    aos_loop_run();

    return 0;
}
