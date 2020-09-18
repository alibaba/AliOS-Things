/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bt_bqb.c
 * @brief:   tg bt bqb adapter
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2020/02/29
 * @version: 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/cli.h>

#include "tg_bluetooth.h"
#include "tg_bt_mesh.h"
#include "bt_err.h"
#include "common/bt_trace.h"
#include "yoc_bt_main.h"
#include "yoc_bt_device.h"
#include "yoc_gap_bt_api.h"
#include "yoc_a2dp_api.h"
#include "yoc_avrc_api.h"
#include "yoc_gap_ble_api.h"
#include "yoc_gatts_api.h"
#include "yoc_gattc_api.h"
#include "yoc_bt_defs.h"
#include "btm_api.h"
#include "bt_hal_os.h"
#include "osi/allocator.h"

#include "tg_bt_mesh.h"
#include "mesh_hal_ble.h"
#include "gatt_api.h"

#if CONFIG_BT_BQB_ENABLED
#define CHECK_CMD(cmd, str) ((strncmp(cmd, str, strlen(cmd)) == 0) && (strlen(cmd) == strlen(str))) 

uint8_t g_tg_bt_adv_flag[] = {0x1a};
static void tg_bt_test_ble_broadcast_start(void)
{
    struct tg_bt_mesh_le_adv_param adv_param;
    uint8_t name[] = "BLE ADV";
    uint8_t manufacture_data[] = {
        0xa8, 0x01, 0x85, 0x01, 0x15, 0x11,
        0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33,
    };

    struct tg_bt_mesh_data ad[] = {
        {
            .type = 1,
            .data_len = sizeof(g_tg_bt_adv_flag),
            .data = g_tg_bt_adv_flag,
        },
        {
            .type = 9,
            .data_len = sizeof(name),
            .data = name,
        },
    };

    struct tg_bt_mesh_data sd[] =  {
        {
            .type = 0xff,
            .data_len = sizeof(manufacture_data),
            .data = manufacture_data,
        },
    };

    adv_param.options = 1;
    adv_param.interval_min = 32;
    adv_param.interval_max = adv_param.interval_min;
    adv_param.own_addr = NULL;
    tg_bt_mesh_adv_start(&adv_param, &ad, 2, &sd, 1);
}

struct gatt_char{
    uint8_t *uuid;
    uint32_t property;
    uint32_t permission;
    uint32_t char_handle;
    uint32_t char_desp_flag;
    uint8_t *value;
    uint8_t value_len;
};

struct gatt_desc {
    uint32_t desc_handle;
    uint8_t *value;
    uint8_t value_len;
    uint8_t uuid_flag;
};


uint8_t g_test_gatt_service_uuid[] = "FFC0";
uint8_t g_test_gatt_char_uuid0[] = "FFE0";
uint8_t g_test_gatt_char_uuid1[] = "FFE1";
uint8_t g_test_gatt_char_uuid2[] = "FFE2";
uint8_t g_test_gatt_char_uuid3[] = "FFE3";
uint8_t g_test_gatt_char_uuid4[] = "FFE4";
uint8_t g_test_gatt_char_uuid5[] = "FFE5";
uint8_t g_test_gatt_char_uuid6[] = "FFE6";
uint8_t g_test_gatt_char_uuid7[] = "FFE7";

uint8_t g_test_gatt_char_value0[2];
uint8_t g_test_gatt_char_value1[32];
uint8_t g_test_gatt_char_value2[32];
uint8_t g_test_gatt_char_value3[32];
uint8_t g_test_gatt_char_value4[2];
uint8_t g_test_gatt_char_value5[32];
uint8_t g_test_gatt_char_value6[4];
uint8_t g_test_gatt_char_value7[4];

#define CHAR_DESC_CEPH       (1<<0)
#define CHAR_DESC_CUD        (1<<1)
#define CHAR_DESC_CCCD       (1<<2)
#define CHAR_DESC_SCCD       (1<<3)
#define CHAR_DESC_CPFD       (1<<4)
#define CHAR_DESC_CAFD       (1<<5)
#define MAX_CHAR_DESC       6
int8_t * CHAR_DESC_UUID[] = {
    "2900",
    "2901",
    "2902",
    "2903",
    "2904",
    "2905",
};

int8_t CHAR_DESC_Length[] = {
    2,
    64,
    2,
    2,
    2,
    7,
};

int32_t CHAR_DESC_PEMMISSION[] = {
    GATT_PERM_READ,
    GATT_PERM_READ,
    GATT_PERM_READ|GATT_PERM_WRITE,
    GATT_PERM_READ|GATT_PERM_WRITE,
    GATT_PERM_READ,
    GATT_PERM_READ,
};

static uint8_t g_test_gatt_desc_value[16][64];

