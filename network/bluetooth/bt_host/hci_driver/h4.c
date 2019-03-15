#include <errno.h>
#include <stddef.h>

#include <zephyr.h>

#include <misc/util.h>
#include <misc/byteorder.h>
#include <string.h>

#ifndef BT_DBG_ENABLED
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_HCI_DRIVER)
#endif
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_driver.h>
#include <common/log.h>

#include "aos/hal/uart.h"

#if defined(CONFIG_BLUETOOTH_NRF51_PM)
#include "../nrf51_pm.h"
#endif

#define H4_NONE 0x00
#define H4_CMD  0x01
#define H4_ACL  0x02
#define H4_SCO  0x03
#define H4_EVT  0x04


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
static struct {
    uint8_t type;
    struct net_buf *buf;
    struct k_fifo   fifo;
} tx;

typedef uint32_t uart_port_t;

#ifndef CONFIG_BLE_HCI_H4_UART_PORT
#error "No uart port specified for BLE HCI H4"
#endif
static uart_dev_t h4_dev = {.port = CONFIG_BLE_HCI_H4_UART_PORT};

static inline void h4_get_type(void)
{
    uint32_t recv_siz = 0;
    int32_t ret;

    /* Get packet type */
    while (1) {
        ret = hal_uart_recv_II(&h4_dev, &rx.type, 1, &recv_siz, -1);
        if (ret != 0 || recv_siz == 0) continue;
        else break;
    }
    BT_DBG("%s %d recv_size: %d, ret: %d", __FILE__, __LINE__, recv_siz, ret);

    switch (rx.type) {
        case H4_EVT:
            rx.remaining = sizeof(rx.evt);
            rx.hdr_len = rx.remaining;
            break;

        case H4_ACL:
            rx.remaining = sizeof(rx.acl);
            rx.hdr_len = rx.remaining;
            break;

        default:
            BT_ERR("Unknown H:4 type 0x%02x", rx.type);
            rx.type = H4_NONE;
    }
}

static inline void get_acl_hdr(void)
{
    struct bt_hci_acl_hdr *hdr = &rx.acl;
    int to_read = sizeof(*hdr) - rx.remaining;
    uint32_t recv_siz = 0;
    int32_t ret;

    while (1) {
        ret = hal_uart_recv_II(&h4_dev, (uint8_t *)hdr + to_read, rx.remaining, &recv_siz, -1);
        if (ret != 0 || recv_siz == 0) continue;
        else break;
    }
    rx.remaining -= recv_siz;

    if (!rx.remaining) {
        rx.remaining = sys_le16_to_cpu(hdr->len);
        BT_DBG("Got ACL header. Payload %u bytes", rx.remaining);
        rx.have_hdr = true;
    }
}

static inline void get_evt_hdr(void)
{
    struct bt_hci_evt_hdr *hdr = &rx.evt;
    int to_read = rx.hdr_len - rx.remaining;
    uint32_t recv_siz = 0;
    int32_t ret;

    while (1) {
        ret = hal_uart_recv_II(&h4_dev, (uint8_t *)hdr + to_read, rx.remaining, &recv_siz, -1);
        if (ret != 0 || recv_siz == 0) continue;
        else break;
    }
    rx.remaining -= recv_siz;

    if (rx.hdr_len == sizeof(*hdr) && rx.remaining < sizeof(*hdr)) {
        switch (rx.evt.evt) {
            case BT_HCI_EVT_LE_META_EVENT:
                rx.remaining++;
                rx.hdr_len++;
                break;
#if defined(CONFIG_BT_BREDR)

            case BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI:
            case BT_HCI_EVT_EXTENDED_INQUIRY_RESULT:
                rx.discardable = true;
                break;
#endif
        }
    }

    if (!rx.remaining) {
        if (rx.evt.evt == BT_HCI_EVT_LE_META_EVENT &&
            rx.hdr[sizeof(*hdr)] == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
            BT_DBG("Marking adv report as discardable");
            rx.discardable = true;
        }

        rx.remaining = hdr->len - (rx.hdr_len - sizeof(*hdr));
        BT_DBG("Got event header. Payload %u bytes", hdr->len);
        rx.have_hdr = true;
    }
}


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

static struct net_buf *get_rx(int timeout)
{
    BT_DBG("type 0x%02x, evt 0x%02x", rx.type, rx.evt.evt);

    if (rx.type == H4_EVT && (rx.evt.evt == BT_HCI_EVT_CMD_COMPLETE ||
                              rx.evt.evt == BT_HCI_EVT_CMD_STATUS)) {
        return bt_buf_get_cmd_complete(timeout);
    }

    return bt_buf_get_rx(BT_BUF_EVT, timeout);
}

static size_t h4_discard(uart_dev_t *uart, size_t len)
{
    uint8_t buf[33];
    uint32_t recv_siz;

    return hal_uart_recv_II(uart, buf, min(len, sizeof(buf)), &recv_siz, 0);
}

