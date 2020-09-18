/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_bt_mesh.c
 * @brief:   vendor bt mesh implementation
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2019/12/15
 * @version: 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tg_bt_mesh.h"
#include <stack/bt_types.h>
#include "bta/bta_api.h"
#include "common/bt_trace.h"
#include "osi/semaphore.h"
#include "yoc_bt_main.h"
#include "yoc_bt_device.h"
#include "yoc_gap_ble_api.h"
#include "bt_hal_os.h"

#define BT_MESH_USE_BTC     1

#ifndef BIT
#define BIT(n)      (1UL << (n))
#endif

#ifndef BLE_MESH_OP
#define BLE_MESH_OP(ogf, ocf)       ((ocf) | ((ogf) << 10))
#endif

/* Scan filter policy */
#define BLE_MESH_SP_ADV_ALL                 0x00
#define BLE_MESH_SP_ADV_WL                  0x01
#define BLE_MESH_SP_ADV_ALL_RPA_DIR_ADV     0x02
#define BLE_MESH_SP_ADV_WL_RPA_DIR_ADV      0x03

/* Scan duplicate operation */
#define BLE_MESH_SCAN_FILTER_DUP_DISABLE    0x00
#define BLE_MESH_SCAN_FILTER_DUP_ENABLE     0x01

/* BD ADDR types */
#define BLE_MESH_ADDR_PUBLIC         0x00
#define BLE_MESH_ADDR_RANDOM         0x01
#define BLE_MESH_ADDR_PUBLIC_ID      0x02
#define BLE_MESH_ADDR_RANDOM_ID      0x03


/* BD ADDR length */
#define BLE_MESH_ADDR_LEN                   0x06

/* Scan types */
#define BLE_MESH_SCAN_PASSIVE               0x00
#define BLE_MESH_SCAN_ACTIVE                0x01

/* advertising channel map */
#define BLE_MESH_ADV_CHNL_37                BIT(0)
#define BLE_MESH_ADV_CHNL_38                BIT(1)
#define BLE_MESH_ADV_CHNL_39                BIT(2)

/* Advertising filter policy */
#define BLE_MESH_AP_SCAN_CONN_ALL           0x00
#define BLE_MESH_AP_SCAN_WL_CONN_ALL        0x01
#define BLE_MESH_AP_SCAN_ALL_CONN_WL        0x02
#define BLE_MESH_AP_SCAN_CONN_WL            0x03

/* Advertising types */
#define BLE_MESH_ADV_IND                    0x00
#define BLE_MESH_ADV_DIRECT_IND             0x01
#define BLE_MESH_ADV_SCAN_IND               0x02
#define BLE_MESH_ADV_NONCONN_IND            0x03
#define BLE_MESH_ADV_DIRECT_IND_LOW_DUTY    0x04

#define TG_BT_MESH_SCAN_ACTIVE              0x00
#define TG_BT_MESH_AGGRESSIVE_ACTIVE        0x01


#define BLE_MESH_BTM_CHECK_STATUS(func) do {                                                     \
        tBTM_STATUS __status = (func);                                                           \
        if ((__status != BTM_SUCCESS) && (__status != BTM_CMD_STARTED)) {                        \
            TG_TRACE_ERROR("%s, Invalid status %d", __func__, __status);                                 \
            return -1;                                                                           \
        }                                                                                        \
    } while(0);


/** Advertising options */
enum {
    /** Convenience value when no options are specified. */
    BLE_MESH_ADV_OPT_NONE = 0,

    /** Advertise as connectable. Type of advertising is determined by
     * providing SCAN_RSP data and/or enabling local privacy support.
     */
    BLE_MESH_ADV_OPT_CONNECTABLE = BIT(0),

    /** Don't try to resume connectable advertising after a connection.
     *  This option is only meaningful when used together with
     *  BLE_MESH_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
     *  when bt_le_adv_stop() is called or when an incoming (slave)
     *  connection happens. If this option is not set the stack will
     *  take care of keeping advertising enabled even as connections
     *  occur.
     */
    BLE_MESH_ADV_OPT_ONE_TIME = BIT(1),
};

