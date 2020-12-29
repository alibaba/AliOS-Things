/** @file
 * @brief Bluetooth GATT shell functions
 *
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <ble_types/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <misc/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include "bt.h"
#include "gatt.h"
#include <aos/ble.h>
#include <aos/gatt.h>
#include <aos/kernel.h>
#include "app_test.h"

#include <work.h>

extern int16_t fail_count;
#define CHAR_SIZE_MAX           512
extern int16_t g_bt_conn_handle;
extern int16_t g_security_level;
#define VALUE_LEN 100
extern char *uuid_str(uuid_t *uuid);
extern void str2hex(uint8_t hex[], char *s, uint8_t cnt);
extern void hexdump(const u8_t *data, size_t len);
struct test_svc_t {
    int16_t conn_handle;
    uint16_t svc_handle;
    uint16_t svc2_handle;
    ccc_value_en ccc;
    uint16_t mtu;
    char value1[VALUE_LEN];
    char value2[VALUE_LEN];
    char value3[8];
    char value3_cud[30];
    char value4[VALUE_LEN];
} test_svc = {
    .conn_handle = -1,
    .svc_handle = 0,
    .svc2_handle = 0,
    .mtu = VALUE_LEN,
    .value1 = "test value 1",
    .value2 = "test value 2",
    .value3 = "test",
    .value3_cud = "this is value3 cud",
    .value4 = "test value 4",
};

#if defined(CONFIG_BT_GATT_CLIENT)

static int event_gatt_mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = event_data;
    printf("Exchange %s\n", e->err == 0 ? "successful" : "failed");
    test_svc.mtu = ble_stack_gatt_mtu_get(e->conn_handle);
    return 0;
}

int cmd_gatt_exchange_mtu(int argc, char *argv[])
{
    int err;
    int getmtu;

    /************API TEST****************/
    if (!strcmp(argv[1], "test")) {
        int ret = 99;
        printf("API exception parameter test start!\n");
        //Exchange mtu
        ret = ble_stack_gatt_mtu_exchange(-1);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_mtu_exchange");

        //Get mtu
        ret = ble_stack_gatt_mtu_get(-1);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_mtu_get1");
        printf("API exception parameter test end!\n");
        return 0;
    }

    /************shell****************/
    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    err = ble_stack_gatt_mtu_exchange(g_bt_conn_handle);

    if (err) {
        printf("Exchange failed (err %d)\n", err);
        return 0;
    } else {
        printf("Exchange pending\n");
    }

    //add by zhangyj to test mtu_get,when define CONFIG_BT_SMP,mtu defult 65,otherwise 23
    aos_msleep(2000);
    getmtu = ble_stack_gatt_mtu_get(g_bt_conn_handle);

    if (getmtu <= 0 && getmtu >= 244) {
        printf("Get mtu failed (err %d)\n", getmtu);
        return 0;
    } else {
        printf("Get mtu success(mtu %d)!\n", getmtu);
    }

    return 0;
}

static void print_chrc_props(u8_t properties)
{
    printf("Properties: ");

    if (properties & GATT_CHRC_PROP_BROADCAST) {
        printf("[bcast]");
    }

    if (properties & GATT_CHRC_PROP_READ) {
        printf("[read]");
    }

    if (properties & GATT_CHRC_PROP_WRITE) {
        printf("[write]");
    }

    if (properties & GATT_CHRC_PROP_WRITE_WITHOUT_RESP) {
        printf("[write w/w rsp]");
    }

    if (properties & GATT_CHRC_PROP_NOTIFY) {
        printf("[notify]");
    }

    if (properties & GATT_CHRC_PROP_INDICATE) {
        printf("[indicate]");
    }

    if (properties & GATT_CHRC_PROP_AUTH) {
        printf("[auth]");
    }

    if (properties & GATT_CHRC_PROP_EXT_PROP) {
        printf("[ext prop]");
    }

    printf("\n");
}