static inline void read_payload(void)
{
    struct net_buf *buf;
    bool prio;
    int read;
    int32_t ret;

    if (!rx.buf) {
        rx.buf = get_rx(K_NO_WAIT);

        if (!rx.buf) {
            if (rx.discardable) {
                BT_WARN("Discarding event 0x%02x", rx.evt.evt);
                rx.discard = rx.remaining;
                reset_rx();
                return;
            }

            BT_WARN("Failed to allocate, deferring to rx_thread");
            return;
        }

        BT_DBG("Allocated rx.buf %p", rx.buf);

        if (rx.remaining > net_buf_tailroom(rx.buf)) {
            BT_ERR("Not enough space in buffer");
            rx.discard = rx.remaining;
            reset_rx();
            return;
        }

        copy_hdr(rx.buf);
    }

    while (1) {
        ret = hal_uart_recv_II(&h4_dev, net_buf_tail(rx.buf), rx.remaining, (uint32_t *)&read, -1);
        if (ret != 0 || read == 0) continue;
        else break;
    }
    net_buf_add(rx.buf, read);
    rx.remaining -= read;

    BT_DBG("got %d bytes, remaining %u", read, rx.remaining);
    BT_DBG("Payload (len %u): %s", rx.buf->len,
           bt_hex(rx.buf->data, rx.buf->len));

    if (rx.remaining) {
        return;
    }

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
        BT_DBG("Putting buf %p to rx fifo", buf);
        net_buf_put(&rx.fifo, buf);
    }
}

static inline void read_header(void)
{
    switch (rx.type) {
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
            return;
    }

    if (rx.have_hdr && rx.buf) {
        if (rx.remaining > net_buf_tailroom(rx.buf)) {
            BT_ERR("Not enough space in buffer");
            rx.discard = rx.remaining;
            reset_rx();
        } else {
            copy_hdr(rx.buf);
        }
    }
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

        BT_DBG("write type %d", tx.type);
        ret = hal_uart_send(&h4_dev, (char *)&tx.type, 1, -1);

        if (ret != 0) {
            BT_WARN("Unable to send H:4 type");
            tx.type = H4_NONE;
            return;
        }
    }

    BT_DBG("write data %s", bt_hex(tx.buf->data, tx.buf->len));
    bytes = tx.buf->len;
    ret = hal_uart_send(&h4_dev, (char *)(tx.buf->data), bytes, -1);
    if (ret != 0) {
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

static inline void process_rx(void)
{
    BT_DBG("remaining %u discard %u have_hdr %u rx.buf %p len %u",
           rx.remaining, rx.discard, rx.have_hdr, rx.buf,
           rx.buf ? rx.buf->len : 0);

    if (rx.discard) {
        rx.discard -= h4_discard(&h4_dev, rx.discard);
        return;
    }

    if (rx.have_hdr) {
        read_payload();
    } else {
        read_header();
    }
}

static void rx_thread(void *p1, void *p2, void *p3)
{
    struct net_buf *buf;

    ARG_UNUSED(p1);

    BT_DBG("started");

    while (1) {
        BT_DBG("rx.buf %p", rx.buf);
        process_rx();

        /* We can only do the allocation if we know the initial
         * header, since Command Complete/Status events must use the
         * original command buffer (if available).
         */
        if (rx.have_hdr && !rx.buf) {
            rx.buf = get_rx(K_FOREVER);
            BT_DBG("Got rx.buf %p", rx.buf);

            if (rx.remaining > net_buf_tailroom(rx.buf)) {
                BT_ERR("Not enough space in buffer");
                rx.discard = rx.remaining;
                reset_rx();
            } else {
                copy_hdr(rx.buf);
            }
        }

        buf = net_buf_get(&rx.fifo, K_NO_WAIT);

        while (buf) {

            BT_DBG("Calling bt_recv(%p)", buf);
            bt_recv(buf);

            /* Give other threads a chance to run if the ISR
             * is receiving data so fast that rx.fifo never
             * or very rarely goes empty.
             */
            k_yield();

            buf = net_buf_get(&rx.fifo, K_NO_WAIT);
        };

        k_yield();
    }
}

static int h4_send(struct net_buf *buf)
{
    BT_DBG("buf %p type %u len %u", buf, bt_buf_get_type(buf), buf->len);

    net_buf_put(&tx.fifo, buf);
    process_tx();
    return 0;
}

static int h4_open(void)
{
    static struct k_thread h4_rx_thread;
    static BT_STACK_NOINIT(rx_thread_stack, CONFIG_BT_HCI_RX_STACK_SIZE);

    BT_INFO("");

    hal_uart_init(&h4_dev);

    h4_discard(&h4_dev, 32);

    k_thread_create(&h4_rx_thread, rx_thread_stack, sizeof(rx_thread_stack), rx_thread,
                   NULL, NULL, NULL, 46, 0, K_NO_WAIT);

    k_fifo_init(&tx.fifo);
    k_fifo_init(&rx.fifo);

    return 0;
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

void hci_h4_set_uart_config(uart_config_t *c)
{
    memcpy((void *)(&(h4_dev.config)), (void *)c, sizeof(uart_config_t));
}
