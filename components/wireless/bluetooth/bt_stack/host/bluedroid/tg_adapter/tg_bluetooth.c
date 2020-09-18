/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bluetooth.c
 * @brief:   vendor bluetooth implementation
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2019/12/15
 * @version: 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/cli.h>

#include "tg_bluetooth.h"
#include "tg_bt_mesh.h"
#include "tg_btsnoop.h"
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
#include "btm_api.h"
#include "bt_hal_os.h"
#include "bt_hal_hcit.h"

typedef void (* bt_app_cb_t) (uint16_t event, void *param);
typedef struct {
    uint16_t             sig;      /*!< signal to bt_app_task */
    uint16_t             event;    /*!< message event id */
    bt_app_cb_t          cb;       /*!< context switch callback */
    void                 *param;   /*!< parameter area needs to be last */
} bt_app_msg_t;
typedef void (* bt_app_copy_cb_t) (bt_app_msg_t *msg, void *p_dest, void *p_src);


static TG_BT_GAP_IMPL_CB_FUNC_T g_tg_bt_gapImplCB;
static TG_BT_A2DP_IMPL_CB_FUNC_T g_tg_bt_a2dpImplCB;
static TG_BT_AVRCP_IMPL_CB_FUNC_T g_tg_bt_avrcpImplCB;
static TG_BT_A2DP_ROLE g_tg_bt_a2dp_role;
static int8_t g_tg_bt_a2dpConnecting_flag;
static int8_t g_tg_bt_a2dpConnected_device[6];


static hal_queue_t s_bt_app_task_queue;
static hal_task_t s_bt_app_task_handle;
static int8_t queue_buf[40 * sizeof(bt_app_msg_t)];


static yoc_a2d_audio_state_t g_tg_bt_audio_state = YOC_A2D_AUDIO_STATE_STOPPED;
static const int8_t *s_a2d_conn_state_str[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
static const int8_t *s_a2d_audio_state_str[] = {"Suspended", "Stopped", "Started"};
static yoc_bt_io_cap_t g_tg_bt_bt_io_cap = YOC_BT_IO_CAP_NONE;
static yoc_bt_io_cap_t g_tg_bt_ble_io_cap = YOC_BT_IO_CAP_NONE;
static int8_t g_tg_bt_gatt_set_adv_data_flag = 0;
static int8_t g_tg_bt_gatt_adv_pending = 0;
static uint8_t g_tg_bt_avrcp_volumesync = 0x7f;

#define BT_APP_SIG_WORK_DISPATCH          (0x01)

static yoc_ble_adv_params_t _tg_bt_adv_params = {
    .adv_int_min        = 0xa0,
    .adv_int_max        = 0xa0,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static int32_t g_server_if = 20;
static TG_BT_GATTS_CB_FUNC_T g_tg_bt_gattServerImplCB;

#define T_STREAM_TO_UINT16(u16, p) {u16 = ((UINT16)(*(p)) << 8) + (((UINT16)(*((p) + 1))));}
#define T_STREAM_TO_UINT32(u32, p) {u32 = ((((UINT32)(*(p))) << 24) + ((((UINT32)(*((p) + 1)))) << 16) + ((((UINT32)(*((p) + 2))))  << 8) + ((((UINT32)(*((p) + 3))))));}
#define T_UINT32_TO_STREAM(p, u32) {*(p + 3) = (UINT8)(u32); *(p + 2) = (UINT8)((u32) >> 8); *(p + 1) = (UINT8)((u32) >> 16); *(p) = (UINT8)((u32) >> 24);}
#define T_UINT16_TO_STREAM(p, u16) {*(p + 1) = (UINT8)(u16); *(p) = (UINT8)((u16) >> 8);}

#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))

static TG_BT_GATTC_CB_FUNC_T g_tg_bt_gattClientImplCB;

void _tg_bt_app_avrcp_ct_cb(yoc_avrc_ct_cb_event_t event, yoc_avrc_ct_cb_param_t *param);
void _tg_bt_app_avrcp_tg_cb(yoc_avrc_tg_cb_event_t event, yoc_avrc_tg_cb_param_t *param);
void bt_mesh_scan_result_handle(yoc_ble_gap_cb_param_t *param);
bool tg_bt_mesh_adv_is_pending(void);
void _tg_bt_adv_callback(void *pArgs);
void _tg_bt_scan_callback(void *pArgs);
void bt_mesh_scan_param_handle(yoc_ble_gap_cb_param_t *param);

#if CONFIG_BT_BQB_ENABLED
void _tg_bt_test_bqb_start(void);
#endif

//#define BT_TEST_ADD     1

#ifdef BT_TEST_ADD
#include "bt_hal_os.h"

static bool mac_str_to_bin(const int8_t *str, uint8_t *mac);
static bool mac_bin_to_str(int8_t *str, uint8_t *mac);

#include "mesh.h"
#include <net/buf.h>
#include <mesh/mesh_main.h>
#include <mesh/access.h>
#include <mesh/cfg_cli.h>
#include <adv.h>
#include "prov.h"

static int32_t mesh_test_netKeyIndex = 0x0000;
static uint8_t mesh_test_netKey[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
static int32_t mesh_test_addr = 0x1001;
static int32_t mesh_test_provisionerAddr = 0x0001;
static int32_t mesh_test_appkey_index = 0x0010;
static uint8_t mesh_test_appkey[16] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

void tg_bt_mesh_prov_event_callback(BT_MESH_EVENT_T *evt)
{
    switch (evt->evt_id) {
    case BT_MESH_EVT_INIT_DONE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_INIT_DONE", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_CAPABILITIES:
        {
            int32_t ret;
            BT_MESH_PROV_CAPABILITIES_T *p_cap_in = &(evt->evt.mesh_evt.mesh.prov_cap.cap);
            int32_t algorithm, publicKey, authMethod, authAction, authSize, ivIndex, addr, flags, mode;

            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_CAPABILITIES", __func__);
            algorithm = p_cap_in->algorithms;
            publicKey = p_cap_in->public_key_type;
            authMethod = 1;
            authAction= 0;
            authSize = 0;
            ivIndex = 0;
            flags = 0;
            mode = 0;
            ret = provisioner_send_start(algorithm, publicKey, authMethod, authAction, authSize, mesh_test_netKey, mesh_test_netKeyIndex, ivIndex, mesh_test_addr, flags, mode);
        }
        break;
    case BT_MESH_EVT_PROV_REQUEST_OOB_PUBLIC_KEY:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_REQUEST_OOB_PUBLIC_KEY", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_DONE:
        {
            int32_t ret, status;

            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_DONE", __func__);

            ret = bt_mesh_cfg_app_key_add(mesh_test_netKeyIndex, mesh_test_addr, mesh_test_netKeyIndex,
                    mesh_test_appkey_index, mesh_test_appkey, &status);
        }
        break;
    case BT_MESH_EVT_PROV_SCAN_UD_RESULT:
        {
            int32_t ret;
            BT_MESH_EVT_PROV_SCAN_UD_T *p_prov_scan_ud_in = &(evt->evt.mesh_evt.mesh.prov_scan_ud);

            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_SCAN_UD_RESULT", __func__);

            set_provisioner_state(2);
            ret = provisioner_send_invite(p_prov_scan_ud_in->ud.uuid, 16, 10);
        }
        break;
    case BT_MESH_EVT_CONFIG_RESET:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_CONFIG_RESET", __func__);
        }
        break;
    case BT_MESH_EVT_FRIENDSHIP_STATUS:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_FRIENDSHIP_STATUS", __func__);
        }
        break;
    case BT_MESH_EVT_LPN_FRIEND_OFFER:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_LPN_FRIEND_OFFER", __func__);
        }
        break;
    case BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM", __func__);
        }
        break;
    case BT_MESH_EVT_HEARTBEAT:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_HEARTBEAT", __func__);
        }
        break;
    case BT_MESH_EVT_IV_UPDATE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_IV_UPDATE", __func__);
        }
        break;
    case BT_MESH_EVT_KEY_REFRESH:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_KEY_REFRESH", __func__);
        }
        break;
    case BT_MESH_EVT_UD_RESULT_COMPLETE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_UD_RESULT_COMPLETE", __func__);
        }
        break;
    case BT_MESH_EVT_PROV_FACTOR:
        {
            BT_MESH_PROV_FACTOR_T *p_prov_factor_in = &(evt->evt.mesh_evt.mesh.prov_factor);
            int32_t type;
            static uint8_t local_cfm_key[16];
            static uint8_t local_auth[16] = {0x04,0x47,0xF0,0x73,0x9B,0x32,0xE9,0x7B,0x24,0x54,0xAA,0x43,0xE4,0x79,0x00,0x2E};
            uint8_t cfm_val[16];

            type = p_prov_factor_in->type;
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_PROV_FACTOR", __func__);
            switch (type) {
            case BT_MESH_PROV_FACTOR_RANDOM_PROVISIONER:
                {
                    bt_mesh_prov_conf(local_cfm_key, p_prov_factor_in->buf, local_auth, cfm_val);
                    provisioner_send_confirm(cfm_val, 16);
                }
                break;
            case BT_MESH_PROV_FACTOR_CONFIRMATION_KEY:
                memcpy(local_cfm_key, p_prov_factor_in->buf, 16);
                break;
            case BT_MESH_PROV_FACTOR_RANDOM_DEVICE:
                {
                    uint8_t auth_rslt[1] = {1};

                    provisioner_set_auth_result(auth_rslt, 1);
                }
                break;
            case BT_MESH_PROV_FACTOR_CONFIRMATION_DEVICE:
                break;
            }
        }
        break;
    case BT_MESH_EVT_SEQ_CHANGE:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_SEQ_CHANGE", __func__);
        }
        break;
    case BT_MESH_EVT_OTA_EVENT:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_OTA_EVENT", __func__);
        }
        break;
    case BT_MESH_EVT_ADV_REPORT:
        {
            TG_TRACE_DEBUG("%s, BT_MESH_EVT_ADV_REPORT", __func__);
        }
        break;
    default:
        TG_TRACE_DEBUG("%s, bad message id: %x", __func__, evt->evt_id);
        return;
    }

}

void tg_bt_test_mesh_start(void)
{
    int32_t ret;
    uint8_t devkey[16] = {0};

    bt_mesh_prov_register_callback((void *)tg_bt_mesh_prov_event_callback);
    ret = RTOSGenie_mesh_init();
    TG_TRACE_WARNING("%s, RTOSGenie_mesh_init, ret = %d", ret);

    bt_mesh_enable_aggressiveSetting(1);
    ret = RTOSGenie_mesh_provisioner_scan(1, 180000);
    TG_TRACE_WARNING("%s, RTOSGenie_mesh_provisioner_scan, ret = %d", ret);

    ret = bt_mesh_provision(mesh_test_netKey, mesh_test_netKeyIndex, 0, 0, 0, mesh_test_provisionerAddr, devkey);
    TG_TRACE_WARNING("%s, bt_mesh_provision, ret = %d", ret);
    
    ret = app_key_set(mesh_test_netKeyIndex, mesh_test_appkey_index, mesh_test_appkey, false);
    TG_TRACE_WARNING("%s, app_key_set, ret = %d", ret);
}
#endif

