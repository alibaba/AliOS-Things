/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/slist.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "hrs.h"
#include "bas.h"
#include "dis.h"

#define BD_ADDR_LEN 6
#if 1
typedef void (*cmd_cb_t)(char *buf, int len, void *priv);
extern void process_ble_help(int argc, char *argv[]);
extern void process_ble_status(int argc, char *argv[]);
extern void process_ble_init(int argc, char *argv[]);
extern void process_ble_gap(int argc, char *argv[]);
extern void process_ble_gatt(int argc, char *argv[]);
extern void process_ble_l2cap(int argc, char *argv[]);
extern void process_ble_sm(int argc, char *argv[]);

extern  void ble_command(char *pwbuf, int blen, int argc, char **argv);
struct cli_command blecmd[] = {
    {
        .name = "ble",
        .help = "ble [help | status | autotest | gap_test | gatt_test]",
        .function = ble_command
    }
};
#endif

static uint8_t m_addr[BD_ADDR_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
extern int hci_driver_init();
extern int bt_enable(bt_ready_cb_t cb);
static void get_bd_addr(uint8_t *addr)
{
    int err;
    bt_addr_le_t laddr;

    err = ais_ota_get_local_addr(&laddr);
    if (err != 0) {
        printf("Failed to get local addr, default will be used.\n");
        memcpy(addr, m_addr, BD_ADDR_LEN);
    } else {
        memcpy(addr, laddr.a.val, BD_ADDR_LEN);
        printf("Local addr got (%02x:%02x:%02x:%02x:%02x:%02x).\n",
               addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
}

void ble_bqb_test(void)
{
    int err;
    uint8_t bd_addr[BD_ADDR_LEN] = {0};

    hci_driver_init();
    err = bt_enable(NULL);
    if (err) {
        printf("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printf("Bluetooth initialized\n");
    get_bd_addr(bd_addr);
    hrs_init(0x01);
    bas_init();
    dis_init("AOS_BLE_BQB", "Manufacturer");

    printf("BLE BQB test cases..\n");
    aos_cli_register_commands(&blecmd[0], sizeof(blecmd) / sizeof(struct cli_command));
    do {
        aos_msleep(400);
        bas_notify();
    } while (1);
}

static void app_delayed_action(void *arg)
{
    ble_bqb_test();
}

int application_start(int argc, char **argv)
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