static struct gatt_desc g_test_gatt_desc_list[16] = {
    {0, g_test_gatt_desc_value[0], sizeof(g_test_gatt_desc_value[0]), 0},
    {0, g_test_gatt_desc_value[1], sizeof(g_test_gatt_desc_value[1]), 0},
    {0, g_test_gatt_desc_value[2], sizeof(g_test_gatt_desc_value[2]), 0},
    {0, g_test_gatt_desc_value[3], sizeof(g_test_gatt_desc_value[3]), 0},
    {0, g_test_gatt_desc_value[4], sizeof(g_test_gatt_desc_value[4]), 0},
    {0, g_test_gatt_desc_value[5], sizeof(g_test_gatt_desc_value[5]), 0},
    {0, g_test_gatt_desc_value[6], sizeof(g_test_gatt_desc_value[6]), 0},
    {0, g_test_gatt_desc_value[7], sizeof(g_test_gatt_desc_value[7]), 0},
    {0, g_test_gatt_desc_value[8], sizeof(g_test_gatt_desc_value[8]), 0},
    {0, g_test_gatt_desc_value[9], sizeof(g_test_gatt_desc_value[9]), 0},
    {0, g_test_gatt_desc_value[10], sizeof(g_test_gatt_desc_value[10]), 0},
    {0, g_test_gatt_desc_value[11], sizeof(g_test_gatt_desc_value[11]), 0},
    {0, g_test_gatt_desc_value[12], sizeof(g_test_gatt_desc_value[12]), 0},
    {0, g_test_gatt_desc_value[13], sizeof(g_test_gatt_desc_value[13]), 0},
    {0, g_test_gatt_desc_value[14], sizeof(g_test_gatt_desc_value[14]), 0},
    {0, g_test_gatt_desc_value[15], sizeof(g_test_gatt_desc_value[15]), 0},
};

static struct gatt_char g_test_gatt_char_uuid_list[] = {
    {
        g_test_gatt_char_uuid0,
        GATT_CHAR_PROP_BIT_READ,
        GATT_PERM_READ,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value0,
        sizeof(g_test_gatt_char_value0)
    },
    {
        g_test_gatt_char_uuid1,
        GATT_CHAR_PROP_BIT_READ,
        GATT_PERM_READ,
        0,
        CHAR_DESC_CCCD|CHAR_DESC_CUD, 
        g_test_gatt_char_value1, 
        sizeof(g_test_gatt_char_value1)
    },
    {
        g_test_gatt_char_uuid2,
        GATT_CHAR_PROP_BIT_READ,
        GATT_READ_ALLOWED,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value2,
        sizeof(g_test_gatt_char_value2)
    },
    {
        g_test_gatt_char_uuid3,
        GATT_CHAR_PROP_BIT_READ,
        GATT_READ_ALLOWED,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value3,
        sizeof(g_test_gatt_char_value3)
    },
    {
        g_test_gatt_char_uuid4,
        GATT_CHAR_PROP_BIT_WRITE,
        GATT_PERM_WRITE,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value4,
        sizeof(g_test_gatt_char_value4)
    },
    {
        g_test_gatt_char_uuid5,
        GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE|GATT_CHAR_PROP_BIT_WRITE_NR|GATT_CHAR_PROP_BIT_AUTH,
        GATT_PERM_READ|GATT_PERM_WRITE|GATT_PERM_WRITE_ENCRYPTED|GATT_PERM_WRITE_SIGNED|GATT_PERM_WRITE_SIGNED_MITM,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value5,
        sizeof(g_test_gatt_char_value5)
    },
    {
        g_test_gatt_char_uuid6,
        GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE|GATT_CHAR_PROP_BIT_NOTIFY,
        GATT_PERM_READ|GATT_PERM_WRITE,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value6,
        sizeof(g_test_gatt_char_value6)
    },
    {   
        g_test_gatt_char_uuid7,
        GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE|GATT_CHAR_PROP_BIT_INDICATE,
        GATT_PERM_READ|GATT_PERM_WRITE,
        0,
        CHAR_DESC_CCCD,
        g_test_gatt_char_value7,
        sizeof(g_test_gatt_char_value7)
    },
};

static hal_timer_t g_test_gatt_timer;
static int32_t g_test_gatt_state;
static int32_t g_test_gatt_server_if;
static int32_t g_test_gatt_server_handle;
static uint16_t g_test_ble_conn_id;
static uint8_t g_test_ble_connnected;