/*==================================================================*/
/*========================    common api  ==========================*/
/*==================================================================*/
bool mac_str_to_bin(const int8_t *str, uint8_t *mac)
{
    int32_t i;
    int8_t *s, *e;

    if ((mac == NULL) || (str == NULL)) {
        return false;
    }

    s = (int8_t *) str;
    for (i = 5; i >= 0; i--) {
        mac[i] = s ? strtoul(s, &e, 16) : 0;
        if (s) {
            s = (*e) ? e + 1 : e;
        }
    }
    return true;
}

static bool mac_bin_to_str(int8_t *str, uint8_t *mac)
{
    int32_t i;

    if ((mac == NULL) || (str == NULL)) {
        return false;
    }

    for (i = 5; i >= 0; i--) {
        sprintf((char *)(str + 3 * (5 - i)), "%02X:", mac[i]);
    }

    str[17] = '\0';
    return true;
}

static int char2byte(int8_t ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }

    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' +10;
    }

    return -1;
}

static int bt_hexstr2bytes(int8_t *str, uint8_t *bytes)
{
    int len = 0;

    while (isxdigit(*(str + len)) && isxdigit(*(str + len + 1))) {
        *bytes = char2byte(*(str + len)) * 16 + char2byte(*(str + len + 1));
        len ++;
        str ++;
        bytes ++;
    }

    return len;
}

static char *bt_bytes2hexstr(char *str, size_t str_size, uint8_t *hex, size_t len)
{
    static char * encode = "0123456789ABCDEF";

    size_t i;

    if ((2*len + 1) > str_size)
        return NULL;

    int j = 0;

    for (i = 0; i < len; i++) {
        unsigned char a = ((unsigned char *) hex)[i];
        str[j++] = encode[(a >> 4) & 0xf];
        str[j++] = encode[a & 0xf];
    }

    str[j] = 0;

    return str;
}

/*==================================================================*/
/*========================    GAP Vendor  ==========================*/
/*==================================================================*/
static void _tg_bt_paired_result_cb(uint8_t result, uint8_t *address)
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];
    int32_t pair_result;

    TG_TRACE_DEBUG("_tg_bt_paired_result_cb: result = %02x", result);

    pair_result = TG_BT_PAIR_SUCCESS;
    if (result != YOC_BT_STATUS_SUCCESS)
        pair_result = TG_BT_PAIR_FAIL;

    mac_bin_to_str(bdAddr, address);
    if (g_tg_bt_gapImplCB.gapPairCB) {
        g_tg_bt_gapImplCB.gapPairCB(pair_result, bdAddr);
    }
}

static void _tg_bt_unpaired_result_cb(int result, uint8_t *addr)
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];

    TG_TRACE_DEBUG("_tg_bt_unpaired_result_cb: result = %02x", result);

    mac_bin_to_str(bdAddr, addr);
    if (g_tg_bt_gapImplCB.gapUnpairCB) {
        g_tg_bt_gapImplCB.gapUnpairCB(result, bdAddr);
    }
}

static void _tg_bt_gap_cb(yoc_bt_gap_cb_event_t event, yoc_bt_gap_cb_param_t *param)
{
    switch (event) {
    case YOC_BT_GAP_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.stat == YOC_BT_STATUS_SUCCESS) {
            TG_TRACE_DEBUG("authentication success: %s", param->auth_cmpl.device_name);

            _tg_bt_paired_result_cb(param->auth_cmpl.stat, param->auth_cmpl.bda);
            //LOG_buffer_hex(BT_AV_TAG, param->auth_cmpl.bda, YOC_BD_ADDR_LEN);
        } else {
            TG_TRACE_ERROR("authentication failed, status:%d", param->auth_cmpl.stat);
            
            _tg_bt_paired_result_cb(param->auth_cmpl.stat, param->auth_cmpl.bda);
        }
        break;
    }
    case YOC_BT_GAP_CFM_REQ_EVT:
        TG_TRACE_DEBUG("YOC_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        // YOC_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case YOC_BT_GAP_KEY_NOTIF_EVT:
        TG_TRACE_DEBUG("YOC_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case YOC_BT_GAP_KEY_REQ_EVT:
        TG_TRACE_DEBUG("YOC_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
    case YOC_BT_GAP_READ_RSSI_DELTA_EVT:
        TG_TRACE_DEBUG("rssi delta %d", param->read_rssi_delta.rssi_delta);
        break;
    case YOC_BT_GAP_CONFIG_EIR_DATA_EVT:
        TG_TRACE_DEBUG("EIR DATA %d", param->config_eir_data.stat);
        break;
    default: 
        {
            TG_TRACE_ERROR("unhandled event: %d", event);
            break;
        }
    }
    return;
}

/*==================================================================*/
/*========================   A2DP Vendor  ==========================*/
/*==================================================================*/
static uint8_t _tg_bt_get_lable(void)
{
    static uint8_t lable = 0;
    
    lable ++;
    lable = lable % 15;
    
    return lable;
}

static bool _tg_bt_app_send_msg(bt_app_msg_t *msg)
{
    if (msg == NULL) {
        return false;
    }

    if (hal_os_queue_send(&s_bt_app_task_queue, msg, sizeof(bt_app_msg_t)) != 0) {
        TG_TRACE_ERROR("%s xQueue send failed", __func__);
        return false;
    }
    return true;
}

static void _tg_bt_a2dp_state_cb(TG_BT_A2DP_STATE state, uint8_t *address)
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];

    mac_bin_to_str(bdAddr, address);

    if (g_tg_bt_a2dpConnecting_flag && state == TG_BT_A2DP_DISCONNECTED) {
        state = TG_BT_A2DP_CONNECT_TIMEOUT;
    }
    g_tg_bt_a2dpConnecting_flag = 0;

    if (state == TG_BT_A2DP_CONNECTED) {
        memcpy(g_tg_bt_a2dpConnected_device, address, 6);
    } else {
        memset(g_tg_bt_a2dpConnected_device, 0, 6);
    }

    TG_TRACE_DEBUG("_tg_bt_a2dp_state_cb: address = %s, state = %d", bdAddr, state);
    if (g_tg_bt_a2dpImplCB.a2dpStateCB) {
        g_tg_bt_a2dpImplCB.a2dpStateCB(state, bdAddr);
    }
}

static void _tg_bt_a2dp_stream_cb(TG_BT_A2DP_STREAM_STATE state)
{
    int32_t a2dp_state;
    TG_TRACE_DEBUG("_tg_bt_a2dp_stream_cb: state = %d", state);

    a2dp_state = 0;
    if (state == TG_BT_A2DP_STREAM_STATE_PLAYING)
        a2dp_state = 1;
    tg_bt_hal_vendor_set_a2dp_state(a2dp_state);
    //bt_mesh_set_a2dp_state(a2dp_state);
    if (g_tg_bt_a2dpImplCB.a2dpStreamCB) {
        g_tg_bt_a2dpImplCB.a2dpStreamCB(state);
    }
}

static bool _tg_bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
{
    TG_TRACE_DEBUG("%s event 0x%x, param len %d", __func__, event, param_len);

    bt_app_msg_t msg;
    memset(&msg, 0, sizeof(bt_app_msg_t));

    msg.sig = BT_APP_SIG_WORK_DISPATCH;
    msg.event = event;
    msg.cb = p_cback;
    if (param_len == 0) {
        return _tg_bt_app_send_msg(&msg);
    } else if (p_params && param_len > 0) {
        if ((msg.param = malloc(param_len)) != NULL) {
            memcpy(msg.param, p_params, param_len);
            /* check if caller has provided a copy callback to do the deep copy */
            if (p_copy_cback) {
                p_copy_cback(&msg, msg.param, p_params);
            }
            return _tg_bt_app_send_msg(&msg);
        }
    }

    return false;
}

static void _tg_bt_app_work_dispatched(bt_app_msg_t *msg)
{
    if (msg->cb) {
        msg->cb(msg->event, msg->param);
    }
}

static void _tg_bt_app_task_handler(void *arg)
{
    bt_app_msg_t msg;
    uint32_t len;
    
    for (;;) {
        if (0 == hal_os_queue_recv(&s_bt_app_task_queue, AOS_WAIT_FOREVER, &msg, &len)) {
            TG_TRACE_ERROR("%s, sig 0x%x, 0x%x", __func__, msg.sig, msg.event);
            switch (msg.sig) {
            case BT_APP_SIG_WORK_DISPATCH:
                _tg_bt_app_work_dispatched(&msg);
                break;
            default:
                TG_TRACE_ERROR("%s, unhandled sig: %d", __func__, msg.sig);
                break;
            } // switch (msg.sig)

            if (msg.param) {
                free(msg.param);
            }
        }
    }
}

static void _tg_bt_app_task_start_up(void)
{
    int ret;
    
    ret = hal_os_queue_new(&s_bt_app_task_queue, queue_buf, 40 * sizeof(bt_app_msg_t), sizeof(bt_app_msg_t));
    if (ret) {
        TG_TRACE_ERROR("%s os_queue_new failed %d", __func__, ret);
    }
    ret = hal_os_thread_new(&s_bt_app_task_handle, "BtAppT", _tg_bt_app_task_handler, NULL, 4096, AOS_DEFAULT_APP_PRI);
    if (ret) {
        TG_TRACE_ERROR("%s os_task_new_ext failed %d", __func__, ret);
    }
    return;
}

