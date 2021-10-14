/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VENDOR_MODEL_H
#define _VENDOR_MODEL_H

#define CONFIG_CID_TAOBAO 0x01A8
#define CONFIG_MESH_VENDOR_COMPANY_ID CONFIG_CID_TAOBAO

#define BT_MESH_MODEL_VND_COMPANY_ID 0x01A8
#define BT_MESH_MODEL_VND_MODEL_SRV 0x0000
#define BT_MESH_MODEL_VND_MODEL_CLI 0x0001


#define VENDOR_OP_ATTR_GET_STATUS        0xD0
#define VENDOR_OP_ATTR_SET_ACK           0xD1
#define VENDOR_OP_ATTR_SET_UNACK         0xD2
#define VENDOR_OP_ATTR_STATUS            0xD3
#define VENDOR_OP_ATTR_TRANS_MSG         0xCF
#define VENDOR_OP_ATTR_TRANS_INDICATE    0xCE
#define VENDOR_OP_ATTR_TRANS_ACK         0xCD
#define VENDOR_OP_ATTR_MESH_AUTOCONFIG   0xD6
#define VENDOR_OP_ATTR_MESH_AUTOCONFIG_GET   0xD7
#define VENDOR_OP_ATTR_MESH_AUTOCONFIG_STATUS 0xD8

#define VENDOR_OP_ATTR_INDICATE          0xD4
#define VENDOR_OP_ATTR_CONFIRM           0xD5
#define VENDOR_OP_ATTR_INDICATE_TG       0xDE
#define VENDOR_OP_ATTR_CONFIRM_TG        0xDF


#define ONOFF_T           0x0100
#define LIGHTNESS_LEVEL_T 0x0121

#define VENDOR_MODEL_MSG_DFT_RETRY_TIMES 5
#define VENDOR_MODEL_MSG_MAX_RETRY_TIMES 10
#define VENDOR_MODEL_MSG_RETRY_PERIOD 500

/**
 * p_elem: pointer to the element which the messsage want to be sent to
 * retry: retry counts before desired confirm message received
 * * fill negative value if retransmission is not necessary
 * * fill positive value if retransmission is needed
 * * * will be round in this scope - [VENDOR_MODEL_MSG_DFT_RETRY_TIMES, VENDOR_MODEL_MSG_MAX_RETRY_TIMES]
 * retry_period: wait for retry_period before retransmit the message, in unit of ms
 * opid: hightest byte in Opcode defined in the vendor model spec designed by Alibaba IoT Group
 * * e.g. for Vendor Message Attr Get message, Opcode is 0xD001A8, corresponding opid is 0xD0
 * * refer to the marco named VENDOR_OP_ATTR_xxxx
 * tid:
 * * if the message is with type of VENDOR_OP_ATTR_CONFIRM or VENDOR_OP_ATTR_CONFIRM_TG,
 * * tid should be filled with the replied message's tid
 * len: payload length
 * data: pointer to the vendor message's payload
 * */
typedef struct _vnd_model_msg {
    struct bt_mesh_model *model;
    uint16_t netkey_idx;
    uint16_t appkey_idx;
    uint16_t dst_addr;
    uint16_t len;
    uint8_t  retry;
    uint8_t  retry_period;
    uint8_t  opid;
    uint8_t  tid;
    uint8_t  *data;
} vnd_model_msg;

typedef struct _vnd_model_msg_n {
    sys_dnode_t node;
    s8_t left_retry;
    long long timeout;
    vnd_model_msg msg;
} vnd_model_msg_n;


#endif //_VENDOR_MODEL_H
