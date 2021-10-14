/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _LIGHT_CTL_CLI_H_
#define _LIGHT_CTL_CLI_H_


#define LIGHT_CTL_CLI_OPC_NUM     5

#define MESH_MODEL_LIGHT_CTL_CLI() BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_CLI, \
        light_ctl_cli_op, &g_ctl_cli_pub, NULL)

#define MESH_MODEL_LIGHT_CTL_CLI_NULL() MESH_MODEL_LIGHT_CTL_CLI()

extern const struct bt_mesh_model_op light_ctl_cli_op[LIGHT_CTL_CLI_OPC_NUM];
extern struct bt_mesh_model_pub g_ctl_cli_pub;

typedef struct _set_light_ctl_arg {
    uint16_t lightness;
    uint16_t temperature;
    uint16_t delta_uv;
    uint16_t range_min;
    uint16_t range_max;
    uint8_t tid;
    uint8_t trans;
    uint8_t delay;
    uint8_t send_trans: 1;
} set_light_ctl_arg;

int ble_mesh_light_ctl_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

int ble_mesh_light_ctl_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_light_ctl_arg *send_arg, bool ack);

int ble_mesh_light_ctl_temp_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

int ble_mesh_light_ctl_temp_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_light_ctl_arg *send_arg, bool ack);

int ble_mesh_light_ctl_def_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

int ble_mesh_light_ctl_def_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack);

int ble_mesh_light_ctl_temp_range_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

int ble_mesh_light_ctl_temp_range_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack);

#endif // MODEL_LIGHT_CTL_CLI_H_


