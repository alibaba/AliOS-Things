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
#include <bluetooth/hci_driver.h>

#include "bt_vendor_drv.h"
#include <hci_api.h>

#include <aos/kernel.h>

#define H4_NONE      0x00
#define H4_CMD       0x01
#define H4_ACL       0x02
#define H4_SCO       0x03
#define H4_EVT       0x04


static int h4_open(void);
static int h4_send(struct net_buf *buf);

static cpu_stack_t hci_rx_task_stack[CONFIG_BT_RX_STACK_SIZE / 4];

static struct bt_hci_driver drv = {
    .name       = "H4",
    .bus        = BT_HCI_DRIVER_BUS_VIRTUAL,
    .open       = h4_open,
    .send       = h4_send,
};

static struct rx_t {
    struct net_buf *buf;
    uint16_t cur_len;

    uint16_t remaining;
    uint16_t discard;

    uint8_t     have_hdr;
    uint8_t     discardable;

    uint8_t  hdr_len;

    uint8_t  type;
    union {
        struct bt_hci_evt_hdr evt;
        struct bt_hci_acl_hdr acl;
        uint8_t hdr[4];
    };

    uint8_t ongoing;
};


static struct {
    const struct bt_hci_driver *drv;
    char *dev_name;
//    aos_dev_t *dev;
    ktask_t task;
    ksem_t sem;
    struct rx_t rx;
} hci_h4 = {
    &drv,
    NULL,
    NULL,
};

#ifndef CHIP_HAAS1000

int tg_bt_hal_vendor_bringup() {
    return bt_vendor_drv_bring_up();
}

void tg_bt_hal_hcit_set_rx_ind_callback(rx_ind_cb_t cb) {
    bt_vendor_drv_set_rx_ind(cb);
}

size_t tg_bt_hal_hcit_rx(uint8_t *data, size_t len) {
    return bt_vendor_drv_rx(data, len);
}

size_t tg_bt_hal_hcit_tx(uint8_t *data, size_t len, void *reserve) {
    return bt_vendor_drv_tx(data, len, reserve);
}

#endif

static int h4_send(struct net_buf *buf)
{
    int ret = -1;
    uint8_t type  = bt_buf_get_type(buf);
    uint8_t hcit_type[1];

    BT_DBG("%s, type = %d", __func__, type);
    if (type == BT_BUF_ACL_OUT) {
        hcit_type[0] = H4_ACL;
        //g_hci_debug_counter.acl_out_count++;
    } else if (type == BT_BUF_CMD) {
        hcit_type[0] = H4_CMD;
        //g_hci_debug_counter.cmd_out_count++;
    } else {
        BT_ERR("Unknown buffer type");
        return -1;
    }

#if 1   //send in one packet
    uint8_t *pBuf;
    const int data_len = buf->len+1;

    pBuf = aos_malloc(data_len);
    if (pBuf == NULL) {
        BT_ERR("malloc failed");
        return -1;
    }

    pBuf[0] = hcit_type[0];
    memcpy(pBuf+1, buf->data, buf->len);

    BT_INFO("Send to driver: %s", bt_hex_real(pBuf, data_len));
    ret = tg_bt_hal_hcit_tx(pBuf, data_len, NULL);
    aos_free(pBuf);
#else
    tg_bt_hal_hcit_tx(hcit_type, 1, NULL);
    BT_DBG("buf %p type %u len %u:%s", buf, type, buf->len, bt_hex(buf->data, buf->len));

    const int data_len = buf->len;

    BT_DBG("payload send");
    ret = tg_bt_hal_hcit_tx(buf->data, buf->len, NULL);
#endif
    if (ret > 0 && ret == data_len) {
        ret = 0;
        net_buf_unref(buf);
    }

    if (ret == -BT_HCI_ERR_MEM_CAPACITY_EXCEEDED) {
        ret = -ENOMEM;
    }
    return ret;
}

static inline int is_adv_report_event(uint8_t *data, uint16_t len)
{
    return (data[0] == H4_EVT && data[1] == BT_HCI_EVT_LE_META_EVENT
            && data[3] == BT_HCI_EVT_LE_ADVERTISING_REPORT);
}

