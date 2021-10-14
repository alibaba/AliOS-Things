/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _LIGHT_LIGHTNESS_CLI_H_
#define _LIGHT_LIGHTNESS_CLI_H_

#define LIGHT_LIGHTNESS_CLI_OPC_NUM 6

#define MESH_MODEL_LIGHTNESS_CLI(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, \
        g_light_lightness_cli_op, &g_light_lightness_cli_pub, _user_data)

#define MESH_MODEL_LIGHTNESS_CLI_NULL()  MESH_MODEL_LIGHTNESS_CLI(NULL)

typedef struct _set_lightness_arg {
    uint16_t lightness;
    uint16_t lightness_linear;
    uint16_t def;
    uint16_t range_min;
    uint16_t range_max;
    uint8_t tid;
    uint8_t trans;
    uint8_t delay;
    uint8_t send_trans: 1;
} set_lightness_arg;

extern const struct bt_mesh_model_op g_light_lightness_cli_op[LIGHT_LIGHTNESS_CLI_OPC_NUM];
extern struct bt_mesh_model_pub g_light_lightness_cli_pub;

int ble_mesh_light_lightness_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);
int ble_mesh_light_lightness_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack);
int ble_mesh_light_lightness_linear_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);
int ble_mesh_light_lightness_linear_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack);
int ble_mesh_light_lightness_def_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);
int ble_mesh_light_lightness_def_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack);
int ble_mesh_light_lightness_range_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);
int ble_mesh_light_lightness_range_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack);
int ble_mesh_light_lightness_last_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

#endif //