#define BLE_MESH_HCI_OP_SET_ADV_DATA        BLE_MESH_OP(BLE_MESH_OGF_LE, 0x0008)
struct bt_mesh_hci_cp_set_adv_data {
    uint8_t  len;
    uint8_t  data[31];
} __attribute__((packed));

#define BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA   BLE_MESH_OP(BLE_MESH_OGF_LE, 0x0009)
struct bt_mesh_hci_cp_set_scan_rsp_data {
    uint8_t  len;
    uint8_t  data[31];
} __attribute__((packed));

static tg_bt_mesh_le_scan_cb_t bt_mesh_scan_dev_found_cb;

static int8_t tg_bt_mesh_adv_sem_flag = 0;
static int8_t tg_bt_mesh_adv_sem_pending = 0;
static osi_sem_t tg_bt_mesh_adv_sem;

static int8_t tg_bt_mesh_scan_sem_flag = 0;
static int8_t tg_bt_mesh_scan_sem_pending = 0;
static osi_sem_t tg_bt_mesh_scan_sem;

static int8_t g_tg_bt_mesh_flag = 0;

static bool valid_scan_param(const struct tg_bt_mesh_le_scan_param *param)
{
    if (param->type != BLE_MESH_SCAN_PASSIVE &&
            param->type != BLE_MESH_SCAN_ACTIVE) {
        return false;
    }

    if (param->filter_dup != BLE_MESH_SCAN_FILTER_DUP_DISABLE &&
            param->filter_dup != BLE_MESH_SCAN_FILTER_DUP_ENABLE) {
        return false;
    }

    if (param->interval < 0x0004 || param->interval > 0x4000) {
        return false;
    }

    if (param->window < 0x0004 || param->window > 0x4000) {
        return false;
    }

    if (param->window > param->interval) {
        return false;
    }

    return true;
}

static bool valid_adv_param(const struct tg_bt_mesh_le_adv_param *param)
{
    if (param->interval_min > param->interval_max ||
            param->interval_min < 0x0010 || param->interval_max > 0x4000) {
        return false;
    }

    return true;
}

static struct tg_bt_mesh_net_buf_simple *bt_mesh_alloc_buf(uint16_t size)
{
    struct tg_bt_mesh_net_buf_simple *buf = NULL;
    uint8_t *data = NULL;

    buf = (struct tg_bt_mesh_net_buf_simple *)hal_malloc(sizeof(struct tg_bt_mesh_net_buf_simple) + size);
    if (!buf) {
        TG_TRACE_ERROR("%s, Failed to allocate memory", __func__);
        return NULL;
    }

    data = (uint8_t *)buf + sizeof(struct tg_bt_mesh_net_buf_simple);

    buf->data = data;
    buf->len = 0;
    buf->size = size;

    return buf;
}

int32_t bt_mesh_buf_simple_headroom(struct tg_bt_mesh_net_buf_simple *buf)
{
    return buf->data - buf->__buf;
}

int32_t bt_mesh_buf_simple_tailroom(struct tg_bt_mesh_net_buf_simple *buf)
{
    return buf->size - bt_mesh_buf_simple_headroom(buf) - buf->len;
}

static inline uint8_t *bt_mesh_buf_simple_tail(struct tg_bt_mesh_net_buf_simple *buf)
{
    return buf->data + buf->len;
}

void *bt_mesh_buf_simple_add(struct tg_bt_mesh_net_buf_simple *buf, int32_t len)
{
    uint8_t *tail = bt_mesh_buf_simple_tail(buf);

//    TG_TRACE_DEBUG("buf %p len %zu", buf, len);

    if(bt_mesh_buf_simple_tailroom(buf) < len) {
//        TG_TRACE_ERROR("buf %p len %zu", buf, len);
    }

    buf->len += len;
    return tail;
}

void *bt_mesh_buf_simple_add_mem(struct tg_bt_mesh_net_buf_simple *buf, const void *mem,
                                int32_t len)
{
//    TG_TRACE_DEBUG("buf %p len %zu", buf, len);

    return memcpy(bt_mesh_buf_simple_add(buf, len), mem, len);
}

