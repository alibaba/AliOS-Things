/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "net.h"
#include "provisioner_main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_SETTINGS_ROLE_NONE     0
#define BLE_MESH_SETTINGS_ROLE_NODE     (BIT(BLE_MESH_NODE))
#define BLE_MESH_SETTINGS_ROLE_PROV     (BIT(BLE_MESH_PROVISIONER))
#define BLE_MESH_SETTINGS_ROLE_BIT_MASK (BIT(BLE_MESH_NODE) | BIT(BLE_MESH_PROVISIONER))

void bt_mesh_store_role(void);
void bt_mesh_store_net(void);
void bt_mesh_store_iv(bool only_duration);
void bt_mesh_clear_iv(void);
void bt_mesh_store_seq(void);
void bt_mesh_clear_seq(void);
void bt_mesh_store_rpl(struct bt_mesh_rpl *rpl);
void bt_mesh_store_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_store_app_key(struct bt_mesh_app_key *key);
void bt_mesh_store_hb_pub(void);
void bt_mesh_store_cfg(void);
void bt_mesh_store_mod_bind(struct bt_mesh_model *mod);
void bt_mesh_store_mod_sub(struct bt_mesh_model *mod);
void bt_mesh_store_mod_pub(struct bt_mesh_model *mod);
void bt_mesh_store_label(void);

void bt_mesh_clear_role(void);
void bt_mesh_clear_net(void);
void bt_mesh_clear_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_clear_app_key(struct bt_mesh_app_key *key);
void bt_mesh_clear_rpl(void);

#if CONFIG_BLE_MESH_PROVISIONER
void bt_mesh_store_prov_info(u16_t primary_addr, u16_t alloc_addr);
void bt_mesh_clear_prov_info(void);
void bt_mesh_store_p_net_idx(void);
void bt_mesh_clear_p_net_idx(void);
void bt_mesh_store_p_app_idx(void);
void bt_mesh_clear_p_app_idx(void);
void bt_mesh_store_p_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_store_p_app_key(struct bt_mesh_app_key *key);
void bt_mesh_clear_p_subnet(u16_t net_idx);
void bt_mesh_clear_p_app_key(u16_t app_idx);
void bt_mesh_clear_rpl_single(u16_t src);
void bt_mesh_store_node_info(struct bt_mesh_node *node);
void bt_mesh_clear_node_info(u16_t unicast_addr);
void bt_mesh_store_node_name(struct bt_mesh_node *node);
void bt_mesh_store_node_comp_data(struct bt_mesh_node *node);
#endif

void bt_mesh_settings_lock(void);
void bt_mesh_settings_unlock(void);

int settings_core_init(void);
int settings_core_load(void);
int settings_core_commit(void);
int settings_core_deinit(void);
int settings_core_erase(void);

int bt_mesh_settings_init(void);
int bt_mesh_settings_deinit(bool erase);

#ifdef __cplusplus
}
#endif

#endif /* _SETTINGS_H_ */