bool mac_str_to_bin(const int8_t *str, uint8_t *mac);
static void tg_bt_test_ble_gatt_timercb(void *pArgs)
{
    int32_t i, j;
    struct gatt_char *pChar;

    if (g_test_gatt_state == 1) {
        tg_bt_gattsAddService(g_test_gatt_server_if, g_test_gatt_service_uuid, 1, 256); 
    }

    if (g_test_gatt_state == 2) {
        for (i = 0; i < sizeof(g_test_gatt_char_uuid_list)/sizeof(struct gatt_char); i++) {
            pChar = &(g_test_gatt_char_uuid_list[i]);
            tg_bt_gattsAddChar(g_test_gatt_server_if, g_test_gatt_server_handle, pChar->uuid, pChar->property, pChar->permission);
            for (j = 0; j < MAX_CHAR_DESC; j++) {
                if (pChar->char_desp_flag & (1<<j)) {
                    tg_bt_gattsAddDesc(g_test_gatt_server_if, g_test_gatt_server_handle, CHAR_DESC_UUID[j], CHAR_DESC_PEMMISSION[j]);
                }
            }
        }

        tg_bt_gattsStartService(g_test_gatt_server_if, g_test_gatt_server_handle, GATT_TRANSPORT_LE);
    }

}

static void tg_bt_test_ble_gattsInitCB(int32_t serverIf)
{
    /* add service by timer */
    TG_TRACE_WARNING("%s:%d\n", __func__, serverIf);

    g_test_gatt_server_if = serverIf;
    g_test_gatt_state = 1;
    hal_os_timer_start(&g_test_gatt_timer);  
}

static void tg_bt_test_ble_gattsAddServiceCB(TG_BT_GATTS_ADD_SRVC_RST_T *add_srv)
{
    /* add char by timer */
    TG_TRACE_WARNING("%s: %d\n", __func__, add_srv->srvc_handle);

    g_test_gatt_server_handle = add_srv->srvc_handle;
    g_test_gatt_state = 2;
    hal_os_timer_start(&g_test_gatt_timer); 
}

static void tg_bt_test_ble_gattsAddCharCB(TG_BT_GATTS_ADD_CHAR_RST_T *add_char)
{
    int32_t i;
    struct gatt_char *pChar;
    
    /* add char by timer */
    TG_TRACE_WARNING("%s: uuid = %s, handle = %d\n", __func__, add_char->uuid, add_char->char_handle);

    for (i = 0; i < sizeof(g_test_gatt_char_uuid_list)/sizeof(struct gatt_char); i++) {
        pChar = &(g_test_gatt_char_uuid_list[i]);
        if (strcmp(pChar->uuid, add_char->uuid) == 0) {
            TG_TRACE_WARNING("%s: uuid = %s, i = %d\n", __func__, add_char->uuid, i);
            pChar->char_handle = add_char->char_handle;
        }
    }
}

static void tg_bt_test_ble_gattsAddDescCB(TG_BT_GATTS_ADD_DESCR_RST_T *add_desc)
{
    int32_t i, j;
    struct gatt_desc *desc;

    /* add char by timer */
    TG_TRACE_WARNING("%s: uuid = %s, handle = %d\n", __func__, add_desc->uuid, add_desc->descr_handle);

    for (i = 0; i < sizeof(g_test_gatt_desc_list)/sizeof(struct gatt_desc); i++) {
        desc = &(g_test_gatt_desc_list[i]);
        if (desc->desc_handle == 0) {
            desc->desc_handle = add_desc->descr_handle;
            for (j = 0; j < MAX_CHAR_DESC; j++) {
                if (strcmp(CHAR_DESC_UUID[j], add_desc->uuid) == 0) {
                    desc->uuid_flag = 1<<j;
                    desc->value_len = CHAR_DESC_Length[j];
                }
            }
            return;
        }
    }
}

static void tg_bt_test_ble_gattsConnectionEventCB(TG_BT_GATTS_EVENT_T event, uint16_t conn_id)
{
    if (event == TG_BT_GATTS_DISCONNECT) {
        g_test_ble_conn_id = 0;
        g_test_ble_connnected = 0;
        tg_bt_test_ble_broadcast_start();
    } else {
        g_test_ble_conn_id = conn_id;
        g_test_ble_connnected = 1;
    }
}

