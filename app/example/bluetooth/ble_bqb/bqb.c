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
#include <misc/printk.h>

#include "bqb_gatt.h"
#include "bqb_gap.h"
#include "bqb_l2cap.h"
#include "bqb_sm.h"

#if 1
typedef void (*cmd_cb_t)(char *buf, int len, void *priv);
void process_ble_help(int argc, char *argv[]);
void process_ble_status(int argc, char *argv[]);
void process_ble_init(int argc, char *argv[]);
void process_ble_gap(int argc, char *argv[]);
void process_ble_gatt(int argc, char *argv[]);
void process_ble_l2cap(int argc, char *argv[]);
void process_ble_sm(int argc, char *argv[]);
#endif

enum {
    MAX_ARGS_NUM = 6,
    RESP_BUF_LEN = 256,
};

typedef struct command_s {
    const char *name;
    void (*function)(int argc, char *argv[]);
} cli_command_t;

const cli_command_t g_commands[] = {
    { "ble help", &process_ble_help },
    { "ble status", &process_ble_help },
    { "ble auto_test", &process_ble_help },
    { "ble init", &process_ble_init },
    { "ble gap [case]", &process_ble_gap },
    { "ble gatt [case]", &process_ble_gatt },
    { "ble l2cap [case]", &process_ble_l2cap },
    { "ble sm [case]", &process_ble_sm },
};

void ble_command(char *pwbuf, int blen, int argc, char **argv);
static struct cli_command blecmd[] = {
    {
        .name = "ble",
        .help = "ble [help | status | autotest | gap_test | gatt_test]",
        .function = ble_command
    }
};

void ble_command(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc >= 6 || argv == NULL) {
        printk("Invalid command\n");
    }

    const char *cmd_main = argc >= 1 ? argv[1] : "";

    if (strcmp(cmd_main, "help") == 0) {
        process_ble_help(argc, argv);
    }

    if (strcmp(cmd_main, "ble_status") == 0) {

    }

    if (strcmp(cmd_main, "ble_init") == 0) {

    }

    if (strcmp(cmd_main, "gap") == 0) {
        printk("gap test...\r\n");
        process_ble_gap(argc, argv);
    }

    if (strcmp(cmd_main, "gatt") == 0) {
        printk("gatt test...\r\n");
        process_ble_gatt(argc, argv);
    }

    if (strcmp(cmd_main, "l2cap") == 0) {
        printk("l2cap test ...\r\n");
        process_ble_l2cap(argc, argv);
    }

    if (strcmp(cmd_main, "sm") == 0) {
        printk("SM test ...\r\n");
        process_ble_sm(argc, argv);
    }

    return;
}

void process_ble_help(int argc, char *argv[])
{
    uint16_t index;

    for (index = 0; index < sizeof(g_commands) / sizeof(g_commands[0]); ++index) {
        printk("%s\r\n", g_commands[index].name);
    }
}

void process_ble_init(int argc, char *argv[])
{
    int err;

    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");
}

static void bt_obsv_scan_cb(const bt_addr_le_t *addr, s8_t rssi,
                            u8_t adv_type, struct net_buf_simple *buf)
{
#if 0
    if (adv_type != BT_LE_ADV_NONCONN_IND) {
        return;
    }
#endif
    printk("Scan hit:%02x %02x %02x %02x %02x %02x :%02x :%d \r\n", addr->a.val[0], \
           addr->a.val[1], addr->a.val[2], addr->a.val[3], addr->a.val[4], \
           addr->a.val[5], addr->type, adv_type);
}

