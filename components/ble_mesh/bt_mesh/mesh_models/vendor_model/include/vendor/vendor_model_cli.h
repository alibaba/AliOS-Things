/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VENDOR_MODEL_CLI_H_
#define _VENDOR_MODEL_CLI_H_

#include "vendor/vendor_model.h"


#define VENDOR_CLI_MODEL_OPC_NUM 6
#define MESH_MODEL_VENDOR_CLI(_user_data) BT_MESH_MODEL_VND(BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_CLI, \
        g_vendor_cli_model_alibaba_op, &g_vendor_cli_model_alibaba_pub, _user_data)


#define MESH_MODEL_VENDOR_CLI_NULL()  MESH_MODEL_VENDOR_CLI(NULL)

extern struct bt_mesh_model_pub g_vendor_cli_model_alibaba_pub;
extern const struct bt_mesh_model_op g_vendor_cli_model_alibaba_op[VENDOR_CLI_MODEL_OPC_NUM];

int ble_mesh_vendor_cli_model_msg_send(vnd_model_msg *model_msg);



#endif //_VENDOR_MODEL_SRV_H

