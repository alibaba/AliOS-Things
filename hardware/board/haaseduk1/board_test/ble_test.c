/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "board.h"
#include "aos/cli.h"
#include <aos/ble.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <atomic.h>
#include <work.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
static int bt_scan_start();
#define EXAMPLE_BLE_DEV_NAME        "HaaS BLE"
#define DEVICE_ADDR {0xE8, 0x3B, 0xE3, 0x88, 0xB4, 0xC8}
static int bt_devices_find = 0;
static void handle_ble_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int id;
    char *onoff;
    printf("enable ble_cmd\n");
    bt_scan_start();
}

static int ble_scanf_result(uint32_t timeout)
{
#if 1
    while (!bt_devices_find || timeout--) {
        aos_msleep(500);
    }
#endif
    ble_stack_scan_stop();
    if (bt_devices_find != 0) {
        printf("====Result: BT test PASS !!!===\n");
        return 0;
    } else {
        printf("\r\n=====BT test : FAIL===\r\n");
        return -1;
    }
}

static void device_find(ble_event_en event, void *event_data)
{
    int ret = -1;
    int uuid_peer = -1;
    evt_data_gap_dev_find_t ee;
    evt_data_gap_dev_find_t *e;
    uint8_t find_dev = 0;
    memcpy(&ee, event_data, sizeof(evt_data_gap_dev_find_t));

    e = &ee;

    if (e->adv_type != ADV_IND) {
        return;
    }
    if (e->adv_len > 31) {
        return;
    }
    printf("find BT device %x-%x-%x-%x-%x-%x RssI:%d\n", e->dev_addr.val[0], e->dev_addr.val[1], e->dev_addr.val[2], e->dev_addr.val[3], e->dev_addr.val[4], e->dev_addr.val[5], e->rssi);
    bt_devices_find++;
}

static int event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_DEV_FIND:
            device_find(event, event_data);
            break;
        default:
            // printf("Unhandle event %d\n", event);
            break;
    }

    return 0;
}

static int bt_scan_start()
{
    int ret;
    scan_param_t param = {
        SCAN_PASSIVE,
        SCAN_FILTER_DUP_ENABLE,
        SCAN_FAST_INTERVAL,
        SCAN_FAST_WINDOW,
    };
    bt_devices_find = 0;
    ret = ble_stack_scan_start(&param);

    if (ret) {
        return ret;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

int example_BLE_init(void)
{
    int ret;
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = EXAMPLE_BLE_DEV_NAME,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };
    /* we need first init hci driver */
    hci_h4_driver_init();

    /* bt stack init */
    ret = ble_stack_init(&init);
    if (ret) {
        return -1;
    }

    ret = ble_stack_event_register(&ble_cb);
    if (ret) {
        return -1;
    }

    return 0;
}

int ble_test_process()
{
    int ret = 0;
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************* BT Connect Test *******************\r\n");
    printf("*How to test: find any BT devices as successed. *****\r\n");
    printf("***************************************************************\r\n");
    printf("=====BT Connect test : Start=====\r\n");
    ret = example_BLE_init();
    if (ret) {
        return ret;
    }
    bt_scan_start();

    return ble_scanf_result(5);
}

static struct cli_command ble_cmd = {
    .name     = "ble",
    .help     = "ble scan",
    .function = handle_ble_cmd
};

int ble_test(void)
{
    aos_cli_register_command(&ble_cmd);
    return ble_test_process();
}
