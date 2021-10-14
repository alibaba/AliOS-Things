/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _MESH_MODEL_H__
#define _MESH_MODEL_H__

#include "api/mesh.h"
#include "api/mesh/main.h"

#if defined(CONFIG_BT_MESH_MODEL_GEN_ONOFF_SRV)
#include "sig_model/generic_onoff_srv.h"
#endif
#if defined(CONFIG_BT_MESH_MODEL_GEN_LEVEL_SRV)
#include "sig_model/generic_level_srv.h"
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV)
#include "sig_model/light_lightness_srv.h"
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CTL_SRV)
#include "sig_model/light_ctl_srv.h"
#endif

#if defined(CONFIG_BT_MESH_MODEL_GEN_ONOFF_CLI)
#include "sig_model/generic_onoff_cli.h"
#endif

#if defined(CONFIG_BT_MESH_MODEL_GEN_LEVEL_CLI)
#include "sig_model/generic_level_cli.h"
#endif

#if defined(CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_CLI)
#include "sig_model/light_lightness_cli.h"
#endif
#if defined(CONFIG_BT_MESH_MODEL_LIGHT_CTL_CLI)
#include "sig_model/light_ctl_cli.h"
#endif

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_SRV)
#include "vendor/vendor_model_srv.h"
#endif

#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI)
#include "vendor/vendor_model_cli.h"
#endif

#include "model_bind_ops.h"
#include <api/mesh/access.h>

typedef enum {
    T_CUR = 0,
    T_TAR,
    TYPE_NUM,
} _VALUE_TYPE;

typedef enum {
    SUCCESS = 0,
    SET_MIN_FAIL,
    SET_MAX_FAIL,
    RFU,
} _STATUS_CODES;

typedef struct _RANGE {
    _STATUS_CODES code;
    u16_t range_min;
    u16_t range_max;
} RANGE_STATUS;

typedef struct {
#ifdef CONFIG_BT_MESH_MODEL_GEN_ONOFF_SRV
    u8_t onoff[TYPE_NUM];
#endif
#ifdef CONFIG_BT_MESH_MODEL_GEN_LEVEL_SRV
    s16_t level[TYPE_NUM];
#endif
#ifdef CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV
    u16_t lightness_linear[TYPE_NUM];
    u16_t lightness_actual[TYPE_NUM];
#endif
#ifdef CONFIG_BT_MESH_MODEL_LIGHT_CTL_SRV
    u16_t lightness[TYPE_NUM];
    u16_t temp[TYPE_NUM];
    u16_t UV[TYPE_NUM];
#endif

    u8_t delay; // unit:5ms
    u8_t trans; // unit:100ms
    bt_u32_t trans_start_time;
    bt_u32_t trans_end_time;
    s16_t trans_step;
    struct k_timer delay_timer;
    struct k_timer trans_timer;
    bool timerInitFlag;
} S_MESH_STATE;

typedef struct {
#ifdef CONFIG_BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV
    u16_t lightness_actual_default;
    u16_t lightness_last;
    RANGE_STATUS lightness_range;
#endif
#ifdef CONFIG_BT_MESH_MODEL_LIGHT_CTL_SRV
    uint16_t lightness_default;
    uint16_t temp_default;
    uint16_t UV_default;
    RANGE_STATUS ctl_temp_range;
#endif
} S_MESH_POWERUP;

typedef struct {
    S_MESH_STATE state;
    S_MESH_POWERUP powerup;
} S_ELEM_STATE;

typedef enum {
    MESH_SUCCESS = 0,
    MESH_TID_REPEAT,
    MESH_ANALYZE_SIZE_ERROR,
    MESH_ANALYZE_ARGS_ERROR,
    MESH_SET_TRANSTION_ERROR,
} E_MESH_ERROR_TYPE;