static int event_gatt_discovery(ble_event_en event, void *event_data)
{
    union {
        evt_data_gatt_discovery_svc_t svc;
        evt_data_gatt_discovery_inc_svc_t svc_inc;
        evt_data_gatt_discovery_char_t char_c;
        evt_data_gatt_discovery_char_des_t char_des;
    } *e;

    e = event_data;

    switch (event) {
        case EVENT_GATT_DISCOVERY_SVC:
            printf("Service %s found: start handle %x, end_handle %x\n",
                   uuid_str(&e->svc.uuid), e->svc.start_handle, e->svc.end_handle);
            break;

        case EVENT_GATT_DISCOVERY_CHAR:
            printf("Characteristic %s found: handle %x\n", (char *)(&e->char_c.uuid),
                   e->char_c.attr_handle);
            print_chrc_props(e->char_c.props);
            break;

        case EVENT_GATT_DISCOVERY_INC_SVC:
            printf("Include %s found: handle %x, start %x, end %x\n",
                   uuid_str(&e->svc_inc.uuid), e->svc_inc.attr_handle, e->svc_inc.start_handle,
                   e->svc_inc.end_handle);
            break;

        default:
            printf("Descriptor %s found: handle %x\n", uuid_str(&e->char_des.uuid), e->char_des.attr_handle);
            break;
    }

    return 0;
}

int cmd_gatt_discover(int argc, char *argv[])
{
    int err;
    gatt_discovery_type_en type;
    struct ut_uuid_128 uuid = {0};
    uint16_t start_handle = 0x0001;
    uint16_t end_handle = 0xffff;

    /************API TEST****************/
    //API test must be execute after connected
    if (!strcmp(argv[1], "test")) {
        fail_count = 0;
        int ret = 99;
        printf("API exception parameter test start!\n");
        ret = ble_stack_gatt_discovery(-1, 0, 2800, 1, 10);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_discovery1");

        ret = ble_stack_gatt_discovery(g_bt_conn_handle, -1, 2800, 1, 10);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_discovery2");

        ret = ble_stack_gatt_discovery(g_bt_conn_handle, 4, 2800, 1, 10);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_discovery3");

        //assert
        ret = ble_stack_gatt_discovery(g_bt_conn_handle, GATT_FIND_PRIMARY_SERVICE, 2800, 1, 10);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_discovery4");

        ret = ble_stack_gatt_discovery(g_bt_conn_handle, GATT_FIND_PRIMARY_SERVICE, 2800, 1, 10);
        TEST_ASSERT_EQUAL(-2, ret, 0, "ble_stack_gatt_discovery5");

        printf("API exception parameter test end!\n");

        if (!fail_count) {
            printf("BLE_GATT_DIS PASS\n");
        } else {
            printf("BLE_GATT_DIS FAILED\n");
        }

        return 0;
    }

    /************shell****************/
    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 2) {
        if (!strcmp(argv[0], "gatt-discover-primary") ||
            !strcmp(argv[0], "gatt-discover-secondary")) {
            return -EINVAL;
        }

        goto done;
    }

    /* Only set the UUID if the value is valid (non zero) */
    if (strlen(argv[1]) == 4) {
        struct ut_uuid_16 uuid1 = {0};
        uuid1.uuid.type = UUID_TYPE_16;
        uuid1.val = strtoul(argv[1], NULL, 16);
        uuid = *(struct ut_uuid_128 *)(&uuid1);
    } else if (strlen(argv[1]) == 32) {
        uuid.uuid.type = UUID_TYPE_128;
        str2hex(uuid.val, argv[1], 16);
    }

    if (argc > 2) {
        start_handle = strtoul(argv[2], NULL, 16);

        if (argc > 3) {
            end_handle = strtoul(argv[3], NULL, 16);
        }
    }

done:

    if (!strcmp(argv[0], "gatt-discover-secondary")) {
        type = BT_GATT_DISCOVER_SECONDARY;
    } else if (!strcmp(argv[0], "gatt-discover-include")) {
        type = GATT_FIND_INC_SERVICE;
    } else if (!strcmp(argv[0], "gatt-discover-characteristic")) {
        type = GATT_FIND_CHAR;
    } else if (!strcmp(argv[0], "gatt-discover-descriptor")) {
        type = GATT_FIND_CHAR_DESCRIPTOR;
    } else {
        type = GATT_FIND_PRIMARY_SERVICE;
    }

    err = ble_stack_gatt_discovery(g_bt_conn_handle, type, (uuid.uuid.type == 0 && uuid.val == 0) ? NULL : (uuid_t *)&uuid, start_handle, end_handle);

    if (err) {
        printf("Discover failed (err %d)\n", err);
    } else {
        printf("Discover pending\n");
    }

    return 0;
}

static int event_gatt_read_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_read_cb_t *e = event_data;
    printf("Read complete: err %u length %u\n", e->err, e->len);
    hexdump(e->data, e->len);

    return 0;
}

