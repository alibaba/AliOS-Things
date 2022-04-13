/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _GENIE_MESH_H_
#define _GENIE_MESH_H_

//#include <zephyr/types.h>
#include <stddef.h>
#include <net/buf.h>

#include <mesh/access.h>
#include <mesh/main.h>
#include <mesh/cfg_srv.h>
#include <mesh/health_srv.h>
#include "vendor_model.h"
#include "vendor_model_srv.h"

#define CONFIG_CID_TAOBAO            0x01A8

#define CONFIG_MESH_VENDOR_COMPANY_ID CONFIG_CID_TAOBAO
#define CONFIG_MESH_VENDOR_MODEL_SRV  0x0000
#define CONFIG_MESH_VENDOR_MODEL_CLI  0x0001

#define STATIC_OOB_LENGTH            16


typedef enum {
/* !!!START!!! --- Don't add new ID before this one */
    GENIE_EVT_START = 0,

/* Reset Related Operation */
    GENIE_EVT_SW_RESET = GENIE_EVT_START, /* triggered from cloud */
    GENIE_EVT_HW_RESET, /* triggered from user */
    GENIE_EVT_RESET_BY_REPEAT_NOTIFY,   /*triggered by reset by repeat module */

/* SDK triggered event, with prefix of GENIE_EVT_STK_MESH_ */
    GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_INIT = GENIE_EVT_SDK_START,
    GENIE_EVT_SDK_MESH_PWRON_INDC,
    GENIE_EVT_SDK_MESH_PBADV_START,
    GENIE_EVT_SDK_MESH_PBADV_TIMEOUT,
    GENIE_EVT_SDK_MESH_SILENT_START,

    GENIE_EVT_SDK_MESH_PROV_START,
    GENIE_EVT_SDK_MESH_PROV_DATA,
    GENIE_EVT_SDK_MESH_PROV_TIMEOUT,
    GENIE_EVT_SDK_MESH_PROV_SUCCESS,
    GENIE_EVT_SDK_MESH_PROV_FAIL,

    GENIE_EVT_SDK_APPKEY_ADD,
    GENIE_EVT_SDK_APPKEY_DEL,
    GENIE_EVT_SDK_APPKEY_UPDATE,
    GENIE_EVT_SDK_NETKEY_ADD,
    GENIE_EVT_SDK_NETKEY_DEL,
    GENIE_EVT_SDK_NETKEY_UPDATE,
    GENIE_EVT_SDK_SEQ_UPDATE,
    //GENIE_EVT_SDK_IVI_UPDATE,
    //GENIE_EVENT_SUB_SET,
    //GENIE_EVENT_HB_SET,

    GENIE_EVT_SDK_ANALYZE_MSG,
    GENIE_EVT_SDK_DELAY_START,
    GENIE_EVT_SDK_DELAY_END,
    GENIE_EVT_SDK_TRANS_START,
    GENIE_EVT_SDK_TRANS_CYCLE,
    GENIE_EVT_SDK_TRANS_END,
    GENIE_EVT_SDK_ACTION_DONE,
    GENIE_EVT_SDK_VENDOR_MSG,

/* APP triggered event, with prefix of GENIE_EVT_APP_ */
    GENIE_EVT_APP_START,
    GENIE_EVT_APP_TURN_ON = GENIE_EVT_APP_START,
    GENIE_EVT_APP_TURN_OFF,
    GENIE_EVT_APP_INDICATE,
    GENIE_EVT_APP_VENDOR_MSG,
    GENIE_EVT_APP_TIMER_MSG,

/* !!!END!!! --- Don't add new ID after this one */
    GENIE_EVT_END

} _GENIE_EVENT;

typedef enum {
    GEN_ONOFF = 0,
} _MODEL_TYPE;

typedef enum {
    T_CUR = 0,
    T_TAR,
    TYPE_NUM,
} _VALUE_TYPE;

typedef struct{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t onoff[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    s16_t level[TYPE_NUM];
#endif

    u16_t actual[TYPE_NUM]; //1.power level 2.lightness

    u16_t linear[TYPE_NUM];

    u16_t temp[TYPE_NUM];
    u16_t UV[TYPE_NUM];

    u16_t hue[TYPE_NUM];
    u16_t sat[TYPE_NUM];

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    u8_t trans_id;
    u8_t trans_src;
    s16_t trans_level;
#endif

    u8_t delay;         //unit:5ms
    u8_t trans;    //unit:100ms

    u32_t trans_start_time;
    u32_t trans_last_time;
    u32_t trans_end_time;
    s16_t actual_trans_step;
    s16_t temp_trans_step;

    struct k_delayed_work delay_timer;
    struct k_delayed_work trans_timer;

    //scene
    u8_t scene_status;
    u16_t scene[TYPE_NUM];
}  S_MESH_STATE;

typedef struct{
    u8_t def_trans;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t default_onoff;
    u8_t last_onoff;
#endif

    u8_t range_status;

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    s16_t default_level;
    s16_t last_level;
#endif

    u16_t default_actual;
    u16_t last_actual;
    u16_t min_actual;
    u16_t max_actual;

    //temp
    u16_t default_temp;
    u16_t last_temp;
    u16_t min_temp;
    u16_t max_temp;

    u16_t default_UV;
    u16_t last_UV;

#if 0
    //hue
    u16_t default_hue;
    u16_t last_hue;
    u16_t min_hue;
    u16_t max_hue;

    //sat
    u16_t default_sat;
    u16_t last_sat;
    u16_t min_sat;
    u16_t max_sat;
#endif
}  S_MESH_POWERUP;

typedef struct{
    u8_t main_elem_num;
    S_MESH_STATE state;
    S_MESH_POWERUP powerup;
    void *user_data;
} S_ELEM_STATE;


#define GENIE_MAX_ELEMENT_COUNT 10

#define ALI_MODEL_TAG "\t[ALI_MODEL]"

#define MODEL_D(f, ...) printf("%d "ALI_MODEL_TAG"[D] %s "f"\n", (u32_t)k_uptime_get_32(), __func__, ##__VA_ARGS__)
#define MODEL_I(f, ...) printf(ALI_MODEL_TAG"[I] %s "f"\n", __func__, ##__VA_ARGS__)
#define MODEL_E(f, ...) printf(ALI_MODEL_TAG"[E] %s "f"\n", __func__, ##__VA_ARGS__)

//#define GENIE_MESH_EVENT_PRINT(id) BT_DBG(F_YELLOW "%s" F_END, genie_event_str[id])
#define GENIE_MESH_EVENT_PRINT(id) BT_DBG("%s", genie_event_str[id])

typedef enum{
    MESH_SUCCESS = 0,
    MESH_TID_REPEAT,
    MESH_ANALYZE_SIZE_ERROR,
    MESH_ANALYZE_ARGS_ERROR,
    MESH_SET_TRANSTION_ERROR,
} E_MESH_ERROR_TYPE;

E_MESH_ERROR_TYPE mesh_check_tid(u16_t src_addr, u8_t tid);
//u8_t get_remain_byte(S_MESH_STATE *state, bool is_ack);
u32_t get_transition_time(u8_t byte);
void genie_event(_GENIE_EVENT event, void *args);
s16_t genie_vendor_model_msg_send(vnd_model_msg *p_vendor_msg);
s16_t genie_light_action_notify(S_ELEM_STATE *p_elem);
u8_t get_remain_byte(S_MESH_STATE *p_state, bool is_ack);
uint8_t elem_state_init(uint8_t state_count, S_ELEM_STATE *p_elem);
void genie_mesh_init(void);

#endif /* __BT_MESH_H */
