/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include <aos/ble.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <atomic.h>
#include <work.h>
#include <conn_internal.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include "netmgr.h"


#define EXAMPLE_BLE_DEV_NAME        "HaaS NetConfig"
#define DEVICE_ADDR {0xE8,0x3B,0xE3,0x88,0xB4,0xC8}

#define EXAMPLE_TRACE_ERROR(fmt, ...)           printf("[ERR]"fmt"\n", ##__VA_ARGS__)
#define EXAMPLE_TRACE_WARNING(fmt, ...)         printf("[WARN]"fmt"\n", ##__VA_ARGS__)
#define EXAMPLE_TRACE_INFO(fmt, ...)            printf("[INFO]"fmt"\n", ##__VA_ARGS__)
#define EXAMPLE_TRACE_DEBUG(fmt, ...)           printf("[DBG]"fmt"\n", ##__VA_ARGS__)

uint8_t g_adv_flag = 0x06;
uint8_t g_adv_name[] = EXAMPLE_BLE_DEV_NAME;
uint16_t g_uuid16_list[] = {0xFFA0};
static int16_t g_conn_handle = -1;
static int8_t g_need_adv = 0;

static int event_callback(ble_event_en event, void *event_data);
static void example_notify_ccc_changed(struct bt_conn *conn,
                                            const struct bt_gatt_attr *attr, uint16_t value);

static void start_adv(void);
static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED && e->err == 0) {
        g_conn_handle = e->conn_handle;
    } else {
        g_conn_handle = -1;
        g_need_adv = 1;
    }
}

static void event_pairing_passkey_display(ble_event_en event, void *event_data)
{

}

static void event_smp_complete(ble_event_en event, void *event_data)
{

}

static void event_smp_cancel(ble_event_en event, void *event_data)
{

}

static void smp_pairing_confirm(evt_data_smp_pairing_confirm_t *e)
{

}

static void conn_security_change(ble_event_en event, void *event_data)
{

}

static void conn_param_update(ble_event_en event, void *event_data)
{

}

static void mtu_exchange(ble_event_en event, void *event_data)
{

}

static int event_callback(ble_event_en event, void *event_data)
{
    EXAMPLE_TRACE_INFO("%s, event = %x", __func__, event);
    switch (event) {
    case EVENT_GAP_CONN_CHANGE:
        conn_change(event, event_data);
        break;

    case EVENT_GAP_CONN_PARAM_UPDATE:
        conn_param_update(event, event_data);
        break;

    case EVENT_SMP_PASSKEY_DISPLAY:
        event_pairing_passkey_display(event, event_data);
        break;

    case EVENT_SMP_PAIRING_COMPLETE:
        event_smp_complete(event, event_data);
        break;

    case EVENT_SMP_PAIRING_CONFIRM:
        smp_pairing_confirm(event_data);

        ble_stack_smp_passkey_confirm(g_conn_handle);
        break;

    case EVENT_SMP_CANCEL:
        event_smp_cancel(event, event_data);
        break;

    case EVENT_GAP_CONN_SECURITY_CHANGE:
        conn_security_change(event, event_data);
        break;

    case EVENT_GATT_MTU_EXCHANGE:
        mtu_exchange(event, event_data);
        break;

    case EVENT_GAP_ADV_TIMEOUT:
        start_adv();
        break;

    default:
        break;
    }

    return 0;
}

static void start_adv(void)
{
    ad_data_t ad[2] = {0};
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&g_adv_flag;
    ad[0].len = 1;

    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)g_uuid16_list;
    ad[1].len = sizeof(g_uuid16_list);

//    ad[2].type = AD_DATA_TYPE_NAME_COMPLETE;
//    ad[2].data = g_adv_name;
//    ad[2].len = sizeof(g_adv_name);

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

    int ret = ble_stack_adv_start(&param);
    if (ret) {
        EXAMPLE_TRACE_ERROR("adv start fail %d!", ret);
    } else {
        EXAMPLE_TRACE_INFO("adv start!");
    }
}

#define UUID_VENDOR_SERVICE                      UUID16_DECLARE(0xFFA0)
#define UUID_VENDOR_CHAR_READ                    UUID16_DECLARE(0xFFA1)
#define UUID_VENDOR_CHAR_WRITE                   UUID16_DECLARE(0xFFA2)
#define UUID_VENDOR_CHAR_NOTIFY                  UUID16_DECLARE(0xFFA3)

uint8_t example_gatt_write_char[16];
uint8_t example_gatt_read_char[16] = "HaaS Read";
uint8_t example_gatt_notify_char[16];
static struct _bt_gatt_ccc example_gatt_notify_ccc = BT_GATT_CCC_INITIALIZER(NULL,
                                                                            example_notify_ccc_changed,
                                                                            NULL);

const char *log_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[4][129];
    static u8_t curbuf;
    const u8_t *b = buf;
    unsigned int mask;
    char *str;
    int i;

    mask = irq_lock();
    str = hexbufs[curbuf++];
    curbuf %= ARRAY_SIZE(hexbufs);
    irq_unlock(mask);

    len = (len < (sizeof(hexbufs[0]) - 1) / 2)?len:(sizeof(hexbufs[0])-1)/2;

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}

uint8_t g_test_ssid[256];
uint8_t g_test_passwd[256];

void wifi_connect_handle()
{
    netmgr_connect(g_test_ssid, g_test_passwd, 5000);
}

