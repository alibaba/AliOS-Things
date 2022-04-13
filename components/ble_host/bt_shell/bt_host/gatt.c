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
#include <ble_os.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include "bt.h"
#include "gatt.h"
#include <aos/ble.h>
#include <aos/gatt.h>
#include <aos/kernel.h>

#include <work.h>

#define CHAR_SIZE_MAX           256
extern int16_t g_bt_conn_handle;
extern int16_t g_security_level;
#define VALUE_LEN 100
extern char *uuid_str(uuid_t *uuid);
extern void str2hex(uint8_t hex[], char *s, uint8_t cnt);
extern void hexdump(const u8_t *data, size_t len);
uint8_t gatt_read_show_mode = 0;

struct test_svc_t {
    int16_t conn_handle;
    uint16_t svc_handle;
    uint16_t svc2_handle;
    ccc_value_en ccc;
    uint16_t mtu;
    char value1[256];
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
    test_svc.mtu = ble_stack_gatt_mtu_get(e->conn_handle);
    printf("Exchange %s, MTU %d\n", e->err == 0 ? "successful" : "failed", test_svc.mtu);

    return 0;
}

int cmd_gatt_exchange_mtu(int argc, char *argv[])
{
    int err;

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    err = ble_stack_gatt_mtu_exchange(g_bt_conn_handle);

    if (err) {
        printf("Exchange failed (err %d)\n", err);
    } else {
        printf("Exchange pending\n");
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
        printf("Characteristic %s found: handle %x\n", uuid_str(&e->char_c.uuid),
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
    union {
        struct ut_uuid_16 uuid16;
        struct ut_uuid_32 uuid32;
        struct ut_uuid_128 uuid128;
    } uuid = {0};

    uint16_t start_handle = 0x0001;
    uint16_t end_handle = 0xffff;

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
    if (strlen(argv[1]) <= 4) {
        uuid.uuid16.uuid.type = UUID_TYPE_16;
        uuid.uuid16.val = strtoul(argv[1], NULL, 16);
    } else if (strlen(argv[1]) == 32) {
        uuid.uuid128.uuid.type = UUID_TYPE_128;
        str2hex(uuid.uuid128.val, argv[1], 16);
    } else {
        printf("invaild uuid\n");
        return -EINVAL;
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

    err = ble_stack_gatt_discovery(g_bt_conn_handle, type, (uuid.uuid16.uuid.type == 0 && uuid.uuid16.val == 0) ? NULL : (uuid_t *)&uuid, start_handle, end_handle);

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
    if(!e->len || !e->data){
      return 0;
    }
    if(!gatt_read_show_mode) {
        hexdump(e->data, e->len);
    } else {
        for(int i = 0; i < e->len; i++) {
            printf("%02x",*e->data++);
        }
        printf("\r\n");
    }
    return 0;
}

int cmd_gatt_read_show_format(int argc, char *argv[])
{
    if (argc < 2) {
        return -EINVAL;
    }
    gatt_read_show_mode = strtoul(argv[1], NULL, 16);
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
    static uint16_t h[8];

    if (g_bt_conn_handle == -1) {
        printf("Not connected\n");
        return 0;
    }

    if (argc < 3) {
        return -EINVAL;
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

    if (argc >= 3 && !strcmp(argv[2], "ind")) {
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

//static struct bt_gatt_ccc_cfg_t bas_ccc_cfg[2] = {};

gatt_service test_service;
gatt_service test2_service;


static  gatt_attr_t test_attrs[] = {
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
    [TEST_IDX_CHAR3_CCC] = GATT_CHAR_CCC_DEFINE(),

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


enum {
    TEST3_IDX_SVC,
    TEST3_IDX_RX,
    TEST3_IDX_RX_VAL,
    TEST3_IDX_TX,
    TEST3_IDX_TX_VAL,
    TEST3_IDX_TX_CCC,
    TEST3_IDX_MAX,
};

#define TEST3_SERVICE_UUID UUID128_DECLARE(0xF0,0x33,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST3_RX_UUID UUID128_DECLARE(0xF1,0x33,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST3_TX_UUID UUID128_DECLARE(0xF2,0x33,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)

static BT_STACK_NOINIT(transport_task_stack, 1024);

//static struct bt_gatt_ccc_cfg_t test3_ccc_cfg[2] = {};

gatt_service test3_service;

struct {
    struct k_thread thread;
    uint8_t registed;
    uint16_t svc_handle;
    int16_t conn_handle;
    uint8_t rx_buf[247];
    uint16_t ccc;
    uint8_t type; //0:server 1:client
    uint8_t mode; // 0:loop  1:single
    uint8_t svc_tx_mode; //0:notify 1:indicate
    uint16_t svc_rx_handle;
    uint8_t cli_tx_mode; //0:write 1:write without response
    uint32_t data_len; // 0:stream otherwise data_size
    uint32_t fail_count;
    uint32_t missmatch_count;
    uint32_t trans_data;
    uint32_t tx_count;
    uint32_t tx_cb_count;
    uint32_t tx_retry_count;
    uint32_t rx_count;
    uint32_t connect_count;
    uint32_t disconn_count;
    aos_sem_t op_sem;
    aos_sem_t sync_sem;
    uint8_t op;
} test3_svc  = {0};

static gatt_attr_t test3_attrs0[] = {
    [TEST2_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST3_SERVICE_UUID),

    [TEST3_IDX_RX] = GATT_CHAR_DEFINE(TEST3_RX_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST3_IDX_RX_VAL] = GATT_CHAR_VAL_DEFINE(TEST3_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST3_IDX_TX] = GATT_CHAR_DEFINE(TEST3_TX_UUID, GATT_CHRC_PROP_NOTIFY),
    [TEST3_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(TEST3_TX_UUID, GATT_PERM_NONE),
    [TEST3_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
};

static gatt_attr_t test3_attrs1[] = {
    [TEST2_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST3_SERVICE_UUID),

    [TEST3_IDX_RX] = GATT_CHAR_DEFINE(TEST3_RX_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [TEST3_IDX_RX_VAL] = GATT_CHAR_VAL_DEFINE(TEST3_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST3_IDX_TX] = GATT_CHAR_DEFINE(TEST3_TX_UUID, GATT_CHRC_PROP_INDICATE),
    [TEST3_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(TEST3_TX_UUID, GATT_PERM_NONE),
    [TEST3_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
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

    printf("event_char_write conn_handle %d char_handle %d len %d offset %d\n",
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
        k_delayed_work_submit(&notify_work, 2000);
    } else if (g_notify_data.notify_type == CCC_VALUE_INDICATE) {
        if (!g_notify_data.indicate_ongoing) {
            g_notify_data.indicate_ongoing = 1;
            ble_stack_gatt_indicate(g_bt_conn_handle, g_notify_data.notify_handle, data, 2);
            k_delayed_work_submit(&notify_work, 2000);
        }
    }
}

static int event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;
    uint16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test_svc.svc_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    if (notify_work.work.handler == NULL) {
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

#if defined(CONFIG_BT_GATT_CLIENT)

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
#endif
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

    printf("event_char_write conn_handle %d char_handle %d len %d offset %d\n",
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

void test3_start_adv(void)
{
    ad_data_t ad[2] = {0};
    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    ad[1].type = AD_DATA_TYPE_NAME_COMPLETE;
    ad[1].data = (uint8_t *)"TEST";
    ad[1].len = strlen("TEST");

    adv_param_t param = {
        ADV_IND,
        ad,
        NULL,
        BLE_ARRAY_NUM(ad),
        0,
        ADV_FAST_INT_MIN_1,
        ADV_FAST_INT_MAX_1,
    };

    int ret = ble_stack_adv_start(&param);

    if (ret) {
        test3_svc.fail_count++;
        printf("adv start fail %d!\n", ret);
    } else {
        printf("adv start!\n");
    }
}

static ble_event_cb_t ble_cb2 = {
    .callback = test2_event_callback,
};

static int data_check(uint8_t *data, uint16_t len)
{
    int i = 1;

    while (i < len && data[i] == i) {
        i++;
    };

    return (i == len);
}

static int event3_char_write(ble_event_en event, void *event_data)
{
    int ret;
    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    int16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test3_svc.svc_handle, TEST3_IDX_MAX, e->char_handle, handle_offset);

    //printf("event_char_write conn_handle %d char_handle %d len %d offset %d\n",
    //       e->conn_handle, e->char_handle, e->len, e->offset);
    //hexdump(e->data, e->len);

    switch (handle_offset) {
    case TEST3_IDX_RX_VAL:
        memcpy(test3_svc.rx_buf + e->offset, e->data, BLE_MIN(e->len, sizeof(test3_svc.rx_buf)));
        e->len = BLE_MIN(e->len, sizeof(test3_svc.rx_buf));

        test3_svc.rx_count++;

        if (data_check(test3_svc.rx_buf, BLE_MIN(e->len, sizeof(test3_svc.rx_buf)))) {
            test3_svc.trans_data += e->len;
        } else {
            test3_svc.missmatch_count++;
        }

        if (test3_svc.mode == 0) {
            if (test3_svc.svc_tx_mode) {
                if (test3_svc.ccc == CCC_VALUE_INDICATE) {
                    ret = ble_stack_gatt_indicate(test3_svc.conn_handle, test3_svc.svc_handle + TEST3_IDX_TX_VAL, test3_svc.rx_buf, e->len);

                    if (ret) {
                        test3_svc.fail_count++;
                        printf("indicate fail %d\n", ret);
                        return ret;
                    }

                    test3_svc.trans_data += e->len;
                    test3_svc.tx_count++;
                } else {
                    test3_svc.fail_count++;
                    printf("indicate is disabled\n");
                }
            } else {
                if (test3_svc.ccc == CCC_VALUE_NOTIFY) {
                    ret = ble_stack_gatt_notificate(test3_svc.conn_handle, test3_svc.svc_handle + TEST3_IDX_TX_VAL, test3_svc.rx_buf, e->len);

                    if (ret) {
                        test3_svc.fail_count++;
                        printf("nofify fail %d\n", ret);
                        return ret;
                    }

                    test3_svc.trans_data += e->len;
                    test3_svc.tx_count++;
                } else {
                    test3_svc.fail_count++;
                    printf("nofify is disabled\n");
                }
            }
        }

        break;

    default:
        e->len = 0;
        break;
    }

    return 0;
}

static int event3_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e = (evt_data_gatt_char_read_t *)event_data;
    int16_t handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(test3_svc.svc_handle, TEST3_IDX_MAX, e->char_handle, handle_offset);

    switch (handle_offset) {
    case TEST3_IDX_RX_VAL:
        e->data = (uint8_t *)test3_svc.rx_buf;
        e->len = sizeof(test3_svc.rx_buf);
        break;

    default:
        e->data = NULL;
        e->len = 0;
        break;
    }

    return 0;
}

static int event3_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;
    test3_svc.ccc = e->ccc_value;
    printf("CCC change %d\n", e->ccc_value);
    return 0;
}

static void event3_conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        test3_svc.connect_count++;
        test3_svc.conn_handle = e->conn_handle;
    } else {
        test3_svc.op = 0;
        test3_svc.conn_handle = -1;
        test3_svc.disconn_count++;
    }
}

static void event3_indicate_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_indicate_cb_t *e = event_data;

    if (e->char_handle == test3_svc.svc_handle + TEST3_IDX_TX_VAL) {
        test3_svc.tx_cb_count++;
        aos_sem_signal(&test3_svc.sync_sem);
    }
}

static void event3_write_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_write_cb_t *e = event_data;

    if (e->char_handle == test3_svc.svc_rx_handle) {
        test3_svc.tx_cb_count++;
        aos_sem_signal(&test3_svc.sync_sem);
    }
}

static void event3_notify(ble_event_en event, void *event_data)
{
    evt_data_gatt_notify_t *e = event_data;
    //printf("event3_notify conn_handle %d char_handle %d len %d\n",
    //       e->conn_handle, e->char_handle, e->len);
    //hexdump(e->data, e->len);
    test3_svc.rx_count++;

    if (data_check((uint8_t *)e->data, e->len)) {
        test3_svc.trans_data += e->len;
    } else {
        test3_svc.missmatch_count++;
    }
}

static void event3_mtu_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = event_data;
    printf("mtu exchange %s, mtu %d\n", e->err ? "fail" : "success", ble_stack_gatt_mtu_get(e->conn_handle));
}

static int test3_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
    case EVENT_GAP_CONN_CHANGE:
        event3_conn_change(event, event_data);
        break;

    case EVENT_GATT_CHAR_READ:
        event3_char_read(event, event_data);
        break;

    case EVENT_GATT_CHAR_WRITE:
        event3_char_write(event, event_data);
        break;

    case EVENT_GATT_CHAR_CCC_CHANGE:
        event3_ccc_change(event, event_data);
        break;

    case EVENT_GATT_INDICATE_CB:
        event3_indicate_cb(event, event_data);
        break;

    case EVENT_GATT_CHAR_WRITE_CB:
        event3_write_cb(event, event_data);
        break;

    case EVENT_GATT_NOTIFY:
        event3_notify(event, event_data);
        break;

    case EVENT_GATT_MTU_EXCHANGE:
        event3_mtu_change(event, event_data);
        break;

    default:
        break;
    }

    return 0;
}

static ble_event_cb_t ble_cb3 = {
    .callback = test3_event_callback,
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

uint8_t test_data[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static int gatt_indicate(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (test3_svc.ccc == CCC_VALUE_INDICATE) {
        ret = aos_sem_wait(&test3_svc.sync_sem, 10000);

        if (ret) {
            printf("indicate sem wait fail %d\n", ret);
            return ret;
        }

        ret = ble_stack_gatt_indicate(test3_svc.conn_handle, test3_svc.svc_handle + TEST3_IDX_TX_VAL, data, len);

        if (ret) {
            test3_svc.fail_count++;
            return ret;
        }

        test3_svc.trans_data += len;
    } else {
        ret = -1;
        test3_svc.fail_count++;
        printf("indicate is disabled\n");
    }

    return ret;
}

static int gatt_notification(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (test3_svc.ccc == CCC_VALUE_NOTIFY) {
        ret = ble_stack_gatt_notificate(test3_svc.conn_handle, test3_svc.svc_handle + TEST3_IDX_TX_VAL, data, len);

        if (ret) {
            return ret;
        }

        test3_svc.trans_data += len;
    } else {
        ret = -1;
        printf("nofify is disabled\n");
    }

    return ret;
}

static int gatt_write(const uint8_t *data, uint16_t len)
{
    int ret = 0;
    ret = aos_sem_wait(&test3_svc.sync_sem, 1000);

    if (ret) {
        printf("write sem wait fail %d\n", ret);
        return ret;
    }

    ret = ble_stack_gatt_write_response(test3_svc.conn_handle, test3_svc.svc_rx_handle, data, len, 0);

    if (ret) {
        return ret;
    }

    test3_svc.trans_data += len;

    return ret;
}

static int gatt_write_no_response(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    ret = ble_stack_gatt_write_no_response(test3_svc.conn_handle, test3_svc.svc_rx_handle, data, len, 0);

    if (ret) {
        return ret;
    }

    test3_svc.trans_data += len;

    return ret;
}

typedef int (*gatt_send_func_t)(const uint8_t *data, uint16_t len);

gatt_send_func_t gatt_send_func_table[2][2] = {
    {gatt_notification, gatt_indicate},
    {gatt_write, gatt_write_no_response}
};


static void transport_task(void *arg)
{
    int ret;
    uint32_t trans_start_time;
    uint32_t trans_end_time;
    printf("transport task start\n");

    while (1) {
        aos_sem_wait(&test3_svc.op_sem, AOS_WAIT_FOREVER);
        trans_start_time = aos_now_ms();
        if (test3_svc.op == 1) {
            gatt_send_func_t send_func;

            if (test3_svc.type == 0) {
                send_func = gatt_send_func_table[test3_svc.type][test3_svc.svc_tx_mode];
            } else {
                send_func = gatt_send_func_table[test3_svc.type][test3_svc.cli_tx_mode];
            }

            uint32_t count = test3_svc.data_len;
            int mtu = ble_stack_gatt_mtu_get(test3_svc.conn_handle) - 3;
            int i = 0;

            if (test3_svc.data_len == 0) {
                count = 0xFFFFFFFF;
            }

            while (count) {
                uint16_t send_count = mtu < count ? mtu : count;
                test_data[0] = i;
                ret = send_func(test_data, send_count);

                if (ret == -ENOMEM) {
                    test3_svc.tx_retry_count++;
                    aos_msleep(1);
                    continue;
                }

                i++;

                if (ret) {
                    test3_svc.fail_count++;
                    printf("send fail %d\n", ret);
                }

                count -= send_count;
                test3_svc.tx_count++;

                if (test3_svc.op == 0) {
                    printf("op stop\n");
                    break;
                }

                if (test3_svc.data_len == 0) {
                    count = 0xFFFFFFFF;
                }
            }
            trans_end_time = aos_now_ms();
            float speed = (test3_svc.data_len * 1000 / 1024) / (trans_end_time - trans_start_time);
            printf("send %d complete in %d ms ,speed %f kB/s\n", test3_svc.data_len,trans_end_time - trans_start_time,speed);
        }
    }
}

static int gatt_transport_test_config(int argc, char *argv[])
{
    int ret;

    if (argc < 6) {
        printf("params num err");
        return -EINVAL;
    }

    if (!test3_svc.registed) {
        memset(&test3_svc, 0, sizeof(test3_svc));
        test3_svc.conn_handle = -1;
    }

    test3_svc.type = atoi(argv[0]);
    test3_svc.mode = atoi(argv[1]);
    test3_svc.svc_tx_mode = atoi(argv[2]);
    test3_svc.svc_rx_handle = atoi(argv[3]);
    test3_svc.cli_tx_mode = atoi(argv[4]);
    test3_svc.data_len = atoi(argv[5]);

    if (!test3_svc.registed) {
        if (test3_svc.type == 0) {
            if (test3_svc.svc_tx_mode) {
                ret = ble_stack_gatt_registe_service(&test3_service, test3_attrs1, BLE_ARRAY_NUM(test3_attrs1));
            } else {
                ret = ble_stack_gatt_registe_service(&test3_service, test3_attrs0, BLE_ARRAY_NUM(test3_attrs0));
            }

            if (ret < 0) {
                printf("Registering test services faild (%d)\n", ret);
                return ret;
            }

            test3_svc.svc_handle = ret;

        }

        ret = ble_stack_event_register(&ble_cb3);

        if (ret) {
            return ret;
        }

        ret = aos_sem_new(&test3_svc.sync_sem, 1);

        if (ret) {
            return ret;
        }

        ret = aos_sem_new(&test3_svc.op_sem, 0);

        if (ret) {
            return ret;
        }

        ret = k_thread_spawn(&test3_svc.thread, "gatt test", transport_task_stack,
                             K_THREAD_STACK_SIZEOF(transport_task_stack),
                             transport_task, NULL, 30);

        if (ret) {
            return ret;
        }

        test3_svc.registed = 1;
    }

    printf("gatt-transport-test-config success!\n");

    if (test3_svc.type == 0) {
        printf("type:server mode:%s tx mode:%s data len:%d\n", test3_svc.mode ? "single" : "loop", test3_svc.svc_tx_mode ? "indicate" : "notify", test3_svc.data_len);
        printf("use <ble adv conn> command to start advertising!\n");
    } else {
        printf("type:client mode:%s svc rx handle:0x%x tx mode: %s data len:%d\n",
               test3_svc.mode ? "single" : "loop", test3_svc.svc_rx_handle,
               test3_svc.cli_tx_mode ? "write without response" : "write",
               test3_svc.data_len);
        printf("use <ble connect> command to connect test device which in server mode!\n");
    }

    return 0;
}

static int gatt_transport_test_start()
{
    if (test3_svc.conn_handle == -1) {
        printf("Not Connected\n");
        return -1;
    }

    test3_svc.op = 1;
    aos_sem_signal(&test3_svc.op_sem);
    return 0;
}

static int gatt_transport_test_show_result()
{
    printf("TRANSPORT TEST REPORT:\n");
    printf("Type                  : %s\n", test3_svc.type ? "Client" : "Server");
    printf("Mode                  : %s\n", test3_svc.mode ? "Single" : "Loop");

    if (test3_svc.type == 0) {
        printf("TX mode               : %s\n", test3_svc.svc_tx_mode ? "Indication" : "Notification");
    } else {
        printf("TX mode               : %s\n", test3_svc.cli_tx_mode ? "Write without response" : "Write with response");
    }

    if (test3_svc.data_len) {
        printf("Data Len              : %d\n", test3_svc.data_len);
    } else {
        printf("Data Len              : stream\n");
    }

    printf("Transport data amount : %d\n", test3_svc.trans_data);
    printf("Transport missmatch   : %d\n", test3_svc.missmatch_count);
    printf("Transport tx count    : %d\n", test3_svc.tx_count);
    printf("Transport tx cb count : %d\n", test3_svc.tx_cb_count);
    printf("Transport resend count: %d\n", test3_svc.tx_retry_count);
    printf("Transport rx count    : %d\n", test3_svc.rx_count);
    printf("Transport fail count  : %d\n", test3_svc.fail_count);
    printf("Transport conn count  : %d\n", test3_svc.connect_count);
    printf("Transport discon count: %d\n", test3_svc.disconn_count);
    return 0;
}

static int gatt_transport_test_op(int argc, char *argv[])
{
    uint8_t op = 0xFF;

    if (argc < 1) {
        printf("params num err");
        return -EINVAL;
    }

    op = atoi(argv[0]);
    test3_svc.op = op;

    if (op == 1) {
        return gatt_transport_test_start();
    } else if (op == 2) {
        return gatt_transport_test_show_result();
    } else if (op == 3) {
        test3_svc.trans_data = 0;
        test3_svc.fail_count = 0;
        test3_svc.missmatch_count = 0;
        test3_svc.tx_count = 0;
        test3_svc.tx_cb_count = 0;
        test3_svc.rx_count = 0;
        test3_svc.tx_retry_count = 0;
    }

    return 0;
}

int cmd_gatt_transport_test(int argc, char *argv[])
{
    if (!strcmp(argv[0], "gatt-transport-test-config")) {
        return gatt_transport_test_config(argc - 1, &argv[1]);
    } else if (!strcmp(argv[0], "gatt-transport-test-op")) {
        return gatt_transport_test_op(argc - 1, &argv[1]);
    }

    return 0;
}

int cmd_gatt_unregister_test_svc(int argc, char *argv[])
{
    printf("Unregistering test vendor services\n");

    return 0;
}
