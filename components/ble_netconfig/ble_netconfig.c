/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <ulog/ulog.h>

#include <aos/ble.h>
#include <atomic.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <netmgr.h>
#include <netmgr_wifi.h>
#include <ble_netconfig.h>
#include <aos/cli.h>

#define TAG "BLE_NETCFG"
#define BLE_NETCFG_CLI      1

#define BLE_NETCFG_LOG_ERROR(fmt, ...)           LOGE(TAG, fmt, ##__VA_ARGS__)
#define BLE_NETCFG_LOG_WARNING(fmt, ...)         LOGE(TAG, fmt, ##__VA_ARGS__)
#define BLE_NETCFG_LOG_INFO(fmt, ...)            LOGE(TAG, fmt, ##__VA_ARGS__)
#define BLE_NETCFG_LOG_DEBUG(fmt, ...)           LOGE(TAG, fmt, ##__VA_ARGS__)

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

#define BLE_NETCFG_DEV_NAME        "HaaS NetConfig"
#define DEVICE_ADDR                 {0xE8,0x3B,0xE3,0x88,0xB4,0xC8}

typedef struct {
    uint8_t inited;
    uint8_t started;
    uint8_t ssid_recved;
    uint8_t ssid[256];
    uint8_t passwd[256];
    BLE_netCfg_callck callback;

    ble_event_cb_t stack_cb;

    int16_t conn_handle;
    int8_t need_adv;
    uint32_t gatt_svc_handle;
}BLE_NetCfg_Info;

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
uint8_t g_BLE_netCfg_gatt_write_char[128];
uint8_t g_BLE_netCfg_gatt_read_char[16] = "NetCfg Read";
uint8_t g_BLE_netCfg_gatt_notify_char[16];

uint8_t g_BLE_netCfg_adv_flag = 0x06;
uint16_t g_BLE_netCfg_uuid16_list[] = {0xFFA0};

static BLE_NetCfg_Info g_BLE_netCfg_info;

static void BLE_NetCfg_start_adv(void);

static BLE_NetCfg_Info * BLE_NetCfg_get_info(void)
{
    return &g_BLE_netCfg_info;
}

static void BLE_NetCfg_wifi_connect_handle(void)
{
    int netmgr_handle, ret;
    netmgr_connect_params_t netmgr_params;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    while (1) {
        if (netCfg_info->ssid_recved) {
            memset(&netmgr_params, 0, sizeof(netmgr_connect_params_t));
            strncpy(netmgr_params.params.wifi_params.ssid, netCfg_info->ssid, NETMGR_SSID_MAX_LEN);
            strncpy(netmgr_params.params.wifi_params.pwd, netCfg_info->passwd, NETMGR_PWD_MAX_LEN);
            netmgr_params.params.wifi_params.timeout = 5000;

            netmgr_handle = netmgr_get_dev("/dev/wifi0");
            ret = netmgr_connect(netmgr_handle, &netmgr_params);
            BLE_NETCFG_LOG_INFO("%s: netmgr_connect return %d", __func__, ret);

            // This setting is for Wi-Fi and BLE coexistence
            netdev_set_epta_params(60000, 40000, 0);

            if (netCfg_info->callback) {
                if (ret == 0) {
                    netCfg_info->callback(BLE_NETCFG_EVENT_SUCCESS, BLE_NETCFG_SUCCESS, NULL);
                } else {
                     netCfg_info->callback(BLE_NETCFG_EVENT_FAILED, BLE_NETCFG_COMMON_FAILED, NULL);
                }
            }

            netCfg_info->ssid_recved = 0;
        }

        if (netCfg_info->need_adv) {
            aos_msleep(100);
            BLE_NetCfg_start_adv();
            netCfg_info->need_adv = 0;
        }

        aos_msleep(300);
    }
}

static void BLE_NetCfg_Parse(uint8_t *buf, uint16_t len)
{
    int8_t ssid_len, pwd_len;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    ssid_len = buf[0];
    pwd_len = buf[1];
    if (len < ssid_len+pwd_len+2) {
        BLE_NETCFG_LOG_ERROR("%s: NetConfig wrong data recv", __func__);
        return;
    }

    if (netCfg_info->started == 0) {
        BLE_NETCFG_LOG_ERROR("%s: not in netcfg state", __func__);
        return;
    }

    strncpy(netCfg_info->ssid, buf+2, sizeof(netCfg_info->ssid) - 1);
    if (ssid_len < sizeof(netCfg_info->ssid))
        netCfg_info->ssid[ssid_len] = 0;
    strncpy(netCfg_info->passwd, buf+2+ssid_len, sizeof(netCfg_info->passwd) - 1);
    if (pwd_len < sizeof(netCfg_info->passwd))
        netCfg_info->passwd[pwd_len] = 0;

    netCfg_info->ssid_recved = 1;

    BLE_NETCFG_LOG_INFO("%s: NetConf SSID = %s, PWD = %s", __func__, netCfg_info->ssid, netCfg_info->passwd);
}

