/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ble_os.h>
#include <bt_errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <api/mesh.h>
#include <api/mesh/proxy.h>

#include <bluetooth/uuid.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROV)
#include "common/log.h"

#include "host/ecc.h"
#include "host/testing.h"

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "prov.h"
#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
#include "mesh_event_port.h"
#endif

#ifdef CONFIG_GENIE_MESH_ENABLE
#include "genie_crypto.h"
#endif

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_prov.h"
#endif

#ifdef MESH_DEBUG_PROV
#ifdef CONFIG_GENIE_MESH_ENABLE
#define PROV_TAG "\t[ALI_PROV]"
#else
#define PROV_TAG "\t[SIG_PROV]"
#endif
#define PROV_D(f, ...) LOGI(PROV_TAG, "\033[0;33m %s " f "\033[0m\n", __func__, ##__VA_ARGS__)
#else
#define PROV_D(f, ...)
#endif

/* 3 transmissions, 20ms interval */
#define PROV_XMIT BT_MESH_TRANSMIT(2, 20)

#define AUTH_METHOD_NO_OOB 0x00
#define AUTH_METHOD_STATIC 0x01
#define AUTH_METHOD_OUTPUT 0x02
#define AUTH_METHOD_INPUT 0x03

#define OUTPUT_OOB_BLINK 0x00
#define OUTPUT_OOB_BEEP 0x01
#define OUTPUT_OOB_VIBRATE 0x02
#define OUTPUT_OOB_NUMBER 0x03
#define OUTPUT_OOB_STRING 0x04

#define INPUT_OOB_PUSH 0x00
#define INPUT_OOB_TWIST 0x01
#define INPUT_OOB_NUMBER 0x02
#define INPUT_OOB_STRING 0x03

#define PROV_ERR_NONE 0x00
#define PROV_ERR_NVAL_PDU 0x01
#define PROV_ERR_NVAL_FMT 0x02
#define PROV_ERR_UNEXP_PDU 0x03
#define PROV_ERR_CFM_FAILED 0x04
#define PROV_ERR_RESOURCES 0x05
#define PROV_ERR_DECRYPT 0x06
#define PROV_ERR_UNEXP_ERR 0x07
#define PROV_ERR_ADDR 0x08

#define PROV_INVITE 0x00
#define PROV_CAPABILITIES 0x01
#define PROV_START 0x02
#define PROV_PUB_KEY 0x03
#define PROV_INPUT_COMPLETE 0x04
#define PROV_CONFIRM 0x05
#define PROV_RANDOM 0x06
#define PROV_DATA 0x07
#define PROV_COMPLETE 0x08
#define PROV_FAILED 0x09

#define PROV_ALG_P256 0x00

#define GPCF(gpc) (gpc & 0x03)
#define GPC_START(last_seg) (((last_seg) << 2) | 0x00)
#define GPC_ACK 0x01
#define GPC_CONT(seg_id) (((seg_id) << 2) | 0x02)
#define GPC_CTL(op) (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX 20
#define CONT_PAYLOAD_MAX 23

#define START_LAST_SEG(gpc) (gpc >> 2)
#define CONT_SEG_INDEX(gpc) (gpc >> 2)

#define BEARER_CTL(gpc) (gpc >> 2)
#define LINK_OPEN 0x00
#define LINK_ACK 0x01
#define LINK_CLOSE 0x02

#define CLOSE_REASON_SUCCESS 0x00
#define CLOSE_REASON_TIMEOUT 0x01
#define CLOSE_REASON_FAILED 0x02

#define XACT_SEG_DATA(_seg) (&plink.rx.buf->data[20 + ((_seg - 1) * 23)])
#define XACT_SEG_RECV(_seg) (plink.rx.seg &= ~(1 << (_seg)))

#define XACT_NVAL 0xFF

enum {
    REMOTE_PUB_KEY, /* Remote key has been received */
    LOCAL_PUB_KEY, /* Local public key is available */
    LINK_ACTIVE, /* Link has been opened */
    HAVE_DHKEY, /* DHKey has been calcualted */
    SEND_CONFIRM, /* Waiting to send Confirm value */
    WAIT_NUMBER, /* Waiting for number input from user */
    WAIT_STRING, /* Waiting for string input from user */

    NUM_FLAGS,
};

struct prov_link {
    ATOMIC_DEFINE(flags, NUM_FLAGS);
#if defined(CONFIG_BT_MESH_PB_GATT)
    struct bt_conn *conn; /* GATT connection */
#endif
    u8_t dhkey[32]; /* Calculated DHKey */
    u8_t expect; /* Next expected PDU */

    u8_t oob_method;
    u8_t oob_action;
    u8_t oob_size;

    u8_t conf[16]; /* Remote Confirmation */
    u8_t rand[16]; /* Local Random */
    u8_t auth[16]; /* Authentication Value */

    u8_t conf_salt[16]; /* ConfirmationSalt */
    u8_t conf_key[16]; /* ConfirmationKey */
    u8_t conf_inputs[145]; /* ConfirmationInputs */
    u8_t prov_salt[16]; /* Provisioning Salt */
#ifdef CONFIG_BT_MESH_PROVISIONER
    u8_t remote_dev_key[16];
    u16_t unicast_addr;
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
    bt_u32_t id; /* Link ID */

    struct {
        u8_t id; /* Transaction ID */
        u8_t prev_id; /* Previous Transaction ID */
        u8_t seg; /* Bit-field of unreceived segments */
        u8_t last_seg; /* Last segment (to check length) */
        u8_t fcs; /* Expected FCS value */
        struct net_buf_simple *buf;
    } rx;

    struct {
        /* Start timestamp of the transaction */
        s64_t start;

        /* Transaction id*/
        u8_t id;

