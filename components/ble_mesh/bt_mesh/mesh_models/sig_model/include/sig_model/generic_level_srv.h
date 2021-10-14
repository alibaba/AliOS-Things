/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _GENERIC_LEVEL_SRV_H_
#define _GENERIC_LEVEL_SRV_H_

#define GEN_LV_OPC_NUM 8


#define MESH_MODEL_GEN_LEVEL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, \
        g_generic_level_op, &g_generic_level_pub, _user_data)

#define MESH_MODEL_GEN_LEVEL_SRV_NULL() MESH_MODEL_GEN_LEVEL_SRV(NULL)

extern struct bt_mesh_model_pub g_generic_level_pub;
extern const struct bt_mesh_model_op g_generic_level_op[GEN_LV_OPC_NUM];

int ble_mesh_generic_level_publication(struct bt_mesh_model *model);


#endif //
