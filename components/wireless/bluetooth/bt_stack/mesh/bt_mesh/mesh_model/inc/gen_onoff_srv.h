/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ALI_MODEL_GEN_ONOFF_SRV_H_
#define _ALI_MODEL_GEN_ONOFF_SRV_H_

#define GEN_ONOFF_OPC_NUM 4

#define MESH_MODEL_GEN_ONOFF_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, \
                                                g_gen_onoff_op, &g_gen_onoff_pub, _user_data)

extern struct bt_mesh_model_pub g_gen_onoff_pub;
extern const struct bt_mesh_model_op g_gen_onoff_op[GEN_ONOFF_OPC_NUM];

void gen_onoff_publication(struct bt_mesh_model *model);

#endif // _ALI_MODEL_GEN_ONOFF_SRV_H_