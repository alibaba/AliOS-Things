/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _GENERIC_ONOFF_CLI_H_
#define _GENERIC_ONOFF_CLI_H_

#define GEN_ONOFF_CLI_OPC_NUM 2

#define MESH_MODEL_GEN_ONOFF_CLI(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI,g_generic_onoff_cli_op, \
        &g_generic_onoff_cli_pub,_user_data)

#define MESH_MODEL_GEN_ONOFF_CLI_NULL()   MESH_MODEL_GEN_ONOFF_CLI(NULL)

typedef struct _set_onoff_arg {
    uint8_t onoff;
    uint8_t tid;
    uint8_t send_trans;
    uint8_t trans;
    uint8_t delay;
} set_onoff_arg;

extern const struct bt_mesh_model_op g_generic_onoff_cli_op[GEN_ONOFF_CLI_OPC_NUM];
extern struct bt_mesh_model_pub g_generic_onoff_cli_pub;

int ble_mesh_generic_onoff_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);
int ble_mesh_generic_onoff_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack);
int ble_mesh_generic_onoff_cli_publish(struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack);

#endif