/* callback for A2DP sink */
static void  _tg_bt_av_hdl_a2d_evt(uint16_t event, void *p_param)
{
    yoc_a2d_cb_param_t *a2d = NULL;

    TG_TRACE_DEBUG("%s evt %d", __func__, event);
    switch (event) {
    case YOC_A2D_CONNECTION_STATE_EVT: 
        {
            uint8_t *bda;
            
            a2d = (yoc_a2d_cb_param_t *)(p_param);
            bda = a2d->conn_stat.remote_bda;
            TG_TRACE_DEBUG("A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            if (a2d->conn_stat.state == YOC_A2D_CONNECTION_STATE_DISCONNECTED) {
                if (a2d->conn_stat.disc_rsn == YOC_A2D_DISC_RSN_NORMAL) {
                    _tg_bt_a2dp_state_cb(TG_BT_A2DP_DISCONNECTED, bda);
                } else {
                    _tg_bt_a2dp_state_cb(TG_BT_A2DP_LINK_LOSS, bda);
                }
            } else if (a2d->conn_stat.state == YOC_A2D_CONNECTION_STATE_CONNECTED){
                _tg_bt_a2dp_state_cb(TG_BT_A2DP_CONNECTED, bda);
            } else if (a2d->conn_stat.state == YOC_A2D_CONNECTION_STATE_CONNECTING) {
                _tg_bt_a2dp_state_cb(TG_BT_A2DP_CONNECTING, bda);
            }
            break;
        }
    case YOC_A2D_AUDIO_STATE_EVT: 
        {
            a2d = (yoc_a2d_cb_param_t *)(p_param);
            
            TG_TRACE_DEBUG("A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
            g_tg_bt_audio_state = a2d->audio_stat.state;
            if (YOC_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
                _tg_bt_a2dp_stream_cb(TG_BT_A2DP_STREAM_STATE_PLAYING);
            } else if (YOC_A2D_AUDIO_STATE_REMOTE_SUSPEND == a2d->audio_stat.state ||
                    YOC_A2D_AUDIO_STATE_STOPPED == a2d->audio_stat.state) {
                _tg_bt_a2dp_stream_cb(TG_BT_A2DP_STREAM_STATE_STOP);
            }
            
            break;
        }
    case YOC_A2D_AUDIO_CFG_EVT: 
        {
            a2d = (yoc_a2d_cb_param_t *)(p_param);
            
            TG_TRACE_DEBUG("A2DP audio stream configuration, codec type %d", a2d->audio_cfg.mcc.type);
            // for now only SBC stream is supported
            if (a2d->audio_cfg.mcc.type == YOC_A2D_MCT_SBC) {
                TG_TRACE_DEBUG("Configure audio player %x-%x-%x-%x",
                         a2d->audio_cfg.mcc.cie.sbc[0],
                         a2d->audio_cfg.mcc.cie.sbc[1],
                         a2d->audio_cfg.mcc.cie.sbc[2],
                         a2d->audio_cfg.mcc.cie.sbc[3]);
            }
            break;
        }
    default:
        TG_TRACE_ERROR("%s unhandled evt %d", __func__, event);
        break;
    }
}

static void _tg_bt_app_a2d_cb(yoc_a2d_cb_event_t event, yoc_a2d_cb_param_t *param)
{
    switch (event) {
    case YOC_A2D_CONNECTION_STATE_EVT:
    case YOC_A2D_AUDIO_STATE_EVT:
    case YOC_A2D_AUDIO_CFG_EVT: 
        {
            _tg_bt_app_work_dispatch(_tg_bt_av_hdl_a2d_evt, event, param, sizeof(yoc_a2d_cb_param_t), NULL);
            break;
        }
    default:
        TG_TRACE_ERROR("Invalid A2DP event: %d", event);
        break;
    }
}

static void _tg_bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    static uint32_t s_pkt_cnt = 0;

    if (++s_pkt_cnt % 100 == 0) {
        TG_TRACE_DEBUG("Audio packet count %u", s_pkt_cnt);
    }
}

static void _tg_bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    TG_TRACE_DEBUG("%s evt %d", __func__, event);
    switch (event) {
    case 0: {
#if BTC_AV_INCLUDED
        /* initialize A2DP sink */
        yoc_a2d_register_callback(_tg_bt_app_a2d_cb);
        yoc_a2d_sink_register_data_callback(_tg_bt_app_a2d_data_cb);
        yoc_a2d_sink_init();

        /* initialize AVRCP controller */
        yoc_avrc_ct_init();
        yoc_avrc_ct_register_callback(_tg_bt_app_avrcp_ct_cb);

        /* initialize AVRCP controller */
        {
            yoc_avrc_rn_evt_cap_mask_t evt_set;
            
            yoc_avrc_tg_init();
            yoc_avrc_tg_register_callback(_tg_bt_app_avrcp_tg_cb);
        
            /* volume sync setup */
            evt_set.bits = 0x2000;
            yoc_avrc_tg_set_rn_evt_cap(&evt_set);
        }
#endif
        /* set discoverable and connectable mode, wait to be connected */
        // yoc_bt_gap_set_scan_mode(YOC_BT_CONNECTABLE, YOC_BT_GENERAL_DISCOVERABLE);
        break;
    }
    default:
        TG_TRACE_ERROR("%s unhandled evt %d", __func__, event);
        break;
    }
}

static void _tg_bt_a2dp_init()
{
    /* Bluetooth device name, connection mode and profile set up */
    _tg_bt_app_work_dispatch(_tg_bt_av_hdl_stack_evt, 0, NULL, 0, NULL);
}

/*==================================================================*/
/*========================   AVRCP Vendor  ==========================*/
/*==================================================================*/
static void _tg_bt_avrcp_state_cb(TG_BT_AVRCP_STATUS state)
{
    TG_TRACE_DEBUG("_tg_bt_avrcp_state_cb: state = %d", state);
    
    if (g_tg_bt_avrcpImplCB.avrcpStateCB) {
        g_tg_bt_avrcpImplCB.avrcpStateCB(state);
    }
}

static void _tg_bt_avrcp_play_state_cb(TG_BT_AVRCP_PLAY_STATUS state)
{
    TG_TRACE_DEBUG("_tg_bt_avrcp_play_state_cb: state = %d", state);

    if (state == TG_BT_AVRCP_PLAY_STATUS_STOPPED || state == TG_BT_AVRCP_PLAY_STATUS_PAUSEED) {
#if 0
        tg_bt_a2dpStopPlayer();
#else
        btc_a2dp_sink_rx_flush_req();
#endif
    }
    if (g_tg_bt_avrcpImplCB.avrcpPlayStateCB) {
        g_tg_bt_avrcpImplCB.avrcpPlayStateCB(state);
    }
}

static void _tg_bt_avrcp_volume_chg_cb(TG_BT_AVRCP_VOLUME_CHANGE dir)
{
    TG_TRACE_DEBUG("_tg_bt_avrcp_volume_chg_cb: dir = %d", dir);
    
    if (g_tg_bt_avrcpImplCB.volumeChangeCB) {
        g_tg_bt_avrcpImplCB.volumeChangeCB(dir);
    }
}

static void _tg_bt_avrcp_abs_volume_cb(uint32_t vol)
{
    TG_TRACE_DEBUG("_tg_bt_avrcp_abs_volume_cb: vol = %d", vol);
    
    if (g_tg_bt_avrcpImplCB.absoluteVolumeCB) {
        g_tg_bt_avrcpImplCB.absoluteVolumeCB(vol);
    }
}

static void _tg_bt_avrcp_passthr_cb(TG_BT_AVRCP_CMD_TYPE cmd)
{
    TG_TRACE_DEBUG("_tg_bt_avrcp_passthr_cb: cmd = %d", cmd);
    
    if (g_tg_bt_avrcpImplCB.avrcpCmdSrcCB) {
        g_tg_bt_avrcpImplCB.avrcpCmdSrcCB(cmd);
    }
}

static void _tg_bt_avrcp_notify_evt_handler(uint8_t event_id, yoc_avrc_rn_param_t *event_parameter)
{
    TG_TRACE_DEBUG("%s %d", __func__, event_id);
    
    switch (event_id) {
    case YOC_AVRC_RN_TRACK_CHANGE:

        break;
    case YOC_AVRC_RN_VOLUME_CHANGE:
        /* FIXME */
        _tg_bt_avrcp_volume_chg_cb(event_parameter->volume);
        break;
    case YOC_AVRC_RN_PLAY_STATUS_CHANGE: 
        {
            TG_BT_AVRCP_PLAY_STATUS   status;
            
            switch (event_parameter->playback) {
            case YOC_AVRC_PLAYBACK_STOPPED:
                status = TG_BT_AVRCP_PLAY_STATUS_STOPPED;
                break;
            case YOC_AVRC_PLAYBACK_PLAYING:
                status = TG_BT_AVRCP_PLAY_STATUS_PLAYING;
                break;
            case YOC_AVRC_PLAYBACK_PAUSED:
                status = TG_BT_AVRCP_PLAY_STATUS_PAUSEED;
                break;
            default:
                return;
            }
            _tg_bt_avrcp_play_state_cb(status);
            break;
        }
    }
}

