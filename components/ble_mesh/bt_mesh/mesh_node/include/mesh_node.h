/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __MESH_NODE
#define __MESH_NODE

#include <stdio.h>
#include <mesh_model/mesh_model.h>
#include "mesh_provisioner.h"

#define DEVICE_NAME_MAX_LENGTH 28
#define RSSI_BEACON_REPORT_PERIOD 1000 //ms
#define DEF_ADD_APPKEY_DELAY (300)
#define DEF_ADD_APPKEY_TIMEOUT (5 * 1000)
#define DEF_AUTOCONFIG_STORE_CHECK_TIMEOUT (1 * 1000)
#define DEF_AUTOCONFIG_STORE_CHECK_RETRY 40
#define DEF_ADD_APPKEY_RETRY 10

typedef struct {
    uint16_t prim_unicast;
    uint8_t  element_num;
} mesh_node_local_t;


typedef enum {
    BT_MESH_EVENT_NODE_REST,
    BT_MESH_EVENT_NODE_PROV_COMP,
    BT_MESH_EVENT_NODE_OOB_INPUT_NUM,
    BT_MESH_EVENT_NODE_OOB_INPUT_STRING,
} mesh_prov_event_en;

typedef enum {
    BT_MESH_AUTO_CONFIG_SUCCEED,
    BT_MESH_AUTO_CONFIG_NOT_START,
    BT_MESH_AUTO_CONFIG_START,
    BT_MESH_AUTO_CONFIG_FAILD,
}meah_auto_config_status_en;

typedef enum {
  BT_MESH_STATUS_SUCCEED,
  BT_MESH_STATUS_ERR,
} status_faild_en;

typedef void (*prov_event_cb)(mesh_prov_event_en event, void *p_arg);

typedef void (*attn_cb)(void);

typedef struct {
 attn_cb att_on;
 attn_cb att_off;
}health_srv_cb;

typedef enum {
 ACTION_NULL,
 ACTION_NUM,
 ACTION_STR,
}oob_action_en;

typedef enum {
 PROVISIONER,
 NODE,
}node_role_en;

typedef struct {
 uint8_t* static_oob_data;
 oob_action_en input_action;
 uint8_t input_max_size;
 oob_action_en output_action;
 uint8_t output_max_size;
}oob_method;

typedef struct {
    provisioner_config_t config;
    uint16_t local_sub;
    uint16_t local_pub;
} provisioner_node;

typedef struct {
    node_role_en role;
#ifdef CONFIG_BT_MESH_PROVISIONER
    provisioner_node *provisioner_config;
#endif
    uint8_t dev_uuid[16];
    uint8_t dev_name[DEVICE_NAME_MAX_LENGTH];
    model_event_cb user_model_cb;
    prov_event_cb user_prov_cb;
#ifdef BT_MESH_BEACON_RSSI_REPORT
    uint8_t rssi_report_enable;
#endif
    health_srv_cb *health_cb;
    oob_method node_oob;
} node_config_t;

typedef struct {
 uint16_t sub_addr;
}model_auto_config_t;


int ble_mesh_node_init(node_config_t *param);
int ble_mesh_node_OOB_input_num(uint32_t num);
int ble_mesh_node_OOB_input_string(const char *str);


#ifdef CONFIG_BT_MESH_PROVISIONER
int ble_mesh_node_appkey_add(uint16_t netkey_idx,uint16_t appkey_idx,uint16_t unicast_addr);
int ble_mesh_node_model_autoconfig(uint16_t netkey_idx,uint16_t appkey_idx, uint16_t unicast_addr, model_auto_config_t auto_conf);
#endif

#endif
