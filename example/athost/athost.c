/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <atparser.h>
#include <aos/network.h>

#include "athost.h"
#include "at_util.h"

#define TAG "athost"

#define MAX_ATCMD_HANDLERS 10
static int            atcmd_handlers_count               = 0;
static atcmd_op_ptr_t atcmd_handlers[MAX_ATCMD_HANDLERS] = { 0 };

static bool inited = false;

static const char *prefix_athost = "AT+";

int atcmd_register_handlers(atcmd_op_ptr_t handler)
{
    if (atcmd_handlers_count >= MAX_ATCMD_HANDLERS) {
        LOGE(TAG, "atcmd handlers exceeds limit\n");
        return -1;
    }

    if (atcmd_handlers[atcmd_handlers_count] != NULL) {
        LOGE(TAG, "atcmd handlers overwrite\n");
        return -1;
    }

    atcmd_handlers[atcmd_handlers_count] = handler;
    atcmd_handlers_count++;

    return 0;
}

static void atcmd_handle()
{
    char            single;
    char            one_more;
    char            prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    atcmd_hdl_ptr_t handler                      = NULL;
    LOGD(TAG, "%s entry.", __func__);

    // uart_echo();
    at.parse(&single, 1);
    one_more = 0;

    // search handler
    for (int i = 0; i < atcmd_handlers_count; i++) {
        if (atcmd_handlers[i] != NULL &&
            atcmd_handlers[i]->prefix[0] == single) {
            if (strlen(atcmd_handlers[i]->prefix) == 1) {
                handler = atcmd_handlers[i]->get_atcmd_handler();
            } else if (strlen(atcmd_handlers[i]->prefix) == 2) {
                if (one_more == 0)
                    at.parse(&one_more, 1);

                if (atcmd_handlers[i]->prefix[1] == one_more) {
                    handler = atcmd_handlers[i]->get_atcmd_handler();
                }
            } else {
                LOGE(TAG, "Not support long prefix check! %s\n",
                     atcmd_handlers[i]->prefix);
            }
        }
    }

    if (handler != NULL) {
        if (handler->function() < 0) {
            LOGD(TAG, "atcmd execution fail!\n");
        }
    } else {
        LOGE(TAG, "atcmd handler not found!\n");
    }

    LOGD(TAG, "%s exit.", __func__);
}

// Add new atcmd handlers here
// extern int add_atcmd_coap_opt();
extern int register_atcmd_mqtt_opt();
extern int register_atcmd_cip_opt();
extern int register_atcmd_uart_opt();
extern int register_atcmd_wifi_mgmt_opt();
extern int register_atcmd_wifi_ywss_opt();

static int athost_pre_init()
{
    register_atcmd_cip_opt();
    register_atcmd_mqtt_opt();
    register_atcmd_wifi_mgmt_opt();
    register_atcmd_wifi_ywss_opt();
    register_atcmd_uart_opt();
}

static int athost_init()
{
    if (inited) {
        LOGW(TAG, "athost already initialized");
        return 0;
    }

    if (uart_send_task_init() < 0) {
        LOGE(TAG, "uart send task init!\n");
        goto err;
    }

    for (int i = 0; i < atcmd_handlers_count; i++) {
        if (atcmd_handlers[i]->init() < 0) {
            LOGE(TAG, "atcmd %s init fail!\n", atcmd_handlers[i]->name);
            goto err;
        }
    }

    inited = true;
    return 0;

err:
    uart_send_task_deinit();

    for (int i = 0; i < atcmd_handlers_count; i++) {
        atcmd_handlers[i]->deinit();
    }

    inited = false;

    return -1;
}

static void app_delayed_action(void *arg)
{
    LOG("AT host server: alive %s:%d %s\r\n", __func__, __LINE__,
        aos_task_name());
    aos_post_delayed_action(50000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    // mk3060: 4096 mk3165: 1024
    at.set_worker_stack_size(4096);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);

    athost_pre_init();
    athost_init();

    at.oob(prefix_athost, NULL, 0, atcmd_handle, NULL);

    LOG("NEW AT host server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    aos_loop_run();

    return 0;
}
