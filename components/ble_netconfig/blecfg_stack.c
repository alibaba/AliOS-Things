/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "blecfg_pub.h"

#define UUID_VENDOR_SERVICE                      UUID16_DECLARE(0xFFA0)
#define UUID_VENDOR_CHAR_READ                    UUID16_DECLARE(0xFFA1)
#define UUID_VENDOR_CHAR_WRITE                   UUID16_DECLARE(0xFFA2)
#define UUID_VENDOR_CHAR_NOTIFY                  UUID16_DECLARE(0xFFA3)

enum {
    BLE_NETCFG_IDX_SVC,
    BLE_NETCFG_IDX_CHAR1,
    BLE_NETCFG_IDX_CHAR1_VAL,
    BLE_NETCFG_IDX_CHAR2,
    BLE_NETCFG_IDX_CHAR2_VAL,
    BLE_NETCFG_IDX_CHAR3,
    BLE_NETCFG_IDX_CHAR3_VAL,
    BLE_NETCFG_IDX_CHAR3_CCC,

    BLE_NETCFG_IDX_MAX,
};

char g_blecfg_devname[128];

typedef struct {
    uint8_t inited;
    uint8_t started;
    ble_event_cb_t stack_cb;
    BleCfg_msg_cb  miniapp_msg_cb;
    int16_t conn_handle;
    uint32_t gatt_svc_handle;
}BleCfg_Info;

