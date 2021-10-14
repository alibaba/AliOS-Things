/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BEACON_TYPE_UNPROVISIONED  0x00
#define BEACON_TYPE_SECURE         0x01

void bt_mesh_beacon_enable(void);
void bt_mesh_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(struct net_buf_simple *buf);

void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
			   struct net_buf_simple *buf);

void bt_mesh_beacon_init(void);

bt_u32_t bt_mesh_get_beacon_interval(u8_t type);

void bt_mesh_set_beacon_interval(u8_t type, bt_u32_t delay);

void genie_set_silent_unprov_beacon_interval(bool is_silent);