        /* Pending outgoing buffer(s) */
        struct net_buf *buf[3];

        /* Retransmit timer */
        struct k_delayed_work retransmit;
    } tx;
#endif
};

struct prov_rx {
    bt_u32_t link_id;
    u8_t xact_id;
    u8_t gpc;
};

#define RETRANSMIT_TIMEOUT K_MSEC(500)
#define BUF_TIMEOUT K_MSEC(400)
#define TRANSACTION_TIMEOUT K_SECONDS(30)
#define PROV_TIMEOUT K_SECONDS(60)

#if defined(CONFIG_BT_MESH_PB_GATT)
#define PROV_BUF_HEADROOM 5
#else
#define PROV_BUF_HEADROOM 0
NET_BUF_SIMPLE_DEFINE_STATIC(rx_buf, 65);
#endif

#define PROV_BUF(name, len) NET_BUF_SIMPLE_DEFINE(name, PROV_BUF_HEADROOM + len)

static struct prov_link plink;

static const struct bt_mesh_prov *prov;

static u8_t prov_method = 0;

#ifndef CONFIG_BT_MESH_PROV_TIMEOUT
#define CONFIG_BT_MESH_PROV_TIMEOUT 1
#endif

#if CONFIG_BT_MESH_PROV_TIMEOUT
static k_timer_t g_prov_timeout;
#endif

static void close_link(u8_t err, u8_t reason);

#ifdef CONFIG_MESH_GENIE_APP
__attribute__((weak)) int genie_set_static_auth(uint8_t *link_auth, uint8_t *random)
{
    (void)link_auth;
    (void)random;
    return 0;
}
#endif

#ifdef CONFIG_BT_BQB
static void reset_state_without_linkclose()
{
    /* Disable Attention Timer if it was set */
    if (plink.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }
#if defined(CONFIG_BT_MESH_PB_ADV)
    /* Clear everything except the retransmit delayed work config */
    struct bt_conn *conn = plink.conn;
    (void)memset(&plink, 0, offsetof(struct prov_link, tx.retransmit));
    plink.rx.prev_id = XACT_NVAL;
    plink.conn = conn;

#if defined(CONFIG_BT_MESH_PB_GATT)
    plink.rx.buf = bt_mesh_proxy_get_buf();
#else
    net_buf_simple_reset(&rx_buf);
    plink.rx.buf = &rx_buf;
#endif /* PB_GATT */

#else
    struct bt_conn *conn = plink.conn;
    (void)memset(&plink, 0, sizeof(plink));
    plink.conn = conn;
#endif /* PB_ADV */
}
#endif
extern int32_t csi_kernel_delay_ms(uint32_t ms);
static void reset_state(void)
{
    prov_method = 0;
#ifdef CONFIG_BT_BQB
    int err;

    /* wait dhkey gen complete */
    csi_kernel_delay_ms(5000);
    extern int bt_pub_key_regen();
    err = bt_pub_key_regen();
    if (err) {
        BT_ERR("Failed to generate public key (%d)", err);
        return;
    }

#endif

    /* Disable Attention Timer if it was set */
    if (plink.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }

#if defined(CONFIG_BT_MESH_PB_GATT)
    if (plink.conn) {
        bt_conn_unref(plink.conn);
    }
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
    /* Clear everything except the retransmit delayed work config */
    (void)memset(&plink, 0, offsetof(struct prov_link, tx.retransmit));
    plink.rx.prev_id = XACT_NVAL;

#if defined(CONFIG_BT_MESH_PB_GATT)
    plink.rx.buf = bt_mesh_proxy_get_buf();
#else
    net_buf_simple_reset(&rx_buf);
    plink.rx.buf = &rx_buf;
#endif /* PB_GATT */

#else
    (void)memset(&plink, 0, sizeof(plink));
#endif /* PB_ADV */
}

