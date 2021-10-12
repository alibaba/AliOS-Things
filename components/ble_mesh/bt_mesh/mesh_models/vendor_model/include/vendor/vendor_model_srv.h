/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VENDOR_MODEL_SRV_H
#define _VENDOR_MODEL_SRV_H

#include "vendor_model.h"

#define VENDOR_SRV_MODEL_OPC_NUM 7
#define MESH_MODEL_VENDOR_SRV(_user_data) BT_MESH_MODEL_VND(BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV, \
        g_vendor_srv_model_alibaba_op, &g_vendor_srv_model_alibaba_pub, _user_data)


#define MESH_MODEL_VENDOR_SRV_NULL()  MESH_MODEL_VENDOR_SRV(NULL)


extern struct bt_mesh_model_pub g_vendor_srv_model_alibaba_pub;
extern const struct bt_mesh_model_op g_vendor_srv_model_alibaba_op[VENDOR_SRV_MODEL_OPC_NUM];

int ble_mesh_vendor_srv_model_msg_send(vnd_model_msg *model_msg);

#endif //_VENDOR_MODEL_SRV_H