gatt_service g_BLE_netCfg_gatt_service;
static  gatt_attr_t g_BLE_netCfg_gatt_attrs[] = {
    [BLE_NETCFG_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(UUID_VENDOR_SERVICE),

    [BLE_NETCFG_IDX_CHAR1] = GATT_CHAR_DEFINE(UUID_VENDOR_CHAR_READ, GATT_CHRC_PROP_READ),
    [BLE_NETCFG_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(UUID_VENDOR_CHAR_READ, GATT_PERM_READ),

    [BLE_NETCFG_IDX_CHAR2] = GATT_CHAR_DEFINE(UUID_VENDOR_CHAR_WRITE, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [BLE_NETCFG_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(UUID_VENDOR_CHAR_WRITE,  GATT_PERM_WRITE),

    [BLE_NETCFG_IDX_CHAR3] = GATT_CHAR_DEFINE(UUID_VENDOR_CHAR_NOTIFY, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_NOTIFY),
    [BLE_NETCFG_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(UUID_VENDOR_CHAR_NOTIFY,  GATT_PERM_READ),
    [BLE_NETCFG_IDX_CHAR3_CCC] = GATT_CHAR_CCC_DEFINE(),
};

uint8_t g_BLE_netCfg_gatt_read_char[16] = "NetCfg Read";

uint8_t g_BLE_netCfg_adv_flag = 0x06;
uint16_t g_BLE_netCfg_uuid16_list[] = {0xFFA0};

static BleCfg_Info g_BLE_netCfg_info;

static BleCfg_Info *BleCfg_get_info(void)
{
    return &g_BLE_netCfg_info;
}

static int BleCfg_start_adv(void)
{
    int ret;
    ad_data_t ad[2] = {0};
    adv_param_t param = {
        .type = ADV_IND,
        .ad = ad,
        .sd = NULL,
        .ad_num = BLE_ARRAY_NUM(ad),
        .sd_num = 0,
        .interval_min = ADV_FAST_INT_MIN_1,
        .interval_max = ADV_FAST_INT_MAX_1,
        .filter_policy = 0,
        .channel_map = 7,
        .direct_peer_addr = {0, {0}},
    };

    /* setup ADV Flags */
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&g_BLE_netCfg_adv_flag;
    ad[0].len = sizeof(g_BLE_netCfg_adv_flag);

    /* setup ADV UUIDS */
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)g_BLE_netCfg_uuid16_list;
    ad[1].len = sizeof(g_BLE_netCfg_uuid16_list);

    /* call ADV start API */
    ret = ble_stack_adv_start(&param);
    if (ret) {
        BLECFG_LOG_ERROR("adv start fail %d!\r\n", ret);
    } else {
        BLECFG_LOG_INFO("adv start!\r\n");
    }

    return ret;
}

static void BleCfg_stop_adv(void)
{
    int ret;

    /* call ADV start API */
    ret = ble_stack_adv_stop();
    if (ret) {
        BLECFG_LOG_ERROR("adv stop fail %d!\r\n", ret);
    } else {
        BLECFG_LOG_INFO("adv stop!\r\n");
    }
}

static void BleCfg_event_conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    BLECFG_LOG_INFO("%s, connected = %d\r\n", __func__, e->connected);

    /* check if connect or disconnect */
    if (e->connected == CONNECTED && e->err == 0) {
        netCfg_info->conn_handle = e->conn_handle;
    } else {
        netCfg_info->conn_handle = -1;
        BleCfg_start_adv();
    }
}

static void BleCfg_event_pairing_passkey_display(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP passkey event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_smp_complete(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP complete event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_smp_cancel(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP cancel event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_smp_pairing_confirm(ble_event_en event, void *event_data)
{
    evt_data_smp_pairing_confirm_t *e = (evt_data_smp_pairing_confirm_t *)event_data;
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    /* this is BLE SMP pair confirm event */
    BLECFG_LOG_INFO("%s, conn_handle = %d\r\n", __func__, e->conn_handle);
    /* handle stack SMP pair confirm event */
    ble_stack_smp_passkey_confirm(netCfg_info->conn_handle);
}

static void BleCfg_event_conn_security_change(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE connection security change event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_conn_param_update(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE connect parameters update event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_mtu_exchange(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP cancel event, not used in this demo */
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static void BleCfg_event_adv_timeout(ble_event_en event, void *event_data)
{
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    /* this is BLE adv timeout event, restart adv */
    BLECFG_LOG_INFO("%s\r\n", __func__);
    //aos_msleep(100);
    BleCfg_start_adv();
}

static int BleCfg_event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;
    int16_t handle_offset = 0;
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    BLE_CHAR_RANGE_CHECK(netCfg_info->gatt_svc_handle, BLE_NETCFG_IDX_MAX, e->char_handle, handle_offset);
    BLECFG_LOG_INFO("%s: conn_handle =%d, char_handle %d, len =%d, offset =%d\r\n",
                    __func__, e->conn_handle, e->char_handle, e->len, e->offset);

    /* check connection handle */
    if (netCfg_info->conn_handle == e->conn_handle) {
        switch (handle_offset) {
            case BLE_NETCFG_IDX_CHAR1_VAL:
                /* if read length bigger than read char value length, return error */
                if (e->len > sizeof(g_BLE_netCfg_gatt_read_char)) {
                    e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                    return 0;
                }

                /* if read length+offset bigger than read char value length, return error */
                if (e->offset + e->len > sizeof(g_BLE_netCfg_gatt_read_char)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                /* data copy */
                memcpy(e->data, g_BLE_netCfg_gatt_read_char + e->offset, BLE_MIN(e->len, sizeof(g_BLE_netCfg_gatt_read_char)));
                e->len = BLE_MIN(e->len, sizeof(g_BLE_netCfg_gatt_read_char));
                break;
            default:
                e->len = 0;
                break;
        }
    }
    return 0;
}

static int BleCfg_event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int16_t handle_offset = 0;
    static int w_len = 0;
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    BLE_CHAR_RANGE_CHECK(netCfg_info->gatt_svc_handle, BLE_NETCFG_IDX_MAX, e->char_handle, handle_offset);
    BLECFG_LOG_INFO("%s: conn_handle = %d, char_handle = %d, len = %d, offset = %d\r\n",
                    __func__, e->conn_handle, e->char_handle, e->len, e->offset);

    BLECFG_LOG_INFO("%s: netCfg_info->conn_handle = %d, e->conn_handle = %d, handle_offset = %d, need = %d\r\n",
                    __func__, netCfg_info->conn_handle, e->conn_handle, handle_offset, BLE_NETCFG_IDX_CHAR2_VAL);

    /* check connection handle */
    if (netCfg_info->conn_handle == e->conn_handle) {
        switch (handle_offset) {
            case BLE_NETCFG_IDX_CHAR2_VAL:
                if (netCfg_info->miniapp_msg_cb != NULL ) {
                    netCfg_info->miniapp_msg_cb(e->data, e->len);
                }
                break;
            default:
                e->len = 0;
                break;
        }
    }

    return 0;
}

static void BleCfg_event_char_ccc_change(ble_event_en event, void *event_data)
{
    BLECFG_LOG_INFO("%s\r\n", __func__);
}

static int BleCfg_event_callback(ble_event_en event, void *event_data)
{
    BLECFG_LOG_INFO("%s, event = %x\r\n", __func__, event);

    /* handle stack events */
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            BleCfg_event_conn_change(event, event_data);
            break;
        case EVENT_GAP_CONN_PARAM_UPDATE:
            BleCfg_event_conn_param_update(event, event_data);
            break;
        case EVENT_SMP_PASSKEY_DISPLAY:
            BleCfg_event_pairing_passkey_display(event, event_data);
            break;
        case EVENT_SMP_PAIRING_COMPLETE:
            BleCfg_event_smp_complete(event, event_data);
            break;
        case EVENT_SMP_PAIRING_CONFIRM:
            BleCfg_event_smp_pairing_confirm(event, event_data);
            break;
        case EVENT_SMP_CANCEL:
            BleCfg_event_smp_cancel(event, event_data);
            break;
        case EVENT_GAP_CONN_SECURITY_CHANGE:
            BleCfg_event_conn_security_change(event, event_data);
            break;
        case EVENT_GATT_MTU_EXCHANGE:
            BleCfg_event_mtu_exchange(event, event_data);
            break;
        case EVENT_GAP_ADV_TIMEOUT:
            BleCfg_event_adv_timeout(event, event_data);
            break;
        case EVENT_GATT_CHAR_READ:
            BleCfg_event_char_read(event, event_data);
            break;
        case EVENT_GATT_CHAR_WRITE:
            BleCfg_event_char_write(event, event_data);
            break;
        case EVENT_GATT_CHAR_CCC_CHANGE:
            BleCfg_event_char_ccc_change(event, event_data);
            break;
        default:
            break;
    }

    return 0;
}

static int BleCfg_gatt_init(void)
{
    int ret;
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    /* register gatt service */
    ret = ble_stack_gatt_registe_service(&g_BLE_netCfg_gatt_service,
                                        g_BLE_netCfg_gatt_attrs,
                                        BLE_ARRAY_NUM(g_BLE_netCfg_gatt_attrs));
    if (ret < 0) {
        BLECFG_LOG_INFO("%s failed!, ret = %x\r\n", __func__, ret);
        return ret;
    }
    netCfg_info->gatt_svc_handle = ret;

    return 0;
}

BLECFG_STATE BleCfg_stack_init(BleCfg_msg_cb msg_callback)
{
    int ret;
    BleCfg_Info *netCfg_info = BleCfg_get_info();
    uint8_t mac[6];
    init_param_t init = {
        .dev_name = NULL,
        .dev_addr = NULL,   //&addr,
        .conn_num_max = 1,
    };
    int fd;

    if (netCfg_info->inited) {
        BLECFG_LOG_ERROR("%s: already initial, ret = %x\r\n", __func__, ret);
        return BLECFG_ALREADY;
    }

    fd = open("/dev/wifi0", O_RDWR);
    if (fd < 0) {
        return BLECFG_COMMON_FAILED;
    }

    ioctl(fd, WIFI_DEV_CMD_GET_MAC, (unsigned long)mac);

    snprintf(g_blecfg_devname, sizeof(g_blecfg_devname), "HaaS-%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    init.dev_name = g_blecfg_devname;

    BLECFG_LOG_INFO("%s: dev_name is '%s'\r\n", __func__, g_blecfg_devname);

    /* bt stack init */
    ret = ble_stack_init(&init);
    if (ret) {
        BLECFG_LOG_ERROR("%s: stack init, ret = %x\r\n", __func__, ret);
        return BLECFG_COMMON_FAILED;
    }

    /* register callback function to stack */
    netCfg_info->stack_cb.callback = BleCfg_event_callback;
    ret = ble_stack_event_register(&(netCfg_info->stack_cb));
    if(ret) {
        BLECFG_LOG_ERROR("%s: stack event register, ret = %x\r\n", __func__, ret);
        return BLECFG_COMMON_FAILED;
    }

    netCfg_info->miniapp_msg_cb = msg_callback;

#ifdef CHIP_HAAS1000
    extern int netdev_set_epta_params(int wlan_duration, int bt_duration, int hw_epta_enable);
    netdev_set_epta_params(60000, 40000, 0);
#endif

    /* gatt service init */
    ret = BleCfg_gatt_init();
    if(ret) {
        BLECFG_LOG_ERROR("%s: gatt init, ret = %x\r\n", __func__, ret);
        return BLECFG_COMMON_FAILED;
    }

    netCfg_info->inited = 1;

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_stack_start(void)
{
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    if (netCfg_info->inited == 0) {
        BLECFG_LOG_ERROR("%s: component not initialed\r\n", __func__);
        return BLECFG_COMMON_FAILED;
    }

    BLECFG_LOG_DEBUG("%s: state = %d\r\n", __func__, netCfg_info->started);
    if (netCfg_info->started == 0) {
        BleCfg_start_adv();
        netCfg_info->started = 1;
    }

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_stack_stop(void)
{
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    if (netCfg_info->inited == 0) {
        BLECFG_LOG_ERROR("%s: component not initialed\r\n", __func__);
        return -1;
    }

    BLECFG_LOG_DEBUG("%s: state = %d\r\n", __func__, netCfg_info->started);
    if (netCfg_info->started == 1) {
        BleCfg_stop_adv();
        netCfg_info->started = 0;
    }

    return BLECFG_SUCCESS;
}

/* ble断连，协议栈回自动回到adv广播模式 */
BLECFG_STATE BleCfg_stack_disconnect(void)
{
    BleCfg_Info *netCfg_info = BleCfg_get_info();

    if (netCfg_info->inited == 0) {
        BLECFG_LOG_ERROR("%s: component not initialed\r\n", __func__);
        return -1;
    }

    if (netCfg_info->conn_handle != -1) {
        ble_stack_disconnect(netCfg_info->conn_handle);
        BLECFG_LOG_INFO("%s: ble_stack_disconnect\r\n", __func__);
    }

    return BLECFG_SUCCESS;

}

BLECFG_STATE BleCfg_notificate(const uint8_t *data, uint16_t size)
{
    BleCfg_Info *netCfg_info = BleCfg_get_info();
    int ret;
    BLECFG_STATE state = BLECFG_COMMON_FAILED;

    ret = ble_stack_gatt_notificate(netCfg_info->conn_handle, netCfg_info->gatt_svc_handle + BLE_NETCFG_IDX_CHAR3_VAL, data, size);
    if (ret == 0) {
        state = BLECFG_SUCCESS;
    }
    BLECFG_LOG_DEBUG("%s: ret  = %d", __func__, ret);

    return state;
}

