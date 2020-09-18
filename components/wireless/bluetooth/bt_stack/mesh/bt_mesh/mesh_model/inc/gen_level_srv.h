/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _GEN_LEVEL_SRV_H_
#define _GEN_LEVEL_SRV_H_

#define GEN_LV_OPC_NUM 8

#define MESH_MODEL_GEN_LEVEL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, \
                                                g_gen_level_op, &g_gen_level_pub, _user_data)

extern struct bt_mesh_model_pub g_gen_level_pub;
extern const struct bt_mesh_model_op g_gen_level_op[GEN_LV_OPC_NUM];

void gen_level_publication(struct bt_mesh_model *model);
void bound_level_by_onoff(S_ELEM_STATE *elem);

#endif // _GEN_LEVEL_SRV_H_
