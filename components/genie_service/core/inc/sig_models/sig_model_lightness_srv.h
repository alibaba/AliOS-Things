/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SIG_MODEL_LIGHTNESS_SRV_H__
#define __SIG_MODEL_LIGHTNESS_SRV_H__

#define LIGHTNESS_OPC_NUM 4
#define LIGHTNESS_DEFAULT 0xE666   //90%
#define LIGHTNESS_MAX_VALUE 0xFFFF //100%

#define MESH_MODEL_LIGHTNESS_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, \
                                                           g_lightness_op, &g_lightness_pub, _user_data)

extern struct bt_mesh_model_pub g_lightness_pub;
extern const struct bt_mesh_model_op g_lightness_op[LIGHTNESS_OPC_NUM];

void sig_model_lightness_update_by_onoff(sig_model_element_state_t *p_elem);

#endif