#if defined(CONFIG_BT_MESH_PB_ADV)
static void buf_sent(int err, void *user_data)
{
    if (!plink.tx.buf[0]) {
        return;
    }
    k_delayed_work_submit(&plink.tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void free_segments(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(plink.tx.buf); i++) {
        struct net_buf *buf = plink.tx.buf[i];

        if (!buf) {
            break;
        }
        plink.tx.buf[i] = NULL;
        /* Mark as canceled */
        BT_MESH_ADV(buf)->busy = 0;
        net_buf_unref(buf);
    }
}

static void prov_clear_tx(void)
{
    BT_DBG("");

    k_delayed_work_cancel(&plink.tx.retransmit);

    free_segments();
}

void reset_link(void)
{
    prov_clear_tx();

    if (prov->link_close) {
        prov->link_close(BT_MESH_PROV_ADV);
    }

    reset_state();
}

static struct net_buf *adv_buf_create(void)
{
    struct net_buf *buf;
    buf = bt_mesh_adv_create(BT_MESH_ADV_PROV, PROV_XMIT, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of provisioning buffers");
        return NULL;
    }

    return buf;
}

static atomic_t pending_ack = 0x00;

static void ack_complete(int err, void *user_data)
{
    struct net_buf *buf = (struct net_buf *)user_data;
    uint32_t link_id = net_buf_pull_be32(buf);
    uint8_t ack_index = net_buf_pull_u8(buf);
    atomic_clear_bit(&pending_ack, ack_index);
    BT_DBG("linkid: %d xact index %u send complete", link_id, ack_index);
    net_buf_unref(buf);
    (void)link_id;
}

static void gen_prov_ack_send(u8_t xact_id)
{
    static const struct bt_mesh_send_cb cb = {
        .end = ack_complete,
    };
    const struct bt_mesh_send_cb *complete = &cb;
    struct net_buf *buf;

    BT_DBG("xact_id %u %02x", xact_id, pending_ack);

    if (atomic_test_and_set_bit(&pending_ack, xact_id)) {
        BT_DBG("The xact_id aready exit int adv pool");
        return;
    }
    buf = adv_buf_create();
    if (!buf) {
        return;
    }
    net_buf_add_be32(buf, plink.id);
    net_buf_add_u8(buf, xact_id);
    net_buf_add_u8(buf, GPC_ACK);
    bt_mesh_adv_send(buf, complete, buf);
}

static void send_reliable(void)
{
    int i;

    plink.tx.start = k_uptime_get();

    for (i = 0; i < ARRAY_SIZE(plink.tx.buf); i++) {
        struct net_buf *buf = plink.tx.buf[i];

        if (!buf) {
            break;
        }

        if (i + 1 < ARRAY_SIZE(plink.tx.buf) && plink.tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
        }
    }
}

static int bearer_ctl_send(u8_t op, void *data, u8_t data_len)
{
    struct net_buf *buf;

    BT_DBG("op 0x%02x data_len %u", op, data_len);

    prov_clear_tx();

    buf = adv_buf_create();
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_be32(buf, plink.id);
    /* Transaction ID, always 0 for Bearer messages */
    net_buf_add_u8(buf, 0x00);
    net_buf_add_u8(buf, GPC_CTL(op));
    net_buf_add_mem(buf, data, data_len);

    plink.tx.buf[0] = buf;
    send_reliable();

    return 0;
}

static u8_t last_seg(u8_t len)
{
    if (len <= START_PAYLOAD_MAX) {
        return 0;
    }

    len -= START_PAYLOAD_MAX;

    return 1 + (len / CONT_PAYLOAD_MAX);
}

static inline u8_t next_transaction_id(void)
{
    if (plink.tx.id != 0 && plink.tx.id != 0xFF) {
        return ++plink.tx.id;
    }

    plink.tx.id = 0x80;
    return plink.tx.id;
}

static int prov_send_adv(struct net_buf_simple *msg)
{
    struct net_buf *start, *buf;
    u8_t seg_len, seg_id;
    u8_t xact_id;

    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));
    // PROV_D("len %u: %s", msg->len, bt_hex(msg->data, msg->len));
    PROV_D("len %u", msg->len);
    prov_clear_tx();

    start = adv_buf_create();
    if (!start) {
        return -ENOBUFS;
    }

    xact_id = next_transaction_id();
    net_buf_add_be32(start, plink.id);
    net_buf_add_u8(start, xact_id);

    net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
    net_buf_add_be16(start, msg->len);
    net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

    plink.tx.buf[0] = start;

    seg_len = MIN(msg->len, START_PAYLOAD_MAX);
    BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
    net_buf_add_mem(start, msg->data, seg_len);
    net_buf_simple_pull(msg, seg_len);

    buf = start;
    for (seg_id = 1; msg->len > 0; seg_id++) {
        if (seg_id >= ARRAY_SIZE(plink.tx.buf)) {
            BT_ERR("Too big message");
            free_segments();
            return -E2BIG;
        }

        buf = adv_buf_create();
        if (!buf) {
            free_segments();
            return -ENOBUFS;
        }

        plink.tx.buf[seg_id] = buf;

        seg_len = MIN(msg->len, CONT_PAYLOAD_MAX);

        BT_DBG("seg_id %u len %u: %s", seg_id, seg_len, bt_hex(msg->data, seg_len));

        net_buf_add_be32(buf, plink.id);
        net_buf_add_u8(buf, xact_id);
        net_buf_add_u8(buf, GPC_CONT(seg_id));
        net_buf_add_mem(buf, msg->data, seg_len);
        net_buf_simple_pull(msg, seg_len);
    }

    send_reliable();

    return 0;
}

#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
static int prov_send_gatt(struct net_buf_simple *msg)
{
    if (!plink.conn) {
        return -ENOTCONN;
    }

    return bt_mesh_proxy_send(plink.conn, BT_MESH_PROXY_PROV, msg);
}
#endif /* CONFIG_BT_MESH_PB_GATT */

static inline int prov_send(struct net_buf_simple *buf)
{
#if defined(CONFIG_BT_MESH_PB_GATT)
    if (plink.conn) {
        return prov_send_gatt(buf);
    }
#endif
#if defined(CONFIG_BT_MESH_PB_ADV)
    return prov_send_adv(buf);
#else
    return 0;
#endif
}

