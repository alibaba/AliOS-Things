/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include "errno.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_HCI_DRIVER)

#include <misc/slist.h>
#include <bluetooth/bluetooth.h>
#include <drivers/hci_driver.h>
#include <common/log.h>
#include <misc/byteorder.h>

#include "esp_bt.h"
#include "nvs_flash.h"
#define HCI_DRV_D(...) 
#define HCI_NONE    0x00
#define HCI_CMD     0x01
#define HCI_ACL     0x02
#define HCI_SCO     0x03
#define HCI_EVT     0x04
#define MIN_HCI_LEN 4
struct hci_cmd_send_t {
    struct k_sem send_sem;
    uint8_t cmd_buf[BT_BUF_RX_SIZE];
    uint16_t buf_len;
} g_esp32_hci_send;

struct hci_cmd_recv_t {
    struct net_buf *buf;
    uint16_t remaining;
    bool discardable;
    uint8_t hdr_len;
    uint8_t type;
    union {
        struct bt_hci_evt_hdr evt;
        struct bt_hci_acl_hdr acl;
        uint8_t hdr[4];
    };
} g_esp32_hci_recv;

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

static void controller_rcv_pkt_ready(void)
{
    k_sem_give(&g_esp32_hci_send.send_sem);
}

static inline void copy_hdr(struct net_buf *buf)
{
    net_buf_add_mem(buf, g_esp32_hci_recv.hdr, g_esp32_hci_recv.hdr_len);
}

static void reset_rx(void)
{
    g_esp32_hci_recv.type = HCI_NONE;
    g_esp32_hci_recv.remaining = 0;
    g_esp32_hci_recv.hdr_len = 0;
    g_esp32_hci_recv.discardable = false;
}

static int get_hci_evt_hdr(uint8_t *data, uint16_t len)
{
    struct bt_hci_evt_hdr *hdr = &g_esp32_hci_recv.evt;
    g_esp32_hci_recv.hdr_len = sizeof(struct bt_hci_evt_hdr);
    memcpy((uint8_t *)hdr, data, g_esp32_hci_recv.hdr_len);

    if (hdr->evt == BT_HCI_EVT_LE_META_EVENT) {
        memcpy((uint8_t *)hdr + g_esp32_hci_recv.hdr_len, data \
               + g_esp32_hci_recv.hdr_len, 1);
        g_esp32_hci_recv.hdr_len++;
    }

    if (hdr->evt == BT_HCI_EVT_LE_META_EVENT &&
        g_esp32_hci_recv.hdr[sizeof(*hdr)] == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
        BT_DBG("Marking adv report as discardable");
        g_esp32_hci_recv.discardable = true;
    }

    g_esp32_hci_recv.remaining = hdr->len - (g_esp32_hci_recv.hdr_len \
                                 - sizeof(struct bt_hci_evt_hdr));
    return g_esp32_hci_recv.hdr_len;
}


static int get_hci_acl_hdr(uint8_t *data, uint16_t len)
{
    struct bt_hci_acl_hdr *hdr = &g_esp32_hci_recv.acl;
    g_esp32_hci_recv.hdr_len = sizeof(struct bt_hci_acl_hdr);
    memcpy((uint8_t *)hdr, data, g_esp32_hci_recv.hdr_len);
    g_esp32_hci_recv.remaining = sys_le16_to_cpu(hdr->len);
    return g_esp32_hci_recv.hdr_len;
}

static struct net_buf *get_rx(int timeout)
{
    BT_DBG("type 0x%02x, evt 0x%02x", g_esp32_hci_recv.type, g_esp32_hci_recv.evt.evt);

    if (g_esp32_hci_recv.type == HCI_EVT && (g_esp32_hci_recv.evt.evt == BT_HCI_EVT_CMD_COMPLETE ||
            g_esp32_hci_recv.evt.evt == BT_HCI_EVT_CMD_STATUS)) {
        return bt_buf_get_cmd_complete(timeout);
    }

    return bt_buf_get_rx(BT_BUF_EVT, timeout);
}