static void tg_bt_test_ble_gattsReqReadCB(TG_BT_GATTS_REQ_READ_RST_T *req_read)
{
    int32_t i;
    struct gatt_char *pChar;
    struct gatt_desc *pDesc;

    /* add char by timer */
    TG_TRACE_WARNING("%s: handle = %d\n", __func__, req_read->attr_handle);

    for (i = 0; i < sizeof(g_test_gatt_char_uuid_list)/sizeof(struct gatt_char); i++) {
        pChar = &(g_test_gatt_char_uuid_list[i]);
        if (pChar->char_handle == req_read->attr_handle) {
            TG_TRACE_WARNING("%s: uuid = %s, i = %d\n", __func__, pChar->uuid, i);
            if (pChar->value_len > req_read->offset) {
                tg_bt_gattsSendResponse(req_read->conn_id, req_read->trans_id, 0, req_read->attr_handle, pChar->value+req_read->offset, req_read->offset, pChar->value_len-req_read->offset, 0);
            } else {
                tg_bt_gattsSendResponse(req_read->conn_id, req_read->trans_id, GATT_INVALID_OFFSET, req_read->attr_handle, NULL, 0, 0, 0);
            }
            return;
        }
    }

    for (i = 0; i < sizeof(g_test_gatt_desc_list)/sizeof(struct gatt_desc); i++) {
        pDesc = &(g_test_gatt_desc_list[i]);
        if (pDesc->desc_handle == req_read->attr_handle) {
            TG_TRACE_WARNING("%s: i = %d\n", __func__, i);
            if (pChar->value_len > req_read->offset) {
                tg_bt_gattsSendResponse(req_read->conn_id, req_read->trans_id, 0, req_read->attr_handle, pDesc->value+req_read->offset, req_read->offset, pDesc->value_len-req_read->offset, 0);
            } else {
                tg_bt_gattsSendResponse(req_read->conn_id, req_read->trans_id, GATT_INVALID_OFFSET, req_read->attr_handle, NULL, 0, 0, 0);
            }

            return;
        }
    }
}

static void tg_bt_test_ble_gattsReqWriteCB(TG_BT_GATTS_REQ_WRITE_RST_T *req_write)
{
    int32_t i;
    struct gatt_char *pChar;
    struct gatt_desc *pDesc;

    /* add char by timer */
    TG_TRACE_WARNING("%s: handle = %d\n", __func__, req_write->attr_handle);

    for (i = 0; i < sizeof(g_test_gatt_char_uuid_list)/sizeof(struct gatt_char); i++) {
        pChar = &(g_test_gatt_char_uuid_list[i]);
        if (pChar->char_handle == req_write->attr_handle) {
            TG_TRACE_WARNING("%s: uuid = %s, i = %d, offset = %d, is_prep = %d, need_rsp = %d\n", __func__, pChar->uuid, i, req_write->offset, req_write->is_prep, req_write->need_rsp);
            memcpy(pChar->value+req_write->offset, req_write->value, (req_write->length+req_write->offset) < pChar->value_len ? req_write->length : pChar->value_len-req_write->offset);
            if ((req_write->is_prep&1) == 0) {
                uint8_t status = GATT_SUCCESS;
                
                if (req_write->length != pChar->value_len)
                    status = GATT_INVALID_ATTR_LEN;
                tg_bt_gattsSendResponse(req_write->conn_id, req_write->trans_id, status, req_write->attr_handle, NULL, 0, 0, 0);
            } else {
                tg_bt_gattsSendResponse(req_write->conn_id, req_write->trans_id, 0, req_write->attr_handle, pChar->value+req_write->offset, req_write->offset, req_write->length, 0);
            }
            return;
        }
    }

    for (i = 0; i < sizeof(g_test_gatt_desc_list)/sizeof(struct gatt_desc); i++) {
        pDesc = &(g_test_gatt_desc_list[i]);
        if (pDesc->desc_handle == req_write->attr_handle) {
            TG_TRACE_WARNING("%s: i = %d\n", __func__, i);
            tg_bt_gattsSendResponse(req_write->conn_id, req_write->trans_id, 0, req_write->attr_handle, NULL, 0, 0, 0);
            return;
        }
    }
}

static int g_tg_bt_client_if = 30;
static uint16_t g_tg_bt_client_conn_id = 0;

void tg_bt_gattc_cb(yoc_gattc_cb_event_t event, yoc_gatt_if_t gattc_if, yoc_ble_gattc_cb_param_t *param)
{
    TG_TRACE_WARNING("%s, %d", __func__, event);
    switch (event) {
    case YOC_GATTC_REG_EVT:
        g_tg_bt_client_if = gattc_if;
        break;
    case YOC_GATTC_OPEN_EVT:
        break;
    case YOC_GATTC_CONNECT_EVT:
        {
            g_tg_bt_client_conn_id = param->connect.conn_id;
        }
        break;
    }
}

static void tg_bt_test_ble_gatt_start(void)
{
    TG_BT_GATTS_CB_FUNC_T callback;

    hal_os_create_timer(&g_test_gatt_timer, tg_bt_test_ble_gatt_timercb, NULL, 5);

    memset(&callback, 0, sizeof(TG_BT_GATTS_CB_FUNC_T));
    callback.gattsInitCB = tg_bt_test_ble_gattsInitCB;
    callback.gattsAddServiceCB = tg_bt_test_ble_gattsAddServiceCB;
    callback.gattsAddCharCB = tg_bt_test_ble_gattsAddCharCB;
    callback.gattsAddDescCB = tg_bt_test_ble_gattsAddDescCB;
    callback.gattsConnectionEventCB = tg_bt_test_ble_gattsConnectionEventCB;
    callback.gattsReqReadCB = tg_bt_test_ble_gattsReqReadCB;
    callback.gattsReqWriteCB = tg_bt_test_ble_gattsReqWriteCB;
    tg_bt_gattsSetCallback(&callback);
    tg_bt_initGatts();

    {
        yoc_ble_gattc_register_callback(tg_bt_gattc_cb);
        yoc_ble_gattc_app_register(g_tg_bt_client_if);
    }
}