static void bt_mesh_free_buf(struct tg_bt_mesh_net_buf_simple *buf)
{
    if (buf) {
        hal_free(buf);
    }
}

static int set_adv_data(uint16_t hci_op, const struct tg_bt_mesh_data *ad, int32_t ad_len)
{
    struct bt_mesh_hci_cp_set_adv_data param = {0};
    int i;

    if (ad == NULL || ad_len == 0) {
        return 0;
    }

    for (i = 0; i < ad_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (param.len + ad[i].data_len + 2 > 31) {
            return -1;
        }

        param.data[param.len++] = ad[i].data_len + 1;
        param.data[param.len++] = ad[i].type;

        memcpy(&param.data[param.len], ad[i].data, ad[i].data_len);
        param.len += ad[i].data_len;
    }

#ifdef BT_MESH_USE_BTC
    if (hci_op == 0) {
        yoc_ble_gap_config_adv_data_raw(param.data, param.len);
    } else if (hci_op == 1) {
        yoc_ble_gap_config_scan_rsp_data_raw(param.data, param.len);
    }
#else
    /* Set adv data and scan rsp data. */
    if (hci_op == 0) {
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteAdvDataRaw(param.data, param.len));
    } else if (hci_op == 1) {
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteScanRspRaw(param.data, param.len));
    }
#endif
    return 0;
}

#ifndef BT_MESH_USE_BTC
static void bt_mesh_scan_results_change_2_bta(tBTM_INQ_RESULTS *p_inq, uint8_t *p_eir,
                tBTA_DM_SEARCH_CBACK *p_scan_cback)
{
    tBTM_INQ_INFO *p_inq_info;
    tBTA_DM_SEARCH result;

    bdcpy(result.inq_res.bd_addr, p_inq->remote_bd_addr);
    result.inq_res.rssi = p_inq->rssi;
    result.inq_res.ble_addr_type    = p_inq->ble_addr_type;
    result.inq_res.inq_result_type  = p_inq->inq_result_type;
    result.inq_res.device_type      = p_inq->device_type;
    result.inq_res.flag             = p_inq->flag;
    result.inq_res.adv_data_len     = p_inq->adv_data_len;
    result.inq_res.scan_rsp_len     = p_inq->scan_rsp_len;
    memcpy(result.inq_res.dev_class, p_inq->dev_class, sizeof(DEV_CLASS));
    result.inq_res.ble_evt_type     = p_inq->ble_evt_type;

    /* application will parse EIR to find out remote device name */
    result.inq_res.p_eir = p_eir;

    if ((p_inq_info = BTM_InqDbRead(p_inq->remote_bd_addr)) != NULL) {
        /* initialize remt_name_not_required to FALSE so that we get the name by default */
        result.inq_res.remt_name_not_required = FALSE;
    }

    if (p_scan_cback) {
        p_scan_cback(BTA_DM_INQ_RES_EVT, &result);
    }

    if (p_inq_info) {
        /* application indicates if it knows the remote name, inside the callback
         copy that to the inquiry data base*/
        if (result.inq_res.remt_name_not_required) {
            p_inq_info->appl_knows_rem_name = TRUE;
        }
    }
}

static void bt_mesh_scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    tg_bt_mesh_addr_le_t addr = {0};
    UINT8 adv_type;
    UINT8 rssi;

    // TG_TRACE_DEBUG("%s, event = %d", __func__, event);

    if (event == BTA_DM_INQ_RES_EVT) {
        /* TODO: How to process scan response here? */
        addr.type = p_data->inq_res.ble_addr_type;
        memcpy(addr.a.val, p_data->inq_res.bd_addr, BLE_MESH_ADDR_LEN);
        rssi = p_data->inq_res.rssi;
        adv_type = p_data->inq_res.ble_evt_type;

        /* scan rsp len: p_data->inq_res.scan_rsp_len */
        struct tg_bt_mesh_net_buf_simple *buf = bt_mesh_alloc_buf(p_data->inq_res.adv_data_len);
        if (!buf) {
            TG_TRACE_ERROR("%s, Failed to allocate memory", __func__);
            return;
        }
        bt_mesh_buf_simple_add_mem(buf, p_data->inq_res.p_eir, p_data->inq_res.adv_data_len);

        if (bt_mesh_scan_dev_found_cb != NULL) {
            bt_mesh_scan_dev_found_cb(&addr, rssi, adv_type, buf);
        }
        bt_mesh_free_buf(buf);
    } else if (event == BTA_DM_INQ_CMPL_EVT) {
        TG_TRACE_DEBUG("%s, Scan completed, number of scan response %d", __func__, p_data->inq_cmpl.num_resps);
    } else {
        TG_TRACE_ERROR("%s, Unexpected event 0x%x", __func__, event);
    }
}

