/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _BLE_NETCONFIG_H_
#define _BLE_NETCONFIG_H_

typedef enum {
    BLECFG_EVENT_NONE,
    BLECFG_EVENT_STARTED,
    BLECFG_EVENT_STOPPED,
    BLECFG_EVENT_CONNECTED,
    BLECFG_EVENT_SUCCESS,
    BLECFG_EVENT_FAILED,
}BLECFG_EVENT;

typedef enum {
    BLECFG_SUCCESS,
    BLECFG_COMMON_FAILED,
    BLECFG_ALREADY,
    BLECFG_UNKNOW,
    BLECFG_WRONG_INFO,
}BLECFG_STATE;

BLECFG_STATE BleCfg_run();
BLECFG_STATE BleCfg_recovery_wifi();
BLECFG_STATE BleCfg_recovery_devinfo();

#endif