typedef void (*tg_bt_bqb_handler)(int32_t, int8_t **);

struct bqb_sub_cmd {
    int8_t *cmd;
    tg_bt_bqb_handler func;
};

static tg_bt_handle_cmd(struct bqb_sub_cmd *cmd, int32_t cmd_num, int32_t argc, int8_t **argv)
{
    int32_t i;

    for (i = 0; i < cmd_num; i++) {
        if (strncmp(argv[0], cmd[i].cmd, strlen(cmd[i].cmd)) == 0) {
            if (cmd[i].func) {
                cmd[i].func(argc-1, argv+1);
            }
        }
    }
}

static void _tg_bt_test_scan_cb(const bt_mesh_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
    TG_TRACE_WARNING("%s", __func__);
    return;
}

void tg_bt_test_bqb_gap_scan_handle(int32_t argc, int8_t **argv)
{
    int32_t i, flag;

    if (argc < 1) {
        return;
    }

    flag = atoi(argv[0]);
    tg_bt_gapSetScanMode((flag>>0)&1, (flag>>1)&1);
}

void tg_bt_test_bqb_gap_connect_handle(int32_t argc, int8_t **argv)
{
    unsigned char *cmd = argv[0];

    if (argc < 1) {
        return;
    }

     tg_bt_a2dpConnect(cmd, TG_BT_A2DP_ROLE_SINK);
}

void tg_bt_test_bqb_gap_disconnect_handle(int32_t argc, int8_t **argv)
{
    unsigned char *cmd = argv[0];

    if (argc < 1) {
        return;
    }

    tg_bt_a2dpDisconnect(cmd, TG_BT_A2DP_ROLE_SINK);
}

void tg_bt_test_bqb_gap_le_scan_handle(int32_t argc, int8_t **argv)
{
    int32_t i, flag;

    if (argc < 1) {
        return;
    }

    flag = atoi(argv[0]);
    if (flag) { 
        struct tg_bt_mesh_le_scan_param scan_param;
        scan_param.interval = 10;
        scan_param.window = 10;
        scan_param.type = 0;
        scan_param.filter_dup = 0;
        tg_bt_mesh_scan_start(&scan_param,_tg_bt_test_scan_cb);
    } else {
        tg_bt_mesh_scan_stop();
    }
}

void tg_bt_test_bqb_gap_le_adv_handle(int32_t argc, int8_t **argv)
{
    int32_t i, flag;
    TG_BT_GATTC_CB_FUNC_T func;

    if (argc < 1) {
        return;
    }

    memset(&func, 0, sizeof(TG_BT_GATTC_CB_FUNC_T));
    tg_bt_gattcSetCallback(&func);

    flag = atoi(argv[0]);
    if (flag == 1) {
        g_tg_bt_adv_flag[0] = 0x1a;
        tg_bt_test_ble_broadcast_start();
    } else if (flag == 2){
        g_tg_bt_adv_flag[0] = 0x00;
        tg_bt_test_ble_broadcast_start();
    } else {
        tg_bt_mesh_adv_stop();
    }
}

#define GAP_SCAN        "scan"
//connect xx:xx:xx:xx:xx:xx
#define GAP_CONNECT        "connect"
//connect xx:xx:xx:xx:xx:xx
#define GAP_DISCONNECT        "disconnect"
#define GAP_LE_SCAN     "le_scan"
#define GAP_LE_ADV      "le_adv"        
static struct bqb_sub_cmd tg_bt_bqb_gap_sub_cmd[] = {
    {GAP_SCAN, tg_bt_test_bqb_gap_scan_handle},
    {GAP_CONNECT, tg_bt_test_bqb_gap_connect_handle},
    {GAP_DISCONNECT, tg_bt_test_bqb_gap_disconnect_handle},
    {GAP_LE_SCAN, tg_bt_test_bqb_gap_le_scan_handle},
    {GAP_LE_ADV, tg_bt_test_bqb_gap_le_adv_handle},
};
void tg_bt_test_bqb_gap_handle(int32_t argc, int8_t **argv)
{
    int32_t i;

    if (argc < 1) {
        return;
    }

    tg_bt_handle_cmd(&tg_bt_bqb_gap_sub_cmd, sizeof(tg_bt_bqb_gap_sub_cmd)/sizeof(struct bqb_sub_cmd), argc, argv);
}

