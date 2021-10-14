/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _GENERIC_LEVEL_CLI_H_
#define _GENERIC_LEVEL_CLI_H_

#define GEN_LEVEL_CLI_OPC_NUM 2
#define MESH_MODEL_GEN_LEVEL_CLI(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_CLI, \
        g_generic_level_cli_op, &g_generic_level_cli_pub,_user_data)

#define MESH_MODEL_GEN_LEVEL_CLI_NULL()  MESH_MODEL_GEN_LEVEL_CLI(NULL)


typedef struct _set_level_arg {
    uint16_t level;
    uint16_t def;
    uint16_t move;
    uint8_t tid;
    uint8_t trans;
    uint8_t delay;
    uint8_t send_trans: 1;
    bt_s32_t delta;
} set_level_arg;

int ble_mesh_generic_level_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model);

int ble_mesh_generic_level_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack);

int ble_mesh_generic_level_move_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack);

int ble_mesh_generic_level_delta_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack);

extern const struct bt_mesh_model_op g_generic_level_cli_op[GEN_LEVEL_CLI_OPC_NUM];
extern struct bt_mesh_model_pub g_generic_level_cli_pub;


#endif