static void BLE_NetCfg_event_conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    BLE_NETCFG_LOG_INFO("%s, connected = %d", __func__, e->connected);
    /* check if connect or disconnect */
    if (e->connected == CONNECTED && e->err == 0) {
        netCfg_info->conn_handle = e->conn_handle;
        if (netCfg_info->callback) {
           netCfg_info->callback(BLE_NETCFG_EVENT_CONNECTED, BLE_NETCFG_SUCCESS, NULL);
        }
    } else {
        netCfg_info->conn_handle = -1;
        netCfg_info->need_adv = 1;
    }
}

static void BLE_NetCfg_event_pairing_passkey_display(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP passkey event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_smp_complete(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP complete event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_smp_cancel(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP cancel event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_smp_pairing_confirm(ble_event_en event, void *event_data)
{
    evt_data_smp_pairing_confirm_t *e = (evt_data_smp_pairing_confirm_t *)event_data;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    /* this is BLE SMP pair confirm event */
    BLE_NETCFG_LOG_INFO("%s, conn_handle = %d", __func__, e->conn_handle);
    /* handle stack SMP pair confirm event */
    ble_stack_smp_passkey_confirm(netCfg_info->conn_handle);
}

static void BLE_NetCfg_event_conn_security_change(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE connection security change event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_conn_param_update(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE connect parameters update event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_mtu_exchange(ble_event_en event, void *event_data)
{
    /* UNDO: this is BLE SMP cancel event, not used in this demo */
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static void BLE_NetCfg_event_adv_timeout(ble_event_en event, void *event_data)
{
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    /* this is BLE adv timeout event, restart adv */
    BLE_NETCFG_LOG_INFO("%s", __func__);
    netCfg_info->need_adv = 1;
}

static void BLE_NetCfg_event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;
    int16_t handle_offset = 0;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    BLE_CHAR_RANGE_CHECK(netCfg_info->gatt_svc_handle, BLE_NETCFG_IDX_MAX, e->char_handle, handle_offset);
    BLE_NETCFG_LOG_INFO("%s: conn_handle =%d, char_handle %d, len =%d, offset =%d", __func__,
                e->conn_handle, e->char_handle, e->len, e->offset);

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
}

static int BLE_NetCfg_event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int16_t handle_offset = 0;
    static int w_len = 0;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    BLE_CHAR_RANGE_CHECK(netCfg_info->gatt_svc_handle, BLE_NETCFG_IDX_MAX, e->char_handle, handle_offset);
    BLE_NETCFG_LOG_INFO("%s: conn_handle = %d, char_handle = %d, len = %d, offset = %d", __func__,
                e->conn_handle, e->char_handle, e->len, e->offset);

    /* check connection handle */
    if (netCfg_info->conn_handle == e->conn_handle) {
        switch (handle_offset) {
        case BLE_NETCFG_IDX_CHAR2_VAL:
            /* if write length bigger than write char value length, return error */
            if (e->len > sizeof(g_BLE_netCfg_gatt_write_char)) {
                e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                return 0;
            }

            /* if write length + offset bigger than write char value length, return error */
            if (e->offset + e->len > sizeof(g_BLE_netCfg_gatt_write_char)) {
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            /* data copy */
            memcpy(g_BLE_netCfg_gatt_write_char + e->offset, e->data, BLE_MIN(e->len, sizeof(g_BLE_netCfg_gatt_write_char)));
            e->len = BLE_MIN(e->len, sizeof(g_BLE_netCfg_gatt_write_char));

            BLE_NetCfg_Parse(e->data, e->len);
            break;
        default:
            e->len = 0;
            break;
        }
    }

    return 0;
}

static void BLE_NetCfg_event_char_ccc_change(ble_event_en event, void *event_data)
{
    BLE_NETCFG_LOG_INFO("%s", __func__);
}

static int BLE_NetCfg_event_callback(ble_event_en event, void *event_data)
{
    BLE_NETCFG_LOG_INFO("%s, event = %x", __func__, event);

    /* handle stack events */
    switch (event) {
    case EVENT_GAP_CONN_CHANGE:
        BLE_NetCfg_event_conn_change(event, event_data);
        break;
    case EVENT_GAP_CONN_PARAM_UPDATE:
        BLE_NetCfg_event_conn_param_update(event, event_data);
        break;
    case EVENT_SMP_PASSKEY_DISPLAY:
        BLE_NetCfg_event_pairing_passkey_display(event, event_data);
        break;
    case EVENT_SMP_PAIRING_COMPLETE:
        BLE_NetCfg_event_smp_complete(event, event_data);
        break;
    case EVENT_SMP_PAIRING_CONFIRM:
        BLE_NetCfg_event_smp_pairing_confirm(event, event_data);
        break;
    case EVENT_SMP_CANCEL:
        BLE_NetCfg_event_smp_cancel(event, event_data);
        break;
    case EVENT_GAP_CONN_SECURITY_CHANGE:
        BLE_NetCfg_event_conn_security_change(event, event_data);
        break;
    case EVENT_GATT_MTU_EXCHANGE:
        BLE_NetCfg_event_mtu_exchange(event, event_data);
        break;
    case EVENT_GAP_ADV_TIMEOUT:
        BLE_NetCfg_event_adv_timeout(event, event_data);
        break;
    case EVENT_GATT_CHAR_READ:
        BLE_NetCfg_event_char_read(event, event_data);
        break;
    case EVENT_GATT_CHAR_WRITE:
        BLE_NetCfg_event_char_write(event, event_data);
        break;
    case EVENT_GATT_CHAR_CCC_CHANGE:
        BLE_NetCfg_event_char_ccc_change(event, event_data);
        break;
    default:
        break;
    }

    return 0;
}

static void BLE_NetCfg_start_adv(void)
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
        .direct_peer_addr = NULL,
    };

    /* setup ADV Flags */
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&g_BLE_netCfg_adv_flag;
    ad[0].len = 1;

    /* setup ADV UUIDS */
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)g_BLE_netCfg_uuid16_list;
    ad[1].len = sizeof(g_BLE_netCfg_uuid16_list);

    /* call ADV start API */
    ret = ble_stack_adv_start(&param);
    if (ret) {
        BLE_NETCFG_LOG_ERROR("adv start fail %d!", ret);
    } else {
        BLE_NETCFG_LOG_INFO("adv start!");
    }
}

static void BLE_NetCfg_stop_adv(void)
{
    int ret;

    /* call ADV start API */
    ret = ble_stack_adv_stop();
    if (ret) {
        BLE_NETCFG_LOG_ERROR("adv stop fail %d!", ret);
    } else {
        BLE_NETCFG_LOG_INFO("adv stop!");
    }
}

static int BLE_NetCfg_gatt_init(void)
{
    int ret;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    /* register gatt service */
    ret = ble_stack_gatt_registe_service(&g_BLE_netCfg_gatt_service, 
                                        g_BLE_netCfg_gatt_attrs, 
                                        BLE_ARRAY_NUM(g_BLE_netCfg_gatt_attrs));
    if (ret < 0) {
        BLE_NETCFG_LOG_INFO("%s failed!, ret = %x\r\n", __func__, ret);
        return ret;
    }
    netCfg_info->gatt_svc_handle = ret;

    return 0;
}

#ifdef BLE_NETCFG_CLI
typedef int (*ble_netCfg_shell_cmd_function_t)(int argc, char *argv[]);

struct ble_netCfg_shell_cmd {
    const char *cmd_name;
    ble_netCfg_shell_cmd_function_t cb;
    const char *help;
    const char *desc;
};

int BLE_NetCfg_cli_cmd_start(int argc, char *argv[])
{
    BLE_NetCfg_start();
    return 0;
}

int BLE_NetCfg_cli_cmd_stop(int argc, char *argv[])
{
    BLE_NetCfg_stop();
    return 0;
}

static const struct ble_netCfg_shell_cmd ble_netCfg_commands[] = {
    { "start", BLE_NetCfg_cli_cmd_start, "None" },
    { "stop", BLE_NetCfg_cli_cmd_stop, "None" },
};

static void BLE_NetCfg_cli_cmd_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int i = 0;
    int err;

    if (argc < 2) {
        BLE_NETCFG_LOG_INFO("Ble NetConfig support commands");

        for (i = 0; ble_netCfg_commands[i].cmd_name != NULL; i ++) {
            BLE_NETCFG_LOG_INFO("    %s %s", ble_netCfg_commands[i].cmd_name, ble_netCfg_commands[i].help);
        }
        return;
    }

    for (i = 0; ble_netCfg_commands[i].cmd_name != NULL; i ++) {
        if (strlen(ble_netCfg_commands[i].cmd_name) == strlen(argv[1]) &&
            !strncmp(ble_netCfg_commands[i].cmd_name, argv[1], strlen(ble_netCfg_commands[i].cmd_name))) {
            if (ble_netCfg_commands[i].cb) {
                err = ble_netCfg_commands[i].cb(argc - 1, &argv[1]);

                if (err) {
                    BLE_NETCFG_LOG_INFO("%s execute fail, %d\n", ble_netCfg_commands[i].cmd_name, err);
                }
                break;
            }
        }
    }
}

static void BLE_NetCfg_cli_reg_cmd(void)
{
    static const struct cli_command cmd_info = {
        "ble_netCfg",
        "ble netConfig commands",
        BLE_NetCfg_cli_cmd_handler,
    };

    aos_cli_register_command(&cmd_info);
}

#endif

BLE_NETCFG_STATE BLE_NetCfg_init(BLE_netCfg_callck callback)
{
    int ret;
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = BLE_NETCFG_DEV_NAME,
        .dev_addr = &addr,   //&addr,
        .conn_num_max = 1,
    };

    if (netCfg_info->inited) {
        BLE_NETCFG_LOG_ERROR("%s: already initial, ret = %x\r\n", __func__, ret);
        return BLE_NETCFG_ALREADY;
    }

    /* bt stack init */
    ret = ble_stack_init(&init);
    if (ret) {
        BLE_NETCFG_LOG_ERROR("%s: stack init, ret = %x\r\n", __func__, ret);
        return BLE_NETCFG_COMMON_FAILED;
    }

    /* register callback function to stack */
    netCfg_info->stack_cb.callback = BLE_NetCfg_event_callback;
    ret = ble_stack_event_register(&(netCfg_info->stack_cb));
    if(ret) {
        BLE_NETCFG_LOG_ERROR("%s: stack init, ret = %x\r\n", __func__, ret);
        return BLE_NETCFG_COMMON_FAILED;
    }

    /* gatt service init */
    ret = BLE_NetCfg_gatt_init();
    if(ret) {
        BLE_NETCFG_LOG_ERROR("%s: stack init, ret = %x\r\n", __func__, ret);
        return BLE_NETCFG_COMMON_FAILED;
    }

    /* netmgr init */
    event_service_init(NULL);
    ret = netmgr_service_init(NULL);
    if(ret) {
        BLE_NETCFG_LOG_ERROR("%s: net mgr init, ret = %x\r\n", __func__, ret);
        return BLE_NETCFG_COMMON_FAILED;
    }

    netCfg_info->callback = callback;
    netCfg_info->inited = 1;
    aos_task_new("wifi_connect_task", (task_entry_t)BLE_NetCfg_wifi_connect_handle, NULL, 4*1024);

#ifdef BLE_NETCFG_CLI
    BLE_NetCfg_cli_reg_cmd();
#endif

    return BLE_NETCFG_SUCCESS;
}

