/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>


/*
 *API test:
 *    ble stack
 *        - bt_enable
 *    ble peripheral
 *        - bt_le_adv_start
 *        - bt_le_adv_stop
 *    ble central
 *        - bt_le_scan_start
 *        - bt_le_scan_stop
 *        - bt_conn_create_le
 *    ble gatt
 *        - bt_gatt_service_register
 *        - bt_service_unregister
 *        - bt_gatt_discover 
 *              - BT_GATT_DISCOVER_PRIMARY
 *              - BT_GATT_DISCOVER_CHARACTERISTIC
 *              - BT_GATT_DISCOVER_DESCRIPTOR
 *        - bt_gatt_subscribe
 *              - BT_GATT_CCC_INDICATE
 *              - BT_GATT_CCC_NOTIFY
 *        - bt_gatt_unsubscribe
 *        - bt_gatt_notify
 *        - bt_gatt_indicate
 *        - bt_gatt_write
 *        - bt_gatt_write_without_response
 *        - bt_gatt_read
 *t-put test
 *        t_put_client
 *        t_put_server
 *          - write
 *          - write_without_rsp
 *          - notify
 *          - indication
 *          - MTU (23, 40, 60, 80, 100,..)(BYTES)
 *          - connection interval(7.5, 15, 40, 100, ...)(ms)
 *stable test
 *        connection
 *           - connection, disconnect 100 times
 *        data stable
 *           - write/notify/indicate  24h
 *
 * */


/*test service*/

static struct bt_uuid_128 test_uuid = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static struct bt_uuid_128 test_chrc_uuid = BT_UUID_INIT_128(
    0xf2, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static u8_t test_value[] = { 'T', 'e', 's', 't', '\0' };

static struct bt_uuid_128 test1_uuid = BT_UUID_INIT_128(
    0xf4, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static const struct bt_uuid_128 test1_nfy_uuid = BT_UUID_INIT_128(
    0xf5, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static u8_t nfy_enabled;

static struct bt_gatt_ccc_cfg test_ccc_cfg[BT_GATT_CCC_MAX] = {};

static void test1_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    nfy_enabled = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t read_test(struct bt_conn *conn, const struct bt_gatt_attr *attr,
            void *buf, u16_t len, u16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
                 strlen(value));
}

static ssize_t write_test(struct bt_conn *conn, const struct bt_gatt_attr *attr,
             const void *buf, u16_t len, u16_t offset,
             u8_t flags)
{
    u8_t *value = attr->user_data;
    
    if (offset + len > sizeof(test_value)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

static struct bt_gatt_attr test_attrs[] = {
    /* Vendor Primary Service Declaration */
    BT_GATT_PRIMARY_SERVICE(&test_uuid),
    BT_GATT_CHARACTERISTIC(&test_chrc_uuid.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE),
    BT_GATT_DESCRIPTOR(&test_chrc_uuid.uuid, BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
                           read_test, write_test, test_value),
#if 0
    BT_GATT_CHARACTERISTIC(&test_chrc_uuid.uuid,
                   BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                   BT_GATT_PERM_READ_AUTHEN |
                   BT_GATT_PERM_WRITE_AUTHEN,
                   read_test, write_test, test_value),
#endif
};
static struct bt_gatt_service test_svc = BT_GATT_SERVICE(test_attrs);

static struct bt_gatt_attr test1_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(&test1_uuid),
    BT_GATT_CHARACTERISTIC(&test1_nfy_uuid.uuid, BT_GATT_CHRC_NOTIFY),
    BT_GATT_DESCRIPTOR(&test1_nfy_uuid.uuid, BT_GATT_PERM_NONE,
                   NULL, NULL, &nfy_enabled),
    BT_GATT_CCC(test_ccc_cfg, test1_ccc_cfg_changed),

#if 0
    BT_GATT_CHARACTERISTIC(&test1_nfy_uuid.uuid,
                   BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE,
                   NULL, NULL, &nfy_enabled),
    BT_GATT_CCC(test1_ccc_cfg_changed,
            BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
#endif
};
static struct bt_gatt_service test1_svc = BT_GATT_SERVICE(test1_attrs);


bool bt_is_ready = false;
static void bt_ready(int err)
{
    if (err) {
        LOGE("ble", "Init failed (err %d)\n", err);
        return;
    }
    bt_is_ready = true;
}


static void test_ble_init(void)
{
    YUNIT_ASSERT_FALSE(bt_enable(bt_ready));
    LOGI("ble","Stack initialized");
    /*for async event*/
    aos_msleep(2*1000);

    YUNIT_ASSERT(bt_is_ready == true);
    if(bt_is_ready == false){
        LOGE("ble", "BLE init failed");
    }
}

static void test_ble_adv(void)
{
    bt_addr_t addr = {.val = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x05 }};
    struct bt_le_adv_param adv_param = {
        .options = 0, \
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2, \
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2, \
        .own_addr = &addr, \
    };

    uint8_t data[] = { 0x6 };  // LE General Discoverable and BR/EDR not supported
    char *adv_name = "AOS-BLE-HELLO";
    const struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
    };

    YUNIT_ASSERT_FALSE(bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0));
    aos_msleep(3*1000);
    YUNIT_ASSERT_FALSE(bt_le_adv_stop());
}

static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t type, struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    /* We're only interested in connectable events */
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    if(rssi < -70){
        return;
    }
    LOGI("ble", "Found devices %s", addr_str);
}
static void test_ble_scan(void)
{
    YUNIT_ASSERT_FALSE(bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found));
    aos_msleep(3*1000);
    YUNIT_ASSERT_FALSE(bt_le_scan_stop());
}