int cmd_gatt_read(int argc, char *argv[])
{
    int err;
    uint16_t handle;
    uint16_t offset = 0;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 2) {
        return -EINVAL;
    }

    if (argc == 4) {
        printf("***** test ble_stack_gatt_read start *****\n");
        handle = strtoul(argv[1], NULL, 16);
        offset = strtoul(argv[2], NULL, 16);

        if (!strcmp(argv[3], "test")) {
            err = ble_stack_gatt_read(-1, handle, offset);
            TEST_ASSERT_EQUAL(-2, err, 1, "test_ble_stack_gatt_read fail");
        }

        printf("***** test ble_stack_gatt_read end *****\n");
        return 0;
    }

    handle = strtoul(argv[1], NULL, 16);

    if (argc > 2) {
        offset = strtoul(argv[2], NULL, 16);
    }

    err = ble_stack_gatt_read(g_bt_conn_handle, handle, offset);

    if (err) {
        printf("Read failed (err %d)\n", err);
    } else {
        printf("Read pending\n");
    }

    return 0;
}

int cmd_gatt_mread(int argc, char *argv[])
{
    int i, err;
    uint16_t h[8];

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 3) {
        return -EINVAL;
    }

    if (argc == 4) {
        if (!strcmp(argv[3], "test")) {
            printf("***** test ble_stack_gatt_read_multiple start *****");

            for (i = 0; i < argc - 1; i++) {
                h[i] = strtoul(argv[i + 1], NULL, 16);
            }

            err = ble_stack_gatt_read_multiple(-1, argc - 1, h);
            TEST_ASSERT_EQUAL(-2, err, 1, "test_ble_stack_gatt_read_multiple fail");
            printf("***** test ble_stack_gatt_read_multiple start *****");
            return 0;
        }
    }

    if (argc - 1 >  BLE_ARRAY_NUM(h)) {
        printf("Enter max %lu handle items to read\n", ARRAY_SIZE(h));
        return 0;
    }

    for (i = 0; i < argc - 1; i++) {
        h[i] = strtoul(argv[i + 1], NULL, 16);
    }

    err = ble_stack_gatt_read_multiple(g_bt_conn_handle, argc - 1, h);

    if (err) {
        printf("GATT multiple read request failed (err %d)\n", err);
    }

    return 0;
}

static u8_t gatt_write_buf[CHAR_SIZE_MAX];

static int event_gatt_write_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_write_cb_t *e = event_data;
    printf("Write complete: err %u\n", e->err);

    return 0;
}

int cmd_gatt_write(int argc, char *argv[])
{
    int err;
    uint16_t handle, offset, len = 1;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 4) {
        return -EINVAL;
    }

    handle = strtoul(argv[1], NULL, 16);
    offset = strtoul(argv[2], NULL, 16);

    gatt_write_buf[0] = strtoul(argv[3], NULL, 16);

    if (argc == 6) {
        int i;
        len = BLE_MIN(strtoul(argv[4], NULL, 16), sizeof(gatt_write_buf));

        for (i = 1; i < len; i++) {
            gatt_write_buf[i] = gatt_write_buf[0];
        }

        if (!strcmp(argv[5], "test")) {
            fail_count = 0;
            printf("****** test ble_stack_gatt_write start *****\n");
            err = ble_stack_gatt_write_response(-1, handle, gatt_write_buf, len, offset);
            TEST_ASSERT_EQUAL(-5, err, 1, "test_ble_stack_gatt_write fail");

            err = ble_stack_gatt_write_response(g_bt_conn_handle, handle, NULL, len, offset);
            TEST_ASSERT_EQUAL(-2, err, 1, "test_ble_stack_gatt_write fail");

            err = ble_stack_gatt_write_response(g_bt_conn_handle, handle, gatt_write_buf, 0, offset);
            TEST_ASSERT_EQUAL(-2, err, 1, "test_ble_stack_gatt_write fail");

            err = ble_stack_gatt_write(g_bt_conn_handle, handle, gatt_write_buf, len, offset, 3);
            TEST_ASSERT_EQUAL(-3, err, 1, "test_ble_stack_gatt_write fail");
            printf("****** test ble_stack_gatt_write end *****\n");

            if (!fail_count) {
                printf("BLE_GATT_WRITE PASS\n");
            } else {
                printf("BLE_GATT_WRITE FAILED\n");
            }
        }

        return 0;
    }

    if (argc == 5) {
        int i;

        len = BLE_MIN(strtoul(argv[4], NULL, 16), sizeof(gatt_write_buf));

        for (i = 1; i < len; i++) {
            gatt_write_buf[i] = gatt_write_buf[0];
        }
    }

    err = ble_stack_gatt_write_response(g_bt_conn_handle, handle, gatt_write_buf, len, offset);

    if (err) {
        printf("Write failed (err %d)\n", err);
    } else {
        printf("Write pending\n");
    }

    return 0;
}