#define AVRCP_PLAY        "play"
#define AVRCP_PAUSE     "pause"
#define AVRCP_STOP        "stop"
#define AVRCP_VOLUME_UP      "vol_up"
#define AVRCP_VOLUME_DOWN      "vol_down"
#define AVRCP_FWD        "fwd"
#define AVRCP_FFWD        "ffwd"
#define AVRCP_BWD        "bwd"
#define AVRCP_CLOSETG        "closeTG"
extern void bta_ar_dereg_avrc(uint16_t service_uuid, uint8_t sys_id);
void tg_bt_test_bqb_avrcp_handle(int32_t argc, int8_t **argv)
{
    TG_BT_AVRCP_CMD_TYPE cmd_type;
    unsigned char *cmd = argv[0];

    if(CHECK_CMD(cmd, AVRCP_PLAY))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_PLAY;
    else if(CHECK_CMD(cmd,AVRCP_PAUSE))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_PAUSE;
    else if(CHECK_CMD(cmd,AVRCP_VOLUME_UP))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_VOL_UP;
    else if(CHECK_CMD(cmd,AVRCP_VOLUME_DOWN))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_VOL_DOWN;
    else if(CHECK_CMD(cmd,AVRCP_STOP))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_STOP;
    else if(CHECK_CMD(cmd,AVRCP_FWD))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_FWD;
    else if(CHECK_CMD(cmd,AVRCP_FFWD))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_FFWD;
    else if(CHECK_CMD(cmd,AVRCP_BWD))
        cmd_type = TG_BT_AVRCP_CMD_TYPE_BWD;
    else if (CHECK_CMD(cmd,AVRCP_CLOSETG)) {
        bta_ar_dereg_avrc (0X110C, 18);
        return;
    } else {
        TG_TRACE_WARNING("avrcp invalid para");
        return;
    }
    tg_bt_avrcpSendPassthroughCmd(cmd_type);
}

#define A2DP_CONNECT "connect"
void tg_bt_test_bqb_a2dp_handle(int32_t argc, int8_t **argv)
{
    unsigned char *cmd = argv[0];

    if (argc < 1) {
        return;
    }
    if(CHECK_CMD(cmd, A2DP_CONNECT)){
        unsigned char * mac = argv[1];
        tg_bt_a2dpConnect(mac, TG_BT_A2DP_ROLE_SINK);
    }
    return;
}

#define L2CAP_SEND "send"
#define L2CAP_DISCONNECT "disconnect"
void tg_bt_test_bqb_l2cap_handle(int32_t argc, int8_t **argv)
{
    unsigned char *cmd = argv[0];
    unsigned char * mac = argv[1];
    if (argc < 1) {
        return;
    }

    if(CHECK_CMD(cmd, L2CAP_SEND)){
        tg_bt_pts_l2cap_send_data(mac);
    }else if(CHECK_CMD(cmd, L2CAP_DISCONNECT)){
        tg_bt_pts_l2cap_disconnect(mac);
    }
    return;
}

void tg_bt_test_bqb_gatt_start_service_handle(int32_t argc, int8_t **argv)
{
    /* ble gatt service add */
    tg_bt_test_ble_gatt_start();
}

void tg_bt_test_bqb_gatt_notify_handle(int32_t argc, int8_t **argv)
{
    int32_t result, i;
    uint16_t att_handle;
    uint8_t value[32];
    uint32_t temp;

    if (argc < 1) {
        return;
    }

    sscanf(argv[0], "%x", &temp);
    att_handle = (uint16_t)temp;
    for (i = 1; i < argc; i++) {
        sscanf(argv[i], "%x", &temp);
        value[i-1] = (uint8_t)temp;
    }
    /* send notification */
    if (g_test_ble_connnected) {
        result = tg_bt_gattsSendIndication(g_test_gatt_server_if, att_handle, g_test_ble_conn_id, 0, value, argc-1);
        if (result < 0) {
            TG_TRACE_ERROR("%s failed, %d", __func__, result);
        }
    }
}

void tg_bt_test_bqb_gatt_indicate_handle(int32_t argc, int8_t **argv)
{
    int32_t result, i;
    uint16_t att_handle;
    uint8_t value[32];
    uint32_t temp;

    if (argc < 1) {
        return;
    }

    sscanf(argv[0], "%x", &temp);
    att_handle = (uint16_t)temp;
    for (i = 1; i < argc; i++) {
        sscanf(argv[i], "%x", &temp);
        value[i-1] = (uint8_t)temp;
    }
    /* send indication */
    if (g_test_ble_connnected) {
        result = tg_bt_gattsSendIndication(g_test_gatt_server_if, att_handle, g_test_ble_conn_id, 1, value, argc-1);
        if (result < 0) {
            TG_TRACE_ERROR("%s failed, %d", __func__, result);
        }
    }
}

