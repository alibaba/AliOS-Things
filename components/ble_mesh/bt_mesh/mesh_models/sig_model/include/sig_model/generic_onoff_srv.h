/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _MODEL_GEN_ONOFF_SRV_H_
#define _MODEL_GEN_ONOFF_SRV_H_

#define GEN_ONOFF_OPC_NUM 4

#define MESH_MODEL_GEN_ONOFF_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, \
        g_generic_onoff_op, &g_generic_onoff_pub, _user_data)

#define MESH_MODEL_GEN_ONOFF_SRV_NULL()  MESH_MODEL_GEN_ONOFF_SRV(NULL)

extern struct bt_mesh_model_pub g_generic_onoff_pub;
extern const struct bt_mesh_model_op g_generic_onoff_op[GEN_ONOFF_OPC_NUM];

int ble_mesh_generic_onoff_publication(struct bt_mesh_model *model);

#endif // _MODEL_GEN_ONOFF_SRV_H_
