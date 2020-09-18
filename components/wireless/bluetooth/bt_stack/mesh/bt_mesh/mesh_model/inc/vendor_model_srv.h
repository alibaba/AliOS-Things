//
// Created by ethan on 2019/7/15.
//

#ifndef _VENDOR_MODEL_SRV_H
#define _VENDOR_MODEL_SRV_H

#define BT_MESH_MODEL_VND_COMPANY_ID 0x01A8
#define BT_MESH_MODEL_VND_MODEL_SRV 0x0000
#define BT_MESH_MODEL_VND_MODEL_CLI 0x0001

#define VENDOR_MODEL_OPC_NUM 8
#define MESH_MODEL_VENDOR_SRV(_user_data) BT_MESH_MODEL_VND(BT_MESH_MODEL_VND_COMPANY_ID, BT_MESH_MODEL_VND_MODEL_SRV, \
                                                g_vendor_model_alibaba_op, &g_vendor_model_alibaba_pub, _user_data)

extern struct bt_mesh_model_pub g_vendor_model_alibaba_pub;
extern const struct bt_mesh_model_op g_vendor_model_alibaba_op[VENDOR_MODEL_OPC_NUM];

typedef struct _vnd_model_msg_n {
    sys_dnode_t node;
    s8_t left_retry;
    long long timeout;
    vnd_model_msg msg;
} vnd_model_msg_n;

#endif //_VENDOR_MODEL_SRV_H