int cmd_gatt_write_without_rsp(int argc, char *argv[])
{
    uint16_t handle;
    uint16_t repeat;
    int err = 0;
    uint16_t len;
    bool sign;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 3) {
        return -EINVAL;
    }

    sign = !strcmp(argv[0], "gatt-write-signed");
    handle = strtoul(argv[1], NULL, 16);
    gatt_write_buf[0] = strtoul(argv[2], NULL, 16);
    len = 1;

    if (argc > 3) {
        int i;

        len = BLE_MIN(strtoul(argv[3], NULL, 16), sizeof(gatt_write_buf));

        for (i = 1; i < len; i++) {
            gatt_write_buf[i] = gatt_write_buf[0];
        }
    }

    repeat = 0;

    if (argc > 4) {
        repeat = strtoul(argv[4], NULL, 16);
    }

    if (!repeat) {
        repeat = 1;
    }

    while (repeat--) {
        if (sign) {
            err = ble_stack_gatt_write_signed(g_bt_conn_handle, handle,
                                              gatt_write_buf, len, 0);
        } else {
            err = ble_stack_gatt_write_no_response(g_bt_conn_handle, handle,
                                                   gatt_write_buf, len, 0);
        }

        if (err) {
            break;
        }
    }

    printf("Write Complete (err %d)\n", err);

    return 0;
}

static int event_gatt_notify(ble_event_en event, void *event_data)
{
    evt_data_gatt_notify_t *e = event_data;

    printf("Notification: char handle %d length %u\n", e->char_handle, e->len);
    hexdump(e->data, e->len);

    return 0;
}

int cmd_gatt_subscribe(int argc, char *argv[])
{
    int err;
    uint16_t ccc_handle;
    uint16_t value = 0;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 3) {
        return -EINVAL;
    }

    ccc_handle = strtoul(argv[1], NULL, 16);
    value = CCC_VALUE_NOTIFY;

    if (argc > 3 && !strcmp(argv[2], "ind")) {
        value = CCC_VALUE_INDICATE;
    }

    err = ble_stack_gatt_write_response(g_bt_conn_handle, ccc_handle, &value, sizeof(value), 0);

    if (err) {
        printf("Subscribe failed (err %d)\n", err);
    } else {
        printf("Subscribed\n");
    }

    return 0;
}

int cmd_gatt_unsubscribe(int argc, char *argv[])
{
    int err;
    uint16_t ccc_handle;
    uint16_t value = CCC_VALUE_NONE;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 2) {
        return -EINVAL;
    }

    ccc_handle = strtoul(argv[1], NULL, 16);

    err = ble_stack_gatt_write_response(g_bt_conn_handle, ccc_handle, &value, sizeof(value), 0);

    if (err) {
        printf("Unsubscribe failed (err %d)\n", err);
    } else {
        printf("Unsubscribe success\n");
    }

    return 0;
}
#endif /* CONFIG_BT_GATT_CLIENT */

static u8_t print_attr(const struct bt_gatt_attr *attr, void *user_data)
{
    printf("attr %p handle 0x%04x uuid %s perm 0x%02x\n",
           attr, attr->handle, bt_uuid_str(attr->uuid), attr->perm);

    return BT_GATT_ITER_CONTINUE;
}

int cmd_gatt_show_db(int argc, char *argv[])
{
    bt_gatt_foreach_attr(0x0001, 0xffff, print_attr, NULL);

    return 0;
}

#define TEST_SERVICE_UUID UUID128_DECLARE(0xF0,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR1_UUID UUID128_DECLARE(0xF1,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR2_UUID UUID128_DECLARE(0xF2,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR3_UUID UUID128_DECLARE(0xF3,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR4_UUID UUID128_DECLARE(0xF4,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR5_UUID UUID128_DECLARE(0xF5,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR6_UUID UUID128_DECLARE(0xF6,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR7_UUID UUID128_DECLARE(0xF7,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)

#define TEST2_SERVICE_UUID UUID128_DECLARE(0xF0,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR1_UUID UUID128_DECLARE(0xF1,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR2_UUID UUID128_DECLARE(0xF2,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR3_UUID UUID128_DECLARE(0xF3,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR4_UUID UUID128_DECLARE(0xF4,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR5_UUID UUID128_DECLARE(0xF5,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST2_CHAR6_UUID UUID128_DECLARE(0xF6,0x30,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)


