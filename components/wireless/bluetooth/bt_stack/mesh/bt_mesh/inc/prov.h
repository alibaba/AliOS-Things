/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __PROV_H
#define __PROV_H

#include <mesh_def.h>

struct prov_send_invite_t {
    u8_t uuid[16];
    u32_t size;
    s32_t attentionDuration;
};

struct prov_send_start_t {
    u32_t algorithms;
    s32_t public_key_oob;
    s32_t auth_method;
    s32_t auth_action;
    s32_t auth_size;
    u8_t netkey[16];
    s32_t netkey_index;
    s32_t iv_index;
    s32_t addr;
    s32_t flags;
    s32_t mode;
};

struct prov_send_confirm_t {
    u8_t confirm[16];
    u32_t size;
};

struct prov_auth_result_t {
    u8_t result[4];
    u32_t size;
};

typedef void (*bt_mesh_prov_eventCbk)(BT_MESH_EVENT_T *event);

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf);

bool bt_prov_active(void);

int bt_mesh_pb_gatt_open(bt_mesh_conn_t conn);
int bt_mesh_pb_gatt_close(bt_mesh_conn_t conn);
int bt_mesh_pb_gatt_recv(bt_mesh_conn_t conn, struct net_buf_simple *buf);

const u8_t *bt_mesh_prov_get_uuid(void);

int bt_mesh_prov_scan(u8_t start, s32_t duration);
int bt_mesh_prov_init(const struct bt_mesh_prov *prov);

void bt_mesh_prov_complete(u16_t net_idx, u16_t addr);
void bt_mesh_prov_reset(void);


u8_t bt_mesh_add_node_device_key(int addr, const u8_t *deviceKey, u8_t keyLength, 
                                   u8_t *deviceUUID, int uuidLength);
int bt_mesh_prov_register_callback(void *cb);
int provisioner_send_invite(u8_t* uuid, u32_t size, s32_t attentionDuration) ;
int provisioner_send_start(u32_t algorithms, s32_t public_key_oob, s32_t auth_method, s32_t auth_action, s32_t auth_size,
                           u8_t* netkey, s32_t netkey_index, s32_t iv_index, s32_t addr, s32_t flags, s32_t mode);

int provisioner_send_confirm(u8_t *confirm, u32_t size);
int provisioner_set_auth_result (u8_t *result, u32_t size);

#endif