static void bt_mesh_scan_results_cb(tBTM_INQ_RESULTS *p_inq, uint8_t *p_eir)
{
    /*UNDO: not implement */
//    TG_TRACE_DEBUG("bt_mesh_scan_results_cb: %02x%02x%02x%02x", p_eir[0], p_eir[1], p_eir[2], p_eir[3]);

    bt_mesh_scan_results_change_2_bta(p_inq, p_eir, bt_mesh_scan_result_callback);
}
#endif

void bt_mesh_scan_result_handle(yoc_ble_gap_cb_param_t *param)
{
    tg_bt_mesh_addr_le_t addr = {0};
    UINT8 adv_type;
    UINT8 rssi;
    struct tg_bt_mesh_net_buf_simple *buf;

    addr.type = param->scan_rst.ble_addr_type;
    memcpy(addr.a.val, param->scan_rst.bda, BLE_MESH_ADDR_LEN);
    rssi = param->scan_rst.rssi;
    adv_type = param->scan_rst.ble_evt_type;
    buf = bt_mesh_alloc_buf(param->scan_rst.adv_data_len);
    if (!buf) {
        TG_TRACE_ERROR("%s, Failed to allocate memory", __func__);
        return;
    }
    bt_mesh_buf_simple_add_mem(buf, param->scan_rst.ble_adv, param->scan_rst.adv_data_len);
    if (bt_mesh_scan_dev_found_cb != NULL) {
        bt_mesh_scan_dev_found_cb(&addr, rssi, adv_type, buf);
    }
    bt_mesh_free_buf(buf);
}

static int start_le_scan(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t filter_dup)
{
    uint8_t scan_fil_policy = BLE_MESH_SP_ADV_ALL; /* No whitelist for BLE Mesh */
    uint8_t addr_type_own = BLE_MESH_ADDR_PUBLIC;  /* Currently only support Public Address */

#ifdef BT_MESH_USE_BTC
    {
        yoc_ble_scan_params_t scan_params;

        scan_params.scan_type = scan_type;
        scan_params.own_addr_type = addr_type_own;
        scan_params.scan_filter_policy = scan_fil_policy;
        scan_params.scan_interval = interval;
        scan_params.scan_window = window;
        scan_params.scan_duplicate = filter_dup;
        yoc_ble_gap_set_scan_params(&scan_params);
        yoc_ble_gap_start_scanning(0);
    }
#else
    {
        tGATT_IF client_if = 0xFF; /* Default GATT interface id */
        
        BTM_BleSetScanFilterParams(client_if, interval, window, scan_type, addr_type_own,
                                    filter_dup, scan_fil_policy, NULL);

        /* BLE Mesh scan permanently, so no duration of scan here */
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleScan(true, 0, bt_mesh_scan_results_cb, NULL, NULL));
    }
#endif
    return 0;
}

int32_t tg_bt_mesh_rand(void *buf, int32_t len)
{
    /*TODO: vendo implement */

    return 0;
}

bool tg_bt_mesh_adv_is_pending(void)
{
    if (tg_bt_mesh_adv_sem_pending)
        return true;

    return false;
}

void tg_bt_mesh_adv_pending_wait(void)
{
    if (tg_bt_mesh_adv_sem_flag == 0) {
        osi_sem_new(&tg_bt_mesh_adv_sem, 1, 0);
        tg_bt_mesh_adv_sem_flag = 1;
    }

    tg_bt_mesh_adv_sem_pending = 1;
    osi_sem_take(&tg_bt_mesh_adv_sem, 2000);
}

