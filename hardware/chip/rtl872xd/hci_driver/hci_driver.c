#include <errno.h>
#include <stddef.h>

#include <zephyr.h>

#include <misc/util.h>
#include <misc/byteorder.h>
#include <string.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HCI_DRIVER)
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_driver.h>
#include <common/log.h>

#include "aos/hal/uart.h"

#include "hci_if.h"

#if 0
#ifdef BT_DBG
#undef BT_DBG
#define BT_DBG(f, ...) printf(f"\n", ##__VA_ARGS__)
#endif

#ifdef BT_INFO
#undef BT_INFO
#define BT_INFO(f, ...) printf(f"\n", ##__VA_ARGS__)
#endif

#ifdef BT_WARN
#undef BT_WARN
#define BT_WARN(f, ...) printf(f"\n", ##__VA_ARGS__)
#endif

#ifdef BT_ERR
#undef BT_ERR
#define BT_ERR(f, ...) printf(f"\n", ##__VA_ARGS__)
#endif
#endif

#define H4_NONE 0x00
#define H4_CMD  0x01
#define H4_ACL  0x02
#define H4_SCO  0x03
#define H4_EVT  0x04

#define HCI_SEND_BUF_SIZE BT_BUF_RX_SIZE

static struct {
    uint8_t type;
    struct net_buf *buf;
    struct k_fifo   fifo;
    struct k_sem    send_sem;
    uint8_t send_buf[HCI_SEND_BUF_SIZE];
} tx;

static struct {
    struct net_buf *buf;
    struct k_fifo   fifo;

    uint16_t remaining;
    uint16_t discard;

    bool     have_hdr;
    bool     discardable;

    uint8_t  hdr_len;

    uint8_t  type;
    union {
        struct bt_hci_evt_hdr evt;
        struct bt_hci_acl_hdr acl;
        uint8_t hdr[4];
    };
} rx;

bool hci_if_open_flag;
struct k_sem hci_if_open_sem;

static inline void copy_hdr(struct net_buf *buf)
{
    net_buf_add_mem(buf, rx.hdr, rx.hdr_len);
}

static void reset_rx(void)
{
    rx.type = H4_NONE;
    rx.remaining = 0;
    rx.have_hdr = false;
    rx.hdr_len = 0;
    rx.discardable = false;
}

static int get_evt_hdr(uint8_t *data, uint16_t len)
{
    struct bt_hci_evt_hdr *hdr = &rx.evt;

    rx.hdr_len = sizeof(struct bt_hci_evt_hdr);
    memcpy((uint8_t *)hdr, data, rx.hdr_len);

    if (rx.evt.evt == BT_HCI_EVT_LE_META_EVENT) {
        memcpy((uint8_t *)hdr + rx.hdr_len, data + rx.hdr_len, 1);
        rx.hdr_len++;
    }

    if (rx.evt.evt == BT_HCI_EVT_LE_META_EVENT &&
        rx.hdr[sizeof(*hdr)] == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
        BT_DBG("Marking adv report as discardable");
        rx.discardable = true;
    }

    rx.remaining = hdr->len - (rx.hdr_len - sizeof(*hdr));
    BT_DBG("Got event header. Payload %u bytes", hdr->len);
    rx.have_hdr = true;

    return rx.hdr_len;
}

static int get_acl_hdr(uint8_t *data, uint16_t len)
{
    struct bt_hci_acl_hdr *hdr = &rx.acl;

    rx.hdr_len = sizeof(struct bt_hci_acl_hdr);
    memcpy((uint8_t *)hdr, data, rx.hdr_len);

    rx.remaining = sys_le16_to_cpu(hdr->len);
    BT_DBG("Got ACL header. Payload %u bytes", rx.remaining);
    rx.have_hdr = true;

    return rx.hdr_len;
}

static struct net_buf *get_rx(int timeout)
{
    BT_DBG("type 0x%02x, evt 0x%02x", rx.type, rx.evt.evt);

    if (rx.type == H4_EVT && (rx.evt.evt == BT_HCI_EVT_CMD_COMPLETE ||
                              rx.evt.evt == BT_HCI_EVT_CMD_STATUS)) {
        return bt_buf_get_cmd_complete(timeout);
    }

    return bt_buf_get_rx(BT_BUF_EVT, timeout);
}