static void _tg_bt_av_hdl_avrcp_ct_evt(uint16_t event, void *p_param)
{
    yoc_avrc_ct_cb_param_t *rc = (yoc_avrc_ct_cb_param_t *)(p_param);

    TG_TRACE_WARNING("%s evt %d", __func__, event);
    switch (event) {
    case YOC_AVRC_CT_CONNECTION_STATE_EVT: 
        {
            uint8_t *bda = rc->conn_stat.remote_bda;
            TG_TRACE_DEBUG("AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    
            if (rc->conn_stat.connected) {
                _tg_bt_avrcp_state_cb(TG_BT_AVRCP_CONNECTED);
                yoc_avrc_ct_send_get_rn_capabilities_cmd(0x00);
            } else {
                _tg_bt_avrcp_state_cb(TG_BT_AVRCP_DISCONNECTED);
            }
            break;
        }
    case YOC_AVRC_CT_PASSTHROUGH_RSP_EVT: 
        {
            TG_BT_AVRCP_CMD_TYPE cmd;
            
            TG_TRACE_DEBUG("AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
            switch (rc->psth_rsp.key_code){
            case YOC_AVRC_PT_CMD_PLAY:
                cmd = TG_BT_AVRCP_CMD_TYPE_PLAY;
                break;
            case YOC_AVRC_PT_CMD_PAUSE :
                cmd = TG_BT_AVRCP_CMD_TYPE_PAUSE;
                break;
            case YOC_AVRC_PT_CMD_FORWARD :
                cmd = TG_BT_AVRCP_CMD_TYPE_FWD;
                break;
            case YOC_AVRC_PT_CMD_BACKWARD :
                cmd = TG_BT_AVRCP_CMD_TYPE_BWD;
                break;
            case YOC_AVRC_PT_CMD_FAST_FORWARD :
                cmd = TG_BT_AVRCP_CMD_TYPE_FFWD;
                break;
            case YOC_AVRC_PT_CMD_REWIND :
                cmd = TG_BT_AVRCP_CMD_TYPE_RWD;
                break;
            case YOC_AVRC_PT_CMD_STOP :
                cmd = TG_BT_AVRCP_CMD_TYPE_STOP;
                break;
            case YOC_AVRC_PT_CMD_VOL_UP :
                cmd = TG_BT_AVRCP_CMD_TYPE_VOL_UP;
                break;
            case YOC_AVRC_PT_CMD_VOL_DOWN :
                cmd = TG_BT_AVRCP_CMD_TYPE_VOL_DOWN;
                break;
            default:
                return;
            }
            _tg_bt_avrcp_passthr_cb(cmd);
            break;
        }
    case YOC_AVRC_CT_METADATA_RSP_EVT: 
        {
            TG_TRACE_DEBUG("AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
            free(rc->meta_rsp.attr_text);
            break;
        }
    case YOC_AVRC_CT_CHANGE_NOTIFY_EVT: 
        {
            TG_TRACE_DEBUG("AVRC event notification: %d, param: %d", rc->change_ntf.event_id, rc->change_ntf.event_parameter);
            _tg_bt_avrcp_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
            break;
        }
    case YOC_AVRC_CT_REMOTE_FEATURES_EVT: 
        {
            TG_TRACE_DEBUG("AVRC remote features %x", rc->rmt_feats.feat_mask);
            break;
        }
    case YOC_AVRC_CT_PLAY_STATUS_RSP_EVT: {
        TG_TRACE_DEBUG("AVRC PLAYER features %x", rc->rmt_feats.feat_mask);
    }
    case YOC_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: 
        {
            TG_TRACE_DEBUG("AVRC VOLUME set %d", rc->set_volume_rsp.volume);
            _tg_bt_avrcp_abs_volume_cb(rc->set_volume_rsp.volume);
        }
    default:
        TG_TRACE_ERROR("%s unhandled evt %d", __func__, event);
        break;
    }
}

static void _tg_bt_av_hdl_avrcp_tg_evt(uint16_t event, void *p_param)
{
    yoc_avrc_tg_cb_param_t *rc = (yoc_avrc_tg_cb_param_t *)(p_param);

    TG_TRACE_WARNING("%s evt %d", __func__, event);
#if BTC_AV_INCLUDED
    switch (event) {
    case YOC_AVRC_TG_REGISTER_NOTIFICATION_EVT: 
        {
            if (rc->reg_ntf.event_id == YOC_AVRC_RN_VOLUME_CHANGE) {
                yoc_avrc_rn_param_t param;

                param.volume = g_tg_bt_avrcp_volumesync;
                yoc_avrc_tg_send_rn_rsp(YOC_AVRC_RN_VOLUME_CHANGE, YOC_AVRC_RN_RSP_INTERIM, &param);
            }
        }
        break;
    case YOC_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
        {
            _tg_bt_avrcp_abs_volume_cb(rc->set_abs_vol.volume);
        }
        break;
    }
#endif
}

static void _tg_bt_app_alloc_meta_buffer(yoc_avrc_ct_cb_param_t *param)
{
    yoc_avrc_ct_cb_param_t *rc = (yoc_avrc_ct_cb_param_t *)(param);
    uint8_t *attr_text;
    
    attr_text = (uint8_t *)malloc(rc->meta_rsp.attr_length + 1);
    if (attr_text == NULL) {
        TG_TRACE_DEBUG("attr_text malloc failed");
        return;
    }
    memcpy(attr_text, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
    attr_text[rc->meta_rsp.attr_length] = 0;

    rc->meta_rsp.attr_text = attr_text;
}

void _tg_bt_app_avrcp_ct_cb(yoc_avrc_ct_cb_event_t event, yoc_avrc_ct_cb_param_t *param)
{
    TG_TRACE_DEBUG("%s: %d", __func__, event);
    switch (event) {
    case YOC_AVRC_CT_METADATA_RSP_EVT:
        _tg_bt_app_alloc_meta_buffer(param);
        /* fall through */
    case YOC_AVRC_CT_CONNECTION_STATE_EVT:
    case YOC_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case YOC_AVRC_CT_CHANGE_NOTIFY_EVT:
    case YOC_AVRC_CT_REMOTE_FEATURES_EVT:
    case YOC_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
    case YOC_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: 
        {
            _tg_bt_app_work_dispatch(_tg_bt_av_hdl_avrcp_ct_evt, event, param, sizeof(yoc_avrc_ct_cb_param_t), NULL);
            break;
        }
    default:
        TG_TRACE_ERROR("Invalid AVRC CT event: %d", event);
        break;
    }
}

void _tg_bt_app_avrcp_tg_cb(yoc_avrc_tg_cb_event_t event, yoc_avrc_tg_cb_param_t *param)
{
    TG_TRACE_DEBUG("%s: %d", __func__, event);
    switch (event) {
    case YOC_AVRC_TG_REGISTER_NOTIFICATION_EVT:
    case YOC_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
        {
            _tg_bt_app_work_dispatch(_tg_bt_av_hdl_avrcp_tg_evt, event, param, sizeof(yoc_avrc_tg_cb_param_t), NULL);
        }
        break;
    default:
        TG_TRACE_ERROR("Invalid AVRC TG event: %d", event);
        break;
    }
}
/*==================================================================*/
/*========================   GATTS Vendor ==========================*/
/*==================================================================*/
void _tg_bt_gatts_init_cb(INT32 server_if)
{
    TG_TRACE_DEBUG("%s: server_if = %d", __func__, server_if);
    if (g_tg_bt_gattServerImplCB.gattsInitCB) {
        g_tg_bt_gattServerImplCB.gattsInitCB(server_if);
    }
}

void _tg_bt_gatts_add_service_cb(TG_BT_GATTS_ADD_SRVC_RST_T *bt_gatts_add_srvc)
{
    TG_TRACE_DEBUG("%s: %d-%d-%d-%d", __func__, 
                    bt_gatts_add_srvc->server_if,
                    bt_gatts_add_srvc->srvc_id.id.inst_id,
                    bt_gatts_add_srvc->srvc_id.is_primary,
                    bt_gatts_add_srvc->srvc_handle);

    if (g_tg_bt_gattServerImplCB.gattsAddServiceCB) {
        g_tg_bt_gattServerImplCB.gattsAddServiceCB(bt_gatts_add_srvc);
    }
}

void _tg_bt_gatts_add_char_cb(TG_BT_GATTS_ADD_CHAR_RST_T *bt_gatts_add_char)
{
    TG_TRACE_DEBUG("%s: %d-%d-%d", __func__,
                    bt_gatts_add_char->server_if,
                    bt_gatts_add_char->srvc_handle,
                    bt_gatts_add_char->char_handle);

    if (g_tg_bt_gattServerImplCB.gattsAddCharCB) {
        g_tg_bt_gattServerImplCB.gattsAddCharCB(bt_gatts_add_char);
    }
}

void _tg_bt_gatts_add_desc_cb(TG_BT_GATTS_ADD_DESCR_RST_T *bt_gatts_add_desc)
{
    TG_TRACE_DEBUG("%s: %d-%d-%d", __func__,
                    bt_gatts_add_desc->server_if,
                    bt_gatts_add_desc->srvc_handle,
                    bt_gatts_add_desc->descr_handle);

    if (g_tg_bt_gattServerImplCB.gattsAddDescCB) {
        g_tg_bt_gattServerImplCB.gattsAddDescCB(bt_gatts_add_desc);
    }
}

void _tg_bt_gatts_reg_write_cb(TG_BT_GATTS_REQ_WRITE_RST_T *bt_gatts_req_write)
{
    TG_TRACE_DEBUG("%s: %d, %d, %d, %d, %d, %d, %d", __func__,
                    bt_gatts_req_write->conn_id,
                    bt_gatts_req_write->attr_handle,
                    bt_gatts_req_write->offset, bt_gatts_req_write->length,
                    bt_gatts_req_write->need_rsp, bt_gatts_req_write->is_prep);

    if (g_tg_bt_gattServerImplCB.gattsReqWriteCB) {
        g_tg_bt_gattServerImplCB.gattsReqWriteCB(bt_gatts_req_write);
    }
}

void _tg_bt_gatts_reg_read_cb(TG_BT_GATTS_REQ_READ_RST_T *bt_gatts_req_read)
{
    TG_TRACE_DEBUG("%s: %d-%d--%d-%d-%d", __func__,
                    bt_gatts_req_read->conn_id, bt_gatts_req_read->trans_id,
                    bt_gatts_req_read->attr_handle, bt_gatts_req_read->offset,
                    bt_gatts_req_read->is_long);

    if (g_tg_bt_gattServerImplCB.gattsReqReadCB) {
        g_tg_bt_gattServerImplCB.gattsReqReadCB(bt_gatts_req_read);
    }
}

void _tg_bt_gatt_conn_evt_cb(TG_BT_GATTS_EVENT_T bt_gatts_connection_evt, uint16_t conn_id)
{
    TG_TRACE_DEBUG("%s: %d", __func__, bt_gatts_connection_evt);

    if (g_tg_bt_gattServerImplCB.gattsConnectionEventCB) {
        g_tg_bt_gattServerImplCB.gattsConnectionEventCB(bt_gatts_connection_evt, conn_id);
    }
}

void _tg_bt_gatts_start_server_cb(void)
{
    TG_TRACE_DEBUG("%s", __func__);

    if (g_tg_bt_gattServerImplCB.gattsStartServerCB) {
        g_tg_bt_gattServerImplCB.gattsStartServerCB();
    }
}

void _tg_bt_gatts_stop_server_cb(void)
{
    TG_TRACE_DEBUG("%s", __func__);

    if (g_tg_bt_gattServerImplCB.gattsStopServerCB) {
        g_tg_bt_gattServerImplCB.gattsStopServerCB();
    }
}

void _tg_bt_gatts_del_server_cb(void)
{
    TG_TRACE_DEBUG("%s", __func__);

    if (g_tg_bt_gattServerImplCB.gattsDeleteServerCB) {
        g_tg_bt_gattServerImplCB.gattsDeleteServerCB();
    }
}

void _tg_bt_gattc_adv_enable_cb(int status)
{
    TG_TRACE_DEBUG("%s %d", __func__, status);

    if (g_tg_bt_gattClientImplCB.gattcAdvEnabledCB != NULL) {
        g_tg_bt_gattClientImplCB.gattcAdvEnabledCB(status);
    }
}

void _tg_bt_gatts_event_handler(yoc_gatts_cb_event_t event, yoc_gatt_if_t gatts_if, yoc_ble_gatts_cb_param_t *param)
{
    TG_TRACE_DEBUG("%s, %d", __func__, event);

    switch (event) {
    case YOC_GATTS_REG_EVT:
        TG_TRACE_DEBUG("REGISTER_APP_EVT, status %d, app_id %d, gatts_if %d\n", param->reg.status, param->reg.app_id, gatts_if);
        g_server_if = gatts_if;
        yoc_ble_gap_config_local_privacy(true);
        _tg_bt_gatts_init_cb(g_server_if);
        break;
    case YOC_GATTS_READ_EVT: 
        {
            TG_BT_GATTS_REQ_READ_RST_T req;

            TG_TRACE_DEBUG("GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);

            req.attr_handle = param->read.handle;
            mac_bin_to_str(req.btaddr, param->read.bda);
            req.conn_id = param->read.conn_id;
            req.is_long = param->read.is_long;
            req.offset = param->read.offset;
            req.trans_id = param->read.trans_id;

            _tg_bt_gatts_reg_read_cb(&req);

            break;
        }
    case YOC_GATTS_WRITE_EVT: 
        {
            TG_BT_GATTS_REQ_WRITE_RST_T req;
            
            TG_TRACE_DEBUG("GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);

            req.attr_handle = param->write.handle;
            mac_bin_to_str(req.btaddr, param->write.bda);
            req.conn_id = param->write.conn_id;
            req.attr_handle = param->write.handle;
            req.need_rsp = param->write.need_rsp;
            req.trans_id = param->write.trans_id;
            req.offset = param->write.offset;
    
            req.length = param->write.len;
            memcpy(req.value, param->write.value, param->write.len);
    
            _tg_bt_gatts_reg_write_cb(&req);
    
            break;
        }
    case YOC_GATTS_EXEC_WRITE_EVT:
        TG_TRACE_DEBUG("YOC_GATTS_EXEC_WRITE_EVT");
        // TG_BT_GATTS_REQ_WRITE_RST_T req;
        // req.attr_handle = param->write.handle;
        // mac_bin_to_str(req.btaddr, param->write.bda);
        // req.conn_id = param->write.conn_id;
        // req.attr_handle = param->write.handle;
        // req.need_rsp = param->write.need_rsp;
        // req.trans_id = param->write.trans_id;
        // req.offset = param->write.offset;

        // req.length = param->write.len;
        // memcpy(req.value, param->write.value, param->write.len);

        // _tg_bt_gatts_reg_write_cb(&req);
        break;
    case YOC_GATTS_MTU_EVT:
        TG_TRACE_DEBUG("YOC_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case YOC_GATTS_UNREG_EVT:
        TG_TRACE_DEBUG("YOC_GATTS_UNREG_EVT");
        break;
    case YOC_GATTS_CREATE_EVT: 
        {
            TG_BT_GATTS_ADD_SRVC_RST_T req;
            uint8_t uuid_tmp[YOC_UUID_LEN_128];
            
            TG_TRACE_DEBUG("CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
            
            req.server_if = g_server_if;
            req.srvc_handle = param->create.service_handle;
            req.srvc_id.id.inst_id = param->create.service_id.id.inst_id;
            req.srvc_id.is_primary = param->create.service_id.is_primary;
    
            switch (param->create.service_id.id.uuid.len) {
            case YOC_UUID_LEN_16:
                T_UINT16_TO_STREAM(uuid_tmp, param->create.service_id.id.uuid.uuid.uuid16);
                break;
            case YOC_UUID_LEN_32:
                T_UINT32_TO_STREAM(uuid_tmp, param->create.service_id.id.uuid.uuid.uuid32);
                break;
            case YOC_UUID_LEN_128:
                memcpy(uuid_tmp,param->create.service_id.id.uuid.uuid.uuid128, YOC_UUID_LEN_128);
                break;
            default:
                return;
            }
    
            bt_bytes2hexstr((char *)req.srvc_id.id.uuid, TG_BT_GATT_MAX_UUID_LEN, uuid_tmp,
                            param->create.service_id.id.uuid.len);
            _tg_bt_gatts_add_service_cb(&req);
            break;
        }
    case YOC_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case YOC_GATTS_ADD_CHAR_EVT: 
        {
            TG_BT_GATTS_ADD_CHAR_RST_T req;
            uint8_t uuid_tmp[YOC_UUID_LEN_128];

            TG_TRACE_DEBUG("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                    param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

            req.char_handle = param->add_char.attr_handle;
            req.server_if = g_server_if;
            req.srvc_handle = param->add_char.service_handle;

            switch (param->add_char.char_uuid.len) {
            case YOC_UUID_LEN_16:
                T_UINT16_TO_STREAM(uuid_tmp, param->add_char.char_uuid.uuid.uuid16);
                break;
            case YOC_UUID_LEN_32:
                T_UINT32_TO_STREAM(uuid_tmp, param->add_char.char_uuid.uuid.uuid32);
                break;
            case YOC_UUID_LEN_128:
                memcpy(uuid_tmp, param->add_char.char_uuid.uuid.uuid128, YOC_UUID_LEN_128);
                break;
            default:
                return;
            }
    
            bt_bytes2hexstr((char *)req.uuid, TG_BT_GATT_MAX_UUID_LEN,
                        uuid_tmp,
                        param->add_char.char_uuid.len);
    
            _tg_bt_gatts_add_char_cb(&req);
    
            break;
        }
    case YOC_GATTS_ADD_CHAR_DESCR_EVT: 
        {
            TG_BT_GATTS_ADD_DESCR_RST_T req;
            uint8_t uuid_tmp[YOC_UUID_LEN_128];

            TG_TRACE_DEBUG("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                     param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);

            req.server_if = g_server_if;
            req.srvc_handle = param->add_char_descr.service_handle;
            req.descr_handle = param->add_char_descr.attr_handle;

            switch (param->add_char_descr.descr_uuid.len) {
            case YOC_UUID_LEN_16:
                T_UINT16_TO_STREAM(uuid_tmp, param->add_char_descr.descr_uuid.uuid.uuid16);
                break;
            case YOC_UUID_LEN_32:
                T_UINT32_TO_STREAM(uuid_tmp, param->add_char_descr.descr_uuid.uuid.uuid32);
                break;
            case YOC_UUID_LEN_128:
                memcpy(uuid_tmp, param->add_char_descr.descr_uuid.uuid.uuid128, YOC_UUID_LEN_128);
                break;
            default:
                return;
            }
    
            bt_bytes2hexstr((char *)req.uuid, TG_BT_GATT_MAX_UUID_LEN,
                     uuid_tmp,
                     param->add_char_descr.descr_uuid.len);
    
            _tg_bt_gatts_add_desc_cb(&req);
            break;
        }
    case YOC_GATTS_DELETE_EVT:
        _tg_bt_gatts_del_server_cb();
        break;
    case YOC_GATTS_START_EVT:
        TG_TRACE_DEBUG("SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        _tg_bt_gatts_start_server_cb();
        break;
    case YOC_GATTS_STOP_EVT:
        _tg_bt_gatts_stop_server_cb();
        break;
    case YOC_GATTS_CONNECT_EVT: 
        {
            yoc_ble_conn_update_params_t conn_params = {0};

            TG_TRACE_DEBUG("YOC_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                     param->connect.conn_id,
                     param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                     param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);            
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(yoc_bd_addr_t));
            /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            _tg_bt_gatt_conn_evt_cb(TG_BT_GATTS_CONNECT, param->connect.conn_id);
    
            break;
        }
    case YOC_GATTS_DISCONNECT_EVT:
        TG_TRACE_DEBUG("YOC_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);

        _tg_bt_gatt_conn_evt_cb(TG_BT_GATTS_DISCONNECT, param->disconnect.conn_id);
        _tg_bt_gattc_adv_enable_cb(0);

        break;
    case YOC_GATTS_CONF_EVT:
        TG_TRACE_DEBUG("YOC_GATTS_CONF_EVT, status %d", param->conf.status);

        break;
    case YOC_GATTS_OPEN_EVT:
    case YOC_GATTS_CANCEL_OPEN_EVT:
    case YOC_GATTS_CLOSE_EVT:
    case YOC_GATTS_LISTEN_EVT:
    case YOC_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

static int g_tg_gatt_client_if = 30;
static uint16_t g_tg_bt_client_conn_id = 0;

void _tg_bt_gattc_event_handler(yoc_gattc_cb_event_t event, yoc_gatt_if_t gattc_if, yoc_ble_gattc_cb_param_t *param)
{
    TG_TRACE_DEBUG("[%s] recieved event.%d\n", __func__, event);
    switch (event) {
        case YOC_GATTC_REG_EVT:
        {
            if (gattc_if != 0) {
                if (param->reg.status == 0) {
                    g_tg_gatt_client_if = gattc_if;
                    if(g_tg_bt_gattClientImplCB.gattcInitedCB) {
                        g_tg_bt_gattClientImplCB.gattcInitedCB(g_tg_gatt_client_if);
                    }
                }
            }
            break;
        }
        case YOC_GATTC_CONNECT_EVT:
        {
            g_tg_bt_client_conn_id = param->connect.conn_id;
            break;
        }
        case YOC_GATTC_OPEN_EVT:
        {
            if (param->open.status == 0
                && g_tg_bt_gattClientImplCB.gattcGattEventCB) {
                g_tg_bt_gattClientImplCB.gattcGattEventCB(g_tg_bt_client_conn_id
                                                         , (signed char*)param->open.remote_bda
                                                         , TG_BT_GATT_EVENT_OPEN, (void*)(&(param->open)));
            }
            break;
        }
        case YOC_GATTC_CLOSE_EVT:
        {
            break;
        }
        case YOC_GATTC_DISCONNECT_EVT:
        {
            if (g_tg_bt_gattClientImplCB.gattcGattEventCB) {
                g_tg_bt_gattClientImplCB.gattcGattEventCB(g_tg_bt_client_conn_id
                                                        , (signed char*)param->disconnect.remote_bda
                                                        , TG_BT_GATT_EVENT_DISCONNECT, NULL);
            }
            break;
        }
        case YOC_GATTC_WRITE_DESCR_EVT:
        {
            if (g_tg_bt_gattClientImplCB.gattcCnDescriptorWriteCB) {
                g_tg_bt_gattClientImplCB.gattcCnDescriptorWriteCB(param->write.conn_id
                                                                , param->write.handle
                                                                , param->write.status);
            }
            break;
        }
        case YOC_GATTC_CFG_MTU_EVT:
        {
            struct gattc_cfg_mtu_evt_param* cfg_mtu = &(param->cfg_mtu);
            if (g_tg_bt_gattClientImplCB.gattcGattEventCB) {
                g_tg_bt_gattClientImplCB.gattcGattEventCB(cfg_mtu->conn_id, NULL
                                                            , TG_BT_GATT_EVENT_MTU_EXCHGED, (void*)cfg_mtu);
            }
        }
        case YOC_GATTC_WRITE_CHAR_EVT:
        {
            if (g_tg_bt_gattClientImplCB.gattcCharacteristicWriteCB) {
                g_tg_bt_gattClientImplCB.gattcCharacteristicWriteCB(param->write.conn_id
                                                                    , param->write.handle
                                                                    , param->write.status);
            }
            break;
        }
        case YOC_GATTC_NOTIFY_EVT:
        {
            if (g_tg_bt_gattClientImplCB.gattcCCharacteristicChangedCB) {
                g_tg_bt_gattClientImplCB.gattcCCharacteristicChangedCB(param->notify.conn_id
                                                                    , param->notify.handle
                                                                    , (signed char*)param->notify.remote_bda
                                                                    , param->notify.value
                                                                    , param->notify.value_len
                                                                    , param->notify.is_notify);
            }
            break;
        }
        case YOC_GATTC_DIS_SRVC_CMPL_EVT:
        {
            if (g_tg_bt_gattClientImplCB.gattcServiceDiscoveredCB) {
                g_tg_bt_gattClientImplCB.gattcServiceDiscoveredCB(param->dis_srvc_cmpl.conn_id
                                                                , param->dis_srvc_cmpl.status);
            } else {
                TG_TRACE_ERROR("YOC_GATTC_DIS_SRVC_CMPL_EVT callback invaild!");
            }
            break;
        }
        case YOC_GATTC_SEARCH_CMPL_EVT:
        case YOC_GATTC_SEARCH_RES_EVT:
        case YOC_GATTC_UNREG_EVT:
        case YOC_GATTC_READ_CHAR_EVT:
        case YOC_GATTC_READ_DESCR_EVT:
        case YOC_GATTC_PREP_WRITE_EVT:
        case YOC_GATTC_EXEC_EVT:
        case YOC_GATTC_ACL_EVT:
        case YOC_GATTC_CANCEL_OPEN_EVT:
        case YOC_GATTC_SRVC_CHG_EVT:
        case YOC_GATTC_ENC_CMPL_CB_EVT:
        case YOC_GATTC_ADV_DATA_EVT:
        case YOC_GATTC_MULT_ADV_ENB_EVT:
        case YOC_GATTC_MULT_ADV_UPD_EVT:
        case YOC_GATTC_MULT_ADV_DATA_EVT:
        case YOC_GATTC_MULT_ADV_DIS_EVT:
        case YOC_GATTC_CONGEST_EVT:
        case YOC_GATTC_BTH_SCAN_ENB_EVT:
        case YOC_GATTC_BTH_SCAN_CFG_EVT:
        case YOC_GATTC_BTH_SCAN_RD_EVT:
        case YOC_GATTC_BTH_SCAN_THR_EVT:
        case YOC_GATTC_BTH_SCAN_PARAM_EVT:
        case YOC_GATTC_BTH_SCAN_DIS_EVT:
        case YOC_GATTC_SCAN_FLT_CFG_EVT:
        case YOC_GATTC_SCAN_FLT_PARAM_EVT:
        case YOC_GATTC_SCAN_FLT_STATUS_EVT:
        case YOC_GATTC_ADV_VSC_EVT:
        case YOC_GATTC_REG_FOR_NOTIFY_EVT:
        case YOC_GATTC_UNREG_FOR_NOTIFY_EVT:
        case YOC_GATTC_READ_MULTIPLE_EVT:
        case YOC_GATTC_QUEUE_FULL_EVT:
        case YOC_GATTC_SET_ASSOC_EVT:
        case YOC_GATTC_GET_ADDR_LIST_EVT:
        default:
        {
            break;
        }
    }
}

//bt_err_t yoc_ble_gattc_app_unregister(yoc_gatt_if_t gattc_if);
//bt_err_t yoc_ble_gattc_close (yoc_gatt_if_t gattc_if, uint16_t conn_id);
void yoc_ble_gap_open_gattc(uint8_t* remote_bda, int type) {
    TG_TRACE_DEBUG("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", __func__
                                                , remote_bda[0]
                                                , remote_bda[1]
                                                , remote_bda[2]
                                                , remote_bda[3]
                                                , remote_bda[4]
                                                , remote_bda[5]);
    yoc_ble_gattc_open(g_tg_gatt_client_if, remote_bda, type, 1);
}

static void _tg_bt_ble_gap_event_handler(yoc_gap_ble_cb_event_t event, yoc_ble_gap_cb_param_t *param)
{
//    TG_TRACE_DEBUG("%s, event %d", __func__, event);
    
    switch (event) {
    case YOC_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        if (g_tg_bt_gatt_set_adv_data_flag) {
            g_tg_bt_gatt_set_adv_data_flag = 0;
            g_tg_bt_gatt_adv_pending = 1;
            yoc_ble_gap_start_advertising(&_tg_bt_adv_params);
        }
        break;
    case YOC_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        if (g_tg_bt_gatt_set_adv_data_flag) {
            g_tg_bt_gatt_set_adv_data_flag = 0;
            g_tg_bt_gatt_adv_pending = 1;
            yoc_ble_gap_start_advertising(&_tg_bt_adv_params);
        }
        break;
    case YOC_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:

        break;
    case YOC_GAP_BLE_SCAN_RESULT_EVT:
    {
        /*
        if (tg_bt_mesh_scan_is_enabled()) {
            bt_mesh_scan_result_handle(param);
        }
        */
        int rssi = param->scan_rst.rssi;
        uint8_t * adv = (uint8_t *)(param->scan_rst.ble_adv);
        const int8_t * bda = (const int8_t *)(param->scan_rst.bda);
        int len = param->scan_rst.adv_data_len + param->scan_rst.scan_rsp_len;
        uint8_t max_len = YOC_BLE_ADV_DATA_LEN_MAX + YOC_BLE_SCAN_RSP_DATA_LEN_MAX;
        if(len == 0) {
            break;
        } else if (len > max_len) {
            len = max_len;
        }
        if (g_tg_bt_gattClientImplCB.gattcScanResultCB) {
            g_tg_bt_gattClientImplCB.gattcScanResultCB(bda, rssi, adv, len);
        } else {
            TG_TRACE_ERROR("gattcScanResultCB is NULL");
        }
        break;
    }
    case YOC_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        if (tg_bt_mesh_scan_is_enabled()) {
            bt_mesh_scan_param_handle(param);
        }
        break;
    case YOC_GAP_BLE_SCAN_START_COMPLETE_EVT:
		{
		    if (tg_bt_mesh_scan_is_pending()) {
                _tg_bt_scan_callback(NULL);
            }
            TG_TRACE_DEBUG("[%s] YOC_GAP_BLE_SCAN_START_COMPLETE_EVT status:%d\n", __func__
                                                            , param->scan_start_cmpl.status);
        }
        break;
    case YOC_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (tg_bt_mesh_scan_is_pending()) {
            _tg_bt_scan_callback(NULL);
        }
        break;
    case YOC_GAP_BLE_ADV_START_COMPLETE_EVT:
        if(tg_bt_mesh_adv_is_pending()) {
            _tg_bt_adv_callback(NULL);
        } else if (g_tg_bt_gatt_adv_pending) {
            g_tg_bt_gatt_adv_pending = 0;
            //        _tg_bt_gattc_adv_enable_cb(1);
        }
        break;
    case YOC_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if(tg_bt_mesh_adv_is_pending()) {
            _tg_bt_adv_callback(NULL);
        } else if (g_tg_bt_gatt_adv_pending) {
            g_tg_bt_gatt_adv_pending = 0;
            _tg_bt_gattc_adv_enable_cb(0);
        }
        break;
    case YOC_GAP_BLE_UPDATE_CONN_PARAMS_EVT:

        break;
    case YOC_GAP_BLE_SEC_REQ_EVT:
        {
            yoc_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, 1);
        }
        break;
    default:
        TG_TRACE_ERROR("%s: unhandled event: %d", __func__, event);
        break;
    }
}


/*==================================================================*/
/*========================       API      ==========================*/
/*==================================================================*/

bool tg_bt_set_get_enable(bool set, bool en) 
{
    static bool enabled = false;

    if (set)
        enabled = en;

    return enabled;
}

#define TG_GATTC_IF_ID (0x06)
void tg_bt_enable(int8_t enable)
{
#if 1
    static bool bt_enable_flag = 0;
    static bool bt_enable_pending = 0;
    static bool bt_mp_mode = 0;
    bt_err_t err;

    TG_TRACE_DEBUG("%s: enable = %d", __func__, enable);
    if (enable == 1) {
        if (bt_enable_flag || bt_enable_pending) {
            TG_TRACE_ERROR("tg_bt_enable, already enabled");
            return;
        }

        if (bt_mp_mode) {
            TG_TRACE_ERROR("tg_bt_enable, in mp mode");
            return;
        }

#if CONFIG_BT_SNOOP_ENABLED
        tg_btsnoop_init();
#endif
        bt_enable_pending = 1;
        tg_bt_set_get_enable(1, 1);
        err = yoc_bluedroid_init();
        if (err != BT_OK) {
            TG_TRACE_ERROR("yoc_bluedroid_init err= %d", err);
            return;
        }

        err = yoc_ble_gatts_register_callback(_tg_bt_gatts_event_handler);
        if (err){
            TG_TRACE_ERROR("gap register error, error code = %x", err);
            return;
        }

        err = yoc_ble_gattc_register_callback(_tg_bt_gattc_event_handler);
        if (err){
            TG_TRACE_ERROR("gap register error, error code = %x", err);
            return;
        }

        err = yoc_bt_gap_register_callback(_tg_bt_gap_cb);
        if (err){
            TG_TRACE_ERROR("gap register error, error code = %x", err);
            return;
        }

        err = yoc_ble_gap_register_callback(_tg_bt_ble_gap_event_handler);
        if (err){
            TG_TRACE_ERROR("gap register error, error code = %x", err);
            return;
        }

        err = yoc_bluedroid_enable();
        if (err != BT_OK) {
            TG_TRACE_ERROR("yoc_bluedroid_enable err= %d", err);
            return;
        }

        /* create application task */
        _tg_bt_app_task_start_up();
        _tg_bt_a2dp_init();

        yoc_bt_sp_param_t param_type_bt = YOC_BT_SP_IOCAP_MODE;
        yoc_bt_gap_set_security_param(param_type_bt, &g_tg_bt_bt_io_cap, sizeof(yoc_bt_io_cap_t));

        yoc_ble_sm_param_t param_type_ble = YOC_BLE_SM_IOCAP_MODE;
        yoc_ble_gap_set_security_param(param_type_ble, &g_tg_bt_ble_io_cap, sizeof(yoc_bt_io_cap_t));

        bt_enable_flag = 1;
        bt_enable_pending = 0;
#ifdef BT_TEST_ADD
        tg_bt_test_mesh_start();
#endif

#if CONFIG_BT_BQB_ENABLED
        _tg_bt_test_bqb_start();
#endif
        netdev_set_epta_params(0xc350, 0xc350, 0x00);
    } 

    if (enable == 0) {
        if (bt_enable_flag == 0) {
            TG_TRACE_ERROR("tg_bt_enable, already disabled");
            return;
        }

        err = yoc_bluedroid_disable();
        if (err != BT_OK) {
            TG_TRACE_ERROR("yoc_bluedroid_disable err= %d", err);
            return;
        }
        err = yoc_bluedroid_deinit();
        if (err != BT_OK) {
            TG_TRACE_ERROR("yoc_bluedroid_deinit err= %d", err);
            return;
        }

        bt_enable_flag = 0;
        tg_bt_set_get_enable(1, 0);
    }

    if (enable == 2) {
        int cnt = 0;

        while (bt_enable_pending) {
            hal_os_delay(10);
            cnt++;
            if (cnt > 200) {
                TG_TRACE_ERROR("bt mp quit failed when pending");
                break;
            }
        }
        tg_bt_hal_hcit_deinit();
        bt_mp_mode = 1;
//        bt_enable_flag = 0;
//        tg_bt_set_get_enable(1, 0);
    }
#endif
    return;
}

void tg_bt_gapCallbackRegister(TG_BT_GAP_IMPL_CB_FUNC_T *func)
{
    TG_TRACE_DEBUG("%s start", __func__);

    g_tg_bt_gapImplCB.gapPairCB = func->gapPairCB;
    g_tg_bt_gapImplCB.gapUnpairCB = func->gapUnpairCB;
    g_tg_bt_gapImplCB.gapGapScanCB = func->gapGapScanCB;

    return;
}

int32_t tg_bt_gapSetOnOff(bool fg_on)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start", __func__);

    return 0;
}

void tg_bt_gapGetLocalDevInfo(TG_BT_DEV_INFO *local_dev_info)
{
    uint8_t *bdaddr;
    uint8_t *p_dev_name;
    
    TG_TRACE_DEBUG("%s start", __func__);
    bdaddr = (uint8_t *)yoc_bt_dev_get_address();
    if (bdaddr) {
        mac_bin_to_str(local_dev_info->bdAddr, bdaddr);
    }

    if (BTM_ReadLocalDeviceName((char **)&p_dev_name) == BTM_SUCCESS) {
        strncpy((char *)local_dev_info->name, (char *)p_dev_name, TG_NAME_MAX_LEN);
    }

    return;
}

void tg_bt_gapGetRemoteDevInfo(TG_BT_DEV_INFO *remote_dev_info)
{
    int8_t bdAddr[TG_BDADDR_MAX_LEN];

    TG_TRACE_DEBUG("%s start", __func__);

    mac_bin_to_str(remote_dev_info->bdAddr, g_tg_bt_a2dpConnected_device);
    yoc_bt_gap_get_bond_device_name(g_tg_bt_a2dpConnected_device, remote_dev_info->name, TG_NAME_MAX_LEN);

    return;
}

void tg_bt_gapSetScanMode(bool conn_flag, bool disc_flag)
{
    bt_err_t ret;
    yoc_bt_connection_mode_t conn;
    yoc_bt_discovery_mode_t  disc;

    TG_TRACE_DEBUG("%s start: %d, %d", __func__, conn_flag, disc_flag);
    if (conn_flag == 0) {
        conn = YOC_BT_NON_CONNECTABLE;
    } else {
        conn = YOC_BT_CONNECTABLE;
    }

    if (disc_flag == 0) {
        disc = YOC_BT_NON_DISCOVERABLE;
    } else {
        disc = YOC_BT_GENERAL_DISCOVERABLE;
    }

    ret = yoc_bt_gap_set_scan_mode(conn, disc);
    return;
}

int32_t tg_bt_gapSetName(const int8_t *set_name)
{
    TG_TRACE_DEBUG("%s start: %s", __func__, set_name);
    yoc_bt_dev_set_device_name(set_name);
    
    return 0;
}

void tg_bt_startInquiry(void)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start", __func__);

    return;
}

void tg_bt_stopInquiry(void)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start", __func__);

    return;
}

void tg_bt_gapUnPair(const int8_t *mac)
{
    yoc_bd_addr_t addr;

    TG_TRACE_DEBUG("%s start: %s", __func__, mac);
    mac_str_to_bin(mac, addr);
    yoc_bt_gap_remove_bond_device(addr);

    return;
}


void tg_bt_a2dpCallbackRegister(TG_BT_A2DP_IMPL_CB_FUNC_T *func)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    g_tg_bt_a2dpImplCB.a2dpStateCB = func->a2dpStateCB;
    g_tg_bt_a2dpImplCB.a2dpStreamCB = func->a2dpStreamCB;

    return;
}

void tg_bt_a2dpConnect(const int8_t *mac, TG_BT_A2DP_ROLE role)
{
    yoc_bd_addr_t addr;

    TG_TRACE_DEBUG("%s start: %s", __func__, mac);
    mac_str_to_bin(mac, addr);
    yoc_a2d_sink_connect(addr);
    g_tg_bt_a2dpConnecting_flag = 1;

    return;
}

void tg_bt_a2dpDisconnect(const int8_t *mac, TG_BT_A2DP_ROLE role)
{
    TG_TRACE_DEBUG("%s start: %s", __func__, mac);

    yoc_bd_addr_t addr;
    mac_str_to_bin(mac, addr);
    yoc_a2d_sink_disconnect(addr);

    return;
}

int32_t tg_bt_a2dpSetRole(TG_BT_A2DP_ROLE role)
{
    TG_TRACE_DEBUG("%s start", __func__);
    g_tg_bt_a2dp_role = role;

    return 0;
}

TG_BT_A2DP_ROLE tg_bt_a2dpGetRole(void)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return g_tg_bt_a2dp_role;
}

void tg_bt_getA2dpPairedList(TG_BT_A2DP_DEV_INFO_LIST *info, TG_BT_A2DP_ROLE role)
{
    int32_t dev_num = 0, i;
    yoc_bd_addr_t *dev_list;

    TG_TRACE_DEBUG("%s start", __func__);

    dev_num = yoc_bt_gap_get_bond_device_num();
    if (dev_num <= 0 || dev_num > 200)
        return;

    dev_list = malloc(sizeof(yoc_bd_addr_t)*dev_num);
    if (dev_list == NULL) {
        info->dev_num = 0;
        return;
    }
    yoc_bt_gap_get_bond_device_list(&dev_num, dev_list);

    if (dev_num > TG_MAX_BT_DEV_NUM)
        dev_num = TG_MAX_BT_DEV_NUM;

    info->dev_num = dev_num;
    for (i = 0; i<  dev_num; i++) {
        mac_bin_to_str(info->device_list[i].bdAddr, dev_list[i]);
        yoc_bt_gap_get_bond_device_name(&(dev_list[i]), info->device_list[i].name, TG_NAME_MAX_LEN);
    }
    free(dev_list);

    return;
}

int32_t tg_bt_a2dpSendCmd(const int8_t *mac, uint8_t signal_id)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return 0;
}

void tg_bt_a2dpStartPlayer(void)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return;
}

void tg_bt_a2dpStopPlayer(void)
{
    TG_TRACE_DEBUG("%s start", __func__);

    yoc_a2d_sink_stop_player();
    return;
}

void tg_bt_avrcpCallbackRegister(TG_BT_AVRCP_IMPL_CB_FUNC_T *func)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    g_tg_bt_avrcpImplCB.avrcpStateCB = func->avrcpStateCB;
    g_tg_bt_avrcpImplCB.avrcpPlayStateCB = func->avrcpPlayStateCB;
    g_tg_bt_avrcpImplCB.volumeChangeCB = func->volumeChangeCB;
    g_tg_bt_avrcpImplCB.absoluteVolumeCB = func->absoluteVolumeCB;
    g_tg_bt_avrcpImplCB.avrcpCmdSrcCB = func->avrcpCmdSrcCB;

    return;
}

void tg_bt_avrcpConnect(const int8_t *mac)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start: %s", __func__, mac);

    return;
}

void tg_bt_avrcpDisconnect(void)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start", __func__);

    return;
}

void tg_bt_avrcpSendPassthroughCmd(TG_BT_AVRCP_CMD_TYPE cmd_type)
{
    int32_t cmd;

    TG_TRACE_DEBUG("%s start: %d", __func__, cmd_type);
    switch (cmd_type){
    case TG_BT_AVRCP_CMD_TYPE_PLAY:
        cmd = YOC_AVRC_PT_CMD_PLAY;
        break;
    case TG_BT_AVRCP_CMD_TYPE_PAUSE:
        cmd = YOC_AVRC_PT_CMD_PAUSE;
        break;
    case TG_BT_AVRCP_CMD_TYPE_FWD:
        cmd = YOC_AVRC_PT_CMD_FORWARD;
        break;
    case TG_BT_AVRCP_CMD_TYPE_BWD:
        cmd = YOC_AVRC_PT_CMD_BACKWARD;
        break;
    case TG_BT_AVRCP_CMD_TYPE_FFWD:
        cmd = YOC_AVRC_PT_CMD_FAST_FORWARD;
        break;
    case TG_BT_AVRCP_CMD_TYPE_RWD:
        cmd = YOC_AVRC_PT_CMD_REWIND;
        break;
    case TG_BT_AVRCP_CMD_TYPE_STOP:
        cmd = YOC_AVRC_PT_CMD_STOP;
        break;
    case TG_BT_AVRCP_CMD_TYPE_VOL_UP:
        cmd = YOC_AVRC_PT_CMD_VOL_UP;
        break;
    case TG_BT_AVRCP_CMD_TYPE_VOL_DOWN:
        cmd = YOC_AVRC_PT_CMD_VOL_DOWN;
        break;
    default: 
        return;
    }
    yoc_avrc_ct_send_passthrough_cmd(_tg_bt_get_lable(), cmd, 0);
    yoc_avrc_ct_send_passthrough_cmd(_tg_bt_get_lable(), cmd, 1);

    return;
}

int32_t tg_bt_avrcpChangeVolume(uint8_t vol, TG_BT_A2DP_ROLE role)
{
    yoc_avrc_rn_param_t param;

    TG_TRACE_DEBUG("%s start: %d", __func__, vol);
    param.volume = vol;
    g_tg_bt_avrcp_volumesync = vol;
    yoc_avrc_tg_send_rn_rsp(YOC_AVRC_RN_VOLUME_CHANGE, YOC_AVRC_RN_RSP_CHANGED, &param);

    return 0;
}

int32_t tg_bt_avrcpGetPlayStatus(const int8_t *mac)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start: %s", __func__, mac);

    return 0;
}

int32_t tg_bt_avrcpGetElementAttrTitle(const int8_t *mac)
{
    TG_TRACE_DEBUG("%s start: %s", __func__, mac);
    yoc_avrc_ct_send_metadata_cmd(_tg_bt_get_lable(), YOC_AVRC_MD_ATTR_TITLE);
    return 0;
}

void tg_bt_gattsSetCallback(TG_BT_GATTS_CB_FUNC_T *callback)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    g_tg_bt_gattServerImplCB.gattsInitCB = callback->gattsInitCB;
    g_tg_bt_gattServerImplCB.gattsAddServiceCB = callback->gattsAddServiceCB;
    g_tg_bt_gattServerImplCB.gattsAddCharCB = callback->gattsAddCharCB;
    g_tg_bt_gattServerImplCB.gattsAddDescCB = callback->gattsAddDescCB;
    g_tg_bt_gattServerImplCB.gattsReqWriteCB = callback->gattsReqWriteCB;
    g_tg_bt_gattServerImplCB.gattsReqReadCB = callback->gattsReqReadCB;
    g_tg_bt_gattServerImplCB.gattsConnectionEventCB = callback->gattsConnectionEventCB;
    g_tg_bt_gattServerImplCB.gattsStartServerCB = callback->gattsStartServerCB;
    g_tg_bt_gattServerImplCB.gattsStopServerCB = callback->gattsStopServerCB;
    g_tg_bt_gattServerImplCB.gattsDeleteServerCB = callback->gattsDeleteServerCB;

    return;
}

