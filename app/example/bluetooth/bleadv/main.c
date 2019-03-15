/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"
#include <aos/kernel.h>

#include <misc/slist.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

extern int hci_driver_init();
extern int bt_enable(bt_ready_cb_t cb);
bool bt_is_ready = false;

static void bt_ready(int err)
{
    if (err) {
        printf("Bluetooth init failed (err %d)\n", err);
        return;
    }

    bt_is_ready = true;

}

void ble_sample(void)
{
    bt_addr_t addr = {.val = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x05 }};
    struct bt_le_adv_param adv_param = {
        .options = 0, \
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2, \
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2, \
        .own_addr = &addr, \
    };
    int err;

    uint8_t data[] = { 0x6 };  // LE General Discoverable and BR/EDR not supported
    char *adv_name = "AOS-BLE-HELLO";
    const struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
    };

    printf("Starting Advertiser Demo\n");

    hci_driver_init();
    err = bt_enable(bt_ready);
    if (err) {
        printf("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printf("Bluetooth initialized\n");

    do {
        aos_msleep(400);
        if (bt_is_ready == false) {
            continue;
        }

        /* Start advertising */
        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printf("Advertising failed to start (err %d)\n", err);
            return;
        }

        aos_msleep(400);

        err = bt_le_adv_stop();
        if (err) {
            printf("Advertising failed to stop (err %d)\n", err);
            return;
        }
    } while (1);
}

int application_start(int argc, char **argv)
{
    ble_sample();
    return 0;
}