void tg_bt_test_bqb_gatt_unsbi01_handle(int32_t argc, int8_t **argv)
{
    gatt_bqb_setFlag(1);
}

void tg_bt_test_bqb_gatt_connect_handle(int32_t argc, int8_t **argv)
{
    yoc_bd_addr_t remote_bda;
    yoc_ble_addr_type_t remote_addr_type;
    uint8_t direct;

    if (argc < 3) {
        return;
    }

    mac_str_to_bin(argv[0], remote_bda);
    remote_addr_type = (yoc_ble_addr_type_t)atoi(argv[1]);
    direct = atoi(argv[2]);
    yoc_ble_gattc_open(g_tg_bt_client_if, remote_bda, remote_addr_type, direct);
}

void tg_bt_test_bqb_gatt_write_handle(int32_t argc, int8_t **argv)
{
    uint16_t att_handle;
    uint16_t value_len,i;
    uint8_t value[32];
    uint32_t temp;

    sscanf(argv[0], "%x", &temp);
    att_handle = (uint16_t)temp;
    value_len = 0;
    for (i = 1; i < argc; i++) {
        sscanf(argv[i], "%x", &temp);
        value[i-1] = (uint8_t)temp;
        value_len++;
    }

    yoc_ble_gattc_write_char(g_tg_bt_client_if, g_tg_bt_client_conn_id, att_handle,
                             value_len, value, YOC_GATT_WRITE_TYPE_RSP, YOC_GATT_AUTH_REQ_NONE);
}

void tg_bt_test_bqb_gatt_disconnect_handle(int32_t argc, int8_t **argv)
{
    yoc_ble_gattc_close(g_tg_bt_client_if, g_tg_bt_client_conn_id);
}


#define GATT_STARTSRVICE        "startService"
#define GATT_NOTIFY             "notify"
#define GATT_INDICATE           "indicate"
#define GATT_UNS_BI_01          "SR-UNS-BI-01-C"
#define GATT_CONNECT            "connect"
#define GATT_WRITE              "write"
#define GATT_DISCONNECT         "disconnect"
static struct bqb_sub_cmd tg_bt_bqb_gatt_sub_cmd[] = {
    {GATT_STARTSRVICE, tg_bt_test_bqb_gatt_start_service_handle},
    {GATT_NOTIFY, tg_bt_test_bqb_gatt_notify_handle},
    {GATT_INDICATE, tg_bt_test_bqb_gatt_indicate_handle},
    {GATT_UNS_BI_01, tg_bt_test_bqb_gatt_unsbi01_handle},
    {GATT_CONNECT, tg_bt_test_bqb_gatt_connect_handle},
    {GATT_WRITE, tg_bt_test_bqb_gatt_write_handle},
    {GATT_DISCONNECT, tg_bt_test_bqb_gatt_disconnect_handle},
};

void tg_bt_test_bqb_gatt_handle(int32_t argc, int8_t **argv)
{
    int32_t i;

    if (argc < 1) {
        return;
    }

    tg_bt_handle_cmd(&tg_bt_bqb_gatt_sub_cmd, sizeof(tg_bt_bqb_gatt_sub_cmd)/sizeof(struct bqb_sub_cmd), argc, argv);
}

void tg_bt_test_bqb_smp_handle(int32_t argc, int8_t **argv)
{

    return;
}

#define AVCTP_METADATA        "metadata"
void tg_bt_test_bqb_avctp_handle(int32_t argc, int8_t **argv)
{
    TG_BT_AVRCP_CMD_TYPE cmd_type;
    unsigned char *cmd = argv[0];

    if(CHECK_CMD(cmd, AVCTP_METADATA))
    {
        tg_bt_avrcpGetElementAttrTitle(NULL);
    }
    return;
}

//L2CAP/COS/CED/BV-03-C 
void tg_bt_pts_l2cap_send_data(const int8_t *mac)
{
    BT_HDR  * data;
    BD_ADDR addr;
    unsigned char para[8];

    data = (BT_HDR  * )osi_malloc(sizeof(BT_HDR) + 13 + sizeof(para));
    data->len = sizeof(para);
    data->offset = 13;
    mac_str_to_bin(mac, &addr);
    L2CA_PTS_DataWrite(&addr, data);    
}

//L2CAP/COS/CED/BV-07-C 
void tg_bt_pts_l2cap_disconnect(const int8_t *mac)
{
    BT_HDR  * data;
    BD_ADDR addr;
    
    mac_str_to_bin(mac, &addr);
    L2CA_PTS_DisconnectReq(&addr);    
}