int32_t tg_bt_initGatts(void)
{
    TG_TRACE_DEBUG("%s start", __func__);
    int ret, cnt;

    cnt = 0;
    while (1) {
        if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_UNINITIALIZED)
            break;

        hal_os_delay(50);
        cnt++;
        if (cnt >= 50) {
            TG_TRACE_ERROR("gatts app register waiting!! timeout");
            return -1;
        }
   }

    ret = yoc_ble_gatts_app_register(g_server_if);
    if (ret){
        TG_TRACE_ERROR("gatts app register error, error code = %x", ret);
        return -1;
    }

    return 0;
}

int32_t tg_bt_deinitGatts(void)
{
    /*TODO: vendo implement */
    TG_TRACE_DEBUG("%s start", __func__);
    
    return 0;
}

TG_BT_GATTS_CB_FUNC_T *tg_bt_gattsGetCallback(void)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    return &g_tg_bt_gattServerImplCB;
}

int32_t tg_bt_gattsAddService(int32_t server_if, int8_t *service_uuid, uint8_t is_primary, int32_t number)
{
    yoc_gatt_srvc_id_t srvc;
    uint8_t uuid_tmp[YOC_UUID_LEN_128];

    TG_TRACE_DEBUG("%s, server_if: %d uuid: %s", __func__, server_if, service_uuid);

    srvc.id.inst_id = 0;
    srvc.id.uuid.len = strlen((char *)service_uuid)/2;
    srvc.is_primary = is_primary;
    bt_hexstr2bytes(service_uuid, uuid_tmp);

    switch (srvc.id.uuid.len) {
    case YOC_UUID_LEN_16:
        T_STREAM_TO_UINT16(srvc.id.uuid.uuid.uuid16, uuid_tmp);
        break;
    case YOC_UUID_LEN_32:
        T_STREAM_TO_UINT32(srvc.id.uuid.uuid.uuid32, uuid_tmp);
        break;
    case YOC_UUID_LEN_128:
        memcpy(srvc.id.uuid.uuid.uuid128, uuid_tmp, YOC_UUID_LEN_128);
        break;
    default:
        return -1;
    }

    return yoc_ble_gatts_create_service(server_if, &srvc, number);
}