static void prov_buf_init(struct net_buf_simple *buf, u8_t type)
{
    net_buf_simple_reserve(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

static void prov_send_fail_msg(u8_t err)
{
    PROV_BUF(buf, 2);

    prov_buf_init(&buf, PROV_FAILED);
    net_buf_simple_add_u8(&buf, err);
    prov_send(&buf);

#ifdef CONFIG_BT_BQB
    reset_state_without_linkclose();
#endif
}

static void prov_invite(const u8_t *data)
{
    PROV_BUF(buf, 12);

    BT_DBG("Attention Duration: %u seconds", data[0]);
    PROV_D(", 1--->2");

    if (data[0]) {
        bt_mesh_attention(NULL, data[0]);
    }

    plink.conf_inputs[0] = data[0];

    prov_buf_init(&buf, PROV_CAPABILITIES);

    /* Number of Elements supported */
    net_buf_simple_add_u8(&buf, bt_mesh_elem_count());

    /* Supported algorithms - FIPS P-256 Eliptic Curve */
    net_buf_simple_add_be16(&buf, BIT(PROV_ALG_P256));

    /* Public Key Type */
    net_buf_simple_add_u8(&buf, 0x00);

    /* Static OOB Type */
    net_buf_simple_add_u8(&buf, prov->static_val && prov->static_val_len ? BIT(0) : 0x00);

    /* Output OOB Size */
    net_buf_simple_add_u8(&buf, prov->output_size);

    /* Output OOB Action */
    net_buf_simple_add_be16(&buf, prov->output_actions);

    /* Input OOB Size */
    net_buf_simple_add_u8(&buf, prov->input_size);

    /* Input OOB Action */
    net_buf_simple_add_be16(&buf, prov->input_actions);

    memcpy(&plink.conf_inputs[1], &buf.data[1], 11);

    if (prov_send(&buf)) {
        BT_ERR("Failed to send capabilities");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    plink.expect = PROV_START;
#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
#if defined(CONFIG_BT_MESH_PB_GATT)
    mesh_model_evt_cb event_cb = bt_mesh_event_get_cb_func();
    if (plink.conn && event_cb) {
        event_cb(BT_MESH_MODEL_EVT_PROV_START, NULL);
    }
#endif
#endif
}

static void prov_capabilities(const u8_t *data)
{
    BT_DBG("Elements: %u", data[0]);

    BT_DBG("Algorithms:        %u", sys_get_be16(&data[1]));

    BT_DBG("Public Key Type:   0x%02x", data[3]);
    BT_DBG("Static OOB Type:   0x%02x", data[4]);
    BT_DBG("Output OOB Size:   %u", data[5]);

    BT_DBG("Output OOB Action: 0x%04x", sys_get_be16(&data[6]));

    BT_DBG("Input OOB Size:    %u", data[8]);

    BT_DBG("Input OOB Action:  0x%04x", sys_get_be16(&data[9]));
}

static bt_mesh_output_action_t output_action(u8_t action)
{
    switch (action) {
    case OUTPUT_OOB_BLINK:
        return BT_MESH_BLINK;
    case OUTPUT_OOB_BEEP:
        return BT_MESH_BEEP;
    case OUTPUT_OOB_VIBRATE:
        return BT_MESH_VIBRATE;
    case OUTPUT_OOB_NUMBER:
        return BT_MESH_DISPLAY_NUMBER;
    case OUTPUT_OOB_STRING:
        return BT_MESH_DISPLAY_STRING;
    default:
        return BT_MESH_NO_OUTPUT;
    }
}

static bt_mesh_input_action_t input_action(u8_t action)
{
    switch (action) {
    case INPUT_OOB_PUSH:
        return BT_MESH_PUSH;
    case INPUT_OOB_TWIST:
        return BT_MESH_TWIST;
    case INPUT_OOB_NUMBER:
        return BT_MESH_ENTER_NUMBER;
    case INPUT_OOB_STRING:
        return BT_MESH_ENTER_STRING;
    default:
        return BT_MESH_NO_INPUT;
    }
}

static int prov_auth(u8_t method, u8_t action, u8_t size)
{
    bt_mesh_output_action_t output;
    bt_mesh_input_action_t input;
    prov_method = method;

    switch (method) {
    case AUTH_METHOD_NO_OOB:
        if (action || size) {
            return -EINVAL;
        }

        memset(plink.auth, 0, sizeof(plink.auth));
        return 0;
    case AUTH_METHOD_STATIC:
        if (action || size) {
            return -EINVAL;
        }

        memcpy(plink.auth + 16 - prov->static_val_len, prov->static_val, prov->static_val_len);
        (void)memset(plink.auth, 0, sizeof(plink.auth) - prov->static_val_len);
        return 0;

    case AUTH_METHOD_OUTPUT:
        output = output_action(action);
        if (!output) {
            return -EINVAL;
        }

        if (!(prov->output_actions & output)) {
            return -EINVAL;
        }

        if (size > prov->output_size) {
            return -EINVAL;
        }

        if (output == BT_MESH_DISPLAY_STRING) {
            unsigned char str[9];
            u8_t i;

            bt_rand(str, size);

            /* Normalize to '0' .. '9' & 'A' .. 'Z' */
            for (i = 0; i < size; i++) {
                str[i] %= 36;
                if (str[i] < 10) {
                    str[i] += '0';
                } else {
                    str[i] += 'A' - 10;
                }
            }
            str[size] = '\0';

            memcpy(plink.auth, str, size);
            (void)memset(plink.auth + size, 0, sizeof(plink.auth) - size);

            return prov->output_string((char *)str);
        } else {
            bt_u32_t div[8] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
            bt_u32_t num;

            bt_rand(&num, sizeof(num));
            num %= div[size - 1];

            sys_put_be32(num, &plink.auth[12]);
            memset(plink.auth, 0, 12);

            return prov->output_number(output, num);
        }

    case AUTH_METHOD_INPUT:
        input = input_action(action);
        if (!input) {
            return -EINVAL;
        }

        if (!(prov->input_actions & input)) {
            return -EINVAL;
        }

        if (size > prov->input_size) {
            return -EINVAL;
        }

        if (input == BT_MESH_ENTER_STRING) {
            atomic_set_bit(plink.flags, WAIT_STRING);
        } else {
            atomic_set_bit(plink.flags, WAIT_NUMBER);
        }

        return prov->input(input, size);

    default:
        return -EINVAL;
    }
}

static void prov_start(const u8_t *data)
{
    BT_DBG("Algorithm:   0x%02x", data[0]);
    BT_DBG("Public Key:  0x%02x", data[1]);
    BT_DBG("Auth Method: 0x%02x", data[2]);
    BT_DBG("Auth Action: 0x%02x", data[3]);
    BT_DBG("Auth Size:   0x%02x", data[4]);
    PROV_D(", 2--->3");

    if (data[0] != PROV_ALG_P256) {
        BT_ERR("Unknown algorithm 0x%02x", data[0]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

#ifdef CONFIG_BT_BQB
    /* not support public key oob */
    if (data[1] != 0x00) {
        BT_ERR("Invalid public key value: 0x%02x", data[1]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }
#else
    if (data[1] > 0x01) {
        BT_ERR("Invalid public key value: 0x%02x", data[1]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }
#endif

    memcpy(&plink.conf_inputs[12], data, 5);

    plink.expect = PROV_PUB_KEY;

    if (prov_auth(data[2], data[3], data[4]) < 0) {
        BT_ERR("Invalid authentication method: 0x%02x; "
               "action: 0x%02x; size: 0x%02x",
               data[2], data[3], data[4]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
    }
}

static void send_confirm(void)
{
    PROV_BUF(cfm, 17);

    BT_DBG("ConfInputs[0]   %s", bt_hex(plink.conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(&plink.conf_inputs[64], 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(&plink.conf_inputs[128], 17));

    if (bt_mesh_prov_conf_salt(plink.conf_inputs, plink.conf_salt)) {
        BT_ERR("Unable to generate confirmation salt");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(plink.conf_salt, 16));

    if (bt_mesh_prov_conf_key(plink.dhkey, plink.conf_salt, plink.conf_key)) {
        BT_ERR("Unable to generate confirmation key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(plink.conf_key, 16));

    if (bt_rand(plink.rand, 16)) {
        BT_ERR("Unable to generate random number");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("LocalRandom: %s", bt_hex(plink.rand, 16));

    prov_buf_init(&cfm, PROV_CONFIRM);

#ifdef CONFIG_GENIE_MESH_ENABLE
    if (prov_method == AUTH_METHOD_STATIC) {
        memcpy(plink.auth, genie_crypto_get_auth(plink.rand), STATIC_OOB_LENGTH);
    }
#endif

    if (bt_mesh_prov_conf(plink.conf_key, plink.rand, plink.auth, net_buf_simple_add(&cfm, 16))) {
        BT_ERR("Unable to generate confirmation value");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    if (prov_send(&cfm)) {
        BT_ERR("Failed to send Provisioning Confirm");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    plink.expect = PROV_RANDOM;
}

static void send_input_complete(void)
{
    PROV_BUF(buf, 1);

    prov_buf_init(&buf, PROV_INPUT_COMPLETE);
    prov_send(&buf);
}

int bt_mesh_input_number(bt_u32_t num)
{
    BT_DBG("%u", num);

    if (!atomic_test_and_clear_bit(plink.flags, WAIT_NUMBER)) {
        return -EINVAL;
    }

    sys_put_be32(num, &plink.auth[12]);

    send_input_complete();

    if (!atomic_test_bit(plink.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (atomic_test_and_clear_bit(plink.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

int bt_mesh_input_string(const char *str)
{
    if (!str) {
        return -EINVAL;
    }

    BT_DBG("%s", str);

    if (!atomic_test_and_clear_bit(plink.flags, WAIT_STRING)) {
        return -EINVAL;
    }

    strncpy((char *)plink.auth, str, prov->input_size);

    send_input_complete();

    if (!atomic_test_bit(plink.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (atomic_test_and_clear_bit(plink.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

static void prov_dh_key_cb(const u8_t key[32])
{
    BT_DBG("%p", key);

    if (!key) {
        BT_ERR("DHKey generation failed");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    sys_memcpy_swap(plink.dhkey, key, 32);

    BT_DBG("DHkey: %s", bt_hex(plink.dhkey, 32));

    atomic_set_bit(plink.flags, HAVE_DHKEY);

    if (atomic_test_bit(plink.flags, WAIT_NUMBER) || atomic_test_bit(plink.flags, WAIT_STRING)) {
        return;
    }

    if (atomic_test_and_clear_bit(plink.flags, SEND_CONFIRM)) {
        send_confirm();
    }
}

static void send_pub_key(void)
{
    PROV_BUF(buf, 65);
    const u8_t *key;

    key = bt_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    prov_buf_init(&buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), &key[32], 32);

    memcpy(&plink.conf_inputs[81], &buf.data[1], 64);

    prov_send(&buf);

    /* Copy remote key in little-endian for bt_dh_key_gen().
	 * X and Y halves are swapped independently.
	 */
    net_buf_simple_reset(&buf);
    sys_memcpy_swap(buf.data, &plink.conf_inputs[17], 32);
    sys_memcpy_swap(&buf.data[32], &plink.conf_inputs[49], 32);

    if (bt_dh_key_gen(buf.data, prov_dh_key_cb)) {
        BT_ERR("Failed to generate DHKey");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    plink.expect = PROV_CONFIRM;
}

static void prov_pub_key(const u8_t *data)
{
    BT_DBG("Remote Public Key: %s", bt_hex(data, 64));
    PROV_D(", 3--->4");

    memcpy(&plink.conf_inputs[17], data, 64);

    if (!bt_pub_key_get()) {
        /* Clear retransmit timer */
#if defined(CONFIG_BT_MESH_PB_ADV)
        prov_clear_tx();
#endif
        atomic_set_bit(plink.flags, REMOTE_PUB_KEY);
        BT_WARN("Waiting for local public key");
        return;
    }

    send_pub_key();
}

static void pub_key_ready(const u8_t *pkey)
{
    if (!pkey) {
        BT_WARN("Public key not available");
        return;
    }

    BT_DBG("Local public key ready");

    if (atomic_test_and_clear_bit(plink.flags, REMOTE_PUB_KEY)) {
        send_pub_key();
    }
}

static void prov_input_complete(const u8_t *data)
{
    BT_DBG("");
}

static void prov_confirm(const u8_t *data)
{
    BT_DBG("Remote Confirm: %s", bt_hex(data, 16));
    PROV_D(", 4--->5");

    memcpy(plink.conf, data, 16);

    if (!atomic_test_bit(plink.flags, HAVE_DHKEY)) {
#if defined(CONFIG_BT_MESH_PB_ADV)
        prov_clear_tx();
#endif
        atomic_set_bit(plink.flags, SEND_CONFIRM);
    } else {
        send_confirm();
    }
}

static void prov_random(const u8_t *data)
{
    PROV_BUF(rnd, 16);
    u8_t conf_verify[16];
    PROV_D(", 5--->6");

    BT_DBG("Remote Random: %s", bt_hex(data, 16));

#ifdef CONFIG_GENIE_MESH_ENABLE
    if (prov_method == AUTH_METHOD_STATIC) {
        memcpy(plink.auth, genie_crypto_get_auth(data), STATIC_OOB_LENGTH);
    }
#endif

    if (bt_mesh_prov_conf(plink.conf_key, data, plink.auth, conf_verify)) {
        BT_ERR("Unable to calculate confirmation verification");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    if (memcmp(conf_verify, plink.conf, 16)) {
        BT_ERR("Invalid confirmation value");
        BT_DBG("Received:   %s", bt_hex(plink.conf, 16));
        BT_DBG("Calculated: %s", bt_hex(conf_verify, 16));
        close_link(PROV_ERR_CFM_FAILED, CLOSE_REASON_FAILED);
        return;
    }

    prov_buf_init(&rnd, PROV_RANDOM);
    net_buf_simple_add_mem(&rnd, plink.rand, 16);

    if (prov_send(&rnd)) {
        BT_ERR("Failed to send Provisioning Random");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    if (bt_mesh_prov_salt(plink.conf_salt, data, plink.rand, plink.prov_salt)) {
        BT_ERR("Failed to generate provisioning salt");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ProvisioningSalt: %s", bt_hex(plink.prov_salt, 16));

    plink.expect = PROV_DATA;
}

static inline bool is_pb_gatt(void)
{
#if defined(CONFIG_BT_MESH_PB_GATT)
    return !!plink.conn;
#else
    return false;
#endif
}

static void prov_data(const u8_t *data)
{
    PROV_BUF(msg, 1);
    u8_t session_key[16];
    u8_t nonce[13];
    u8_t dev_key[16];
    u8_t pdu[25];
    u8_t flags;
    bt_u32_t iv_index;
    u16_t addr;
    u16_t net_idx;
    int err;
    bool identity_enable;

    BT_DBG("");
    PROV_D(", 6--->7");

    err = bt_mesh_session_key(plink.dhkey, plink.prov_salt, session_key);
    if (err) {
        BT_ERR("Unable to generate session key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(plink.dhkey, plink.prov_salt, nonce);
    if (err) {
        BT_ERR("Unable to generate session nonce");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    err = bt_mesh_prov_decrypt(session_key, nonce, data, pdu);
    if (err) {
        BT_ERR("Unable to decrypt provisioning data");
        close_link(PROV_ERR_DECRYPT, CLOSE_REASON_FAILED);
        return;
    }

    err = bt_mesh_dev_key(plink.dhkey, plink.prov_salt, dev_key);
    if (err) {
        BT_ERR("Unable to generate device key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("DevKey: %s", bt_hex(dev_key, 16));

    net_idx = sys_get_be16(&pdu[16]);
    flags = pdu[18];
    iv_index = sys_get_be32(&pdu[19]);
    addr = sys_get_be16(&pdu[23]);

    BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x", net_idx, iv_index, addr);

#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
    mesh_model_evt_cb event_cb = bt_mesh_event_get_cb_func();
    if (event_cb) {
        event_cb(BT_MESH_MODEL_EVT_PROV_DATA, &addr);
    }
#endif

    prov_buf_init(&msg, PROV_COMPLETE);
    prov_send(&msg);

    /* Ignore any further PDUs on this plink */
    plink.expect = 0;

    /* Store info, since bt_mesh_provision() will end up clearing it */
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        identity_enable = is_pb_gatt();
    } else {
        identity_enable = false;
    }

    err = bt_mesh_provision(pdu, net_idx, flags, iv_index, addr, dev_key);
    if (err) {
        BT_ERR("Failed to provision (err %d)", err);
        return;
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
    bt_mesh.p_net_idx_next = net_idx;
    struct node_info node_info;
    struct bt_le_oob oob;
    bt_le_oob_get_local(0, &oob);
    memcpy(&node_info.addr, &oob.addr, sizeof(bt_addr_le_t));
    memcpy(node_info.uuid, prov->uuid, 16);
    node_info.oob_info = prov->oob_info;
    node_info.element_num = bt_mesh_elem_count();
    node_info.unicast_addr = addr;
    node_info.net_idx = net_idx;
    node_info.flags = flags;
    node_info.iv_index = iv_index;
    bt_mesh_provisioner_add_node(&node_info, dev_key);
#endif

    /* After PB-GATT provisioning we should start advertising
	 * using Node Identity.
	 */
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) && identity_enable) {
        bt_mesh_proxy_identity_enable();
    }
}

static void prov_complete(const u8_t *data)
{
    BT_DBG("");
    PROV_D(", 9--->10");
}

static void prov_failed(const u8_t *data)
{
    BT_WARN("Error: 0x%02x", data[0]);
}

static const struct {
    void (*func)(const u8_t *data);
    u16_t len;
} prov_handlers[] = {
    { prov_invite, 1 },
    { prov_capabilities, 11 },
    {
        prov_start,
        5,
    },
    { prov_pub_key, 64 },
    { prov_input_complete, 0 },
    { prov_confirm, 16 },
    { prov_random, 16 },
    { prov_data, 33 },
    { prov_complete, 0 },
    { prov_failed, 1 },
};

static void close_link(u8_t err, u8_t reason)
{
#if defined(CONFIG_BT_MESH_PB_GATT)
    if (plink.conn) {
        if (err) {
            prov_send_fail_msg(err);
        }
#ifndef CONFIG_BT_BQB
        bt_mesh_pb_gatt_close(plink.conn);
#endif
        return;
    }
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
    if (err) {
        prov_send_fail_msg(err);
    }

    plink.rx.seg = 0;
    bearer_ctl_send(LINK_CLOSE, &reason, sizeof(reason));
#endif

    reset_state();
}

#if defined(CONFIG_BT_MESH_PB_ADV)
static void prov_retransmit(struct k_work *work)
{
    int i;

    BT_DBG("");

    if (!atomic_test_bit(plink.flags, LINK_ACTIVE)) {
        BT_WARN("Link not active");
        return;
    }

    if (k_uptime_get() - plink.tx.start > TRANSACTION_TIMEOUT) {
        BT_WARN("Giving up transaction");
#if CONFIG_BT_MESH_PROV_TIMEOUT
        k_timer_stop(&g_prov_timeout);
#endif
        reset_link();
        return;
    }

    for (i = 0; i < ARRAY_SIZE(plink.tx.buf); i++) {
        struct net_buf *buf = plink.tx.buf[i];

        if (!buf) {
            break;
        }

        if (BT_MESH_ADV(buf)->busy) {
            continue;
        }

        BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

        if (i + 1 < ARRAY_SIZE(plink.tx.buf) && plink.tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
        }
    }
}

static void link_open(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len < 16) {
        BT_ERR("Too short bearer open message (len %u)", buf->len);
        return;
    }

    if (atomic_test_bit(plink.flags, LINK_ACTIVE)) {
        /* Send another plink ack if the provisioner missed the last */
        if (plink.id == rx->link_id && plink.expect == PROV_INVITE) {
            BT_DBG("Resending link ack");
            bearer_ctl_send(LINK_ACK, NULL, 0);
        } else {
            BT_WARN("Ignoring bearer open: plink already active");
        }

        return;
    }

    if (memcmp(buf->data, prov->uuid, 16)) {
        BT_DBG("Bearer open message not for us");
        return;
    }

    if (prov->link_open) {
        prov->link_open(BT_MESH_PROV_ADV);
    }

    plink.id = rx->link_id;
    atomic_set_bit(plink.flags, LINK_ACTIVE);
#if CONFIG_BT_MESH_PROV_TIMEOUT
    k_timer_start(&g_prov_timeout, PROV_TIMEOUT);
#endif
    net_buf_simple_reset(plink.rx.buf);

    bearer_ctl_send(LINK_ACK, NULL, 0);

#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
    mesh_model_evt_cb event_cb = bt_mesh_event_get_cb_func();
    if (event_cb) {
        event_cb(BT_MESH_MODEL_EVT_PROV_START, &plink.id);
    }
#endif

    plink.expect = PROV_INVITE;
}

static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
}

static void link_close(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
#if CONFIG_BT_MESH_PROV_TIMEOUT
    k_timer_stop(&g_prov_timeout);
#endif
    reset_link();
}

static void gen_prov_ctl(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

    switch (BEARER_CTL(rx->gpc)) {
    case LINK_OPEN:
        link_open(rx, buf);
        break;
    case LINK_ACK:
        if (!atomic_test_bit(plink.flags, LINK_ACTIVE)) {
            return;
        }

        link_ack(rx, buf);
        break;
    case LINK_CLOSE:
        if (!atomic_test_bit(plink.flags, LINK_ACTIVE)) {
            return;
        }
        link_close(rx, buf);
        break;
    default:
        BT_ERR("Unknown bearer opcode: 0x%02x", BEARER_CTL(rx->gpc));

        if (IS_ENABLED(CONFIG_BT_TESTING)) {
            bt_test_mesh_prov_invalid_bearer(BEARER_CTL(rx->gpc));
        }

        return;
    }
}

static void prov_msg_recv(void)
{
    u8_t type = plink.rx.buf->data[0];

    BT_DBG("type 0x%02x len %u", type, plink.rx.buf->len);

    if (!bt_mesh_fcs_check(plink.rx.buf, plink.rx.fcs)) {
        BT_ERR("Incorrect FCS");
        return;
    }
    gen_prov_ack_send(plink.rx.id);
    plink.rx.prev_id = plink.rx.id;
    plink.rx.id = 0;

    if (type != PROV_FAILED && type != plink.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, plink.expect);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return;
    }

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        close_link(PROV_ERR_NVAL_PDU, CLOSE_REASON_FAILED);
        return;
    }

    if (1 + prov_handlers[type].len != plink.rx.buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x", plink.rx.buf->len, type);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    prov_handlers[type].func(&plink.rx.buf->data[1]);
}

static void gen_prov_cont(struct prov_rx *rx, struct net_buf_simple *buf)
{
    u8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (!plink.rx.seg && plink.rx.prev_id == rx->xact_id) {
        BT_DBG("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    if (rx->xact_id != plink.rx.id) {
        BT_WARN("Data for unknown transaction (%u != %u)", rx->xact_id, plink.rx.id);
        return;
    }

    if (seg > plink.rx.last_seg) {
        BT_ERR("Invalid segment index %u", seg);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    } else if (seg == plink.rx.last_seg) {
        u8_t expect_len;

        expect_len = (plink.rx.buf->len - 20U - ((plink.rx.last_seg - 1) * 23U));
        if (expect_len != buf->len) {
            BT_ERR("Incorrect last seg len: %u != %u", expect_len, buf->len);
            close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
            return;
        }
    }

    if (!(plink.rx.seg & BIT(seg))) {
        BT_DBG("Ignoring already received segment");
        return;
    }

    memcpy(XACT_SEG_DATA(seg), buf->data, buf->len);
    XACT_SEG_RECV(seg);

    if (!plink.rx.seg) {
        prov_msg_recv();
    }
}

static void gen_prov_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
    if (!plink.tx.buf[0]) {
        return;
    }

    if (rx->xact_id == plink.tx.id) {
        prov_clear_tx();
    }
}

static void gen_prov_start(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (plink.rx.seg) {
        BT_WARN("Got Start while there are unreceived segments");
        return;
    }

    if (plink.rx.prev_id == rx->xact_id) {
        BT_DBG("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    plink.rx.buf->len = net_buf_simple_pull_be16(buf);
    plink.rx.id = rx->xact_id;
    plink.rx.fcs = net_buf_simple_pull_u8(buf);

    BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len, START_LAST_SEG(rx->gpc), plink.rx.buf->len,
           plink.rx.fcs);

    if (plink.rx.buf->len < 1) {
        BT_ERR("Ignoring zero-length provisioning PDU");
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    if (plink.rx.buf->len > plink.rx.buf->size) {
        BT_ERR("Too large provisioning PDU (%u bytes)", plink.rx.buf->len);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && plink.rx.buf->len <= 20) {
        BT_ERR("Too small total length for multi-segment PDU");
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    plink.rx.seg = (1 << (START_LAST_SEG(rx->gpc) + 1)) - 1;
    plink.rx.last_seg = START_LAST_SEG(rx->gpc);
    memcpy(plink.rx.buf->data, buf->data, buf->len);
    XACT_SEG_RECV(0);

    if (!plink.rx.seg) {
        prov_msg_recv();
    }
}

static const struct {
    void (*func)(struct prov_rx *rx, struct net_buf_simple *buf);
    bool require_link;
    u8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true, 3 },
    { gen_prov_ack, true, 0 },
    { gen_prov_cont, true, 0 },
    { gen_prov_ctl, false, 0 },
};

static void gen_prov_recv(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
        return;
    }

    if (!atomic_test_bit(plink.flags, LINK_ACTIVE) && gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active plink");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(rx, buf);
}

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf)
{
    struct prov_rx rx;

    if (!bt_prov_active() && bt_mesh_is_provisioned()) {
        BT_DBG("Ignoring provisioning PDU - already provisioned");
        return;
    }

    if (buf->len < 6) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return;
    }

    rx.link_id = net_buf_simple_pull_be32(buf);
    rx.xact_id = net_buf_simple_pull_u8(buf);
    rx.gpc = net_buf_simple_pull_u8(buf);

    BT_DBG("link_id 0x%08x xact_id %u", rx.link_id, rx.xact_id);

    if (atomic_test_bit(plink.flags, LINK_ACTIVE) && plink.id != rx.link_id) {
        BT_DBG("Ignoring mesh beacon for unknown plink");
        return;
    }

    gen_prov_recv(&rx, buf);
}
#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
int bt_mesh_pb_gatt_recv(struct bt_conn *conn, struct net_buf_simple *buf)
{
    u8_t type;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (plink.conn != conn) {
        BT_WARN("Data for unexpected connection");
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);
    if (type != PROV_FAILED && type != plink.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, plink.expect);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        plink.expect = 0xFF;
        return -EINVAL;
    }

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        return -EINVAL;
    }

    if (prov_handlers[type].len != buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x", buf->len, type);
        return -EINVAL;
    }

    prov_handlers[type].func(buf->data);

    return 0;
}

int bt_mesh_pb_gatt_open(struct bt_conn *conn)
{
    BT_DBG("conn %p", conn);

    if (atomic_test_and_set_bit(plink.flags, LINK_ACTIVE)) {
        return -EBUSY;
    }
#if CONFIG_BT_MESH_PROV_TIMEOUT
    k_timer_start(&g_prov_timeout, PROV_TIMEOUT);
#endif
    plink.conn = bt_conn_ref(conn);
    plink.expect = PROV_INVITE;

    if (prov->link_open) {
        prov->link_open(BT_MESH_PROV_GATT);
    }

    return 0;
}

int bt_mesh_pb_gatt_close(struct bt_conn *conn)
{
    BT_DBG("conn %p", conn);

    if (plink.conn != conn) {
        BT_ERR("Not connected");
        return -ENOTCONN;
    }
#if CONFIG_BT_MESH_PROV_TIMEOUT
    k_timer_stop(&g_prov_timeout);
#endif
    if (prov->link_close) {
        prov->link_close(BT_MESH_PROV_GATT);
    }

    reset_state();

    return 0;
}
#endif /* CONFIG_BT_MESH_PB_GATT */

const struct bt_mesh_prov *bt_mesh_prov_get(void)
{
    return prov;
}

bool bt_prov_active(void)
{
    return atomic_test_bit(plink.flags, LINK_ACTIVE);
}

#if CONFIG_BT_MESH_PROV_TIMEOUT
static void prov_timeout(void *timer, void *arg)
{
    reset_link();
}
#endif

int bt_mesh_prov_init(const struct bt_mesh_prov *prov_info)
{
    static struct bt_pub_key_cb pub_key_cb = {
        .func = pub_key_ready,
    };
    int err;

    if (!prov_info) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    err = bt_pub_key_gen(&pub_key_cb);
    if (err) {
        BT_ERR("Failed to generate public key (%d)", err);
        return err;
    }

    prov = prov_info;

#if defined(CONFIG_BT_MESH_PB_ADV)
    k_delayed_work_init(&plink.tx.retransmit, prov_retransmit);
#endif

#if CONFIG_BT_MESH_PROV_TIMEOUT
    k_timer_init(&g_prov_timeout, prov_timeout, NULL);
#endif
    reset_state();

    return 0;
}

void bt_mesh_prov_complete(u16_t net_idx, u16_t addr)
{
    if (prov->complete) {
        prov->complete(net_idx, addr);
    }
}

void bt_mesh_prov_reset(void)
{
    if (prov->reset) {
        prov->reset();
    }
}
