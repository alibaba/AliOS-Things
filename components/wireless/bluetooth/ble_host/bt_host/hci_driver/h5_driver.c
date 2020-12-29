/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <stddef.h>

#include <ble_os.h>

#define BT_DBG_ENABLED 0

#include <common/log.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/hci_driver.h>

#include "h5.h"
#include "host/hci_core.h"

#define H4_NONE      0x00
#define H4_CMD       0x01
#define H4_ACL_UP    0x02
#define H4_SCO       0x03
#define H4_EVT       0x04
#define H4_ACL_DOWN  0x05

#define H5_ACK_PKT              0x00
#define HCI_COMMAND_PKT         0x01
#define HCI_ACLDATA_PKT         0x02
#define HCI_SCODATA_PKT         0x03
#define HCI_EVENT_PKT           0x04
#define H5_VDRSPEC_PKT          0x0E
#define H5_LINK_CTL_PKT         0x0F

const static h5_t *h5_ctx;
extern u16_t bt_hci_get_cmd_opcode(struct net_buf *buf);
static int h5_send(struct net_buf *buf)
{
    u16_t opcode;
    uint8_t type  = bt_buf_get_type(buf);

    BT_DBG("buf %p type %u len %u:%s", buf, type, buf->len, bt_hex(buf->data, buf->len));

    switch (type) {
    case BT_BUF_CMD:
        opcode = bt_hci_get_cmd_opcode(buf);

        if (opcode == HCI_VSC_H5_INIT) {
            h5_ctx->h5_send_sync_cmd(opcode, NULL, buf->len);
            break;
        }

        h5_ctx->h5_send_cmd(HCI_COMMAND_PKT, buf->data, buf->len);
        break;
    case BT_BUF_ACL_OUT:
        h5_ctx->h5_send_acl_data(HCI_ACLDATA_PKT, buf->data, buf->len);
        break;

    default:
        BT_ERR("Unknown buffer type");
        return -1;
    }

    net_buf_unref(buf);

    return 0;
}

static inline int is_adv_report_event(uint8_t *data, uint16_t len)
{
    return (data[0] == H4_EVT && data[1] == BT_HCI_EVT_LE_META_EVENT
            && data[3] == BT_HCI_EVT_LE_ADVERTISING_REPORT);
}

int hci_h5_event_recv(uint8_t *data, uint16_t data_len)
{
    struct net_buf *buf;
    uint8_t *pdata = data;
    int32_t len = data_len;
    struct bt_hci_evt_hdr hdr;
    uint8_t sub_event = 0;
    uint8_t discardable = 0;

    if (pdata == NULL || len == 0) {
        return -1;
    }

    if (len < 3) {
        goto err;
    }

    hdr.evt = *pdata++;
    hdr.len = *pdata++;

    if (len < hdr.len + 2) {
        goto err;
    }

    if (hdr.evt == BT_HCI_EVT_LE_META_EVENT) {
        sub_event = *pdata++;

        if (sub_event == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
            discardable = 1;
        }
    }

    if (hdr.evt == BT_HCI_EVT_CMD_COMPLETE ||
        hdr.evt  == BT_HCI_EVT_CMD_STATUS) {
        buf = bt_buf_get_cmd_complete(0);

        if (buf == NULL) {
            // g_hci_debug_counter.event_in_is_null_count++;
            goto err;
        }
    } else {
        buf = bt_buf_get_rx(BT_BUF_EVT, 0);
    }

    if (!buf && discardable) {
        // g_hci_debug_counter.event_discard_count++;
        goto err;
    }

    if (!buf) {
        // g_hci_debug_counter.event_in_is_null_count++;
        goto err;
    }

    bt_buf_set_type(buf, BT_BUF_EVT);

    net_buf_add_mem(buf, ((uint8_t *)(data)), hdr.len + sizeof(hdr));

    BT_DBG("event %s", bt_hex(buf->data, buf->len));
    // g_hci_debug_counter.event_in_count++;

    if (bt_hci_evt_is_prio(hdr.evt)) {
        bt_recv_prio(buf);
    } else {
        bt_recv(buf);
    }

    return 0;

err:
    return -1;
}

int hci_h5_acl_recv(uint8_t *data, uint16_t data_len)
{
    struct net_buf *buf;
    uint16_t acl_len;
    if (data == NULL || data_len == 0) {
        return -1;
    }

    if (data_len < 4) {
        goto err;
    }

    acl_len = (data[3] << 8) | data[2];

    if (data_len < acl_len + 4) {
        goto err;
    }

    buf = bt_buf_get_rx(BT_BUF_ACL_IN, 0);

    if (!buf) {
        // g_hci_debug_counter.hci_in_is_null_count++;
        goto err;
    }

    net_buf_add_mem(buf, data, acl_len + 4);
    // g_hci_debug_counter.acl_in_count++;

    bt_recv(buf);
    return 0;

err:
    return -1;
}

static void packet_recv_cb(hci_data_type_t type, uint8_t *data, uint32_t len)
{
    switch (type) {
        case DATA_TYPE_ACL:
            hci_h5_acl_recv(data, len);
            break;
        case  DATA_TYPE_EVENT:
            hci_h5_event_recv(data, len);
            break;
        default:
            break;
    }

    return;
}

static int h5_open(void)
{
    h5_ctx = get_h5_interface();

    h5_ctx->h5_int_init(packet_recv_cb);

    return 0;
}

static struct bt_hci_driver drv = {
    .name       = "H5",
    .bus        = BT_HCI_DRIVER_BUS_UART,
    .open       = h5_open,
    .send       = h5_send,
};

int hci_h5_driver_init()
{
    int ret;

    ret = bt_hci_driver_register(&drv);

    if (ret) {
        return ret;
    }

    return 0;
}