int32_t tg_bt_gattsAddChar(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t properties,
                  int32_t permissions)
{
    yoc_bt_uuid_t char_uuid;
    uint8_t uuid_tmp[YOC_UUID_LEN_128];
    bt_err_t add_char_ret;

    TG_TRACE_DEBUG("%s start, server_if: %d uuid: %s", __func__, server_if, uuid);

    char_uuid.len = strlen((char *)uuid)/2;
    bt_hexstr2bytes(uuid, uuid_tmp);

    switch (char_uuid.len) {
    case YOC_UUID_LEN_16:
        T_STREAM_TO_UINT16(char_uuid.uuid.uuid16, uuid_tmp);
        break;
    case YOC_UUID_LEN_32:
        T_STREAM_TO_UINT32(char_uuid.uuid.uuid32, uuid_tmp);
        break;
    case YOC_UUID_LEN_128:
        memcpy(char_uuid.uuid.uuid128, uuid_tmp, YOC_UUID_LEN_128);
        break;
    default:
        return -1;
    }

    add_char_ret = yoc_ble_gatts_add_char(service_handle,
                        &char_uuid, permissions, properties, NULL, NULL);

    return add_char_ret;
}

int32_t tg_bt_gattsAddDesc(int32_t server_if, int32_t service_handle, int8_t *uuid, int32_t permissions)
{
    yoc_bt_uuid_t descr_uuid;
    uint8_t uuid_tmp[YOC_UUID_LEN_128];
    bt_err_t add_descr_ret;

    TG_TRACE_DEBUG("%s start, server_if: %d uuid: %s", __func__, server_if, uuid);

    descr_uuid.len = strlen((char *)uuid) / 2;
    bt_hexstr2bytes(uuid, uuid_tmp);

    switch (descr_uuid.len) {
    case YOC_UUID_LEN_16:
        T_STREAM_TO_UINT16(descr_uuid.uuid.uuid16, uuid_tmp);
        break;
    case YOC_UUID_LEN_32:
        T_STREAM_TO_UINT32(descr_uuid.uuid.uuid32, uuid_tmp);
        break;
    case YOC_UUID_LEN_128:
        memcpy(descr_uuid.uuid.uuid128, uuid_tmp, YOC_UUID_LEN_128);
        break;
    default:
        return -1;
    }

    add_descr_ret = yoc_ble_gatts_add_char_descr(service_handle,
                                &descr_uuid, permissions, NULL, NULL);

    return add_descr_ret;
}