static int h4_receive(uint8_t *data, uint16_t len)
{
    uint8_t *pdata = data;
    uint16_t length = len;
    bool prio;
    struct net_buf *buf;

    BT_DBG("%s", bt_hex(data, len));

    if (NULL == pdata) {
        BT_ERR("Receive no data");
        return -EINVAL;
    }

    rx.type = *pdata;
    pdata++;
    length--;
    switch (rx.type) {
        case H4_EVT:
            pdata += get_evt_hdr(pdata, length);
            break;

        case H4_ACL:
            pdata += get_acl_hdr(pdata, length);
            break;

        default:
            BT_ERR("Unknown H:4 type 0x%02x", rx.type);
            rx.type = H4_NONE;
            return -EINVAL;
    }

    if (!rx.buf) {
        rx.buf = get_rx(K_NO_WAIT);

        if (!rx.buf) {
            if (rx.discardable) {
                BT_WARN("Discarding event 0x%02x", rx.evt.evt);
                rx.discard = rx.remaining;
                reset_rx();
                return 0;
            }

            BT_WARN("Failed to allocate, deferring to rx_thread");
            return -ENOMEM;
        }

        copy_hdr(rx.buf);
    }

    net_buf_add_mem(rx.buf, pdata, rx.remaining);

    prio = (rx.type == H4_EVT && bt_hci_evt_is_prio(rx.evt.evt));

    buf = rx.buf;
    rx.buf = NULL;

    if (rx.type == H4_EVT) {
        bt_buf_set_type(buf, BT_BUF_EVT);
    } else {
        bt_buf_set_type(buf, BT_BUF_ACL_IN);
    }

    reset_rx();

    if (prio) {
        BT_DBG("Calling bt_recv_prio(%p)", buf);
        bt_recv_prio(buf);
    } else {
        BT_DBG("Calling bt_recv(%p)", buf);
        bt_recv(buf);
    }

    return 0;
}

static bool hci_tp_tx_callback(void)
{
    BT_DBG("hci send complete callback");
    memset(tx.send_buf, 0, HCI_SEND_BUF_SIZE);
    k_sem_give(&tx.send_sem);
}

static inline void process_tx(void)
{
    int bytes;
    int32_t ret;

    if (!tx.buf) {
        tx.buf = net_buf_get(&tx.fifo, K_NO_WAIT);

        if (!tx.buf) {
            BT_ERR("TX interrupt but no pending buffer!");
            return;
        }
    }

    if (!tx.type) {
        switch (bt_buf_get_type(tx.buf)) {
            case BT_BUF_ACL_OUT:
                tx.type = H4_ACL;
                break;

            case BT_BUF_CMD:
                tx.type = H4_CMD;
                break;

            default:
                BT_ERR("Unknown buffer type");
                goto done;
        }
    }

    BT_DBG("write type %d, data %s", tx.type, bt_hex(tx.buf->data, tx.buf->len));
    bytes = tx.buf->len;
    memcpy(tx.send_buf, &tx.type, 1);
    memcpy(tx.send_buf + 1, tx.buf->data, bytes);

    ret = hci_tp_send(tx.send_buf, bytes + 1, hci_tp_tx_callback);
    if (ret != true) {
        BT_ERR("Failed to send data");
    }

    net_buf_pull(tx.buf, bytes);

    if (tx.buf->len) {
        return;
    }

done:
    tx.type = H4_NONE;
    net_buf_unref(tx.buf);
    tx.buf = net_buf_get(&tx.fifo, K_NO_WAIT);
}

static int h4_send(struct net_buf *buf)
{
    int ret;

    ret = k_sem_take(&tx.send_sem, K_FOREVER);
    if (ret == 0) {
        BT_DBG("buf %p type %u len %u", buf, bt_buf_get_type(buf), buf->len);

        net_buf_put(&tx.fifo, buf);
        process_tx();
    } else {
        BT_ERR("Take tx.send_sem error! ret = 0x%x", ret);
    }
    return ret;
}

#if 1
static bool hci_if_callback(T_HCI_IF_EVT evt, bool status, uint8_t *p_buf, uint32_t len)
{
    if (evt == HCI_IF_EVT_OPENED) {
        if (status == true) {
            BT_DBG("hci_if_open success");
        } else {
            BT_DBG("hci_if_open fail");
        }

        hci_if_open_flag = status;
        k_sem_give(&hci_if_open_sem);
    } else if (evt == HCI_IF_EVT_DATA_IND) {
        BT_DBG("hci indicate to host");
        h4_receive(p_buf, len);
    } else {
        BT_ERR("Other event enter hci_if_callback! evt = 0x%x, status = 0x%x", evt, status);
    }
}
#endif

static int h4_open(void)
{
    int ret;
    hci_if_open_flag = false;

    ret = k_sem_init(&hci_if_open_sem, 0, 1);
    if (ret != 0) {
        BT_ERR("Init hci_if_open_sem error! ret = 0x%x", ret);
        return ret;
    }

    hci_if_open(hci_if_callback);

    ret = k_sem_take(&hci_if_open_sem, K_FOREVER);
    if (ret == 0) {
        if (hci_if_open_flag == true) {
            memset(tx.send_buf, 0, HCI_SEND_BUF_SIZE);
            k_fifo_init(&tx.fifo);
            ret = k_sem_init(&tx.send_sem, 1, 1);
            if (ret != 0) {
                BT_ERR("Init tx.send_sem error! ret = 0x%x", ret);
            }
        } else {
            ret = 1;
        }
    } else {
        BT_ERR("Take hci_if_open_sem error! ret = 0x%x", ret);
    }

    k_sem_delete(&hci_if_open_sem);
    return ret;
}

static struct bt_hci_driver drv = {
    .name       = "H:4",
    .bus        = BT_HCI_DRIVER_BUS_UART,
    .open       = h4_open,
    .send       = h4_send,
};

int hci_driver_init()
{
    BT_INFO("");
    bt_hci_driver_register(&drv);
    return 0;
}