BLE_NETCFG_STATE BLE_NetCfg_start(void)
{
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    if (netCfg_info->inited == 0) {
        BLE_NETCFG_LOG_ERROR("%s: component not initialed", __func__);
        return BLE_NETCFG_COMMON_FAILED;
    }

    BLE_NETCFG_LOG_DEBUG("%s: state = %d", __func__, netCfg_info->started);
    if (netCfg_info->started == 0) {
        BLE_NetCfg_start_adv();
        netCfg_info->started = 1;
    }

    if (netCfg_info->callback) {
        netCfg_info->callback(BLE_NETCFG_EVENT_STARTED, BLE_NETCFG_SUCCESS, NULL);
    }
    return BLE_NETCFG_SUCCESS;
}

BLE_NETCFG_STATE BLE_NetCfg_stop(void)
{
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();

    if (netCfg_info->inited == 0) {
        BLE_NETCFG_LOG_ERROR("%s: component not initialed", __func__);
        return -1;
    }

    BLE_NETCFG_LOG_DEBUG("%s: state = %d", __func__, netCfg_info->started);
    if (netCfg_info->started == 1) {
        BLE_NetCfg_stop_adv();
        netCfg_info->started = 0;
    }

    if (netCfg_info->callback) {
        netCfg_info->callback(BLE_NETCFG_EVENT_STOPPED, BLE_NETCFG_SUCCESS, NULL);
    }

    return BLE_NETCFG_SUCCESS;
}

BLE_NETCFG_STATE BLE_NetCfg_notificate(const uint8_t *data, uint16_t size)
{
    BLE_NetCfg_Info *netCfg_info = BLE_NetCfg_get_info();
    int ret;
    BLE_NETCFG_STATE state = BLE_NETCFG_COMMON_FAILED;

    ret = ble_stack_gatt_notificate(netCfg_info->conn_handle, netCfg_info->gatt_svc_handle + BLE_NETCFG_IDX_CHAR3_VAL, data, size);
    if (ret == 0) {
        state = BLE_NETCFG_SUCCESS;
    }
    BLE_NETCFG_LOG_DEBUG("%s: ret  = %d", __func__, ret);

    return state;
}