int32_t tg_bt_gattsStartService(int32_t server_if, int32_t service_handle, int32_t transport)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    return yoc_ble_gatts_start_service(service_handle);
}

int32_t tg_bt_gattsStopService(int32_t server_if, int32_t service_handle)
{
    TG_TRACE_DEBUG("%s start", __func__);
    
    return yoc_ble_gatts_stop_service(service_handle);
}

int32_t tg_bt_gattsDeleteService(int32_t server_if, int32_t service_handle)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return yoc_ble_gatts_delete_service(service_handle);
}


int32_t tg_bt_gattsUnregisterService(int32_t server_if)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return yoc_ble_gatts_app_unregister(server_if); 
}

int32_t tg_bt_gattsSendResponse(int32_t conn_id, int32_t trans_id, int32_t status, int32_t handle, int8_t *p_value,
                       int32_t offset, int32_t value_len, int32_t auth_req)
{
    yoc_gatt_rsp_t rep;

    TG_TRACE_DEBUG("%s start", __func__);
    rep.attr_value.handle = handle;
    rep.attr_value.len = value_len;
    rep.attr_value.offset = offset;
    memcpy(rep.attr_value.value, p_value, value_len);
    rep.attr_value.auth_req = auth_req;
    return yoc_ble_gatts_send_response(g_server_if ,conn_id, trans_id, status, &rep);
}