static struct bt_gatt_ccc_cfg_t test3_ccc_cfg[2] = {};


enum {
    TEST_IDX_SVC,
    TEST_IDX_CHAR1,
    TEST_IDX_CHAR1_VAL,
    TEST_IDX_CHAR2,
    TEST_IDX_CHAR2_VAL,
    TEST_IDX_CHAR5,
    TEST_IDX_CHAR5_VAL,
    TEST_IDX_CHAR3,
    TEST_IDX_CHAR3_VAL,
    TEST_IDX_CHAR3_CUD,
    TEST_IDX_CHAR3_CCC,
    TEST_IDX_CHAR4,
    TEST_IDX_CHAR4_VAL,
    TEST_IDX_CHAR6,
    TEST_IDX_CHAR6_VAL,
    TEST_IDX_CHAR7,
    TEST_IDX_CHAR7_VAL,

    TEST_IDX_MAX,
};

gatt_service test_service;
gatt_service test2_service;
static gatt_attr_t test_attrs[] = {
    [TEST_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST_SERVICE_UUID),

    [TEST_IDX_CHAR1] = GATT_CHAR_DEFINE(TEST_CHAR1_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR1_UUID, GATT_PERM_READ),

    [TEST_IDX_CHAR2] = GATT_CHAR_DEFINE(TEST_CHAR2_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR2_UUID, GATT_PERM_READ | GATT_PERM_READ_AUTHEN),

    [TEST_IDX_CHAR5] = GATT_CHAR_DEFINE(TEST_CHAR5_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR5_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR5_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST_IDX_CHAR3] = GATT_CHAR_DEFINE(TEST_CHAR3_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_NOTIFY | GATT_CHRC_PROP_INDICATE | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR3_UUID,  GATT_PERM_READ | GATT_PERM_WRITE),
    [TEST_IDX_CHAR3_CUD] = GATT_CHAR_CUD_DEFINE(test_svc.value3_cud,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),
    [TEST_IDX_CHAR3_CCC] = GATT_CHAR_CCC_DEFINE(test3_ccc_cfg),

    [TEST_IDX_CHAR4] = GATT_CHAR_DEFINE(TEST_CHAR4_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR4_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR4_UUID,  GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),

    [TEST_IDX_CHAR6] = GATT_CHAR_DEFINE(TEST_CHAR6_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR6_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR6_UUID,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),

    [TEST_IDX_CHAR7] = GATT_CHAR_DEFINE(TEST_CHAR7_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR7_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR7_UUID,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),
};

enum {
    TEST2_IDX_SVC,
    TEST2_IDX_CHAR1,
    TEST2_IDX_CHAR1_VAL,
    TEST2_IDX_CHAR2,
    TEST2_IDX_CHAR2_VAL,
    TEST2_IDX_CHAR5,
    TEST2_IDX_CHAR5_VAL,
    TEST2_IDX_CHAR3,
    TEST2_IDX_CHAR3_VAL,
    TEST2_IDX_CHAR4,
    TEST2_IDX_CHAR4_VAL,
    TEST2_IDX_CHAR6,
    TEST2_IDX_CHAR6_VAL,

    TEST2_IDX_MAX,
};

static gatt_attr_t test2_attrs[] = {
    [TEST2_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST2_SERVICE_UUID),

    [TEST2_IDX_CHAR1] = GATT_CHAR_DEFINE(TEST2_CHAR1_UUID, GATT_CHRC_PROP_READ),
    [TEST2_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR1_UUID, GATT_PERM_READ),

    [TEST2_IDX_CHAR2] = GATT_CHAR_DEFINE(TEST2_CHAR2_UUID, GATT_CHRC_PROP_READ),
    [TEST2_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR2_UUID, GATT_PERM_READ | GATT_PERM_READ_ENCRYPT),

    [TEST2_IDX_CHAR5] = GATT_CHAR_DEFINE(TEST2_CHAR5_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST2_IDX_CHAR5_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR5_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),

    [TEST2_IDX_CHAR3] = GATT_CHAR_DEFINE(TEST2_CHAR3_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST2_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR3_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST2_IDX_CHAR4] = GATT_CHAR_DEFINE(TEST2_CHAR4_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST2_IDX_CHAR4_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR4_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_WRITE_ENCRYPT | GATT_PERM_PREPARE_WRITE),

    [TEST2_IDX_CHAR6] = GATT_CHAR_DEFINE(TEST2_CHAR6_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST2_IDX_CHAR6_VAL] = GATT_CHAR_VAL_DEFINE(TEST2_CHAR6_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_WRITE_ENCRYPT),

};

