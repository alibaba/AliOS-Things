/*
 * copyright (c) 2015-2016 intel corporation
 *
 * licensed under the apache license, version 2.0 (the "license");
 * you may not use this file except in compliance with the license.
 * you may obtain a copy of the license at
 *
 *     http://www.apache.org/licenses/license-2.0
 *
 * unless required by applicable law or agreed to in writing, software
 * distributed under the license is distributed on an "as is" basis,
 * without warranties or conditions of any kind, either express or implied.
 * see the license for the specific language governing permissions and
 * limitations under the license.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/byteorder.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "hrs.h"
#include "bas.h"
#include "dis.h"

#define DEVICE_NAME	"AOS-BLE-PERIPHERAL"
#define DEVICE_NAME_LEN	(sizeof(DEVICE_NAME) - 1)

extern int hci_driver_init();
struct bt_conn *default_conn;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0d, 0x18, 0x0f, 0x18, 0x05, 0x18),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		printf("Failed to connect to %s (%u)\n", addr, err);
		return;
	}

        default_conn = bt_conn_ref(conn);
	printf("Connected %s\n", addr);

	if (bt_conn_security(conn, BT_SECURITY_HIGH)) {
		printf("Failed to set security\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        if (default_conn) {
            bt_conn_unref(default_conn);
            default_conn = NULL;
        }

	printf("Disconnected from %s (reason %u)\n", addr, reason);
}

static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
                              const bt_addr_le_t *identity)
{
        char addr_identity[BT_ADDR_LE_STR_LEN];
        char addr_rpa[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
        bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

        printf("Identity resolved %s -> %s\n", addr_rpa, addr_identity);
}

static void security_changed(struct bt_conn *conn, bt_security_t level)
{
        char addr[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        printf("Security changed: %s level %u\n", addr, level);
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
        .identity_resolved = identity_resolved,
        .security_changed = security_changed,
};


static void bt_ready(int err)
{
        if (err) {
                printf("Bluetooth init failed (err %d)\n", err);
                return;
        }

        printf("Bluetooth initialized\n");

        hrs_init(0x01);
        bas_init();
        dis_init("AOS_BLE_MODEL", "Manufacturer");

        err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                              sd, ARRAY_SIZE(sd));
        if (err) {
                printf("Advertising failed to start (err %d)\n", err);
                return;
        }

        printf("Advertising successfully started\n");
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Passkey for %s: %u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};


extern int hci_driver_init();
void ble_sample(void)
{
	int err = 0;

        hci_driver_init();
	err = bt_enable(bt_ready);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_conn_auth_cb_register(&auth_cb_display);
	bt_conn_cb_register(&conn_callbacks);

        while (1) {
                aos_msleep(1000);

                /* Heartrate measurements simulation */
                hrs_notify();

                /* Battery level simulation */
                bas_notify();
        }

	printf("Advertising successfully started\n");
}

static void app_delayed_action(void *arg)
{
    ble_sample();
}

int application_start(int argc, char **argv)
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
