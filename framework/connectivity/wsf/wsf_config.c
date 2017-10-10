/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>

#include "wsf_defines.h"
#include "wsf_client.h"
#include "wsf_config.h"
#include "wsf_log.h"


static wsf_config_t wsf_global_config;

static const char *default_server_name = "alink.tcp.aliyun.com";
static const unsigned short default_server_port = 443;
static const unsigned short default_version = 1;

/*
 * 1) wsf server收到active包时，update心跳间隔：最小值为12s, 最大值为255
 *    if (12 < period && period > 0)
 *          period = 12;
 *    else if (period <= 0)
 *          period = 0;
 *
 * 2) wsf收到业务login时，为每个连接创建心跳timer, timer周期=2.5个心跳包，
 *    也就是2.5个心跳间隔会去check下连接是否正常。假如心跳为12S, 2.5个心跳为30S,
 *    也就是wsf server 每隔30S会去check下心跳状态
 *
 * 3) wsf server 后台有一个专门的线程，每隔40S check下所有连接(每台PC 2~3条连接?)看是否有连接超时
 */
static const int default_heartbeat_interval = 12;
static const int default_request_timeout = 10;
static const int default_msg_queue_length = 20;
static const int default_msg_recv_length = 2048 * 2;
static const int default_enable_ssl = 1;
static const ssl_version default_ssl_version = TLSV1;

extern char *default_ca_str;
#define MODULE_NAME "wsf_msg"
wsf_config_t *wsf_get_config()
{
    return &wsf_global_config;
}

wsf_code wsf_config(config_opt opt, void *value)
{
    if (!value) {
        return WSF_FAIL;
    }

    wsf_code ret = WSF_SUCCESS;

    switch (opt) {

        case SERVER_NAME_STRING:
            if (wsf_global_config.server_name) {
                os_free(wsf_global_config.server_name);
            }
            char *str = (char *)value;
            wsf_global_config.server_name = (char *)os_malloc(strlen(str) + 1);
            if (wsf_global_config.server_name) {
                strcpy(wsf_global_config.server_name, str);
            } else {
                ret = WSF_FAIL;
                LOGE(MODULE_NAME, "out of memory");
            }

            break;
        case SERVER_PORT_INT :
            wsf_global_config.server_port = *((unsigned short *)value);
            break;

        case HEARTBEAT_INTERVAL_INT:
            wsf_global_config.heartbeat_interval = *((int *)value);
            break;

        case VERSION_SHORT:
            wsf_global_config.version = *((unsigned short *)value);
            break;

        case REQUEST_TIMEOUT_INT:
            wsf_global_config.request_timeout = *((int *)value);
            break;

        case MSG_QUEUE_LENGTH_INT:
            wsf_global_config.max_msg_queue_length = *((int *)value);
            break;
        case MAX_MSG_RECV_LENGTH_INT:
            wsf_global_config.max_msg_recv_length = *((int *)value);
            break;
        case ENABLE_SSL_INT:
            wsf_global_config.enable_ssl = *((int *)value);
            break;
        case CERTIFICATE_INT:
            wsf_global_config.global_ca_str = ((const char *)value);
            //FIXME: value can be a pointer to a binary ca
            wsf_global_config.ca_str_len = strlen((const char *)value);
            break;
        case SSL_VERSION_INT:
            wsf_global_config.ssl_version = *((int *)value);
            break;
        case USER_HEARTBEAT_INTERVAL_INT:
            wsf_global_config.user_heartbeat_interval = *((int *)value);
            break;
        default:
            LOGE(MODULE_NAME, "unknown config option: %d", opt);
            ret = WSF_UNKNOWN_OPTION;
            break;
    }

    return ret;
}

wsf_code wsf_init_config()
{
    if (!wsf_global_config.server_name) {
        size_t len = strlen(default_server_name);
        wsf_global_config.server_name = (char *)os_malloc(len + 1);
        if (wsf_global_config.server_name) {
            strcpy(wsf_global_config.server_name, default_server_name) ;
        } else {
            LOGE(MODULE_NAME, "out of memory");
            return WSF_FAIL;
        }

    }

    if (wsf_global_config.server_port <= 0) {
        wsf_global_config.server_port = default_server_port;
    }

    if (wsf_global_config.heartbeat_interval <= 0) {
        wsf_global_config.heartbeat_interval = default_heartbeat_interval;
        //wsf_global_config.user_heartbeat_interval = default_heartbeat_interval;
    }

    if (wsf_global_config.version <= 0) {
        wsf_global_config.version = default_version;
    }

    if (wsf_global_config.request_timeout <= 0) {
        wsf_global_config.request_timeout = default_request_timeout;
    }

    if (wsf_global_config.max_msg_queue_length <= 0) {
        wsf_global_config.max_msg_queue_length = default_msg_queue_length;
    }
    if (wsf_global_config.max_msg_recv_length <= 0) {
        wsf_global_config.max_msg_recv_length = default_msg_recv_length;
    }

    if (wsf_global_config.global_ca_str == NULL) {
        wsf_global_config.global_ca_str = default_ca_str;
        wsf_global_config.ca_str_len = strlen(default_ca_str);
    }

    wsf_global_config.enable_ssl = default_enable_ssl;
    wsf_global_config.ssl_version = default_ssl_version;

    return WSF_SUCCESS;
}

wsf_code wsf_destroy_config()
{
    if (wsf_global_config.server_name) {
        os_free(wsf_global_config.server_name);
        wsf_global_config.server_name = NULL;
    }
    return WSF_SUCCESS;
}
