/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _LIGHT_LIGHTNESS_SRV_H_
#define _LIGHT_LIGHTNESS_SRV_H_

#define LIGHT_LIGHTNESS_OPC_NUM 14

#define MESH_MODEL_LIGHTNESS_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, \
        g_light_lightness_op, &g_light_lightness_pub, _user_data)

#define MESH_MODEL_LIGHTNESS_SRV_NULL() MESH_MODEL_LIGHTNESS_SRV(NULL)

extern struct bt_mesh_model_pub g_light_lightness_pub;
extern const struct bt_mesh_model_op g_light_lightness_op[LIGHT_LIGHTNESS_OPC_NUM];


int ble_mesh_light_lightness_publication(struct bt_mesh_model *model);


#endif //
