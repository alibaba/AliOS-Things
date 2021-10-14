/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _ALI_MODEL_LIGHT_CTL_SRV_H_
#define _ALI_MODEL_LIGHT_CTL_SRV_H_

#define CTL_TEMP_MIN 800
#define CTL_TEMP_MAX 20000
#define CTL_TEMP_DEFAULT CTL_TEMP_MAX


#define CTL_OPC_NUM 6
#define CTL_SETUP_OPC_NUM 5
#define CTL_TEMP_OPC_NUM 4


#define MESH_MODEL_CTL_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, \
        g_ctl_srv_op, &g_ctl_srv_pub, _user_data)


#define MESH_MODEL_CTL_SETUP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, \
        g_ctl_setup_srv_op, NULL, _user_data)


#define MESH_MODEL_CTL_TEMP_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV, \
        g_ctl_temp_srv_op, &g_ctl_srv_pub, _user_data)


#define MESH_MODEL_CTL_SRV_NULL()  MESH_MODEL_CTL_SRV(NULL)
#define MESH_MODEL_CTL_SETUP_SRV_NULL() MESH_MODEL_CTL_SETUP_SRV(NULL)
#define MESH_MODEL_CTL_TEMP_SRV_NULL() MESH_MODEL_CTL_TEMP_SRV(NULL)

extern struct bt_mesh_model_pub g_ctl_srv_pub;
extern const struct bt_mesh_model_op g_ctl_srv_op[CTL_OPC_NUM];
extern const struct bt_mesh_model_op g_ctl_setup_srv_op[CTL_SETUP_OPC_NUM];
extern const struct bt_mesh_model_op g_ctl_temp_srv_op[CTL_TEMP_OPC_NUM];


int ble_mesh_light_ctl_publication(struct bt_mesh_model *model);


#endif //