void example_BLENetConfig_Parse(uint8_t *buf, uint16_t len)
{
    netmgr_ap_config_t* ap = NULL;
    int8_t ssid_len, pwd_len;

    ssid_len = buf[0];
    pwd_len = buf[1];

    if (len < ssid_len+pwd_len+2) {
        EXAMPLE_TRACE_ERROR("%s: NetConfig wrong data recv", __func__);
        return;
    }
    if (ap = (netmgr_ap_config_t*)malloc(sizeof(netmgr_ap_config_t))) {
        memset(ap, 0, sizeof(netmgr_ap_config_t));
    } else {
        return;
    }

    strncpy(ap->ssid, buf+2, sizeof(ap->ssid) - 1);
    if (ssid_len < sizeof(ap->ssid))
        ap->ssid[ssid_len] = 0;
    strncpy(ap->pwd, buf+2+ssid_len, sizeof(ap->pwd) - 1);
    if (pwd_len < sizeof(ap->pwd))
        ap->pwd[pwd_len] = 0;

    EXAMPLE_TRACE_INFO("%s: NetConf SSID = %s, PWD = %s", __func__, ap->ssid, ap->pwd);

	strncpy(g_test_ssid, ap->ssid, sizeof(g_test_ssid));
	strncpy(g_test_passwd, ap->pwd, sizeof(g_test_passwd));
	aos_task_new("wifi_connect_task",(task_entry_t)wifi_connect_handle, NULL, 4*1024);

    free(ap);
}

static int32_t example_char_read_cb(struct bt_conn *conn,
                        const struct bt_gatt_attr *attr, void *buf,
                        uint16_t len, uint16_t offset)
{
    EXAMPLE_TRACE_INFO("%s:uuid = %x", __func__, ((struct bt_uuid_16 *)attr->uuid)->val);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(example_gatt_read_char));
}

static int32_t example_char_write_cb(struct bt_conn *conn,
                        const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags)
{
    uint8_t *value = attr->user_data;

    EXAMPLE_TRACE_INFO("%s:uuid = %x, val = %s", __func__, ((struct bt_uuid_16 *)attr->uuid)->val, log_hex(buf, len));
    if (offset + len > sizeof(example_gatt_write_char)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    example_BLENetConfig_Parse(buf, len);

    return len;
}

static int32_t example_char_notify_cb(struct bt_conn *conn,
                        const struct bt_gatt_attr *attr, void *buf,
                        uint16_t len, uint16_t offset)
{
    EXAMPLE_TRACE_INFO("%s:uuid = %x", __func__, ((struct bt_uuid_16 *)attr->uuid)->val);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(example_gatt_notify_char));
}

static void example_notify_ccc_changed(struct bt_conn *conn,
                                            const struct bt_gatt_attr *attr, uint16_t value)
{
    EXAMPLE_TRACE_INFO("%s:value = %x", __func__, value);
}

gatt_attr_t  g_example_BLE_gatt_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(UUID_VENDOR_SERVICE),
    /* READ CHAR */
    BT_GATT_CHARACTERISTIC(UUID_VENDOR_CHAR_READ, BT_GATT_CHRC_READ,
                            BT_GATT_PERM_READ,
                            example_char_read_cb, NULL,
                            &example_gatt_read_char),

    /* WRITE CHAR */
    BT_GATT_CHARACTERISTIC(UUID_VENDOR_CHAR_WRITE, BT_GATT_CHRC_WRITE,
                            BT_GATT_PERM_WRITE,
                            NULL, example_char_write_cb,
                            &example_gatt_write_char),

    /* NOTIFY CHAR */
    BT_GATT_CHARACTERISTIC(UUID_VENDOR_CHAR_NOTIFY, BT_GATT_CHRC_READ|BT_GATT_CHRC_NOTIFY,
                            BT_GATT_PERM_READ,
                            example_char_notify_cb, NULL,
                            &example_gatt_notify_char),
    BT_GATT_CCC_MANAGED(&example_gatt_notify_ccc, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service g_example_BLE_gatt_service = BT_GATT_SERVICE(g_example_BLE_gatt_attrs);

int example_NetConfig_Gatt_init(void)
{
    int ret;

    ret = bt_gatt_service_register(&g_example_BLE_gatt_service);
    if (ret != 0) {
        EXAMPLE_TRACE_ERROR("Register Gatt failed!, ret = %x\r\n", ret);
    }
}

int example_NetConfig_BLE_init(void)
{
    int ret;
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = EXAMPLE_BLE_DEV_NAME,
        .dev_addr = &addr,   //&addr,
        .conn_num_max = 1,
    };

    aos_msleep(1*1000);
    /* we need first init hci driver */
    hci_h4_driver_init();

    /* bt stack init */
    ret = ble_stack_init(&init);
    if (ret) {
        EXAMPLE_TRACE_ERROR("ble_stack_init!, ret = %x\r\n", ret);
        return -1;
    }

    ret = ble_stack_event_register(&ble_cb);
    if(ret) {
        return -1;
    }

    return 0;
}

int application_start(int argc, char *argv[])
{
    int count = 0;
    int ret;

    EXAMPLE_TRACE_DEBUG("nano entry here!\r\n");

    netmgr_init();
//    aos_msleep(300);
//    netmgr_start(true);

    ret = example_NetConfig_BLE_init();
    if (ret) {
        EXAMPLE_TRACE_DEBUG("NetConfig BLE init failed");
        return ret;
    }

    ret = example_NetConfig_Gatt_init();
    if (ret) {
        EXAMPLE_TRACE_DEBUG("NetConfig Gatt init failed");
        return ret;
    }

    start_adv();
    while(1) {
        if (g_need_adv) {
            aos_msleep(300);
            start_adv();
            g_need_adv = 0;
        }
        aos_msleep(1000);
    };
}