int hci_event_recv(uint8_t *data, uint16_t data_len)
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

    if (*pdata++ != H4_EVT) {
        goto err;
    }

    if (len < 3) {
        goto err;
    }

    hdr.evt = *pdata++;
    hdr.len = *pdata++;

    if (len < hdr.len + 3) {
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
            //g_hci_debug_counter.event_in_is_null_count++;
            goto err;
        }
    } else {
        buf = bt_buf_get_rx(BT_BUF_EVT, 0);
    }

    if (!buf && discardable) {
        //g_hci_debug_counter.event_discard_count++;
        goto err;
    }

    if (!buf) {
        //g_hci_debug_counter.event_in_is_null_count++;
        goto err;
    }

    bt_buf_set_type(buf, BT_BUF_EVT);

    net_buf_add_mem(buf, ((uint8_t *)(data)) + 1, hdr.len + sizeof(hdr));

    BT_DBG("event %s", bt_hex(buf->data, buf->len));
    //g_hci_debug_counter.event_in_count++;

    if (bt_hci_evt_is_prio(hdr.evt)) {
        bt_recv_prio(buf);
    } else {
        bt_recv(buf);
    }

    return 0;

err:
    return -1;
}

int hci_acl_recv(uint8_t *data, uint16_t data_len)
{
    struct net_buf *buf;
    uint8_t *pdata = data;
    int32_t len = data_len;
    uint16_t handle;
    uint16_t acl_len;

    if (pdata == NULL || len == 0) {
        return -1;
    }

    if (*pdata++ != H4_ACL) {
        goto err;
    }

    if (len < 5) {
        goto err;
    }

    handle = ((*pdata + 1) << 16) | (*(pdata));
    pdata += 2;
    acl_len = ((*pdata + 1) << 16) | (*(pdata));
    pdata += 2;

    (void)handle;

    if (len < acl_len + 5) {
        goto err;
    }

    buf = bt_buf_get_rx(BT_BUF_ACL_IN, 0);

    if (!buf) {
        //g_hci_debug_counter.hci_in_is_null_count++;
        goto err;
    }

    bt_buf_set_type(buf, BT_BUF_ACL_IN);

    net_buf_add_mem(buf, data + 1, acl_len + 4);
    //g_hci_debug_counter.acl_in_count++;
    BT_DBG("acl %s", bt_hex(buf->data, buf->len));

    bt_recv(buf);
    return 0;

err:
    return -1;
}

static void _hci_recv_event2(void)
{
    krhino_sem_give(&hci_h4.sem);
}

static void copy_hdr(struct net_buf *buf)
{
    net_buf_add_mem(buf, hci_h4.rx.hdr, hci_h4.rx.hdr_len);
}

static void reset_rx(void)
{
    hci_h4.rx.type = H4_NONE;
    hci_h4.rx.remaining = 0;
    hci_h4.rx.have_hdr = false;
    hci_h4.rx.hdr_len = 0;
    hci_h4.rx.discardable = false;
}

static uint32_t read_byte(uint8_t *data, uint32_t len)
{
    int32_t read_len;

    read_len = tg_bt_hal_hcit_rx(data, len);

    if (read_len == 0) {
        hci_h4.rx.ongoing = 0;
    }

    return read_len;
}

static inline void h4_get_type(void)
{
    /* Get packet type */
    if (read_byte(&hci_h4.rx.type, 1) != 1) {
        hci_h4.rx.type = H4_NONE;
        return;
    }

    switch (hci_h4.rx.type) {
        case H4_EVT:
            hci_h4.rx.remaining = sizeof(hci_h4.rx.evt);
            hci_h4.rx.hdr_len = hci_h4.rx.remaining;
            break;

        case H4_ACL:
            hci_h4.rx.remaining = sizeof(hci_h4.rx.acl);
            hci_h4.rx.hdr_len = hci_h4.rx.remaining;
            break;

        default:
            BT_ERR("Unknown H:4 type 0x%02x", hci_h4.rx.type);
            hci_h4.rx.type = H4_NONE;
    }
}

static inline void get_acl_hdr(void)
{
    struct bt_hci_acl_hdr *hdr = &hci_h4.rx.acl;
    int to_read = sizeof(*hdr) - hci_h4.rx.remaining;

    hci_h4.rx.remaining -= read_byte((uint8_t *)hdr + to_read, hci_h4.rx.remaining);
    if (!hci_h4.rx.remaining) {
        hci_h4.rx.remaining = hdr->len;
        BT_DBG("Got ACL header. Payload %u bytes", hci_h4.rx.remaining);
        hci_h4.rx.have_hdr = true;
    }
}

static inline void get_evt_hdr(void)
{
    struct bt_hci_evt_hdr *hdr = &hci_h4.rx.evt;
    int32_t to_read = hci_h4.rx.hdr_len - hci_h4.rx.remaining;

    hci_h4.rx.remaining -= read_byte((uint8_t *)hdr + to_read, hci_h4.rx.remaining);
    if (!hci_h4.rx.remaining) {
        hci_h4.rx.remaining = hdr->len - (hci_h4.rx.hdr_len - sizeof(*hdr));

        BT_DBG("Got event header. Payload %u bytes", hdr->len);
        hci_h4.rx.have_hdr = true;
    }
}