#define AVDTP_CLOSE        "close"
#define AVDTP_ABORT        "abort"
extern unsigned char bta_pts_abort_flag;
void tg_bt_test_bqb_avdtp_handle(int32_t argc, int8_t **argv)
{
    TG_BT_AVRCP_CMD_TYPE cmd_type;
    unsigned char *cmd = argv[0];
    if(CHECK_CMD(cmd, AVDTP_CLOSE))
    {
        tg_bt_a2dpDisconnect(NULL, TG_BT_A2DP_ROLE_SINK);
    }else if(CHECK_CMD(cmd, AVDTP_ABORT))
    {
        //if test this case should input avdtp abort first
        //AVDTP/SNK/INT/SIG/SMG/BV-23-C 
        bta_pts_abort_flag = true;
    }
    return;
}

void tg_bt_test_bqb_gavdp_handle(int32_t argc, int8_t **argv)
{

    return;
}

void tg_bt_test_bqb_mesh_aggressive_handle(int32_t argc, int8_t **argv)
{
    int32_t flag;

    if (argc < 1) {
        return;
    }

    flag = atoi(argv[0]);
    if (flag) {
        tg_bt_mesh_enable_aggressive_setting(1);
    } else {
        tg_bt_mesh_enable_aggressive_setting(0);
    }
}


void tg_bt_test_bqb_mesh_epta_handle(int32_t argc, int8_t **argv)
{
    int32_t flag;
    int32_t param1, param2, param3;

    if (argc < 3) {
        return;
    }

    sscanf(argv[0], "%x", &param1);
    sscanf(argv[1], "%x", &param2);
    sscanf(argv[2], "%x", &param3);
    netdev_set_epta_params(param1, param2, param3);
}

#define MESH_AGGRESSIVE        "aggressive"
#define MESH_EPTA              "epta"
static struct bqb_sub_cmd tg_bt_bqb_mesh_sub_cmd[] = {
    {MESH_AGGRESSIVE, tg_bt_test_bqb_mesh_aggressive_handle},
    {MESH_EPTA, tg_bt_test_bqb_mesh_epta_handle},
};
void tg_bt_test_bqb_mesh_handle(int32_t argc, int8_t **argv)
{
    if (argc < 1) {
        return;
    }

    tg_bt_handle_cmd(&tg_bt_bqb_mesh_sub_cmd, sizeof(tg_bt_bqb_mesh_sub_cmd)/sizeof(struct bqb_sub_cmd), argc, argv);

    return;
}

#define BQB_GAP     "gap"
#define BQB_AVRCP   "avrcp"
#define BQB_A2DP    "a2dp"
#define BQB_L2CAP   "l2cap"
#define BQB_GATT    "gatt"
#define BQB_SMP     "smp"
#define BQB_AVDTP   "avdtp"
#define BQB_AVCTP   "avctp"
#define BQB_GAVDP   "gavdp"
#define BQB_MESH    "mesh"
static struct bqb_sub_cmd tg_bt_bqb_sub_cmd[] = {
    {BQB_GAP, tg_bt_test_bqb_gap_handle},
    {BQB_AVRCP, tg_bt_test_bqb_avrcp_handle},
    {BQB_A2DP, tg_bt_test_bqb_a2dp_handle},
    {BQB_L2CAP, tg_bt_test_bqb_l2cap_handle},
    {BQB_GATT, tg_bt_test_bqb_gatt_handle},
    {BQB_SMP, tg_bt_test_bqb_smp_handle},
    {BQB_AVDTP, tg_bt_test_bqb_avdtp_handle},
    {BQB_AVCTP, tg_bt_test_bqb_avctp_handle},
    {BQB_GAVDP, tg_bt_test_bqb_gavdp_handle},
    {BQB_MESH, tg_bt_test_bqb_mesh_handle},
};

static void tg_bt_test_bqb_cmd(int8_t *pwbuf, int32_t blen, int32_t argc, int8_t **argv)
{
    int32_t i = 0;

    TG_TRACE_WARNING("%s", __func__);
    if (argc < 1)
        return;

    tg_bt_handle_cmd(&tg_bt_bqb_sub_cmd, sizeof(tg_bt_bqb_sub_cmd)/sizeof(struct bqb_sub_cmd), argc-1, argv+1);
}

static struct cli_command ncmd = {
    .name = "bt_bqb_test",
    .help = "bt_bqb_test",
    .function = tg_bt_test_bqb_cmd,
};

static void tg_bt_test_register_cli(void)
{
    aos_cli_register_command(&ncmd);
}

void _tg_bt_test_bqb_start(void)
{
    TG_TRACE_ERROR("%s", __func__);

    tg_bt_test_register_cli();
}
#endif