void process_ble_gap(int argc, char *argv[])
{
    int err;
    if (argc > 6 | argv == NULL) {
        return;
    }
    bt_addr_t addr = {.val = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x05 }};
    struct bt_le_adv_param adv_param = {
        .options = 0,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .own_addr = &addr,
    };

    /*just for default settings*/
    /*ADV/BV-02-C, local name*/
    //BT_LE_AD_LIMITED | BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR
    uint8_t data[] = { (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR) };  // LE General Discoverable and BR/EDR not supported
    char *adv_name = "AOS-BLE-BQB";
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
    };

    const char *gap_sub = argv[2];

    /*BROB_BCST_BV_01_C
     *Verify the IUT in Broadcast Mode that does not implement scan response data; the peer device is Passive Scanning.
     *Settings: Contains the FLAGS AD Type with both the LE Limited Discoverable Flag and the LE General Discoverable Flag set to 0.
     *
     *
     * */
    if (strcmp(gap_sub, "BROB_BCST_BV_01_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("BROB_BCST_BV_01_C\n");
            return;
        }

        uint8_t data[0] = {BT_LE_AD_NO_BREDR};
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("BROB_BCST_BV_01_C...\r\n");

    }
    /*BROB_OBSV_BV_01_C
     *Verify the IUT performing the Observation Procedure using Passive Scanning.
     *
     *
     *The Upper Tester orders the IUT to perform the Observation procedure using Passive Scanning.
     * */
    if (strcmp(gap_sub, "BROB_OBSV_BV_01_C") == 0) {
#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10

        err = bt_le_adv_stop();
        if (err) {
            printk("adv stop error\n");
            return;
        }
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_PASSIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };

        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);
        printk("BROB_OBSV_BV_01_C...");

    }
    /*BROB_OBSV_BV_02_C
     *Verify the IUT performing the Observation Procedure using activee Scanning.
     *
     *TBD...if the IUT get the scan addr, send scan REQ PUD.
     *The Upper Tester orders the IUT to perform the Observation procedure using active Scanning.
     * */
    if (strcmp(gap_sub, "BROB_OBSV_BV_02_C") == 0) {
#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10

        err = bt_le_adv_stop();
        if (err) {
            printk("adv stop error\n");
            return;
        }
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);
        printk("BROB_OBSV_BV_02_C...");

    }

    /*"DISC_NON_BV_01_C"
     *Verify the IUT in Non-Discoverable Mode and Non-Connectable Mode is not discoverable by a device performing the General Discovery Procedure using Active Scanning.
     *
     *Setting:The IUT is operating in the Peripheral role.
     *The Upper Tester orders IUT to enter Non-Discoverable Mode and Non-Connectable Mode.
     *Contains the FLAGS AD Type with both the LE Limited Discoverable Flag and the LE General Discoverable Flag set to 0.
     *If the Flags AD Type is present in the advertising data then it only appears once per advertising event.
     *The Flags AD Type is not present in any scan response data received.
     *
     * */

    if (strcmp(gap_sub, "DISC_NON_BV_01_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }

        struct bt_le_adv_param adv_param = {
            .options = 0,//non-connectable
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {  BT_LE_AD_NO_BREDR };  // LE Limited Discovery =0; General Discoverable= 0; and BR/EDR not supported =1
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("BDISC_NON_BV_01_C...\r\n");
    }
    /*Verify the IUT in Non-Discoverable Mode and Undirected Connectable Mode is not discoverable by a device performing the General Discovery Procedure.
     *The Upper Tester orders IUT to enter Non-Discoverable Mode and Undirected Connectable Mode.
     *
     *
     *
     *The advertising data received by the Lower Tester does not contain the Flags AD type with the Limited Discoverable or General Discoverable flags set to 1.
     *If the Flags AD Type is present in the advertising data then it only appears once per advertising event.
     *The Flags AD Type is not present in any scan response data received.
     *
     *
     *
     * */
    if (strcmp(gap_sub, "DISC_NON_BV_02_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }
        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        //BT_LE_AD_LIMITED | BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR
        uint8_t data[] = { BT_LE_AD_NO_BREDR};  // LE Limited Discovery =0; General Discoverable= 0; and BR/EDR not supported =1
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("BDISC_NON_BV_02_C...\r\n");
    }

    /*
     *Verify an LE only IUT in General Discoverable Mode and the Undirected Connectable Mode can be discovered by a device performing the General Discovery Procedure.
     *
     *It must be possible to order IUT to enter General Discoverable Mode and Undirected Connectable Mode.
     *
     *The advertising data received by the Lower Tester contains the Flags AD type as described:
         *- Limited Discoverable flag set to 0
         *- General Discoverable flag set to 1
    If the Flags AD Type is present in the advertising data then it only appears once per advertising event.
    The Flags AD Type is not present in any scan response data received.
     *
     * */
    if (strcmp(gap_sub, "DISC_GENM_BV_04_C") == 0 ) {
        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {   BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x00),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x0F)
        };

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("DISC_GENM_BV_04_C..\r\n");

    }
    /*
     *Verify that IUT can perform the General Discovery Procedure and can find a device in the General Discoverable Mode.
     *
     *The Upper Tester orders IUT to start the General Discovery Procedure.???
     *
     *  scan during Tgap(gen_disc_scan_min), and check on the scan callback , find adv puds with General Discovery Mode and Limited Discovery mode
     * */
    if (strcmp(gap_sub, "DISC_GENP_BV_01_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);

        printk("DISC_GENP_BV_01_C..\r\n");

    }
    /*
     *
     *Verify the IUT can perform the General Discovery Procedure and can find devices in the Limited Discoverable Mode.
     *
     *The Upper Tester orders IUT to start the General Discovery Procedure.
     *
     * */
    if (strcmp(gap_sub, "DISC_GENP_BV_02_C") == 0 ) {

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);

        printk("DISC_GENP_BV_02_C..\r\n");

    }
    /*TBD
     *Verify the IUT can perform the General Discovery Procedure and does not find a device in the Broadcast Mode.
     *
     *
     *The Upper Tester orders IUT to perform the General Discovery Procedure.
     *
     * */
    if (strcmp(gap_sub, "DISC_GENP_BV_03_C") == 0 ) {

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);
        printk("DISC_GENP_BV_03_C..\r\n");

    }
    /*
     *
     *Verify the IUT can perform the General Discovery Procedure and does not find a device in the Undirected Connectable Mode.
     *
     *The Lower Tester enters Undirected Connectable Mode; the Lower Tester does not include the Flags AD Type in the advertising data with either the General Discoverable Flag or Limited Discoverable             Flag set to 1. The Upper Tester orders IUT to perform the General Discovery Procedure.
     *
     * */
    if (strcmp(gap_sub, "DISC_GENP_BV_04_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);
        printk("DISC_GENP_BV_04_C..\r\n");

    }
    /*
     *
     *Verify the IUT can perform the General Discovery Procedure and does not find a device in the Directed Connectable Mode.
     *
     *The Upper Tester orders IUT to perform the General Discovery Procedure.
     *
     * */
    if (strcmp(gap_sub, "DISC_GENP_BV_05_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

#define SCAN_INTERVAL   0x10
#define SCAN_WINDOWS    0x10
        struct bt_le_scan_param scan_param = {
            .type = BT_HCI_LE_SCAN_ACTIVE,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
            .interval   = SCAN_INTERVAL,
            .window     = SCAN_WINDOWS
        };
        bt_le_scan_start(&scan_param, bt_obsv_scan_cb);
        printk("DISC_GENP_BV_05_C..\r\n");

    }
    /*
     * TBD
     *Verify the IUT can perform the Name Discovery Procedure and retrieve the device name from a peer device.
     * The IUT is operating as the GATT client.
     *
     *The Upper Tester orders the IUT to perform the Name Discovery Procedure.
     *If the complete device name is not acquired while performing either
     * the limited discovery procedure or the general discovery procedure, then the name discovery procedure may be performed.
     *The name discovery procedure shall be performed as follows:
     *1. The Host shall establish a connection using one of the connection establish
     *ment procedures as defined in Section 9.3.
     2. The Host shall read the device name characteristic using the GATT
         *procedure Read Using Characteristic UUID [Vol 3] Part G, Section 4.8.2
     3. The connection may be terminated after the GATT procedure has completed.
     *
     * */
    if (strcmp(gap_sub, "IDLE_NAMP_BV_01_C") == 0 ) {
        printk("IDLE_NAMP_BV_01_C...\r\n");
    }
    /*TBD
     *
     *Verify the IUT can support the Name Discovery Procedure and allow a peer device to retrieve the device name.
     *
     *The IUT is in Link Layer state ‘Standby’.
     *The Lower Tester and IUT are connected.
         *The IUT is a GATT server and exposes the Device Name characteristic.
         *The Device Name Characteristic value for the IUT is specified in the IXIT [5]
     *
     * *The Upper Tester orders the IUT to perform the Name Discovery Procedure.
         *If the complete device name is not acquired while performing either
         * the limited discovery procedure or the general discovery procedure, then the name discovery procedure may be performed.
         *The name discovery procedure shall be performed as follows:
         *1. The Host shall establish a connection using one of the connection establish
         *ment procedures as defined in Section 9.3.
         2. The Host shall read the device name characteristic using the GATT
         *procedure Read Using Characteristic UUID [Vol 3] Part G, Section 4.8.2
         3. The connection may be terminated after the GATT procedure has completed.
     *
     * */
    if (strcmp(gap_sub, "IDLE_NAMP_BV_02_C") == 0 ) {
        printk("IDLE_NAMP_BV_02_C...\r\n");
    }

    /*CONN_NCON_BV_01_C
     *
     *Verify the IUT in the Non-Connectable Mode does not allow another device performing the Directed Connection Establishment Procedure to connect.
     *The IUT is operating in the Broadcaster role or the Peripheral role or the Observer role.
     *
     *The Upper Tester orders IUT to enter Non-Connectable Mode.
     *
     *
     * */
    if (strcmp(gap_sub, "CONN_NCON_BV_01_C") == 0 ) {
        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_NONE),
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {   BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x00),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x0F)
        };

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }

        printk("CONN_NCON_BV_01_C...\r\n");


    }

    /*CONN_UCON_BV_01_C
     *
     *Verify the IUT in Undirected Connectable Mode can connect with another device performing the
     *General Connection Establishment Procedure. The IUT is operating in the Peripheral role.
     *
     *
     *The Upper Tester orders IUT to enter Undirected Connectable Mode and Non-Discoverable Mode;
     *the IUT sets the advertiser address to the public/static address of the IUT.
     *
     * */
    if (strcmp(gap_sub, "CONN_UCON_BV_01_C") == 0 ) {
        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {   BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x00),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x0F)
        };

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("DCONN_UCON_BV_01_C..\r\n");

    }
    /*CONN_UCON_BV_02
     *
     *Verify the IUT in Undirected Connectable Mode and General Discoverable Mode
     *can connect with another device performing the General Connection Establishment Procedure.

     *
     *
     *TBD add GATT support for connection....
     *
     *
     * */
    if (strcmp(gap_sub, "CONN_UCON_BV_02_C") == 0 ) {

        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME),
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {   BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x00),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x0F)
        };

        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("DCONN_UCON_BV_02_C..\r\n");

    }

    /*TBD... CONN_DCEP_BV_01_C
     *
     *
     * The Upper Tester orders the IUT to perform the Directed Connection Establishment Procedure using the static address, public address or non-resolvable private address of the Lower Tester.
     *Verify the IUT can perform the Direct Connection Establishment Procedure to connect to another device in the Directed Connectable Mode.
     *The IUT is operating in the Central role.
     *
     * The IUT is in Link Layer state ‘Standby’. The IUT has the address of the peer device.
     *
     * */
    if (strcmp(gap_sub, "CONN_DCEP_BV_01_C") == 0 ) {
        printk("CONN_DCEP_BV_01_C..\r\n");

    }
    /*TBD...
     *
     *Verify the IUT can perform the Direct Connection Establishment Procedure to connect to another device in the Undirected Connectable Mode.
     *The IUT is operating in the Central role.
     *
     *
     *
     * */
    if (strcmp(gap_sub, "CONN_DCEP_BV_03_C") == 0 ) {

        printk("CONN_DCEP_BV_03_C..\r\n");

    }
    /*TBD...CONN_CPUP_BV_04_C
     *Verify the IUT accepts the connection parameter update request from a peer device performing the
     *Connection Parameter Update Procedure using valid parameters for the IUT.
     *
     *
     * The Lower Tester performs the Connection Parameter Update Procedure using the specified valid connection update parameters.
     *
     * */

    if (strcmp(gap_sub, "CONN_DCEP_BV_04_C") == 0 ) {
        /*bit 0--LE Limited Discoverable Mode*/
        /*bit 1--LE General Discoverable Mode*/
        /*bit 2--BR/EDR Not Supported*/
        uint8_t data[] = { 0x5 };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
        };
        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }

    }
    /*TBD...CONN_DCEP_BV_05
     *
     *Verify the IUT rejects the connection parameter update request from a peer device performing the
     *Connection Parameter Update Procedure using invalid connection parameters for the IUT.
     *The Lower Tester is operating in the Peripheral role and is the initiator performing
     *the Connection Parameter Update Procedure and the IUT is operating in the Central role and is the responder.
     *
     *
     * */
    if (strcmp(gap_sub, "CONN_DCEP_BV_05_C") == 0 ) {
        /*bit 0--LE Limited Discoverable Mode*/
        /*bit 1--LE General Discoverable Mode*/
        /*bit 2--BR/EDR Not Supported*/
        uint8_t data[] = { 0x5 };  // LE General Discoverable and BR/EDR not supported
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
        };
        err = bt_le_adv_stop();
        if (err) {
            printk("TS adv stoped\n");
            return;
        }

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
        if (err) {
            printk("TS adv failed\n");
            return;
        }

    }
    /*TBD CONN_CPUP_BV_06_C
     *
     *Verify the IUT can perform the Connection Parameter Update Procedure using valid parameters for the peer device;
     *the peer device accepts the updated connection parameters.
     *The IUT is operating in the Central role and is the initiator performing the Connection Parameter Update Procedure
     *and the Lower Tester is operating in the Peripheral role and is the responder.
     *
     * */
    if (strcmp(gap_sub, "CONN_CPUP_BV_06_C") == 0 ) {
        printk("CONN_CPUP_BV_06_C...\n");
    }
    /*TBD CONN_TERM_BV_01_C
     *
     *Verify the IUT can perform the Terminate Connection Procedure.
     *The IUT is Central or Peripheral and the Lower Tester is Peripheral or Central respectively.
     *
     *
     * */
    if (strcmp(gap_sub, "CONN_CPUP_BV_06_C") == 0 ) {
        printk("CONN_CPUP_BV_06_C...\r\n");

    }

    /*TBD...BOND_NBON_BV_03_C
     *
     *Verify that the IUT does not exchange bonding information after pairing while in non-bondable mode.
     *Lower Tester is the initiator. The Lower Tester shall require authentication to invoke the pairing procedure.
     *IUT supports security manager pairing, but is in non-bondable mode. The pairing is performed as unauthenticated pairing (Just Works).
     *
     * */
    if (strcmp(gap_sub, "BOND_NBON_BV_03_C") == 0 ) {
        printk("BOND_NBON_BV_0#_C...\r\n");

    }
    /* ADV_BV_01_C
     *
     *Verify the IUT sends valid Service UUID AD Type in advertising and scan response data.
     *The IUT is Broadcaster or Peripheral. The Lower Tester is Observer or Central.
     *
     * */

    if (strcmp(gap_sub, "ADV_BV_01_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }

        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE),//non-connectable
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x00),
            BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x18, 0x01)
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }

        printk("ADV_BV_02_C..\r\n");
    }

    /*ADV_BV_02_C
     *
     *Verify the IUT sends valid Local Name AD Type in advertising and scan response data.
     *The IUT is Broadcaster or Peripheral. The Lower Tester is Observer or Central.     *
     *
     *
     *
     * */

    if (strcmp(gap_sub, "ADV_BV_02_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }

        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE),//non-connectable
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }

        printk("ADV_BV_02_C..\r\n");
    }

    /* ADV_BV_03_C
     *
     *Verify the behavior of the IUT with respect to the Flags AD Type in advertising data.
     *The IUT is Broadcaster or Peripheral. The Lower Tester is Observer or Central.
     *
     *
     *
     * */
    if (strcmp(gap_sub, "ADV_BV_03_C") == 0 ) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }

        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE),//non-connectable
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }

        printk("ADV_BV_03_C..\r\n");
    }

    /*
     *ADV_BV_04_C
     *
     *Verify the IUT sends valid Manufacturer Specific Data AD Type in advertising and scan response data.
     *
     * */
    if (strcmp(gap_sub, "ADV_BV_04_C") == 0) {
        err = bt_le_adv_stop();
        if (err) {
            printk("Stop adv failed\n");
        }

        struct bt_le_adv_param adv_param = {
            .options = (BT_LE_ADV_OPT_CONNECTABLE),//non-connectable
            .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
            .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        };

        uint8_t data[] = {  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };
        struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        struct bt_data scan_rsp[] = {
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
        };

        err = bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_rsp, ARRAY_SIZE(scan_rsp));
        if (err) {
            printk("TS adv failed\n");
            return;
        }
        printk("ADV_BV_04_C..\r\n");
    }
    return;

}

void process_ble_gatt(int argc, char *argv[])
{
    int err;
    if (argc > 6 || argv == NULL) {
        return;
    }

    bt_addr_t addr = {.val = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x05 }};
    struct bt_le_adv_param adv_param = {
        .options = 0,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .own_addr = &addr,
    };

    /*just for default settings*/
    //BT_LE_AD_LIMITED | BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR
    uint8_t data[] = { (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR) };  // LE General Discoverable and BR/EDR not supported
    char *adv_name = "AOS-BLE-BQB";
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name))
    };

    const char *gatt_sub = argv[2];

    if (strcmp(gatt_sub, "CL/GAC/BV-01-C") == 0) {
        process_cl_gac_bv_01_c(argc, argv);
    }

    if (strcmp(gatt_sub, "SR/GAC/BV-01-C") == 0) {
        process_sr_gac_bv_01_c(argc, argv);
    }
}

void process_ble_sm(int argc, char *argv[])
{

}

void process_ble_l2cap(int argc, char *argv[])
{

}

void process_ble_status(int argc, char *argv[])
{

}