void tg_bt_mesh_adv_pending_end(void)
{
    if (tg_bt_mesh_adv_sem_flag == 0)
        return;

    tg_bt_mesh_adv_sem_pending = 0;
    osi_sem_give(&tg_bt_mesh_adv_sem);
}

void _tg_bt_adv_callback(void *pArgs)
{
    tg_bt_mesh_adv_pending_end();
}

int32_t tg_bt_mesh_adv_start(const struct tg_bt_mesh_le_adv_param *param,
                          const struct tg_bt_mesh_data *ad, int32_t ad_len,
                          const struct tg_bt_mesh_data *sd, int32_t sd_len)
{
    tBTM_BLE_ADV_CHNL_MAP channel_map;
    tBLE_ADDR_TYPE addr_type_own;
    tBTM_BLE_AFP adv_fil_pol;
    uint8_t adv_type;
    int32_t err;

    TG_TRACE_DEBUG("%s", __func__);
    if (!valid_adv_param(param)) {
        TG_TRACE_ERROR("%s, Invalid adv parameters", __func__);
        return -1;
    }

    err = set_adv_data(0, ad, ad_len);
    if (err) {
        TG_TRACE_ERROR("%s, Failed to set adv data", __func__);
        return err;
    }

    /*
     * We need to set SCAN_RSP when enabling advertising type that allows
     * for Scan Requests.
     *
     * If sd was not provided but we enable connectable undirected
     * advertising sd needs to be cleared from values set by previous calls.
     * Clearing sd is done by calling set_adv_data() with NULL data and zero len.
     * So following condition check is unusual but correct.
     */
    if (sd && (param->options & BLE_MESH_ADV_OPT_CONNECTABLE)) {
        err = set_adv_data(1, sd, sd_len);
        if (err) {
            TG_TRACE_ERROR("%s, Failed to set scan rsp data", __func__);
            return err;
        }
    }

    if (param->options & BLE_MESH_ADV_OPT_CONNECTABLE) {
        adv_type = BLE_MESH_ADV_IND;
    } else if (sd != NULL) {
        adv_type = BLE_MESH_ADV_SCAN_IND;
    } else {
        adv_type = BLE_MESH_ADV_NONCONN_IND;
    }
    addr_type_own = BLE_MESH_ADDR_PUBLIC; /* Currently only support Public Address */
    channel_map = BLE_MESH_ADV_CHNL_37 | BLE_MESH_ADV_CHNL_38 | BLE_MESH_ADV_CHNL_39;
    adv_fil_pol = BLE_MESH_AP_SCAN_CONN_ALL;

#ifdef BT_MESH_USE_BTC
    {
        yoc_ble_adv_params_t adv_param;

        memset(&adv_param, 0, sizeof(adv_param));
        adv_param.adv_int_max = param->interval_max;
        adv_param.adv_int_min = param->interval_min;
        adv_param.own_addr_type = addr_type_own;
        adv_param.adv_type = adv_type;
        adv_param.channel_map = channel_map;
        adv_param.adv_filter_policy = adv_fil_pol;
        yoc_ble_gap_start_advertising(&adv_param);
    }
#else
    {
        tBLE_BD_ADDR p_dir_bda = {0};
        tBTA_START_ADV_CMPL_CBACK *p_start_adv_cb;

        p_start_adv_cb = _tg_bt_adv_callback;
        /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */    
        BLE_MESH_BTM_CHECK_STATUS(
            BTM_BleSetAdvParamsAll(param->interval_min, param->interval_max, adv_type,
                                    addr_type_own, &p_dir_bda,
                                    channel_map, adv_fil_pol, p_start_adv_cb));
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleStartAdv());
    }
#endif
    tg_bt_mesh_adv_pending_wait();

    return 0;
}

int32_t tg_bt_mesh_adv_stop(void)
{
    TG_TRACE_DEBUG("%s", __func__);

#ifdef BT_MESH_USE_BTC
    yoc_ble_gap_stop_advertising();
#else
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleBroadcast(false, _tg_bt_adv_callback));
#endif
    tg_bt_mesh_adv_pending_wait();

    return 0;
}

