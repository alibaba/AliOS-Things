/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "ulog/ulog.h"
#include <netmgr.h>

#define TAG "httpdns_test"
#include "http.h"
#include "httpdns/httpdns.h"

static bool _network_ready = false;

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP){
        return;
    }

    _network_ready = true;
}

static void app_delayed_action(void *arg)
{
    LOG("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
}

static void print_help_message()
{
    LOG("Usage:\r\n"
        "    # hdns [-h] | domain name\r\n"
        "- Note: please make sure network is connected before executing \"websoc\"\r\n"
        "        You can connect network with \"netmgr connect <ssid> <passwd>\"\r\n"
        "- To get help:\r\n"
        "    # hdns -h \r\n"
        "- To query domain\r\n"
        "    # hdns www.aliyun.com\r\n");
}

static void hdns_req_command(char *destName)
{
    dns_cache_t *cache = NULL;
    char ipAddr[20] = {0};

    if (!_network_ready) {
        LOGE(TAG, "Connect network first!");
        return;
    }

    if(!httpdns_is_disabled()) {
        /* prfetch */
        httpdns_prefetch(destName, &cache);

        /* exist */
        if (cache) {
            if (httpdns_get_ipaddr_from_cache(cache, ipAddr)) {
                LOGI(TAG, "hdns fetch result:%s", ipAddr);
                httpdns_free_cache(cache);
                return;
            }

            httpdns_free_cache(cache);
        }

        /* query */
        httpdns_resolv_host(destName, ASYNC);
        httpdns_prefetch_timeout(destName, &cache, 1000);

        /* exist */
        if (cache) {
            if (httpdns_get_ipaddr_from_cache(cache, ipAddr)) {
                LOGI(TAG, "hdns fetch result:%s", ipAddr);
                httpdns_free_cache(cache);
                return;
            }

            httpdns_free_cache(cache);
        }
    }

    LOGI(TAG, "hdns fetch fail\n");
    return false;
}

static void hdns_command(char *buffer, int32_t buf_len, int32_t argc, char **argv)
{
    if (argc < 2) {
        print_help_message();
        return;
    }

    if (strcmp(argv[1], "-h") == 0) {
        print_help_message();
    } else {
        hdns_req_command(argv[1]);
    }
}

/**
 * To use the httpdns test commands, please follow below steps:
 *   - Setup the network with "netmgr" cli:
 *     # netmgr connect <WiFi_AP_ssid> <WiFi_AP_password>
 *   - Start httpdns tests with "hdns" cli:
 *     # hdns www.aliyun.com
 */
static struct cli_command httpdns_test_commands[] = {
    {"hdns", "hdns [-h] domain", hdns_command},
};

int application_start(int argc, char *argv[])
{
     /* set log level */
    aos_set_log_level(AOS_LL_INFO);

    /* register websocket test commands */
    aos_cli_register_commands(httpdns_test_commands,
        sizeof(httpdns_test_commands) / sizeof(struct cli_command));

    /* setup wifi network event handler */
    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    /* use netmgr command to setup network connection */
    netmgr_init();
    netmgr_start(false);

    http_client_initialize();

    /* enter AOS loop */
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}