int32_t tg_bt_gattsSendIndication(int32_t server_if, int32_t handle, int32_t conn_id, int32_t fg_confirm,
                         int8_t *p_value, int32_t value_len)
{
    TG_TRACE_DEBUG("%s start", __func__);

    return yoc_ble_gatts_send_indicate(server_if, conn_id, handle,
                                value_len, (uint8_t*)p_value, fg_confirm);; 
}

void tg_bt_gattsSetServerIf(int32_t serverIf)
{
    TG_TRACE_DEBUG("%s start: %d", __func__, serverIf);
    
    g_server_if = serverIf;
    yoc_ble_gatts_app_register(serverIf);

    return; 
}

void tg_bt_gattsDelServerIf(int32_t serverIf)
{
    TG_TRACE_DEBUG("%s start: %d", __func__, serverIf);
    
    g_server_if = -1;
    yoc_ble_gatts_app_unregister(serverIf);

    return; 
}

int32_t tg_bt_gattsSetAdvData(uint8_t *data, int len)
{
    TG_TRACE_DEBUG("%s", __func__);

    g_tg_bt_gatt_set_adv_data_flag = 1;
    return yoc_ble_gap_config_adv_data_raw(data, len); 
}

int32_t tg_bt_gattsEnableAdv(bool enable)
{
    TG_TRACE_DEBUG("%s", __func__);
    
    /* FIXME: do nothing when enable == 1, cause tg_bt_setAdvData haven called yet */
    if (enable == 0) {
        g_tg_bt_gatt_adv_pending = 1;
        return yoc_ble_gap_stop_advertising();
    } else {
        _tg_bt_gattc_adv_enable_cb(1);
    }

    return 0; 
}

void tg_bt_gattcSetCallback(TG_BT_GATTC_CB_FUNC_T *callback)
{
    TG_TRACE_DEBUG("%s", __func__);

    g_tg_bt_gattClientImplCB.gattcInitedCB = callback->gattcInitedCB;
    g_tg_bt_gattClientImplCB.gattcScanResultCB = callback->gattcScanResultCB;
    g_tg_bt_gattClientImplCB.gattcGattEventCB = callback->gattcGattEventCB;
    g_tg_bt_gattClientImplCB.gattcServiceDiscoveredCB = callback->gattcServiceDiscoveredCB;
    g_tg_bt_gattClientImplCB.gattcCharacteristicReadCB = callback->gattcCharacteristicReadCB;
    g_tg_bt_gattClientImplCB.gattcCharacteristicWriteCB = callback->gattcCharacteristicWriteCB;
    g_tg_bt_gattClientImplCB.gattcDescriptorReadCB = callback->gattcDescriptorReadCB;
    g_tg_bt_gattClientImplCB.gattcCnDescriptorWriteCB = callback->gattcCnDescriptorWriteCB;
    g_tg_bt_gattClientImplCB.gattcCCharacteristicChangedCB = callback->gattcCCharacteristicChangedCB;
    g_tg_bt_gattClientImplCB.gattcSetCharacteristicNotiCB = callback->gattcSetCharacteristicNotiCB;
    g_tg_bt_gattClientImplCB.gattcAdvEnabledCB = callback->gattcAdvEnabledCB;
    yoc_ble_gattc_app_register(TG_GATTC_IF_ID);
}

int32_t tg_bt_gattcSetAdvData(uint8_t *data, int len)
{
    TG_TRACE_DEBUG("%s", __func__);

    g_tg_bt_gatt_set_adv_data_flag = 1;
    return yoc_ble_gap_config_adv_data_raw(data, len); 
}

int32_t tg_bt_gattcEnableAdv(bool enable)
{
    TG_TRACE_DEBUG("%s", __func__);
    
    /* FIXME: do nothing when enable == 1, cause tg_bt_setAdvData haven called yet */
    if (enable == 0) {
        g_tg_bt_gatt_adv_pending = 1;
        return yoc_ble_gap_stop_advertising();
    } else {
        _tg_bt_gattc_adv_enable_cb(1);
    }

    return 0; 
}