typedef enum {
    BT_MESH_MODEL_CFG_APP_KEY_ADD = 0x00,
    BT_MESH_MODEL_CFG_COMP_DATA_STATUS = 0x02,
    BT_MESH_MODEL_CFG_HEARTBEAT_PUB_STATUS = 0x06,
    BT_MESH_MODEL_CFG_APPKEY_STATUS = 0x8003,
    BT_MESH_MODEL_CFG_BEACON_STATUS = 0x800b,
    BT_MESH_MODEL_CFG_TTL_STATUS = 0x800e,
    BT_MESH_MODEL_CFG_FRIEND_STATUS = 0x8011,
    BT_MESH_MODEL_CFG_PROXY_STATUS = 0x8014,
    BT_MESH_MODEL_CFG_NET_KRP_STATUS = 0x8017,
    BT_MESH_MODEL_CFG_PUB_STATUS = 0x8019,
    BT_MESH_MODEL_CFG_SUB_STATUS = 0x801f,
    BT_MESH_MODEL_CFG_SUB_LIST = 0x802a,
    BT_MESH_MODEL_CFG_SUB_LIST_VND = 0x802c,
    BT_MESH_MODEL_CFG_RELAY_STATUS = 0x8028,
    BT_MESH_MODEL_CFG_HEARTBEAT_SUB_STATUS = 0x803c,
    BT_MESH_MODEL_CFG_APPKEY_BIND_STATUS = 0x803e,
    BT_MESH_MODEL_CFG_RST_STATUS = 0x804a,
    BT_MESH_MODEL_CFG_NET_KEY_STATUS = 0x8044,
/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
    BT_MESH_MODEL_CFG_CTRL_RELAY_STATUS = 0x8072,
#endif
    /*[Genie end] add by wenbing.cwb at 2021-01-21*/
    BT_MESH_MODEL_ONOFF_SET = 0x8202,
    BT_MESH_MODEL_ONOFF_STATUS = 0x8204,
    BT_MESH_MODEL_LEVEL_SET = 0x8206,
    BT_MESH_MODEL_LEVEL_MOVE_SET = 0x820B,
    BT_MESH_MODEL_LEVEL_STATUS = 0x8208,
    BT_MESH_MODEL_LEVEL_DELTA_SET = 0x8209,
    BT_MESH_MODEL_LIGHTNESS_SET = 0x824c,
    BT_MESH_MODEL_LIGHTNESS_STATUS = 0x824e,
    BT_MESH_MODEL_LIGHTNESS_LINEAR_SET = 0x8250,
    BT_MESH_MODEL_LIGHTNESS_LINEAR_STATUS = 0x8252,
    BT_MESH_MODEL_LIGHTNESS_LAST_STATUS = 0x8254,
    BT_MESH_MODEL_LIGHTNESS_DEF_STATUS = 0x8256,
    BT_MESH_MODEL_LIGHTNESS_RANGE_STATUS = 0x8258,
    BT_MESH_MODEL_LIGHTNESS_DEF_SET = 0x8259,
    BT_MESH_MODEL_LIGHTNESS_RANGE_SET = 0x825b,
    BT_MESH_MODEL_LIGHT_CTL_SET = 0x825e,
    BT_MESH_MODEL_LIGHT_CTL_STATUS = 0x8260,
    BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS = 0x8263,
    BT_MESH_MODEL_LIGHT_CTL_TEMP_SET = 0x8264,
    BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS = 0x8266,
    BT_MESH_MODEL_LIGHT_CTL_DEF_STATUS = 0x8268,
    BT_MESH_MODEL_LIGHT_CTL_DEF_SET = 0x8269,
    BT_MESH_MODEL_LIGHT_CTL_RANGE_SET = 0x826b,
    BT_MESH_MODEL_VENDOR_MESSAGES = 0xcf01a8,
    BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG = 0xd601a8,
    BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_GET = 0xd701a8,
    BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_STATUS = 0xd801a8,
} mesh_model_event_en;

typedef void (*model_event_cb)(mesh_model_event_en event, void *p_arg);

typedef struct {
    void *user_data;
    uint16_t data_len;
} vendor_data;

typedef struct {
    uint16_t source_addr;
    uint16_t dst_addr;
    struct net_buf_simple *status_data;
    void *user_data;
    vendor_data ven_data;
} model_message;

int ble_mesh_model_init(const struct bt_mesh_comp *comp);

const struct bt_mesh_comp *ble_mesh_model_get_comp_data();

int ble_mesh_model_set_cb(model_event_cb event_cb);

struct bt_mesh_model *ble_mesh_model_find(uint16_t elem_idx, uint16_t mod_idx, uint16_t CID);

int ble_mesh_model_status_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr,
                              struct bt_mesh_model *model, uint32_t op_code);

void model_event(mesh_model_event_en event, void *p_arg);

#endif // _MESH_MODEL_H_
