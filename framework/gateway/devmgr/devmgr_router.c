/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include "devmgr.h"
#include "devmgr_router.h"
#include "aos/aos.h"
#include "json_parser.h"
#include "msdp.h"
#include "service_manager.h"
#include "connectivity_manager.h"
#include "digest_algorithm.h"
#include "config.h"

#define MODULE_NAME MODULE_NAME_DEVMGR

extern void *devlist_lock;
extern dlist_t dev_head;

static int router_register_device(dev_info_t *devinfo)
{
    int ret;
    char tx_buff[512] = {0};
    char result[128] = {0};

    alink_data_t data = {METHOD_DEVICE_REGISTER_SUB_WIFI, tx_buff};

    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_REGISTER_SUB_WIFI_FMT,
                                    devinfo->router_base.name,
                                    devinfo->router_base.model,
                                    devinfo->router_base.ostype,
                                    devinfo->router_base.category,
                                    devinfo->dev_base.u.ether_mac,
                                    devinfo->router_base.manufacturer)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);

    ret = ((service_t *)sm_get_service("accs"))->get((void *)&data, sizeof(data),
                                                     result, sizeof(result));
    if (SERVICE_RESULT_OK == ret) {
        int len = 0;
        char *json_str = json_get_value_by_name(result, strlen(result), JSON_KEY_UUID,
                                                &len, NULL);
        if (NULL == json_str) {
            log_error("get uuid fail, result data = %s", result);
            return SERVICE_RESULT_ERR;
        }

        strncpy(devinfo->dev_base.uuid, json_str,
                sizeof(devinfo->dev_base.uuid) > len ? len : sizeof(devinfo->dev_base.uuid));

        log_info("register device success, uuid:%s", devinfo->dev_base.uuid);

    }

    return ret;

}

static int router_login_device(dev_info_t *devinfo)
{
    int ret;
    char tx_buff[512] = {0};
    char result[128] = {0};

    alink_data_t data = {METHOD_DEVICE_ATTACH_SUB_WIFI, tx_buff};

    if (devinfo->dev_base.uuid[0] == '\0') {
        return SERVICE_RESULT_ERR;
    }

    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_ATTACH_SUB_WIFI_FMT,
                                    config_get_main_uuid(),
                                    devinfo->dev_base.uuid)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);

    ret = ((service_t *)sm_get_service("accs"))->get((void *)&data, sizeof(data),
                                                     result, sizeof(result));
    if (SERVICE_RESULT_OK == ret) {
        int len = 0;
        char *json_str = json_get_value_by_name(result, strlen(result), JSON_KEY_CODE,
                                                &len, NULL);
        if (NULL == json_str) {
            log_error("get rsp code fail, result data = %s", result);
            return SERVICE_RESULT_ERR;
        }

        log_info("attach wifi device result, code:%s", json_str);
    }

    return ret;

}

static int router_logout_device(dev_info_t *devinfo)
{
    int ret;
    char tx_buff[512] = {0};
    char result[128] = {0};

    alink_data_t data = {METHOD_DEVICE_DETACH_SUB_WIFI, tx_buff};

    if (devinfo->dev_base.uuid[0] == '\0') {
        return SERVICE_RESULT_ERR;
    }

    if (sizeof(tx_buff) == snprintf(tx_buff, sizeof(tx_buff),
                                    PARAMS_DEVICE_DETACH_SUB_WIFI_FMT,
                                    config_get_main_uuid(),
                                    devinfo->dev_base.uuid)) {
        return SERVICE_RESULT_ERR;
    }

    log_debug("send:%s", tx_buff);

    ret = ((service_t *)sm_get_service("accs"))->get((void *)&data, sizeof(data),
                                                     result, sizeof(result));
    if (SERVICE_RESULT_OK == ret) {
        int len = 0;
        char *json_str = json_get_value_by_name(result, strlen(result), JSON_KEY_CODE,
                                                &len, NULL);
        if (NULL == json_str) {
            log_error("get rsp code fail, result data = %s", result);
            return SERVICE_RESULT_ERR;
        }

        log_info("deattach wifi device result, code:%s", json_str);
    }

    return ret;

}

int router_network_down_event_handler(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_OK;

    switch (devinfo->cloud_state) {
        case DEVICE_STATE_LOGOUTED:
            //nothing todo
            break;
        case DEVICE_STATE_REGISTERED:
        case DEVICE_STATE_LOGINED:
            ret = router_logout_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                break;
            }
            devinfo->cloud_state = DEVICE_STATE_LOGOUTED;

            break;
        default:
            break;
    }

    return ret;
}


int router_network_up_event_handler(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_OK;

    switch (devinfo->cloud_state) {
        case DEVICE_STATE_LOGOUTED:
        case DEVICE_STATE_INITIAL:
            ret = router_register_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                goto ERR;
            }
            devinfo->cloud_state = DEVICE_STATE_REGISTERED;

        case DEVICE_STATE_REGISTERED:
            ret = router_login_device(devinfo);
            if (ret != SERVICE_RESULT_OK) {
                goto ERR;
            }
            devinfo->cloud_state = DEVICE_STATE_LOGINED;

            break;

        default:
            break;
    }

ERR:
    log_error("ret = %d.", ret);

    return ret;
}

int router_link_state_event_handler(dev_info_t *devinfo, link_state_t state)
{
    int ret = SERVICE_RESULT_ERR;

    devinfo->link_state = state;
    if (LINK_STATE_ONLINE == state) {
        ret = router_network_up_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_up_event_handler");
    } else if (LINK_STATE_OFFLINE == state) {
        ret = router_network_down_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_down_event_handler");
    } else {
        log_error("unknown link state, state:%d", state);
    }

    return ret;
}

static int router_network_event_cb(network_event_t event)
{
    //遍历设备，逐个注册、attatch
    int ret = SERVICE_RESULT_OK;
    dev_info_t *pos;
    dlist_t *next = NULL;

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry_safe(&dev_head, next, pos, dev_info_t, list_node) {
        if (pos->dev_base.dev_type != DEV_TYPE_WIFI) {
            continue;
        }

        switch (event) {
            case NETWORK_EVENT_DOWN: //network disconnected
                ret = router_network_down_event_handler(pos);
                break;
            case NETWORK_EVENT_UP: //network connected
                ret = router_network_up_event_handler(pos);
                break;
            default:
                break;
        }
    }
    os_mutex_unlock(devlist_lock);

    return ret;
}

static int devmgr_listener(int type, void *data, int dlen, void *result,
                           int *rlen)
{
    if (type == SERVICE_EVENT) {
        int st = *((int *)data);
        log_trace("DEVMGR recv %s, %s", sm_code2string(type), sm_code2string(st));
        if (st == SERVICE_STATE_READY) {
            router_network_event_cb(NETWORK_EVENT_UP);
        } else if (st == SERVICE_STATE_STOP || st == SERVICE_STATE_INIT) {
            router_network_event_cb(NETWORK_EVENT_DOWN);
        }
    }

    return EVENT_IGNORE;
}

int devmgr_router_init()
{
    log_trace("devmgr_router init success.");
    sm_attach_service("accs", &devmgr_listener);
    return 0;
}

void devmgr_router_exit()
{
    sm_detach_service("accs", &devmgr_listener);
    log_trace("devmgr_router exit.");
}