static int host_rcv_pkt(uint8_t *data, uint16_t len)
{
    uint8_t *pdata = data;
    uint16_t length = len;
    bool prio;
    struct net_buf *buf;

    BT_DBG("%s", bt_hex(data, len));

    if (NULL == pdata || length < MIN_HCI_LEN) {
        BT_ERR("wrong hci data len,%d\n", len);
        return -EINVAL;
    }

    g_esp32_hci_recv.type = *pdata++;
    length--;
    HCI_DRV_D("RCV %d",g_esp32_hci_recv.type);
    switch (g_esp32_hci_recv.type) {
        case HCI_EVT:
            pdata += get_hci_evt_hdr(pdata, length);
            break;

        case HCI_ACL:
            pdata += get_hci_acl_hdr(pdata, length);
            break;

        default:
            BT_ERR("Unknown H:4 type 0x%02x", g_esp32_hci_recv.type);
            g_esp32_hci_recv.type = HCI_NONE;
            return -EINVAL;
    }

    if (!g_esp32_hci_recv.buf) {
        g_esp32_hci_recv.buf = get_rx(K_NO_WAIT);

        if (!g_esp32_hci_recv.buf) {
            if (g_esp32_hci_recv.discardable) {
                BT_WARN("Discarding event 0x%02x", g_esp32_hci_recv.evt.evt);
                reset_rx();
                return 0;
            }

            BT_WARN("Failed to allocate, deferring to rx_thread");
            return -ENOMEM;
        }

        copy_hdr(g_esp32_hci_recv.buf);
    }

    net_buf_add_mem(g_esp32_hci_recv.buf, pdata, g_esp32_hci_recv.remaining);

    buf = g_esp32_hci_recv.buf;
    g_esp32_hci_recv.buf = NULL;

    if (g_esp32_hci_recv.type == HCI_EVT) {
        bt_buf_set_type(buf, BT_BUF_EVT);
    } else {
        bt_buf_set_type(buf, BT_BUF_ACL_IN);
    }

    prio = (g_esp32_hci_recv.type == HCI_EVT && bt_hci_evt_is_prio(g_esp32_hci_recv.evt.evt));

    reset_rx();

    if (prio) {
        HCI_DRV_D("Calling bt_recv_prio(%p)", buf);
        bt_recv_prio(buf);
    } else {
        HCI_DRV_D("Putting buf %p to rx fifo", buf);
        bt_recv(buf);
    }

    return 0;
}

static const esp_vhci_host_callback_t vhci_host_cb = {
    .notify_host_send_available = controller_rcv_pkt_ready,
    .notify_host_recv = host_rcv_pkt,
};

static int esp32_hci_driver_send(struct net_buf *buf)
{
    if (NULL == buf) {
        return -EINVAL;
    }

    switch (bt_buf_get_type(buf)) {
        case BT_BUF_ACL_OUT:
            g_esp32_hci_send.cmd_buf[0] = HCI_ACL;
            break;

        case BT_BUF_CMD:
            g_esp32_hci_send.cmd_buf[0] = HCI_CMD;
            break;

        default:
            BT_ERR("Unknown buffer type");
            return -EINVAL;
    }

    memcpy(g_esp32_hci_send.cmd_buf + 1, buf->data, buf->len);
    g_esp32_hci_send.buf_len = buf->len;

    BT_DBG("%s", bt_hex(buf->data, buf->len));
    net_buf_unref(buf);


    if(0==k_sem_take(&g_esp32_hci_send.send_sem, K_FOREVER)){
        if (esp_vhci_host_check_send_available()) {
            HCI_DRV_D(">>>");
            esp_vhci_host_send_packet(g_esp32_hci_send.cmd_buf, g_esp32_hci_send.buf_len + 1);
        }else{
            HCI_DRV_D("VHCI NOT AVA");
        }
    }else{
        HCI_DRV_D("WAIT SEND AVA TIMEOUT");
    }

    return 0;
}

static int esp32_hci_driver_open(void)
{
    esp_vhci_host_register_callback(&vhci_host_cb);
    return 0;
}

static struct bt_hci_driver drv = {
    .name   = "esp32 Controller",
    .bus    = BT_HCI_DRIVER_BUS_VIRTUAL,
    .open   = esp32_hci_driver_open,
    .send   = esp32_hci_driver_send,
};

int hci_driver_init()
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
#if 0
    esp_err_t ret = nvs_flash_init();
    LOG(">>> nvs_flash_init %d",ret);
    return 0;
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
#endif

    bt_cfg.controller_task_prio = 23;  // set valid task priority
    LOG(">>> bt controller init task pri %d",bt_cfg.controller_task_prio);
    const esp_err_t rc = esp_bt_controller_init(&bt_cfg);
    if ( rc != ESP_OK) {
        LOG("### bt controller init fail %d",rc);
        return -1;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_BTDM) != ESP_OK) {
        LOG("### bt controller enable fail");
        return -1;
    }

    k_sem_init(&g_esp32_hci_send.send_sem, 1, 1);
    HCI_DRV_D("k sem init done");

    bt_hci_driver_register(&drv);
    HCI_DRV_D("bt_hci_driver_register done");

    memset(g_esp32_hci_send.cmd_buf, 0, sizeof(g_esp32_hci_send.cmd_buf));
    g_esp32_hci_send.buf_len = 0;
    return 0;
}
