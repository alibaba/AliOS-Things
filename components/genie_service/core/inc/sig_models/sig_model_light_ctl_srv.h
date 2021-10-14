/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SIG_MODEL_LIGHT_CTL_SRV_H__
#define __SIG_MODEL_LIGHT_CTL_SRV_H__

#define COLOR_TEMPERATURE_MIN 800
#define COLOR_TEMPERATURE_MAX 20000
#define COLOR_TEMPERATURE_DEFAULT COLOR_TEMPERATURE_MAX

#define LIGHT_CTL_OPCODE_NUM 4

#define MESH_MODEL_CTL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, \
                                                     g_ctl_srv_op, &g_ctl_srv_pub, _user_data)

extern struct bt_mesh_model_pub g_ctl_srv_pub;
extern const struct bt_mesh_model_op g_ctl_srv_op[LIGHT_CTL_OPCODE_NUM];

void sig_model_light_ctl_update_by_onoff(sig_model_element_state_t *p_elem);

#endif