struct {
    uint16_t notify_handle;
    uint8_t notify_type;
    uint8_t indicate_ongoing;
} g_notify_data = {0};

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        test_svc.conn_handle = e->conn_handle;
    } else {
        test_svc.conn_handle = -1;
        g_notify_data.notify_type = 0;
        g_notify_data.indicate_ongoing = 0;
    }
}

static int event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int16_t handle_offset = 0;
    static int w_len = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    printf("event_char_write conn_handle %d char_handle %d len %ld offset %d\n",
           e->conn_handle, e->char_handle, e->len, e->offset);
    hexdump(e->data, e->len);

    if (test_svc.conn_handle == e->conn_handle) {
        switch (handle_offset) {
            case TEST_IDX_CHAR3_VAL:
                if (e->len > sizeof(test_svc.value3)) {
                    e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                    return 0;
                }

                if (e->offset + e->len > sizeof(test_svc.value3)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                if (e->flag) {
                    e->len = 0;
                    return 0;
                }

                memcpy(test_svc.value3 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value3)));
                e->len = BLE_MIN(e->len, sizeof(test_svc.value3));
                break;

            case TEST_IDX_CHAR5_VAL:
                if (e->offset + e->len > sizeof(test_svc.value3)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                if (e->flag) {
                    e->len = 0;
                    return 0;
                }

                memcpy(test_svc.value3 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value3)));
                e->len = BLE_MIN(e->len, sizeof(test_svc.value3));
                break;

            case TEST_IDX_CHAR4_VAL:
            case TEST_IDX_CHAR7_VAL:
                if (e->offset + e->len > sizeof(test_svc.value4)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                if (e->flag) {
                    e->len = 0;
                    return 0;
                }

                memcpy(test_svc.value4 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value4)));
                e->len = BLE_MIN(e->len, sizeof(test_svc.value4));
                break;

            case TEST_IDX_CHAR3_CUD:
                if (e->offset + e->len > sizeof(test_svc.value3_cud)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                if (e->flag) {
                    e->len = 0;
                    return 0;
                }

                memcpy(test_svc.value3_cud + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value3_cud)));
                e->len = BLE_MIN(e->len, sizeof(test_svc.value3_cud));
                break;

            case TEST_IDX_CHAR6_VAL:
                if (e->flag) {
                    if (e->offset == 0) {
                        w_len = 0;
                    }

                    w_len += e->len;
                    e->len = 0;
                    return 0;
                }

                if (w_len > sizeof(test_svc.value4)) {
                    e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                    return 0;
                }

                w_len = 0;

                if (e->len > sizeof(test_svc.value4)) {
                    e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                    return 0;
                }

                if (e->offset + e->len > sizeof(test_svc.value4)) {
                    e->len = -ATT_ERR_INVALID_OFFSET;
                    return 0;
                }

                memcpy(test_svc.value4 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value4)));
                e->len = BLE_MIN(e->len, sizeof(test_svc.value4));
                break;

            default:
                e->len = 0;
                break;
        }
    }

    return 0;
}

static int event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;
    int16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    if (test_svc.conn_handle == e->conn_handle) {
        switch (handle_offset) {
            case TEST_IDX_CHAR1_VAL:
                e->data = (uint8_t *)test_svc.value1;
                e->len = test_svc.mtu;
                break;

            case TEST_IDX_CHAR2_VAL:
                e->data = (uint8_t *)test_svc.value2;
                e->len = sizeof(test_svc.value1);
                break;

            case TEST_IDX_CHAR3_CUD:
                e->data = (uint8_t *)test_svc.value3_cud;
                e->len = sizeof(test_svc.value3_cud);
                break;

            case TEST_IDX_CHAR3_VAL:
                e->data = (uint8_t *)test_svc.value3;
                e->len = sizeof(test_svc.value3);
                break;

            case TEST_IDX_CHAR5_VAL:
                e->data = (uint8_t *)test_svc.value3;
                e->len = sizeof(test_svc.value3);
                break;

            case TEST_IDX_CHAR6_VAL:
                e->data = (uint8_t *)test_svc.value4;
                e->len = sizeof(test_svc.value4);
                break;

            case TEST_IDX_CHAR7_VAL:
                e->data = (uint8_t *)test_svc.value4;
                e->len = sizeof(test_svc.value4);
                break;

            default:
                e->data = NULL;
                e->len = 0;
                break;
        }
    }

    return 0;
}