static void test_ble_gatt_register(void)
{

    /*register 2 test services*/
    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test_svc));
    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test1_svc));

    /*reigster duplicate services*/
    YUNIT_ASSERT_TRUE(bt_gatt_service_register(&test_svc));
    YUNIT_ASSERT_TRUE(bt_gatt_service_register(&test1_svc));
}
static void test_ble_gatt_unregister(void)
{
    /*unregister last service*/
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test1_svc));
    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test1_svc));

    /*unregister first service*/
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test_svc));
#if 0
    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test_svc));

    /*unregister all reverse order*/
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test1_svc));
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test_svc));

    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test_svc));
    YUNIT_ASSERT_FALSE(bt_gatt_service_register(&test1_svc));

    /*unregister all reverse order*/
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test_svc));
    YUNIT_ASSERT_FALSE(bt_gatt_service_unregister(&test1_svc));
#endif
}


static u8_t count_attr(const struct bt_gatt_attr *attr, void *user_data)
{
    u16_t *count = user_data;

    (*count)++;

    return BT_GATT_ITER_CONTINUE;
}

static u8_t find_attr(const struct bt_gatt_attr *attr, void *user_data)
{
    const struct bt_gatt_attr **tmp = user_data;

    *tmp = attr;

    return BT_GATT_ITER_CONTINUE;
}
static void test_ble_gatt_foreach(void)
{
    uint16_t num = 0;
    const struct bt_gatt_attr *attr;

    int err = bt_gatt_service_register(&test_svc);
    YUNIT_ASSERT(err == 0);
    if(err){
        LOGE("ble", "gatt service register failed %d", err);
    }
    err = bt_gatt_service_register(&test1_svc);
    YUNIT_ASSERT(err == 0);
    if(err){
        LOGE("ble", "gatt service1 register failed %d", err);
    }
#if 0
    /* Iterate attributes */
    bt_gatt_foreach_attr(test_attrs[0].handle, 0xffff, count_attr, &num);
    YUNIT_ASSERT(num == 7);
   /*interate 1 attr*/ 
    num = 0;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff, NULL, NULL, 1,
                  count_attr, &num);
    YUNIT_ASSERT(num == 1);
    
    /*serach attr by UUID*/
    attr = NULL;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff,
                  &test_chrc_uuid.uuid, NULL, 0, find_attr, &attr);
    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    if(attr){
        YUNIT_ASSERT_PTR_EQUAL(attr->uuid, &test_chrc_uuid.uuid);
    }

    /*search attr by data*/
    attr = NULL;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff, NULL,
                  test_value, 0, find_attr, &attr);
    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    if(attr){
        YUNIT_ASSERT_PTR_EQUAL(attr->user_data, &test_value);
    }

    /*find all chars*/
    num = 0;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff,
                  BT_UUID_GATT_CHRC, NULL, 0, count_attr, &num);

    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    /*find all attr by UUID and data*/
    attr = NULL;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff,
                  &test1_nfy_uuid.uuid, &nfy_enabled, 1,
                  find_attr, &attr);

    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    if(attr){
        YUNIT_ASSERT_PTR_EQUAL(attr->uuid, &test1_nfy_uuid.uuid);
        YUNIT_ASSERT_PTR_EQUAL(attr->user_data, &nfy_enabled);
    }
#endif

}
static void test_ble_gatt_read(void)
{
    const struct bt_gatt_attr *attr;
    uint8_t buf[256];
    ssize_t ret;
#if 0

    /*find all attrs by UUID*/
    bt_gatt_foreach_attr；
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff,
                  &test_chrc_uuid.uuid, NULL, 0, find_attr, &attr);
    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    YUNIT_ASSERT_PTR_EQUAL(attr->uuid, &test_chrc_uuid.uuid);
    ret = attr->read(NULL, attr, (void *)buf, sizeof(buf), 0);
    YUNIT_ASSERT(ret == strlen(test_value));
    YUNIT_ASSERT_STR_EQUAL(buf, test_value);
#endif
}
static void test_ble_gatt_write(void)
{
#if 0
    const struct bt_gatt_attr *attr;
    char *value = "    ";
    ssize_t ret;
    /*find attr by UUID*/
    attr = NULL;
    bt_gatt_foreach_attr_type(test_attrs[0].handle, 0xffff,
                  &test_chrc_uuid.uuid, NULL, 0, find_attr,
                  &attr);
    YUNIT_ASSERT_PTR_NOT_NULL(attr);
    ret = attr->write(NULL, attr, (void *)value, strlen(value), 0, 0);
    YUNIT_ASSERT(ret == strlen(test_value));
    YUNIT_ASSERT_STR_EQUAL(value, test_value);
#endif
}

extern int hci_driver_init(void);
static int init(void)
{
    int err = hci_driver_init();
    YUNIT_ASSERT(err == 0);
    if(err < 0){
        LOGE("ble", "Driver init failed");
        return -1;
    }
    return 0;
}

static int cleanup(void)
{
    LOGE("ble", "test ble finished");
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_ble_testcases[] = {
    { "ble init", test_ble_init },
    { "ble gap: adv", test_ble_adv },
    { "ble gap: scan", test_ble_scan },
    //{ "ble smp: ", test_ble_smp },
    { "ble gatt: register", test_ble_gatt_register},
    { "ble gatt: unregister", test_ble_gatt_unregister},
    //{ "ble gatt: register", test_ble_gatt_foreach},
    //{ "ble gatt: read", test_ble_gatt_read},
    //{ "ble gatt: write", test_ble_gatt_write},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "ble", init, cleanup, setup, teardown, aos_ble_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_ble(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_ble);