static int32_t h4_discard(int32_t len)
{
    uint8_t buf[33];

    return read_byte(buf, (len<sizeof(buf)?len:sizeof(buf)));
}

static inline void read_payload(void)
{
    struct net_buf *buf;
    bool prio;
    int read;

    if (!hci_h4.rx.buf) {
        if (hci_h4.rx.type == H4_ACL) {
            hci_h4.rx.buf = buf = bt_buf_get_rx(BT_BUF_ACL_IN, 0);
        } else {
            if (hci_h4.rx.evt.evt == BT_HCI_EVT_CMD_COMPLETE || hci_h4.rx.evt.evt  == BT_HCI_EVT_CMD_STATUS) {
                hci_h4.rx.buf = bt_buf_get_cmd_complete(0);
            } else {
                hci_h4.rx.buf = buf = bt_buf_get_rx(BT_BUF_EVT, 0);
            }
        }

        if (!hci_h4.rx.buf) {
            if (hci_h4.rx.discardable) {
                BT_DBG("Discarding event 0x%02x", hci_h4.rx.evt.evt);
                hci_h4.rx.discard = hci_h4.rx.remaining;
                reset_rx();
                return;
            }

            BT_ERR("Failed to allocate, deferring to rx_thread");
            return;
        }

        BT_DBG("Allocated h4_dev.rx.buf %p", hci_h4.rx.buf);

        copy_hdr(hci_h4.rx.buf);
        hci_h4.rx.cur_len = hci_h4.rx.hdr_len + 1;
    }

    read = read_byte(net_buf_tail(hci_h4.rx.buf), hci_h4.rx.remaining);
    net_buf_add(hci_h4.rx.buf, read);
    hci_h4.rx.cur_len += read;
    hci_h4.rx.remaining -= read;

    BT_DBG("got %d bytes, remaining %u", read, hci_h4.rx.remaining);

    if (hci_h4.rx.remaining) {
        return;
    }

    prio = ((hci_h4.rx.type == H4_EVT) && bt_hci_evt_is_prio(hci_h4.rx.evt.evt));
    buf = hci_h4.rx.buf;
    hci_h4.rx.buf = NULL;

    if (hci_h4.rx.type == H4_EVT) {
        bt_buf_set_type(buf, BT_BUF_EVT);
    } else {
        bt_buf_set_type(buf, BT_BUF_ACL_IN);
    }

    reset_rx();
    if (prio) {
        BT_DBG("Calling bt_recv_prio(%p)", buf);
        bt_recv_prio(buf);
    } else {
        BT_DBG("Putting buf %p to rx fifo", buf);
        bt_recv(buf);
    }
}

static inline void read_header(void)
{
    switch (hci_h4.rx.type) {
        case H4_NONE:
            h4_get_type();
            return;

        case H4_EVT:
            get_evt_hdr();
            break;
        case H4_ACL:
            get_acl_hdr();
            break;

        default:
            reset_rx();
            return;
    }
}

static void process_rx(void)
{
    BT_DBG("remaining %u discard %u have_hdr %u h4_dev.rx.buf %p",
           hci_h4.rx.remaining, hci_h4.rx.discard, hci_h4.rx.have_hdr, hci_h4.rx.buf);

    if (hci_h4.rx.discard) {
        hci_h4.rx.discard -= h4_discard(hci_h4.rx.discard);
        return;
    }

    if (hci_h4.rx.have_hdr) {
        read_payload();
    } else {
        read_header();
    }
}

static void recv_data(void)
{
    do {
        hci_h4.rx.ongoing = 1;
        process_rx();
    } while (hci_h4.rx.ongoing);
}

static void hci_rx_task(void *arg)
{
    while (1) {
        krhino_sem_take(&hci_h4.sem, RHINO_WAIT_FOREVER);
        recv_data();
    }
}

static int h4_open(void)
{
    krhino_sem_create(&hci_h4.sem, "h4", 0);


    int ret;

    ret = tg_bt_hal_vendor_bringup();
    if (ret != 0) {
        BT_ERR("vendor chip bringup failed");
        return false;
    }
    tg_bt_hal_hcit_set_rx_ind_callback(_hci_recv_event2);

    krhino_task_create(&hci_h4.task, "hci_rx_task", NULL,
                       CONFIG_BT_RX_PRIO, 1, hci_rx_task_stack,
                       CONFIG_BT_RX_STACK_SIZE / 4, (task_entry_t)hci_rx_task, 1);

    return 0;
}

int hci_driver_init(char *name)
{
    int ret;

    ret = bt_hci_driver_register(&drv);

    if (ret) {
        return ret;
    }

    hci_h4.dev_name = name;

    return 0;
}

int hci_h4_driver_init()
{
    hci_driver_init("hci");
    return 0;
}
