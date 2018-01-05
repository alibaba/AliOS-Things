/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_HELPER_INTERNAL_H_
#define _BLE_HELPER_INTERNAL_H_

#include "ble_app_framework.h"
#include <stdio.h>
#include "sdpdefs.h"
#include "mico_bt_cfg.h"
#include "mico_bt.h"
#include "mico_bt_peripheral.h"

typedef struct peripheral_socket_s {
    peripheral_hdl_t hdl;
    mico_bt_peripheral_socket_t skt;
    struct peripheral_socket_s *next;
} peripheral_socket_t;

extern peripheral_socket_t *g_skt_head;

#endif
