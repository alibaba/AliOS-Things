/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _BLECFG_PUB_H_
#define _BLECFG_PUB_H_

#include <stdio.h>
#include <stdint.h>

#include "ulog/ulog.h"
#include "aos/ble.h"
#include "atomic.h"
#include "bluetooth/bluetooth.h"
#include "bluetooth/gatt.h"
#include "bluetooth/uuid.h"
#include "netmgr.h"
#include "netmgr_wifi.h"
#include "ble_netconfig.h"
#include "uservice/eventid.h"
#include "vfsdev/wifi_dev.h"

/* 日志系统 */
#define BLECFG_TAG "BLECFG"

#define BLECFG_LOG_ERROR(fmt, ...)           LOGE(BLECFG_TAG, fmt, ##__VA_ARGS__)
#define BLECFG_LOG_WARNING(fmt, ...)         LOGW(BLECFG_TAG, fmt, ##__VA_ARGS__)
#define BLECFG_LOG_INFO(fmt, ...)            LOGI(BLECFG_TAG, fmt, ##__VA_ARGS__)
#define BLECFG_LOG_DEBUG(fmt, ...)           LOGD(BLECFG_TAG, fmt, ##__VA_ARGS__)

/* 返回给miniapp的消息类型 */
typedef enum {
    BLECFG_DHCP_SUCCESS = 0,
    BLECFG_DHCP_FAIL,
    BLECFG_PING_SUCCESS,
    BLECFG_PING_FAIL,
} BLENET_RESPONSE;

/* 手机侧APP小程序下发命令的回调函数 */
typedef void (*BleCfg_msg_cb)(uint8_t *buf, int32_t len);

/* wifi链接热点成功时的回调函数 */
typedef void (*BleCfg_dhcp_cb)();

/* 链接linksdk成功时的回调函数 */
typedef void (*BleCfg_link_cb)();

BLECFG_STATE BleCfg_stack_init(BleCfg_msg_cb msg_callback);
BLECFG_STATE BleCfg_stack_start();
BLECFG_STATE BleCfg_stack_stop();
BLECFG_STATE BleCfg_stack_disconnect();
BLECFG_STATE BleCfg_notificate(const uint8_t *data, uint16_t size);

BLECFG_STATE BleCfg_wifi_set(char *ssid, char *passwd);
BLECFG_STATE BleCfg_wifi_connect();
BLECFG_STATE BleCfg_wifi_save();
BLECFG_STATE BleCfg_wifi_restore();
BLECFG_STATE BleCfg_netmgr_init(BleCfg_dhcp_cb dhcp_callback);

BLECFG_STATE BleCfg_dev_set(char *pk, char *dn, char *ds);
BLECFG_STATE BleCfg_dev_connect();
BLECFG_STATE BleCfg_dev_save();
BLECFG_STATE BleCfg_dev_restore();
BLECFG_STATE BleCfg_linksdk_init(BleCfg_link_cb linksdk_callback);

#endif
