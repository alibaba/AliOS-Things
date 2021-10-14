/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _MESH_EVENT_H_
#define _MESH_EVENT_H_

typedef enum {
    BT_MESH_MODEL_EVT_PROV_START = 0x00,
    BT_MESH_MODEL_EVT_PROV_DATA,

    BT_MESH_MODEL_EVT_APPKEY_ADD,
    BT_MESH_MODEL_EVT_APPKEY_UPDATE,
    BT_MESH_MODEL_EVT_APPKEY_DEL,
    BT_MESH_MODEL_EVT_APPKEY_BIND_STATUS,
    BT_MESH_MODEL_EVT_APPKEY_STATUS,

    BT_MESH_MODEL_EVT_NETKEY_ADD,
    BT_MESH_MODEL_EVT_NETKEY_UPDATE,
    BT_MESH_MODEL_EVT_NETKEY_DEL,
    BT_MESH_MODEL_EVT_NETKEY_STATUS,
    BT_MESH_MODEL_EVT_NET_KRP_STATUS,

    BT_MESH_MODEL_EVT_SEQ_UPDATE,

    BT_MESH_MODEL_EVT_COMP_DATA_STATUS,
    BT_MESH_MODEL_EVT_BEACON_STATUS,
    BT_MESH_MODEL_EVT_TTL_STATUS,
    BT_MESH_MODEL_EVT_FRIEND_STATUS,
    BT_MESH_MODEL_EVT_PROXY_STATUS,
    BT_MESH_MODEL_EVT_RELAY_STATUS,

    BT_MESH_MODEL_EVT_PUB_STATUS,
    BT_MESH_MODEL_EVT_SUB_STATUS,

    BT_MESH_MODEL_EVT_SUB_ADD,
    BT_MESH_MODEL_EVT_SUB_DEL,
    BT_MESH_MODEL_EVT_SUB_LIST,
    BT_MESH_MODEL_EVT_SUB_LIST_VND,

    BT_MESH_MODEL_EVT_HB_SUB_STATUS,
    BT_MESH_MODEL_EVT_HB_PUB_STATUS,
/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    BT_MESH_MODEL_EVT_CTRL_RELAY_STATUS,
#endif
/*[Genie end] add by wenbing.cwb at 2021-01-21*/
    BT_MESH_MODEL_EVT_NODE_RESET_OP,
    BT_MESH_MODEL_EVT_NODE_RESET_STATUS,

    BT_MESH_MODEL_EVT_RPL_IS_FULL,
} mesh_model_event_e;

typedef struct {
    uint16_t source_addr;
    void *user_data;
} bt_mesh_model_evt_t;

typedef void (*mesh_model_evt_cb)(mesh_model_event_e event, void *p_arg);

int bt_mesh_event_init(void);

int bt_mesh_event_register(mesh_model_evt_cb event_cb);

mesh_model_evt_cb bt_mesh_event_get_cb_func(void);


#endif /* _GENIE_MESH_API_H_ */
