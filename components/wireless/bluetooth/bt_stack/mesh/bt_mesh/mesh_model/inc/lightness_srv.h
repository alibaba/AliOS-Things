/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ALI_MODEL_LIGHTNESS_SRV__H_
#define _ALI_MODEL_LIGHTNESS_SRV_H_

#define LIGHTNESS_OPC_NUM 14

#define MESH_MODEL_LIGHTNESS_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, \
                                                g_lightness_op, &g_lightness_pub, _user_data)

extern struct bt_mesh_model_pub g_lightness_pub;
extern const struct bt_mesh_model_op g_lightness_op[LIGHTNESS_OPC_NUM];

void lightness_publication(struct bt_mesh_model *p_model);

#endif // _ALI_MODEL_LIGHTNESS_SRV_H_