static int event_gatt_indicate_confirm(ble_event_en event, void *event_data)
{
    evt_data_gatt_indicate_cb_t *e = event_data;

    if (e->err) {
        printf("indicate fail, err %d\n", e->err);
    } else {
        printf("indicate char %d success\n", e->char_handle);
    }

    g_notify_data.indicate_ongoing = 0;
    return 0;
}

struct k_delayed_work notify_work = {0};

void notify_action(struct k_work *work)
{
    uint8_t data[2] = {1, 2};

    if (g_notify_data.notify_type == CCC_VALUE_NOTIFY) {
        ble_stack_gatt_notificate(g_bt_conn_handle, g_notify_data.notify_handle, data, 2);
    } else if (g_notify_data.notify_type == CCC_VALUE_INDICATE) {
        if (!g_notify_data.indicate_ongoing) {
            g_notify_data.indicate_ongoing = 1;
            ble_stack_gatt_indicate(g_bt_conn_handle, g_notify_data.notify_handle, data, 2);
        }
    }
}

static int event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;
    uint16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    if (notify_work.work_q == NULL) {
        k_delayed_work_init(&notify_work, notify_action);
    }

    if (handle_offset == TEST_IDX_CHAR3_CCC) {
        test_svc.ccc = e->ccc_value;
        printf("ccc handle %d change %d\n", e->char_handle, test_svc.ccc);
        g_notify_data.notify_handle = e->char_handle - 2;

        if (test_svc.ccc == CCC_VALUE_NOTIFY) {
            g_notify_data.notify_type = CCC_VALUE_NOTIFY;
            k_delayed_work_submit(&notify_work, 2000);
        } else if (test_svc.ccc == CCC_VALUE_INDICATE) {
            g_notify_data.notify_type = CCC_VALUE_INDICATE;
            k_delayed_work_submit(&notify_work, 2000);
        } else {
            g_notify_data.notify_type = 0;
        }
    }

    return 0;
}

