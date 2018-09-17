/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/gatt.h>
#include <bluetooth/conn.h>
#include <misc/printk.h>

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static struct bt_conn *g_conn;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0a, 0x18),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, u8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
    } else {
        g_conn = conn;
        printk("Connection is established\n");
    }
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    printk("Disconnected (reason %u)\n", reason);
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

static void get_mtu_response(struct bt_conn *conn, u8_t err,
                             struct bt_gatt_exchange_params *params)
{
    u16_t mtu;

    if (err) {
        printk("mtu response failed (err %u)\n", err);
    } else {
        mtu = bt_gatt_get_mtu(conn);
        printk("get mtu response: %d\n", mtu);
    }
}

/* preamble procedure:
 * 1. Establish an LE transport connection between the IUT and the Lower Tester.
 * 2. Establish an L2CAP channel 0x0004 between the IUT and the Lower Tester over that LE transport.
 */
void preamble_procedure(void)
{
    int err;

    bt_conn_cb_register(&conn_callbacks);

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Advertising successfully started\n");
}

/* gatt/cl/gac/bv-01-c: server configuration - by client
 * verify that a generic attribute profile client can generate an exchange mtu request command to configure att_mtu over le.
 */
void process_cl_gac_bv_01_c(int argc, char *argv[])
{
    int err;
    struct bt_att_req req;
    struct bt_gatt_exchange_params param;

    param._req = req;
    param.func = get_mtu_response;

    printk("cl/gac/bv-01-c: server configuration - by client\n");

    preamble_procedure();

    err = bt_gatt_exchange_mtu(g_conn, &param);
    if (err) {
        printk("Exchange mtu failed (err %d)\n", err);
        return;
    }
}

void process_sr_gac_bv_01_c(int argc, char *argv[])
{

}
