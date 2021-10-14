/** @file
 *  @brief Bluetooth Mesh Configuration Server Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_CTRL_RELAY_H
#define __BT_MESH_CTRL_RELAY_H

#include <net/buf.h>
#include "net.h"

#define CTRL_RELAY_N_MIN 3
#define CTRL_RELAY_N_MAX 7

#define CTRL_RELAY_N_DEF 5
#define CTRL_RELAY_RSSI_DEF 90
#define CTRL_RELAY_STA_PERIOD_DEF 60
#define CTRL_RELAY_CHK_PERIOD_DEF 70
#define CTRL_RELAY_KEEP_PERIOD 10

#define CTRL_RELAY_CACHE_TIMEOUT 70
#define CTRL_RELAY_CACHE_EXPIRE 90

#define CTRL_RELAY_CHK_PROTECT 120

#define CTRL_RELAY_CACHE_LIMIT 8
#define CTRL_RELAY_CACHE_SIZE 10

#define RSSI_VAL(n) (((n) < 0) ? -(n) : (n))

typedef enum {
    CTRL_RELAY_TYPE_STATUS = 0,
    CTRL_RELAY_TYPE_REQ,
    CTRL_RELAY_TYPE_OPEN,
    CTRL_RELAY_TYPE_NONE
} ctrl_relay_send_type;

typedef enum {
    CTRL_RELAY_STATUS_PUB = 0,
    CTRL_RELAY_STATUS_CHK_NOTIFY,
    CTRL_RELAY_STATUS_FD_NOTIFY,
    CTRL_RELAY_STATUS_FD_DISABLE,
    CTRL_RELAY_STATUS_REQ,
    CTRL_RELAY_STATUS_OPEN,
    CTRL_RELAY_STATUS_NONE
} ctrl_relay_status_sub_type;

struct ctrl_relay_cache {
    bt_u32_t tmms;
    u16_t addr;
    u16_t nb_addr[CTRL_RELAY_CACHE_SIZE];
} __packed;

struct ctrl_relay_status {
    bt_u32_t tmms;
    bt_u32_t flood_ms;
    u8_t flood_flg;
    u8_t enable;
    u8_t nb_relay_cnt;
} __packed;

struct ctrl_relay_param {
    u8_t enable;
    u8_t trd_n;
    u8_t rssi;
    u8_t sta_period;
    u8_t chk_period;
    u8_t req_period;
} __packed;

struct ctrl_relay_cfg {
    struct ctrl_relay_param param;
    struct ctrl_relay_status curr_stat;
    struct bt_mesh_model *model;
    struct k_delayed_work sta_work;
    struct k_delayed_work chk_work;
    struct k_delayed_work keep_work;
};

int ctrl_relay_init(struct bt_mesh_model *model);
int ctrl_relay_deinit(void);
void ctrl_relay_conf_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf);
void ctrl_relay_conf_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf);
int ctrl_relay_msg_recv(u8_t ctl_op, struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);
void ctrl_relay_set_flood_flg(void);

#endif /* __BT_MESH_CTRL_RELAY_H */