static int test_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ:
            event_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            event_char_write(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE:
            event_char_ccc_change(event, event_data);
            break;

        case EVENT_GATT_DISCOVERY_SVC:
        case EVENT_GATT_DISCOVERY_INC_SVC:
        case EVENT_GATT_DISCOVERY_CHAR:
        case EVENT_GATT_DISCOVERY_CHAR_DES:
            event_gatt_discovery(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ_CB:
            event_gatt_read_cb(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE_CB:
            event_gatt_write_cb(event, event_data);
            break;

        case EVENT_GATT_NOTIFY:
            event_gatt_notify(event, event_data);
            break;

        case EVENT_GATT_INDICATE_CB:
            event_gatt_indicate_confirm(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            event_gatt_mtu_exchange(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb1 = {
    .callback = test_event_callback,
};

static int event2_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int16_t handle_offset = 0;
    static int w_len = 0;
    static int w_len2 = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc2_handle, TEST2_IDX_MAX, e->char_handle, handle_offset);

    printf("event_char_write conn_handle %d char_handle %d len %ld offset %d\n",
           e->conn_handle, e->char_handle, e->len, e->offset);
    hexdump(e->data, e->len);

    switch (handle_offset) {
        case TEST2_IDX_CHAR3_VAL:
            if (e->len > sizeof(test_svc.value3)) {
                e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                return 0;
            }

            if (g_security_level < 2) {
                e->len = -ATT_ERR_AUTHORIZATION;
                return 0;
            }

            if (e->offset + e->len > sizeof(test_svc.value3)) {
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            if (e->flag) {
                e->len = 0;
                return 0;
            }

            memcpy(test_svc.value3 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value3)));
            e->len = BLE_MIN(e->len, sizeof(test_svc.value3));
            break;

        case TEST2_IDX_CHAR4_VAL:
            if (e->flag) {
                if (e->offset == 0) {
                    w_len = 0;
                }

                w_len += e->len;
                e->len = 0;

                if (ble_stack_enc_key_size_get(e->conn_handle) < 10) {
                    e->len = -ATT_ERR_ENCRYPTION_KEY_SIZE;
                    return 0;
                }

                return 0;
            }

            if (w_len > sizeof(test_svc.value4)) {
                e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                return 0;
            }

            w_len = 0;

            if (e->offset + e->len > sizeof(test_svc.value4)) {
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            memcpy(test_svc.value4 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value4)));
            e->len = BLE_MIN(e->len, sizeof(test_svc.value4));
            break;

        case TEST2_IDX_CHAR5_VAL:
            if (e->flag) {
                if (g_security_level < 2) {
                    e->len = -ATT_ERR_AUTHORIZATION;
                    return 0;
                }

                if (e->offset == 0) {
                    w_len = 0;
                }

                w_len += e->len;
                e->len = 0;
                return 0;
            }

            if (e->offset + e->len > sizeof(test_svc.value4)) {
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            if (w_len > sizeof(test_svc.value4)) {
                e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                return 0;
            }

            w_len = 0;

            memcpy(test_svc.value4 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value4)));
            e->len = BLE_MIN(e->len, sizeof(test_svc.value4));
            break;

        case TEST2_IDX_CHAR6_VAL:
            if (e->flag) {
                if (e->offset == 0) {
                    w_len2 = 0;
                }

                w_len2 += e->len;
                e->len = 0;
                return 0;
            }

            if (w_len2 > sizeof(test_svc.value3)) {
                e->len = -ATT_ERR_INVALID_ATTRIBUTE_LEN;
                return 0;
            }

            w_len2 = 0;

            if (ble_stack_enc_key_size_get(e->conn_handle) < 10) {
                e->len = -ATT_ERR_ENCRYPTION_KEY_SIZE;
                return 0;
            }

            if (e->offset + e->len > sizeof(test_svc.value3)) {
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            memcpy(test_svc.value3 + e->offset, e->data, BLE_MIN(e->len, sizeof(test_svc.value3)));
            e->len = BLE_MIN(e->len, sizeof(test_svc.value3));
            break;

        default:
            e->len = 0;
            break;
    }

    return 0;
}

static int event2_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;
    int16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc2_handle, TEST2_IDX_MAX, e->char_handle, handle_offset);

    switch (handle_offset) {
        case TEST2_IDX_CHAR1_VAL:
            if (g_security_level < 2) {
                e->len = -ATT_ERR_AUTHORIZATION;
                return 0;
            }

            e->data = (uint8_t *)test_svc.value1;
            e->len = sizeof(test_svc.value1);
            break;

        case TEST2_IDX_CHAR2_VAL:
            if (ble_stack_enc_key_size_get(e->conn_handle) < 10) {
                e->len = -ATT_ERR_ENCRYPTION_KEY_SIZE;
                return 0;
            }

            e->data = (uint8_t *)test_svc.value2;
            e->len = sizeof(test_svc.value2);
            break;

        case TEST2_IDX_CHAR3_VAL:
            e->data = (uint8_t *)test_svc.value3;
            e->len = sizeof(test_svc.value3);
            break;

        case TEST2_IDX_CHAR4_VAL:
            e->data = (uint8_t *)test_svc.value4;
            e->len = sizeof(test_svc.value4);
            break;

        case TEST2_IDX_CHAR5_VAL:
            e->data = (uint8_t *)test_svc.value4;
            e->len = sizeof(test_svc.value4);
            break;

        case TEST2_IDX_CHAR6_VAL:
            e->data = (uint8_t *)test_svc.value3;
            e->len = sizeof(test_svc.value3);
            break;

        default:
            e->data = NULL;
            e->len = 0;
            break;
    }

    return 0;
}


static int test2_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GATT_CHAR_READ:
            event2_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            event2_char_write(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb2 = {
    .callback = test2_event_callback,
};

int cmd_gatt_register_test_svc(int argc, char *argv[])
{

    int ret;

    if (!strcmp(argv[0], "gatt-register-service")) {
        ret = ble_stack_gatt_registe_service(&test_service, test_attrs, BLE_ARRAY_NUM(test_attrs));

        if (ret < 0) {
            printf("Registering test services faild (%d)\n", ret);
            return ret;
        }

        test_svc.svc_handle = ret;
        ret = ble_stack_event_register(&ble_cb1);
    } else {
        ret = ble_stack_gatt_registe_service(&test2_service, test2_attrs, BLE_ARRAY_NUM(test2_attrs));

        if (ret < 0) {
            printf("Registering test services faild (%d)\n", ret);
            return ret;
        }

        test_svc.svc2_handle = ret;
        ret = ble_stack_event_register(&ble_cb2);
    }

    if (ret) {
        return ret;
    }

    printf("Registering test services\n");

    return 0;
}

int cmd_gatt_unregister_test_svc(int argc, char *argv[])
{
    printf("Unregistering test vendor services\n");

    return 0;
}