int32_t tg_bt_mesh_set_flag(uint8_t flag_id, uint8_t enable)
{
    if (enable) {
        g_tg_bt_mesh_flag |= (1<<flag_id);
    } else {
        g_tg_bt_mesh_flag &= ~(1<<flag_id);
    }
    if((flag_id == TG_BT_MESH_SCAN_ACTIVE) && (enable == 0)){
        return 0;
    }

    return tg_bt_hal_vendor_set_mesh_state(g_tg_bt_mesh_flag);
}

bool tg_bt_mesh_scan_is_enabled(void)
{
    if (bt_mesh_scan_dev_found_cb) {
        return true;
    }
    return false;
}

extern uint8_t mesh_open_enalbe;
bool tg_bt_mesh_scan_is_pending(void)
{
    if (tg_bt_mesh_scan_sem_pending)
        return true;

    return false;
}

void tg_bt_mesh_scan_pending_wait(void)
{
    TG_TRACE_WARNING("%s", __func__);
    if (tg_bt_mesh_scan_sem_flag == 0) {
        osi_sem_new(&tg_bt_mesh_scan_sem, 1, 0);
        tg_bt_mesh_scan_sem_flag = 1;
    }

    tg_bt_mesh_scan_sem_pending = 1;
    osi_sem_take(&tg_bt_mesh_scan_sem, 2000);
}

void tg_bt_mesh_scan_pending_end(void)
{
    TG_TRACE_WARNING("%s", __func__);
    if (tg_bt_mesh_scan_sem_flag == 0)
        return;

    tg_bt_mesh_scan_sem_pending = 0;
    osi_sem_give(&tg_bt_mesh_scan_sem);
}

void _tg_bt_scan_callback(void *pArgs)
{
    tg_bt_mesh_scan_pending_end();
}

void bt_mesh_scan_param_handle(yoc_ble_gap_cb_param_t *param)
{
    TG_TRACE_WARNING("%s: %d", __func__, param->scan_param_cmpl.status);
}

int32_t tg_bt_mesh_scan_start(const struct tg_bt_mesh_le_scan_param *param,
                           tg_bt_mesh_le_scan_cb_t cb)
{
    int32_t err;

    TG_TRACE_DEBUG("%s", __func__);
    if (!valid_scan_param(param)) {
        TG_TRACE_ERROR("%s:valid_scan_param failed", __func__);
        return -1;
    }
    if(mesh_open_enalbe){
        err = start_le_scan(param->type, param->interval, param->window, param->filter_dup);
        bt_mesh_scan_dev_found_cb = cb;
        tg_bt_mesh_scan_pending_wait();
    } else {
        TG_TRACE_ERROR("%s,mesh has been disabled", __func__);
        return 0;
    }
    if (err) {
        return err;
    }
    tg_bt_mesh_set_flag(TG_BT_MESH_SCAN_ACTIVE, 1);

    return 0;
}

int32_t tg_bt_mesh_scan_stop(void)
{
    TG_TRACE_DEBUG("%s", __func__);

#ifdef BT_MESH_USE_BTC
    if(mesh_open_enalbe){
        yoc_ble_gap_stop_scanning();
        bt_mesh_scan_dev_found_cb = NULL;
        tg_bt_mesh_scan_pending_wait();
    } else {
        TG_TRACE_ERROR("%s , mesh has been disabled", __func__);
        return 0;
    }
#else
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleScan(false, 0, NULL, NULL, NULL));
#endif
    tg_bt_mesh_set_flag(TG_BT_MESH_SCAN_ACTIVE, 0);

    return 0;
}

int32_t tg_bt_mesh_enable_aggressive_setting(bool enable)
{
    int32_t ret;

    ret = tg_bt_mesh_set_flag(TG_BT_MESH_AGGRESSIVE_ACTIVE, enable);
    return ret;
}

int32_t tg_bt_mesh_set_oneshot_data(const struct tg_bt_mesh_data *ad, int32_t ad_len,
                                        const struct tg_bt_mesh_data *sd, int32_t sd_len)
{
    /*TODO: vendo implement */

    return 0;
}

int32_t tg_bt_mesh_send_adv_oneshot(void)
{
    /*TODO: vendo implement */

    return 0;
}

