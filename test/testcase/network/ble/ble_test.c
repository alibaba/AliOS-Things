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

bool bt_is_ready = false;
static void bt_ready(int err)
{
    if (err) {
        LOGE("ble", "Init failed (err %d)\n", err);
        return;
    }
    bt_is_ready = true;
}


static void test_ble_unit(void)
{
    int err;
    bt_addr_t addr = {.val = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x05 }};
    struct bt_le_adv_param adv_param = {
        .options = 0, \
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2, \
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2, \
        .own_addr = &addr, \
    };

    uint8_t data[] = { 0x6 };  // LE General Discoverable and BR/EDR not supported
    char *adv_name = "BLE-YTS-TEST";
    const struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
    };

    err = bt_enable(bt_ready);
    YUNIT_ASSERT(err == 0);
    if(err < 0){
        LOGE("ble", "BLE enable failed %d", err);
        return;
    }
    LOGI("ble","Stack initialized");
    /*for async event*/
    aos_msleep(2*1000);

    YUNIT_ASSERT(bt_is_ready == true);
    if(bt_is_ready == false){
        LOGE("ble", "BLE init failed");
    }
    LOGI("ble","adv succeed");
}

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
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{
    int err = bt_le_adv_stop();
    YUNIT_ASSERT(err == 0);
    if(err){
        LOGE("ble", "adv stop failed\n");
    }

}

static yunit_test_case_t aos_ble_testcases[] = {
    { "ble", test_ble_unit },
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
