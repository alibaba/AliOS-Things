/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"

#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef CSP_LINUXHOST
#include <signal.h>
#endif

#include <k_api.h>

#ifdef LINKKIT_GATEWAY_TEST_CMD
#include "simulate_subdev/testcmd.h"
#endif

static char linkkit_started = 0;
static char awss_running    = 0;

void linkkit_main(void *p);
void set_iotx_info();
void do_awss_active();

#ifdef CONFIG_PRINT_HEAP
void print_heap()
{
    extern k_mm_head *g_kmm_head;
    int    free = g_kmm_head->free_size;
    LOG("============free heap size =%d==========", free);
}
#endif

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
        // clear_wifi_ssid();
        return;
    }

    if (!linkkit_started) {
#ifdef CONFIG_PRINT_HEAP
        print_heap();
#endif
#ifdef MQTT_DIRECT
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 6);
#else
        aos_task_new("linkkit", (void (*)(void *))linkkit_main, NULL, 1024 * 8);
#endif
        linkkit_started = 1;
    }
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
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device
                              // discover
            // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable
            LOG("IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            LOG("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            LOG("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            LOG("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
                                     // discover, router solution)
            LOG("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            LOG("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
                                     // (AP and router solution)
            LOG("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            LOG("IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
                                            // router.
            LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
                               // ip address
            LOG("IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
                                   // sucess)
            LOG("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
                                    // support bind between user and device
            LOG("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            LOG("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
                                   // net_sockets.h for error code
            LOG("IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
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
    #ifdef WIFI_AWSS_ENABLED
    extern int awss_config_press();
    awss_config_press();
    #endif
}

static void linkkit_reset(void *p)
{
    netmgr_clear_ap_config();
    HAL_Sys_reboot();
}

extern int  awss_report_reset();
static void do_awss_reset()
{
#ifdef WIFI_AWSS_ENABLED
    aos_task_new("reset", (void (*)(void *))awss_report_reset, NULL, 2048);
#endif
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
static void handle_gw_mm_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int loglevel_bakups =  LITE_get_loglevel();
    LITE_set_loglevel(LOG_DEBUG_LEVEL);
    LITE_dump_malloc_free_stats(LOG_DEBUG_LEVEL);
    LITE_set_loglevel(loglevel_bakups);
}
static struct cli_command gw_mm = {
    .name = "gw_mm",
    .help = "gw_mm",
    .function = handle_gw_mm_cmd
};
static void handle_gw_start_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc != 5) {
		printf("Warning:Invalid gateway Info argc=%d!\n\r",argc);
        return;
    }
    if (NULL == argv[1] || NULL == argv[2] || NULL == argv[3] || NULL == argv[4]
		||strlen(argv[1])<=0 || strlen(argv[2])<=0 || strlen(argv[3])<=0 || strlen(argv[4]) <= 0)
		{
            printf("Warning:Invalid gateway Info!\n\r");
            return;
		}
	printf("******set gateway tuple*********************\n\r");
	printf("******PK:%s\n\r",argv[1]);		
	printf("******DN:%s\n\r",argv[2]);	
	printf("******DS:%s\n\r",argv[3]);
    printf("******PS:%s\n\r",argv[4]);
	HAL_SetProductKey(argv[1]);
	HAL_SetDeviceName(argv[2]);
	HAL_SetDeviceSecret(argv[3]);
    HAL_SetProductSecret(argv[4]);

}

static struct cli_command gw_start = {
    .name = "gwstart",
    .help = "gwstart [PK|PN|DS|PS]",
    .function = handle_gw_start_cmd
};


static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_reset, NULL);
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_active, NULL);
}

static struct cli_command resetcmd = { .name     = "reset",
                                       .help     = "factory reset",
                                       .function = handle_reset_cmd };

static struct cli_command ncmd = { .name     = "active_awss",
                                   .help     = "active_awss [start]",
                                   .function = handle_active_cmd };
#endif

#ifdef CONFIG_PRINT_HEAP
static void duration_work(void *p)
{
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
}
#endif

int application_start(int argc, char **argv)
{


#ifdef CONFIG_PRINT_HEAP
    print_heap();
    aos_post_delayed_action(5000, duration_work, NULL);
#endif

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);
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
    aos_cli_register_command(&gw_start);
    aos_cli_register_command(&gw_mm);

#ifdef LINKKIT_GATEWAY_TEST_CMD
    aos_cli_register_command(&gw_test_cmd);
#endif /* LINKKIT_GATEWAY_TEST_CMD */    
#endif
    set_iotx_info();
    aos_task_new("netmgr", start_netmgr, NULL, 4096);

    aos_loop_run();

    return 0;
}
