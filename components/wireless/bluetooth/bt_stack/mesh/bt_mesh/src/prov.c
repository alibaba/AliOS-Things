/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>
//#include <conn.h>
#include <api/mesh.h>
#include <mesh_bt_uuid.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROV)
#include "common/log.h"

#ifdef MESH_DEBUG_PROV
#define ALI_PROV_TAG "\t[ALI_PROV]"
#define PROV_D(f, ...) BT_WARN(ALI_PROV_TAG"[D] %s "f, __func__, ##__VA_ARGS__)
#else
#define PROV_D(f, ...)
#endif
//#include "ecc.h"

#include "mesh_crypto.h"
#include "adv.h"
#include "mesh.h"
#include "../inc/net.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "prov.h"
#ifndef CONFIG_MESH_STACK_FOR_GENIE
#include "genie_mesh.h"
#endif
//#include "bt_mesh_custom_log.h"

/* 4 transmissions, 20ms interval */
#ifdef ADV_ONE_SHOT
#define PROV_XMIT_COUNT        0
#define PROV_XMIT_INT          10
#else
#define PROV_XMIT_COUNT        4
#define PROV_XMIT_INT          20
#endif

#define PROV_PROVISIONEE       0x00
#define PROV_PROVISIONER       0x01

#define AUTH_METHOD_NO_OOB     0x00
#define AUTH_METHOD_STATIC     0x01
#define AUTH_METHOD_OUTPUT     0x02
#define AUTH_METHOD_INPUT      0x03

#define PUBLIC_KEY_NO_OOB      0x00
#define PUBLIC_KEY_OOB         0x01

#define OUTPUT_OOB_BLINK       0x00
#define OUTPUT_OOB_BEEP        0x01
#define OUTPUT_OOB_VIBRATE     0x02
#define OUTPUT_OOB_NUMBER      0x03
#define OUTPUT_OOB_STRING      0x04

#define INPUT_OOB_PUSH         0x00
#define INPUT_OOB_TWIST        0x01
#define INPUT_OOB_NUMBER       0x02
#define INPUT_OOB_STRING       0x03

#define PROV_ERR_NONE          0x00
#define PROV_ERR_NVAL_PDU      0x01
#define PROV_ERR_NVAL_FMT      0x02
#define PROV_ERR_UNEXP_PDU     0x03
#define PROV_ERR_CFM_FAILED    0x04
#define PROV_ERR_RESOURCES     0x05
#define PROV_ERR_DECRYPT       0x06
#define PROV_ERR_UNEXP_ERR     0x07
#define PROV_ERR_ADDR          0x08

#define PROV_LINK_ACK          0x10

#define PROV_INVITE            0x00
#define PROV_CAPABILITIES      0x01
#define PROV_START             0x02
#define PROV_PUB_KEY           0x03
#define PROV_INPUT_COMPLETE    0x04
#define PROV_CONFIRM           0x05
#define PROV_RANDOM            0x06
#define PROV_DATA              0x07
#define PROV_COMPLETE          0x08
#define PROV_FAILED            0x09

#define PROV_ALG_P256          0x00

#define GPCF(gpc)              (gpc & 0x03)
#define GPC_START(last_seg)    (((last_seg) << 2) | 0x00)
#define GPC_ACK                0x01
#define GPC_CONT(seg_id)       (((seg_id) << 2) | 0x02)
#define GPC_CTL(op)            (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX      20
#define CONT_PAYLOAD_MAX       23

#define START_LAST_SEG(gpc)    (gpc >> 2)
#define CONT_SEG_INDEX(gpc)    (gpc >> 2)

#define BEARER_CTL(gpc)        (gpc >> 2)
#define LINK_OPEN              0x00
#define LINK_ACK               0x01
#define LINK_CLOSE             0x02

#define CLOSE_REASON_SUCCESS   0x00
#define CLOSE_REASON_TIMEOUT   0x01
#define CLOSE_REASON_FAILED    0x02

#define XACT_SEG_DATA(_seg) (&link.rx.buf->data[20 + ((_seg - 1) * 23)])
#define XACT_SEG_RECV(_seg) (link.rx.seg &= ~(1 << (_seg)))

#define XACT_NVAL              0xff

#define PROVER_ATTENTION_DURATION   0x02

#define PROVISIONING_TIMEOUT        K_SECONDS(30)

enum {
    REMOTE_PUB_KEY,        /* Remote key has been received */
    LOCAL_PUB_KEY,         /* Local public key is available */
    LINK_ACTIVE,           /* Link has been opened */
    HAVE_DHKEY,            /* DHKey has been calcualted */
    SEND_CONFIRM,          /* Waiting to send Confirm value */
    WAIT_NUMBER,           /* Waiting for number input from user */
    WAIT_STRING,           /* Waiting for string input from user */

    NUM_FLAGS,
};

struct prov_link {
    ATOMIC_DEFINE(flags, NUM_FLAGS);
#if defined(CONFIG_BT_MESH_PB_GATT)
    bt_mesh_conn_t conn;    /* GATT connection */
#endif
    u8_t  dhkey[32];         /* Calculated DHKey */
    u8_t  expect;            /* Next expected PDU */
    u8_t  expect_ack;        /* Expect ACK, especially prov start PDU */

    u8_t  oob_method;
    u8_t  oob_action;
    u8_t  oob_size;

    u8_t  conf[16];          /* Remote Confirmation */
    u8_t  rand[16];          /* Local Random */
    u8_t  auth[16];          /* Authentication Value */

    u8_t  conf_salt[16];     /* ConfirmationSalt */
    u8_t  conf_key[16];      /* ConfirmationKey */
    u8_t  conf_inputs[145];  /* ConfirmationInputs */
    u8_t  prov_salt[16];     /* Provisioning Salt */

    u8_t  device_key[16];   /* DeviceKey */
#ifdef CONFIG_BT_MESH_PROVISIONER
    u8_t  remote_dev_key[16];
    u16_t unicast_addr;
    u8_t pub_key_type;
    u8_t element_num;
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
    u32_t id;                /* Link ID */

    struct {
        u8_t  id;        /* Transaction ID */
        u8_t  prev_id;   /* Previous Transaction ID */
        u8_t  seg;       /* Bit-field of unreceived segments */
        u8_t  last_seg;  /* Last segment (to check length) */
        u8_t  fcs;       /* Expected FCS value */
        struct net_buf_simple *buf;
    } rx;

    struct {
        /* Start timestamp of the transaction */
        s64_t start;

        /* Transaction id*/
        u8_t id;
        u8_t prev_id;

        /* Pending outgoing buffer(s) */
        struct net_buf *buf[3];

        /* Retransmit timer */
        struct k_delayed_work retransmit;
    } tx;
#endif
#ifdef CONFIG_BT_MESH_PROVISIONER
    /* Retransmit timer */
    struct k_delayed_work prov_scan_timer;
    struct k_delayed_work provisioning_timer;
#endif
};

struct prov_rx {
    u32_t link_id;
    u8_t  xact_id;
    u8_t  gpc;
};

#define RETRANSMIT_TIMEOUT   K_MSEC(60)
#define BUF_TIMEOUT          K_MSEC(400)
#define TRANSACTION_TIMEOUT  K_SECONDS(30)

#if defined(CONFIG_BT_MESH_PB_GATT)
#define PROV_BUF_HEADROOM 5
#else
#define PROV_BUF_HEADROOM 0
static struct net_buf_simple *rx_buf = NET_BUF_SIMPLE(65);
#endif

#define PROV_BUF(len) NET_BUF_SIMPLE(PROV_BUF_HEADROOM + len)

static struct prov_link link;

static const struct bt_mesh_prov *prov;

static u8_t link_device_key[16];

static bt_mesh_prov_eventCbk g_bt_mesh_prov_callback;

static void prov_failed(const u8_t *data);
static void send_pub_key(void);
int bt_mesh_prov_pub_key_cb(u8_t *buf, u16_t buf_len);
int bt_mesh_prov_confirmation_key_device_cb(u8_t *buf, u16_t buf_len);
int bt_mesh_prov_random_device_cb(u8_t *buf, u16_t buf_len);
int bt_mesh_prov_local_confirmation_key_cb(u8_t *buf, u16_t buf_len);
int bt_mesh_prov_local_random_cb(u8_t *buf, u16_t buf_len) ;
int bt_mesh_prov_factor_cb(u8_t type, u8_t *buf, u16_t buf_len);
int bt_mesh_prov_seq_change_comp_cb(u32_t seq_num);
void bt_mesh_prov_event_callback(BT_MESH_EVENT_T *evt);

static void close_link(u8_t err, u8_t reason);

#ifdef CONFIG_BT_MESH_PROVISIONER
static void prov_start_auth(void);
#endif

extern void set_provisioner_state(u8_t state);
extern int bt_mesh_prov_encrypt(const u8_t key[16], u8_t nonce[13], const u8_t data[25], u8_t out[33]);
extern int bt_mesh_schedule_message_send(int msg_id, void *msg);

#if defined(CONFIG_BT_MESH_PB_ADV)
static void buf_sent(int err, void *user_data)
{
    if (!link.tx.buf[0]) {
        return;
    }
    BT_DBG("buf_sent");
    k_delayed_work_submit(&link.tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void free_segments(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        link.tx.buf[i] = NULL;
        /* Mark as canceled */
        BT_MESH_ADV(buf)->busy = 0;
        net_buf_unref(buf);
    }
}

static void prov_clear_tx(void)
{
    BT_DBG("prov_clear_tx");

    k_delayed_work_cancel(&link.tx.retransmit);

    free_segments();
}

static void reset_link(void)
{
    prov_clear_tx();

    if (prov->link_close) {
        prov->link_close(BT_MESH_PROV_ADV);
    }

    /* Clear everything except the retransmit delayed work config */
    memset(&link, 0, offsetof(struct prov_link, tx.retransmit));

    link.rx.prev_id = XACT_NVAL;

    if (bt_mesh_pub_key_get()) {
        atomic_set_bit(link.flags, LOCAL_PUB_KEY);
    }

#if defined(CONFIG_BT_MESH_PB_GATT)
    link.rx.buf = bt_mesh_proxy_get_buf();
#else
    net_buf_simple_init(rx_buf, 0);
    link.rx.buf = rx_buf;
#endif
#ifndef CONFIG_MESH_STACK_FOR_GENIE
    /* Disable Attention Timer if it was set */
    if (link.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }
#endif
}

static struct net_buf *adv_buf_create(void)
{
    struct net_buf *buf;

    buf = bt_mesh_adv_create(BT_MESH_ADV_PROV, PROV_XMIT_COUNT,
                             PROV_XMIT_INT, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of provisioning buffers");
        return NULL;
    }

    return buf;
}

static u8_t pending_ack = XACT_NVAL;

static void ack_complete(u16_t duration, int err, void *user_data)
{
    BT_DBG("xact %u complete", (u8_t)pending_ack);
    pending_ack = XACT_NVAL;
}

static void gen_prov_ack_send(u8_t xact_id)
{
    static const struct bt_mesh_send_cb cb = {
        .start = ack_complete,
    };
    const struct bt_mesh_send_cb *complete;
    struct net_buf *buf;

    BT_DBG("xact_id %u", xact_id);

    if (pending_ack == xact_id) {
        BT_DBG("Not sending duplicate ack");
        return;
    }

    buf = adv_buf_create();
    if (!buf) {
        return;
    }

    if (pending_ack == XACT_NVAL) {
        pending_ack = xact_id;
        complete = &cb;
    } else {
        complete = NULL;
    }

    net_buf_add_be32(buf, link.id);
    net_buf_add_u8(buf, xact_id);
    net_buf_add_u8(buf, GPC_ACK);

    bt_mesh_adv_send(buf, complete, NULL);
    net_buf_unref(buf);
}

static void send_reliable(void)
{
    int i;

    link.tx.start = k_uptime_get();

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
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

    net_buf_add_be32(buf, link.id);
    /* Transaction ID, always 0 for Bearer messages */
    net_buf_add_u8(buf, 0x00);
    net_buf_add_u8(buf, GPC_CTL(op));
    net_buf_add_mem(buf, data, data_len);

    link.tx.buf[0] = buf;
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
    link.tx.prev_id = link.tx.id;
    if (link.tx.id != 0x7F) {
        return link.tx.id++;
    }
    link.tx.id = 0x00;
    link.tx.prev_id = link.tx.id;
    return link.tx.id++;
}

static int prov_send_adv(struct net_buf_simple *msg)
{
    struct net_buf *start, *buf;
    u8_t seg_len, seg_id;
    u8_t xact_id;

    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));
    //PROV_D("len %u: %s", msg->len, bt_hex(msg->data, msg->len));
    PROV_D("len %u", msg->len);

    prov_clear_tx();
    BT_DBG("prov_send_adv: adv_buf_create");
    start = adv_buf_create();
    if (!start) {
        BT_DBG("prov_send_adv: adv_buf_create error");
        return -ENOBUFS;
    }

    BT_DBG("prov_send_adv: adv_buf_create success");
    xact_id = next_transaction_id();
    net_buf_add_be32(start, link.id);
    net_buf_add_u8(start, xact_id);

    net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
    net_buf_add_be16(start, msg->len);
    net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

    link.tx.buf[0] = start;

    seg_len = min(msg->len, START_PAYLOAD_MAX);
    BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
    net_buf_add_mem(start, msg->data, seg_len);
    net_buf_simple_pull(msg, seg_len);

    buf = start;
    for (seg_id = 1; msg->len > 0; seg_id++) {
        if (seg_id >= ARRAY_SIZE(link.tx.buf)) {
            BT_ERR("Too big message");
            free_segments();
            return -E2BIG;
        }

        buf = adv_buf_create();
        if (!buf) {
            free_segments();
            return -ENOBUFS;
        }

        link.tx.buf[seg_id] = buf;

        seg_len = min(msg->len, CONT_PAYLOAD_MAX);

        BT_DBG("seg_id %u len %u: %s", seg_id, seg_len,
               bt_hex(msg->data, seg_len));

        net_buf_add_be32(buf, link.id);
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
    if (!link.conn) {
        return -ENOTCONN;
    }

    return bt_mesh_proxy_send(link.conn, BT_MESH_PROXY_PROV, msg);
}
#endif /* CONFIG_BT_MESH_PB_GATT */

static inline int prov_send(struct net_buf_simple *buf)
{
#if defined(CONFIG_BT_MESH_PB_GATT)
    if (link.conn) {
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
    net_buf_simple_init(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

static void prov_send_fail_msg(u8_t err)
{
    struct net_buf_simple *buf = PROV_BUF(2);

    prov_buf_init(buf, PROV_FAILED);
    net_buf_simple_add_u8(buf, err);
    prov_send(buf);
}

static void prov_invite(const u8_t *data)
{
    struct net_buf_simple *buf = PROV_BUF(12);

    BT_DBG("Attention Duration: %u seconds", data[0]);
    PROV_D(", 1--->2");
#ifndef CONFIG_MESH_STACK_FOR_GENIE
    if (data[0]) {
        bt_mesh_attention(NULL, data[0]);
    }
#endif
    link.conf_inputs[0] = data[0];

    prov_buf_init(buf, PROV_CAPABILITIES);

    /* Number of Elements supported */
    net_buf_simple_add_u8(buf, bt_mesh_elem_count());

    /* Supported algorithms - FIPS P-256 Eliptic Curve */
    net_buf_simple_add_be16(buf, MESH_BIT(PROV_ALG_P256));

    /* Public Key Type */
    net_buf_simple_add_u8(buf, prov->public_key_type);

    /* Static OOB Type */
    net_buf_simple_add_u8(buf, prov->static_val ? MESH_BIT(0) : 0x00);

    /* Output OOB Size */
    net_buf_simple_add_u8(buf, prov->output_size);

    /* Output OOB Action */
    net_buf_simple_add_be16(buf, prov->output_actions);

    /* Input OOB Size */
    net_buf_simple_add_u8(buf, prov->input_size);

    /* Input OOB Action */
    net_buf_simple_add_be16(buf, prov->input_actions);

    memcpy(&link.conf_inputs[1], &buf->data[1], 11);

    if (prov_send(buf)) {
        BT_ERR("Failed to send capabilities");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    link.expect = PROV_START;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
#define PVNR_START_DELAY 1000
u8_t PTS_default_public_key[64] = {
    0xF4,0x65,0xE4,0x3F,0xF2,0x3D,0x3F,0x1B,
    0x9D,0xC7,0xDF,0xC0,0x4D,0xA8,0x75,0x81,
    0x84,0xDB,0xC9,0x66,0x20,0x47,0x96,0xEC,
    0xCF,0x0D,0x6C,0xF5,0xE1,0x65,0x00,0xCC,
    0x02,0x01,0xD0,0x48,0xBC,0xBB,0xD8,0x99,
    0xEE,0xEF,0xC4,0x24,0x16,0x4E,0x33,0xC2,
    0x01,0xC2,0xB0,0x10,0xCA,0x6B,0x4D,0x43,
    0xA8,0xA1,0x55,0xCA,0xD8,0xEC,0xB2,0x79
    };
static int prover_auth(u8_t method, u8_t action, u8_t size)
{
    bt_mesh_output_action_t output = BT_MESH_NO_OUTPUT;
    bt_mesh_input_action_t input = BT_MESH_NO_INPUT;
    BT_DBG("prover_auth method = %d, action = %d", method, action);
    link.expect = PROV_CONFIRM;
    switch (method) {
        case AUTH_METHOD_NO_OOB:
            if (action || size) {
                return -EINVAL;
            }

            memset(link.auth, 0, sizeof(link.auth));
            /* denis: modified! not use timer to start auth. */
            prov_start_auth();
            return 0;
        case AUTH_METHOD_STATIC:
            if (action || size) {
                return -EINVAL;
            }
            memcpy(link.auth + 16 - prov->static_val_len,
                   prov->static_val, prov->static_val_len);
            memset(link.auth, 0, sizeof(link.auth) - prov->static_val_len);
            /* denis: modified! not use timer to start auth */
            prov_start_auth();
            return 0;

        case AUTH_METHOD_INPUT:
            switch (action) {
                case INPUT_OOB_NUMBER:
                    output = BT_MESH_DISPLAY_NUMBER;
                    break;
                case INPUT_OOB_STRING:
                    output = BT_MESH_DISPLAY_STRING;
                    break;
            }
            link.expect = PROV_INPUT_COMPLETE;
            if (output == BT_MESH_DISPLAY_STRING) {
                unsigned char str[9];
                u8_t i;

                //bt_rand(str, size);
                bt_mesh_rand(str, size);

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

                memcpy(link.auth, str, size);
                memset(link.auth + size, 0, sizeof(link.auth) - size);

                return prov->output_string((char *)str);
            } else {
                u32_t num;

                //bt_rand(&num, sizeof(num));
                bt_mesh_rand(&num, sizeof(num));
                num %= 10;

                sys_put_be32(num, &link.auth[12]);
                memset(link.auth, 0, 12);

                return prov->output_number(output, num);
            }

        case AUTH_METHOD_OUTPUT:
            switch (action) {
                case OUTPUT_OOB_BLINK:
                    input = BT_MESH_ENTER_NUMBER;
                    break;
                case OUTPUT_OOB_BEEP:
                    input = BT_MESH_ENTER_NUMBER;
                    break;
                case OUTPUT_OOB_VIBRATE:
                    input = BT_MESH_ENTER_NUMBER;
                    break;
                case OUTPUT_OOB_NUMBER:
                    input = BT_MESH_ENTER_NUMBER;
                    break;
                case OUTPUT_OOB_STRING:
                    input = BT_MESH_ENTER_STRING;
                    break;
                default:
                    input = BT_MESH_NO_INPUT;
                    break;
            }

            if (!input) {
                return -EINVAL;
            }

            if (input == BT_MESH_ENTER_STRING) {
                atomic_set_bit(link.flags, WAIT_STRING);
            } else {
                atomic_set_bit(link.flags, WAIT_NUMBER);
            }
            return prov->input(input, size);

        default:
            return -EINVAL;
    }
}

//denis: added! not use timer to send public key.
void prov_send_pub_key(void)
{
    switch (link.pub_key_type) {
    case PUBLIC_KEY_OOB:
        /*modified for PTS, need to get public key from oob*/
        memcpy(&link.conf_inputs[81], PTS_default_public_key, 64);
        if (prover_auth(link.oob_method, link.oob_action, link.oob_size) < 0) {
            BT_ERR("Capabilities Error");
            prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        }
    case PUBLIC_KEY_NO_OOB:
       //send public key
        if (!atomic_test_bit(link.flags, LOCAL_PUB_KEY)) {
            /* Clear retransmit timer */
#if defined(CONFIG_BT_MESH_PB_ADV)
            prov_clear_tx();
#endif
            atomic_set_bit(link.flags, REMOTE_PUB_KEY);
            BT_WARN("Waiting for local public key");
            return;
        }
        send_pub_key();
        break;
    default:
        BT_ERR("PROVISIONER Public key type error");
        break;
    }
}

static void prov_scan_timeout(struct k_work *work)
{
    BT_DBG("prov_scan_timeout");

    set_provisioner_state(0);
    bt_mesh_prov_ud_result_comp_cb();
}


static void prov_provisioning_timeout(struct k_work *work)
{
    u8_t prov_fail_reason[3];

    BT_DBG("prov_provisioning_timeout");

    reset_link();
    prov_fail_reason[0] = 7;
    prov_failed(prov_fail_reason);
}
#endif

static void prov_capabilities(const u8_t *data)
{
    u16_t algorithms, output_action, input_action;
    u8_t  element_num, pub_key_oob, static_oob,
          output_size, input_size;

    element_num = data[0];
    link.element_num = element_num;
    BT_DBG("Elements: %u", element_num);
    
    algorithms = sys_get_be16(&data[1]);
    BT_DBG("Algorithms:        %u", algorithms);

    pub_key_oob = data[3];
    link.pub_key_type = pub_key_oob;
    BT_DBG("Public Key Type:   0x%02x", pub_key_oob);

    static_oob = data[4];
    BT_DBG("Static OOB Type:   0x%02x", static_oob);

    output_size = data[5];
    BT_DBG("Output OOB Size:   %u", output_size);

    output_action = sys_get_be16(&data[6]);
    BT_DBG("Output OOB Action: 0x%04x", output_action);

    input_size = data[8];
    BT_DBG("Input OOB Size: %u", input_size);

    input_action = sys_get_be16(&data[9]);
    BT_DBG("Input OOB Action: 0x%04x", input_action);

#ifdef CONFIG_BT_MESH_PROVISIONER
    /*Send Prov_start*/
    memcpy(&link.conf_inputs[1], &data[0], 11);
#ifdef CONFIG_MESH_STACK_FOR_GENIE
    bt_mesh_prov_cap_cb(element_num, algorithms, pub_key_oob, static_oob, output_size, output_action, input_size, input_action);
#endif
#endif
}

#if 0
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
#endif

static int prov_auth(u8_t method, u8_t action, u8_t size)
{
    switch (method) {
        case AUTH_METHOD_STATIC:
            if (action || size) {
                return -EINVAL;
            }

            memcpy(link.auth + 16 - prov->static_val_len,
                   prov->static_val, prov->static_val_len);
            memset(link.auth, 0, sizeof(link.auth) - prov->static_val_len);
            return 0;
#if 0   //refuse other method
        case AUTH_METHOD_NO_OOB:
            if (action || size) {
                return -EINVAL;
            }

            memset(link.auth, 0, sizeof(link.auth));
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

                bt_mesh_rand(str, size);

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

                memcpy(link.auth, str, size);
                memset(link.auth + size, 0, sizeof(link.auth) - size);

                return prov->output_string((char *)str);
            } else {
                u32_t div[8] = { 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000
                               };
                u32_t num;

                bt_mesh_rand(&num, sizeof(num));
                num %= div[size - 1];

                sys_put_be32(num, &link.auth[12]);
                memset(link.auth, 0, 12);

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
                atomic_set_bit(link.flags, WAIT_STRING);
            } else {
                atomic_set_bit(link.flags, WAIT_NUMBER);
            }

            return prov->input(input, size);
#endif
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
        /* added for PTS NODE-PROV-BI-03-C*/
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        link.expect = PROV_FAILED;
        return;
    }

    if (data[1] != prov->public_key_type) {
        BT_ERR("Invalid public key value: 0x%02x", data[1]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        /* added for PTS NODE-PROV-BI-03-C*/
        link.expect = PROV_FAILED;
        return;
    }

    memcpy(&link.conf_inputs[12], data, 5);

    link.expect = PROV_PUB_KEY;

    if (prov_auth(data[2], data[3], data[4]) < 0) {
        BT_ERR("Invalid authentication method: 0x%02x; "
               "action: 0x%02x; size: 0x%02x", data[2], data[3],
               data[4]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
    }
}

static void send_confirm(void)
{
    BT_DBG("ConfInputs[0]   %s", bt_hex(link.conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(&link.conf_inputs[64], 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(&link.conf_inputs[128], 17));

    if (bt_mesh_prov_conf_salt(link.conf_inputs, link.conf_salt)) {
        BT_ERR("Unable to generate confirmation salt");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(link.conf_salt, 16));

    if (bt_mesh_prov_conf_key(link.dhkey, link.conf_salt, link.conf_key)) {
        BT_ERR("Unable to generate confirmation key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(link.conf_key, 16));

    if (bt_mesh_rand(link.rand, 16)) {
        BT_ERR("Unable to generate random number");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("LocalRandom: %s", bt_hex(link.rand, 16));

#ifdef CONFIG_MESH_STACK_FOR_GENIE
    bt_mesh_prov_local_confirmation_key_cb(link.conf_key, 16);
    bt_mesh_prov_local_random_cb(link.rand, 16);
#endif
}

#if 0
static void send_input_complete(void)
{
    struct net_buf_simple *buf = PROV_BUF(1);

    prov_buf_init(buf, PROV_INPUT_COMPLETE);
    prov_send(buf);
}
#endif

int bt_mesh_input_number(u32_t num)
{
    BT_DBG("%u", num);

    if (!atomic_test_and_clear_bit(link.flags, WAIT_NUMBER)) {
        return -EINVAL;
    }

    sys_put_be32(num, &link.auth[12]);
#ifdef CONFIG_BT_MESH_PROVISIONER
    prov_start_auth();
#endif

    return 0;
}

int bt_mesh_input_string(const char *str)
{
    BT_DBG("%s", str);

    if (!atomic_test_and_clear_bit(link.flags, WAIT_STRING)) {
        return -EINVAL;
    }

    strncpy((char *)link.auth, str, prov->input_size);

#ifdef CONFIG_BT_MESH_PROVISIONER
    send_confirm();
#endif

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

    sys_memcpy_swap(link.dhkey, key, 32);

    BT_DBG("DHkey: %s", bt_hex(link.dhkey, 32));

    atomic_set_bit(link.flags, HAVE_DHKEY);

    if (atomic_test_bit(link.flags, WAIT_NUMBER) ||
        atomic_test_bit(link.flags, WAIT_STRING)) {
        return;
    }

#ifdef CONFIG_BT_MESH_PROVISIONER
    send_confirm();
    return;
#endif

    if (atomic_test_and_clear_bit(link.flags, SEND_CONFIRM)) {
        send_confirm();
    }
}

static void send_pub_key(void)
{
    struct net_buf_simple *buf = PROV_BUF(65);
    const u8_t *key;

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    prov_buf_init(buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(buf, 32), &key[32], 32);

#ifdef  CONFIG_BT_MESH_PROVISIONER
    memcpy(&link.conf_inputs[17], &buf->data[1], 64);
#endif

    prov_send(buf);
}

#ifdef  CONFIG_BT_MESH_PROVISIONER
static void prov_start_auth(void)
{
    struct net_buf_simple *buf = PROV_BUF(65);

    /* Copy remote key in little-endian for bt_dh_key_gen().
     * X and Y halves are swapped independently.
     */
    net_buf_simple_init(buf, 0);

    sys_memcpy_swap(buf->data, &link.conf_inputs[81], 32);
    sys_memcpy_swap(&buf->data[32], &link.conf_inputs[113], 32);

    //if (bt_dh_key_gen(buf->data, prov_dh_key_cb)) {
    if (bt_mesh_dh_key_gen(buf->data, prov_dh_key_cb)) {
        BT_ERR("Failed to generate DHKey");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    link.expect = PROV_CONFIRM;
}
#endif

static void prov_pub_key(const u8_t *data)
{
    BT_DBG("Remote Public Key: %s", bt_hex(data, 64));
    PROV_D(", 3--->4");

#ifdef  CONFIG_BT_MESH_PROVISIONER
    memcpy(&link.conf_inputs[81], data, 64);
#endif

#ifdef CONFIG_BT_MESH_PROVISIONER
    {
        if (prover_auth(link.oob_method, link.oob_action, link.oob_size) < 0) {
                BT_ERR("Capabilities Error");
                prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        }

#ifdef CONFIG_MESH_STACK_FOR_GENIE
        bt_mesh_prov_pub_key_cb((u8_t *)data, 64);
#endif
    }
#endif
    
}

static void pub_key_ready(const u8_t *pkey)
{
    if (!pkey) {
        BT_WARN("Public key not available");
        return;
    }

    BT_DBG("Local public key ready, key = %s", bt_hex(pkey, 64));

    atomic_set_bit(link.flags, LOCAL_PUB_KEY);

    if (atomic_test_and_clear_bit(link.flags, REMOTE_PUB_KEY)) {
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

    memcpy(link.conf, data, 16);
#ifdef CONFIG_BT_MESH_PROVISIONER
    {
        struct net_buf_simple *rnd = PROV_BUF(16);

        prov_buf_init(rnd, PROV_RANDOM);
        net_buf_simple_add_mem(rnd, link.rand, 16);
        if (prov_send(rnd)) {
            BT_ERR("Failed to send Provisioning Random");
            close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
            return;
        }
        link.expect = PROV_RANDOM;

#ifdef CONFIG_MESH_STACK_FOR_GENIE
        bt_mesh_prov_confirmation_key_device_cb((u8_t *)data, 16);
#endif
    }
#endif
}

static void prov_random(const u8_t *data)
{
    struct net_buf_simple *rnd = PROV_BUF(16);

    BT_DBG("Remote Random: %s", bt_hex(data, 16));
    PROV_D(", 5--->6");

#ifdef CONFIG_BT_MESH_PROVISIONER
    {
        if (bt_mesh_prov_salt(link.conf_salt, link.rand, data,
                            link.prov_salt)) {
            BT_ERR("Failed to generate provisioning salt");
            close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
            return;
        }

        BT_DBG("ProvisioningSalt: %s", bt_hex(link.prov_salt, 16));
                            
        bt_mesh_prov_random_device_cb((u8_t *)data, 16);
        
        return;
    }
#endif
    prov_buf_init(rnd, PROV_RANDOM);
    net_buf_simple_add_mem(rnd, link.rand, 16);

    if (prov_send(rnd)) {
        BT_ERR("Failed to send Provisioning Random");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return;
    }

    if (bt_mesh_prov_salt(link.conf_salt, data, link.rand,
                          link.prov_salt)) {
        BT_ERR("Failed to generate provisioning salt");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("ProvisioningSalt: %s", bt_hex(link.prov_salt, 16));

    link.expect = PROV_DATA;
}

#ifdef CONFIG_BT_MESH_PROVISIONER
void send_prov_data(void)
{
    struct net_buf_simple *data = PROV_BUF(34);
    struct bt_mesh_subnet *sub = &bt_mesh.sub[0];
    u8_t flags = (bt_mesh.iv_update << 1) || (sub->kr_flag);
    u16_t unicast_addr;
    u8_t pdu[25];
    u8_t encrypted_pdu[33];
    u8_t session_key[16];
    u8_t nonce[13];
    int err;

    err = bt_mesh_session_key(link.dhkey, link.prov_salt, session_key);
    if (err) {
        BT_ERR("Unable to generate session key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link.dhkey, link.prov_salt, nonce);
    if (err) {
        BT_ERR("Unable to generate session nonce");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    err = bt_mesh_dev_key(link.dhkey, link.prov_salt, link_device_key);
    if (err) {
        BT_ERR("Unable to generate device key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("DevKey: %s", bt_hex(link_device_key, 16));
    memcpy(bt_mesh.dev_key, link_device_key, 16);

    prov_buf_init(data, PROV_DATA);

    unicast_addr = link.unicast_addr;
    if (sub->kr_flag) {
        memcpy(&pdu[0], sub->keys[1].net, 16);
    } else {
        memcpy(&pdu[0], sub->keys[0].net, 16);
    }
    sys_put_be16(sub->net_idx, &pdu[16]);
    pdu[18] = flags;
    sys_put_be32(bt_mesh.iv_index, &pdu[19]);
    sys_put_be16(unicast_addr, &pdu[23]);
    BT_DBG("start encrypt pdu %s", bt_hex(pdu, 25));

    err = bt_mesh_prov_encrypt(session_key, nonce, pdu, encrypted_pdu);
    if (!err){
        net_buf_simple_add_mem(data, encrypted_pdu, 33);
        prov_send(data);
    }
    
}
#endif

static void prov_data(const u8_t *data)
{
    struct net_buf_simple *msg = PROV_BUF(1);
    u8_t session_key[16];
    u8_t nonce[13];
    u8_t dev_key[16];
    u8_t pdu[25];
    u8_t flags;
    u32_t iv_index;
    u16_t addr;
    u16_t net_idx;
    int err;

    BT_DBG("");
    PROV_D(", 6--->7");

    err = bt_mesh_session_key(link.dhkey, link.prov_salt, session_key);
    if (err) {
        BT_ERR("Unable to generate session key");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link.dhkey, link.prov_salt, nonce);
    if (err) {
        BT_ERR("Unable to generate session nonce");
        close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    err = bt_mesh_prov_decrypt(session_key, nonce, data, pdu);
    if (err) {
        BT_ERR("Unable to decrypt provisioning data");
        /* modified for NODE/PROV/BV-10-C in PTS 7.4.1*/
        prov_send_fail_msg(PROV_ERR_DECRYPT);
        link.expect = PROV_FAILED;
        //close_link(PROV_ERR_DECRYPT, CLOSE_REASON_FAILED);
        return;
    }

    err = bt_mesh_dev_key(link.dhkey, link.prov_salt, dev_key);
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

    BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x",
           net_idx, iv_index, addr);

    prov_buf_init(msg, PROV_COMPLETE);
    prov_send(msg);

    /* Ignore any further PDUs on this link */
    link.expect = 0;

    bt_mesh_provision(pdu, net_idx, flags, iv_index, 0, addr, dev_key);
}

static void prov_complete(const u8_t *data)
{
    BT_DBG("");
    PROV_D(", 9--->10");
    close_link(PROV_ERR_NONE, CLOSE_REASON_SUCCESS);

    k_delayed_work_cancel(&link.provisioning_timer);
    bt_mesh_prov_done_cb(link.unicast_addr, link_device_key, true, false, 0);
}

static void prov_failed(const u8_t *data)
{
    BT_WARN("Error: 0x%02x", data[0]);
    close_link(PROV_ERR_UNEXP_ERR, CLOSE_REASON_FAILED);

    k_delayed_work_cancel(&link.provisioning_timer);
    bt_mesh_prov_done_cb(link.unicast_addr, link_device_key, false, false, data[0]);
}

static const struct {
    void (*func)(const u8_t *data);
    u16_t len;
} prov_handlers[] = {
    { prov_invite, 1 },
    { prov_capabilities, 11 },
    { prov_start, 5, },
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
    PROV_D(", 10--->0");
#if defined(CONFIG_BT_MESH_PB_GATT)
    if (link.conn) {
        bt_mesh_pb_gatt_close(link.conn);
        return;
    }
#endif

#if defined(CONFIG_BT_MESH_PB_ADV)
    if (err) {
        prov_send_fail_msg(err);
    }

    link.rx.seg = 0;
    bearer_ctl_send(LINK_CLOSE, &reason, sizeof(reason));
#endif

    atomic_clear_bit(link.flags, LINK_ACTIVE);
#ifndef CONFIG_MESH_STACK_FOR_GENIE
    /* Disable Attention Timer if it was set */
    if (link.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }
#endif
}

#if defined(CONFIG_BT_MESH_PB_ADV)
static void prov_retransmit(struct k_work *work)
{
    int i;

    BT_DBG("prov_retransmit");

    if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
        BT_WARN("Link not active");
        return;
    }

    if (k_uptime_get() - link.tx.start > TRANSACTION_TIMEOUT) {
        BT_WARN("Giving up transaction");
        reset_link();
        return;
    }

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        if (BT_MESH_ADV(buf)->busy) {
            continue;
        }

        BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

        if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
        }

    }
}

static void link_open(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("link_open: len %u", buf->len);
    PROV_D("link_id: 0x%08x, 0--->1", rx->link_id);

    if (buf->len < 16) {
        BT_ERR("Too short bearer open message (len %u)", buf->len);
        return;
    }

    if (atomic_test_bit(link.flags, LINK_ACTIVE)) {
        BT_WARN("Ignoring bearer open: link already active");
        return;
    }

    if (memcmp(buf->data, prov->uuid, 16)) {
        BT_DBG("Bearer open message not for us");
        return;
    }

    if (prov->link_open) {
        prov->link_open(BT_MESH_PROV_ADV);
    }

    link.id = rx->link_id;
    atomic_set_bit(link.flags, LINK_ACTIVE);
    net_buf_simple_init(link.rx.buf, 0);

    bearer_ctl_send(LINK_ACK, NULL, 0);

    link.expect = PROV_INVITE;
}

static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
#ifdef CONFIG_BT_MESH_PROVISIONER
    struct net_buf_simple *invite_buf = PROV_BUF(2);

    if (rx->link_id != link.id || link.expect != PROV_LINK_ACK){
        return;
    }

    prov_buf_init(invite_buf, PROV_INVITE);
    net_buf_simple_add_u8(invite_buf, PROVER_ATTENTION_DURATION);

    link.conf_inputs[0] = PROVER_ATTENTION_DURATION;
    prov_send(invite_buf);

    link.expect = PROV_CAPABILITIES;
#endif
}

static void link_close(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
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
            if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
                return;
            }

            link_ack(rx, buf);
            break;
        case LINK_CLOSE:
            if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
                return;
            }

            link_close(rx, buf);
            break;
        default:
            BT_ERR("Unknown bearer opcode: 0x%02x", BEARER_CTL(rx->gpc));
            return;
    }
}

static void prov_msg_recv(void)
{
    u8_t type = link.rx.buf->data[0];

    BT_DBG("type 0x%02x len %u", type, link.rx.buf->len);

    if (!bt_mesh_fcs_check(link.rx.buf, link.rx.fcs)) {
        BT_ERR("Incorrect FCS");
        return;
    }

    gen_prov_ack_send(link.rx.id);
    link.rx.prev_id = link.rx.id;
    link.rx.id = 0;

    if (type != PROV_FAILED && type != link.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, link.expect);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        /* added for NODE/PROV/BV-10-C in PTS 7.4.1*/
        link.expect = PROV_FAILED;
        return;
    }

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        close_link(PROV_ERR_NVAL_PDU, CLOSE_REASON_FAILED);
        return;
    }

    if (1 + prov_handlers[type].len != link.rx.buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x",
               link.rx.buf->len, type);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    prov_handlers[type].func(&link.rx.buf->data[1]);
}

static void gen_prov_cont(struct prov_rx *rx, struct net_buf_simple *buf)
{
    u8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (!link.rx.seg && link.rx.prev_id == rx->xact_id) {
        BT_WARN("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    if (rx->xact_id != link.rx.id) {
        BT_WARN("Data for unknown transaction (%u != %u)",
                rx->xact_id, link.rx.id);
        return;
    }

    if (seg > link.rx.last_seg) {
        BT_ERR("Invalid segment index %u", seg);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    } else if (seg == link.rx.last_seg) {
        u8_t expect_len;

        expect_len = (link.rx.buf->len - 20 -
                      (23 * (link.rx.last_seg - 1)));
        if (expect_len != buf->len) {
            BT_ERR("Incorrect last seg len: %u != %u",
                   expect_len, buf->len);
            close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
            return;
        }
    }

    if (!(link.rx.seg & MESH_BIT(seg))) {
        BT_WARN("Ignoring already received segment");
        return;
    }

    memcpy(XACT_SEG_DATA(seg), buf->data, buf->len);
    XACT_SEG_RECV(seg);

    if (!link.rx.seg) {
        prov_msg_recv();
    }
}

static void gen_prov_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (!link.tx.buf[0]) {
        return;
    }

    if (rx->xact_id == link.tx.prev_id) {
        prov_clear_tx();
        /* denis: added! not use timer to send public key. */
        if (link.expect_ack == 1) {
            link.expect_ack = 0;
            prov_send_pub_key();
        }
    }
}

static void gen_prov_start(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (link.rx.seg) {
        BT_WARN("Got Start while there are unreceived segments");
        return;
    }

    if (link.rx.prev_id == rx->xact_id) {
        BT_WARN("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    link.rx.buf->len = net_buf_simple_pull_be16(buf);
    link.rx.id  = rx->xact_id;
    link.rx.fcs = net_buf_simple_pull_u8(buf);

    BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len,
           START_LAST_SEG(rx->gpc), link.rx.buf->len, link.rx.fcs);

    if (link.rx.buf->len < 1) {
        BT_ERR("Ignoring zero-length provisioning PDU");
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    if (link.rx.buf->len > link.rx.buf->size) {
        BT_ERR("Too large provisioning PDU (%u bytes)",
               link.rx.buf->len);
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && link.rx.buf->len <= 20) {
        BT_ERR("Too small total length for multi-segment PDU");
        close_link(PROV_ERR_NVAL_FMT, CLOSE_REASON_FAILED);
        return;
    }

    link.rx.seg = (1 << (START_LAST_SEG(rx->gpc) + 1)) - 1;
    link.rx.last_seg = START_LAST_SEG(rx->gpc);
    memcpy(link.rx.buf->data, buf->data, buf->len);
    XACT_SEG_RECV(0);

    if (!link.rx.seg) {
        prov_msg_recv();
    }
}

static const struct {
    void (*const func)(struct prov_rx *rx, struct net_buf_simple *buf);
    const u8_t require_link;
    const u8_t min_len;
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

    if (!atomic_test_bit(link.flags, LINK_ACTIVE) &&
        gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active link");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(rx, buf);
}

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf)
{
    struct prov_rx rx;

    if (!bt_prov_active()) {
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

    if (atomic_test_bit(link.flags, LINK_ACTIVE) && link.id != rx.link_id) {
        BT_DBG("Ignoring mesh beacon for unknown link");
        return;
    }

    gen_prov_recv(&rx, buf);
}
#endif /* CONFIG_BT_MESH_PB_ADV */

#if defined(CONFIG_BT_MESH_PB_GATT)
int bt_mesh_pb_gatt_recv(bt_mesh_conn_t conn, struct net_buf_simple *buf)
{
    u8_t type;

    BT_DBG("%s, %u bytes: %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    if (link.conn != conn) {
        BT_WARN("Data for unexpected connection");
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);
    if (type != PROV_FAILED && type != link.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, link.expect);
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

int bt_mesh_pb_gatt_open(bt_mesh_conn_t conn)
{
    BT_DBG("conn %p", conn);

    if (atomic_test_and_set_bit(link.flags, LINK_ACTIVE)) {
        return -EBUSY;
    }

    link.conn = bt_mesh_conn_ref(conn);
    link.expect = PROV_INVITE;

    if (prov->link_open) {
        prov->link_open(BT_MESH_PROV_GATT);
    }

    return 0;
}

int bt_mesh_pb_gatt_close(bt_mesh_conn_t conn)
{
    bool pub_key;

    BT_DBG("conn %p", conn);

    if (link.conn != conn) {
        BT_ERR("Not connected");
        return -ENOTCONN;
    }
#ifndef CONFIG_MESH_STACK_FOR_GENIE
    /* Disable Attention Timer if it was set */
    if (link.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }
#endif
    if (prov->link_close) {
        prov->link_close(BT_MESH_PROV_GATT);
    }

    bt_mesh_conn_unref(link.conn);

    pub_key = atomic_test_bit(link.flags, LOCAL_PUB_KEY);
    memset(&link, 0, sizeof(link));

    if (pub_key) {
        atomic_set_bit(link.flags, LOCAL_PUB_KEY);
    }

    return 0;
}
#endif /* CONFIG_BT_MESH_PB_GATT */

const u8_t *bt_mesh_prov_get_uuid(void)
{
    return prov->uuid;
}

const struct bt_mesh_prov *bt_mesh_prov_get(void)
{
    return prov;
}

bool bt_prov_active(void)
{
    return atomic_test_bit(link.flags, LINK_ACTIVE);
}

int bt_mesh_prov_init(const struct bt_mesh_prov *prov_info)
{
    static struct bt_mesh_pub_key_cb pub_key_cb = {
        .func = pub_key_ready,
    };
    int err;

    if (!prov_info) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    err = bt_mesh_pub_key_gen(&pub_key_cb);
    if (err) {
        BT_ERR("Failed to generate public key (%d)", err);
        return err;
    }

    prov = prov_info;

#if defined(CONFIG_BT_MESH_PB_ADV)
    k_delayed_work_init(&link.tx.retransmit, prov_retransmit);
#ifdef CONFIG_BT_MESH_PROVISIONER
    k_delayed_work_init(&link.prov_scan_timer, prov_scan_timeout);
    k_delayed_work_init(&link.provisioning_timer, prov_provisioning_timeout);
#endif
    link.rx.prev_id = XACT_NVAL;

#if defined(CONFIG_BT_MESH_PB_GATT)
    link.rx.buf = bt_mesh_proxy_get_buf();
#else
    net_buf_simple_init(rx_buf, 0);
    link.rx.buf = rx_buf;
#endif

#endif /* CONFIG_BT_MESH_PB_ADV */

#if 0
    if (IS_ENABLED(CONFIG_BT_DEBUG)) {
        struct bt_mesh_uuid_128 uuid = { .uuid.type = BT_MESH_UUID_TYPE_128 };
        memcpy(uuid.val, prov->uuid, 16);
        BT_INFO("Device UUID: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                uuid.val[0], uuid.val[1], uuid.val[2], uuid.val[3],
                uuid.val[4], uuid.val[5], uuid.val[6], uuid.val[7],
                uuid.val[8], uuid.val[9], uuid.val[10], uuid.val[11],
                uuid.val[12], uuid.val[13], uuid.val[14], uuid.val[15],
               );
    }
#endif

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


#ifdef CONFIG_BT_MESH_PROVISIONER
int bt_mesh_prov_scan(u8_t start, s32_t duration)
{
    int ret = 0;

    if (start) {
        if (!bt_mesh_get_scan_en()) {
            bt_mesh_scan_enable();
        }
        set_provisioner_state(1);

        k_delayed_work_cancel(&link.prov_scan_timer);
        k_delayed_work_submit(&link.prov_scan_timer, duration);
    } else {
        k_delayed_work_cancel(&link.prov_scan_timer);
        set_provisioner_state(0);
    }

    return ret;
}

u8_t bt_mesh_prover_start_prov(u8_t *uuid)
{
    if (atomic_test_bit(link.flags, LINK_ACTIVE)) {
        BT_WARN("Ignoring bearer open: link already active");
        return 1;
    }

    bt_mesh_rand(&link.id, sizeof(link.id));
    BT_DBG("link id %x", link.id);
    atomic_set_bit(link.flags, LINK_ACTIVE);
    net_buf_simple_init(link.rx.buf, 0);

    bearer_ctl_send(LINK_OPEN, uuid, 16);
    link.expect = PROV_LINK_ACK;

    return 0;
}

void unprovisioned_device_found(struct net_buf_simple *buf)
{
    u8_t *uuid;
    u16_t oob_info;
    if (buf->len < 18) {
        BT_ERR("Too short Unprovisioned Device beacon (len %u)", buf->len);
        return;
    }
    uuid = buf->data;
    net_buf_simple_pull(buf, 16);
    oob_info = net_buf_simple_pull_be16(buf);
    
    /*TODO: URI hash implementation*/
    bt_mesh_prov_scan_ud_cb(uuid, oob_info, NULL);
}

u8_t bt_mesh_add_node_device_key(int addr, const u8_t *deviceKey, u8_t keyLength, 
                                   u8_t *deviceUUID, int uuidLength)
{
    /* TODO: zaiyuan */
    memcpy(bt_mesh.dev_key, deviceKey, 16);

    return 0;
}

int bt_mesh_prov_init_done_cb(void)
{
    BT_MESH_EVENT_T evt;

    evt.evt_id = BT_MESH_EVT_INIT_DONE;
    BT_INFO("%s: init done cb\n", __func__);

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_cap_cb(
    u8_t number_of_elements,
    u16_t algorithms,
    u8_t public_key_type,
    u8_t static_oob_type,
    u8_t output_oob_size,
    u16_t output_oob_action,
    u8_t input_oob_size,
    u16_t input_oob_action)
{

    BT_MESH_EVENT_T evt;
    BT_MESH_PROV_CAPABILITIES_T *p_cap = (BT_MESH_PROV_CAPABILITIES_T *)&(evt.evt.mesh_evt.mesh.prov_cap);

    BT_INFO("%s: prov cap cb, noe:%d, alg:%d, pkt:%d, sot:%d, oos:%d, ooa:%d, ios:%d, ioa:%d\n",
            __func__,
            number_of_elements,
            algorithms,
            public_key_type,
            static_oob_type,
            output_oob_size,
            output_oob_action,
            input_oob_size,
            input_oob_action
    );

    evt.evt_id = BT_MESH_EVT_PROV_CAPABILITIES;

    p_cap->number_of_elements = number_of_elements;
    p_cap->algorithms = algorithms;
    p_cap->public_key_type = public_key_type;
    p_cap->static_oob_type = static_oob_type;
    p_cap->output_oob_size = output_oob_size;
    p_cap->output_oob_action = output_oob_action;
    p_cap->input_oob_size = input_oob_size;
    p_cap->input_oob_action = input_oob_action;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_req_oob_pub_key_cb(void)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: oob pub key cb\n", __func__);
    evt.evt_id = BT_MESH_EVT_PROV_REQUEST_OOB_PUBLIC_KEY;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_req_auth_cb(u8_t method, u8_t action, u8_t size)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_EVT_PROV_REQUEST_AUTH_T *p_prov_request_auth = &evt.evt.mesh_evt.mesh.prov_request_auth;

    BT_INFO("%s: req auth cb, method:%d, action:%d, size:%d\n",
            __func__,
            method,
            action,
            size);

    evt.evt_id = BT_MESH_EVT_PROV_REQUEST_OOB_AUTH_VALUE;
    p_prov_request_auth->method = method;
    p_prov_request_auth->action = action;
    p_prov_request_auth->size = size;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_show_pub_key_cb(u8_t *pk)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: show pub key cb, pk:%p\n", __func__, pk);
    evt.evt_id = BT_MESH_EVT_PROV_SHOW_OOB_PUBLIC_KEY;
    evt.evt.mesh_evt.mesh.prov_show_pk.pk = pk;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_show_auth_value_cb(u8_t *auth)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: show auth value cb, auth:%p\n", __func__, auth);

    evt.evt_id = BT_MESH_EVT_PROV_SHOW_OOB_AUTH_VALUE;
    memcpy(evt.evt.mesh_evt.mesh.prov_show_auth.auth, auth, BT_MESH_AUTHENTICATION_SIZE);

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_done_cb(u16_t address, u8_t *device_key, bool success, bool gatt_bearer, s32_t reason)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: prov done cb, addr:0x%x, dev_key:%p, success:%d, is batt:%d, reason:%d\n",
            __func__,
            address,
            device_key,
            success,
            gatt_bearer,
            reason);

    evt.evt_id = BT_MESH_EVT_PROV_DONE;
    BT_MESH_EVT_PROV_DONE_T *p_prov_done = &evt.evt.mesh_evt.mesh.prov_done;

    p_prov_done->address = address;
    memcpy(p_prov_done->device_key, device_key, BT_MESH_DEVKEY_SIZE);
    p_prov_done->success = success;
    p_prov_done->gatt_bearer = gatt_bearer;
    p_prov_done->reason = reason;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}


int bt_mesh_prov_scan_ud_cb(
    u8_t *uuid,
    u16_t oob_info,
    u8_t *uri_hash)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: scan ud cb, uuid:%s, oob_info:%d, uri_hash:%p\n",
            __func__, uuid, oob_info, uri_hash);

    evt.evt_id = BT_MESH_EVT_PROV_SCAN_UD_RESULT;
    BT_MESH_EVT_PROV_SCAN_UD_T *p_prov_scan_ud = &evt.evt.mesh_evt.mesh.prov_scan_ud;
    BT_MESH_PROV_UNPROVISIONED_DEVICE_T *p_ud = &p_prov_scan_ud->ud;

    p_prov_scan_ud->total_count = 1;
    p_prov_scan_ud->current_index = 1;

    memcpy(p_ud->uuid, uuid, BT_MESH_UUID_SIZE);
    p_ud->oob_info = oob_info;
    p_ud->uri_hash = uri_hash;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_config_reset_cb(u8_t *pk)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: prov config reset cb, pk:%p\n", __func__, pk);

    evt.evt_id = BT_MESH_EVT_CONFIG_RESET;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_friend_status_cb(u16_t address, u16_t status)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_EVT_FRIDSHIP_STATUS_T *p_friendship_status = &evt.evt.mesh_evt.mesh.friendship_status;

    BT_INFO("%s: prov friend status cb, addr:0x%x, status:%d\n", __func__, address, status);
    evt.evt_id = BT_MESH_EVT_FRIENDSHIP_STATUS;
    p_friendship_status->address = address;
    p_friendship_status->status = status; //Ethan: need to translate

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_heartbeat_cb(u16_t address, u8_t active)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_EVT_HEARTBEAT_T *p_heartbeat = &evt.evt.mesh_evt.mesh.heartbeat;

    BT_INFO("%s: prov heartbeat status cb, addr:0x%x, status:%d\n", __func__, address, active);

    evt.evt_id = BT_MESH_EVT_HEARTBEAT;
    p_heartbeat->address = address;
    p_heartbeat->active = active;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_iv_update_cb(u32_t iv_index, u8_t state, u8_t iv_phase)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_EVT_IV_UPDATE_T *p_iv_update = &evt.evt.mesh_evt.mesh.iv_update;

    BT_INFO("%s: prov iv update cb, iv_index:0x%x, state:%d, iv_phase\n", __func__, iv_index, state, iv_phase);

    evt.evt_id = BT_MESH_EVT_IV_UPDATE;
    p_iv_update->iv_index = iv_index;
    p_iv_update->state = state;
    p_iv_update->iv_phase = iv_phase;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_key_refresh_cb(u16_t netkey_index, u8_t state)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_EVT_KEY_REFRESH_T *p_key_refresh = &evt.evt.mesh_evt.mesh.key_refresh;

    BT_INFO("%s: prov key refresh cb, netkey_index:0x%x, state:%d\n", __func__, netkey_index, state);

    evt.evt_id = BT_MESH_EVT_KEY_REFRESH;

    p_key_refresh->netkey_index = netkey_index;
    p_key_refresh->phase = state;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_ud_result_comp_cb(void)
{
    BT_MESH_EVENT_T evt;

    BT_INFO("%s: prov ud resultcb\n", __func__);

    evt.evt_id = BT_MESH_EVT_UD_RESULT_COMPLETE;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_pub_key_cb(u8_t *buf, u16_t buf_len)
{
    return bt_mesh_prov_factor_cb(BT_MESH_PROV_FACTOR_PUB_KEY, buf, buf_len);
}

int bt_mesh_prov_confirmation_key_device_cb(u8_t *buf, u16_t buf_len)
{
    BT_DBG("[PROV FACT]cfm device: %s", bt_hex(buf, buf_len));
    return bt_mesh_prov_factor_cb(BT_MESH_PROV_FACTOR_CONFIRMATION_DEVICE, buf, buf_len);
}

int bt_mesh_prov_random_device_cb(u8_t *buf, u16_t buf_len)
{
    BT_DBG("[PROV FACT]random device: %s", bt_hex(buf, buf_len));
    return bt_mesh_prov_factor_cb(BT_MESH_PROV_FACTOR_RANDOM_DEVICE, buf, buf_len);
}


int bt_mesh_prov_local_confirmation_key_cb(u8_t *buf, u16_t buf_len)
{
    BT_DBG("[PROV FACT]cfm local: %s", bt_hex(buf, buf_len));
    return bt_mesh_prov_factor_cb(BT_MESH_PROV_FACTOR_CONFIRMATION_KEY, buf, buf_len);
}

int bt_mesh_prov_local_random_cb(u8_t *buf, u16_t buf_len)
{
    BT_DBG("[PROV FACT]random local: %s", bt_hex(buf, buf_len));
    return bt_mesh_prov_factor_cb(BT_MESH_PROV_FACTOR_RANDOM_PROVISIONER, buf, buf_len);
}

int bt_mesh_prov_factor_cb(u8_t type, u8_t *buf, u16_t buf_len)
{
    BT_MESH_EVENT_T evt;
    BT_MESH_PROV_FACTOR_T *p_prov_factor = &evt.evt.mesh_evt.mesh.prov_factor;

    evt.evt_id = BT_MESH_EVT_PROV_FACTOR;
    p_prov_factor->type = type;
    p_prov_factor->buf = buf;
    p_prov_factor->buf_len = buf_len;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

int bt_mesh_prov_seq_change_comp_cb(u32_t seq_num)
{
    BT_MESH_EVENT_T evt;

    evt.evt_id = BT_MESH_EVT_SEQ_CHANGE;
    evt.evt.mesh_evt.mesh.seq_change.seq_num = seq_num;

    bt_mesh_prov_event_callback(&evt);

    return 0;
}

void bt_mesh_prov_event_callback(BT_MESH_EVENT_T *evt)
{
    if (g_bt_mesh_prov_callback)
        g_bt_mesh_prov_callback(evt);
}

int bt_mesh_prov_register_callback(void *cb){
    g_bt_mesh_prov_callback = (bt_mesh_prov_eventCbk)cb;

    return 0;
}

int provisioner_send_invite(u8_t* uuid, u32_t size, s32_t attentionDuration) 
{
    int ret;
    int msg_id;
    struct prov_send_invite_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct prov_send_invite_t *)k_malloc(sizeof(struct prov_send_invite_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    memcpy(msg->uuid, uuid, 16);
    msg->size = size;
    msg->attentionDuration = attentionDuration;

    msg_id = MSG_ID(MSG_GROUP_PROV, MSG_ID_PROV_SEND_INVITE);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }

    return ret;
}

int provisioner_send_start(u32_t algorithms, s32_t public_key_oob, s32_t auth_method, s32_t auth_action, s32_t auth_size,
                           u8_t* netkey, s32_t netkey_index, s32_t iv_index, s32_t addr, s32_t flags, s32_t mode) 
{
    int ret;
    int msg_id;
    struct prov_send_start_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct prov_send_start_t *)k_malloc(sizeof(struct prov_send_start_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    msg->algorithms = algorithms;
    msg->public_key_oob = public_key_oob;
    msg->auth_method = auth_method;
    msg->auth_action = auth_action;
    msg->auth_size = auth_size;
    memcpy(msg->netkey, netkey, 16);
    msg->netkey_index = netkey_index;
    msg->iv_index = iv_index;
    msg->addr = addr;
    msg->flags = flags;
    msg->mode = mode;

    msg_id = MSG_ID(MSG_GROUP_PROV, MSG_ID_PROV_SEND_START);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }

    return ret;
}

int provisioner_send_confirm(u8_t *confirm, u32_t size)
{
    int ret;
    int msg_id;
    struct prov_send_confirm_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct prov_send_confirm_t *)k_malloc(sizeof(struct prov_send_confirm_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    memcpy(msg->confirm, confirm, 16);
    msg->size = size;

    msg_id = MSG_ID(MSG_GROUP_PROV, MSG_ID_PROV_SEND_CONFIRM);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }

    return ret;
}

int provisioner_set_auth_result(u8_t *result, u32_t size)
{
    int ret;
    int msg_id;
    struct prov_auth_result_t *msg;

    BT_DBG("%s", __func__);
    msg = (struct prov_auth_result_t *)k_malloc(sizeof(struct prov_auth_result_t));
    if (msg == NULL) {
        BT_ERR("%s: malloc failed", __func__);
        return -1;
    }

    memcpy(msg->result, result, 1);
    msg->size = size;

    msg_id = MSG_ID(MSG_GROUP_PROV, MSG_ID_PROV_AUTH_RESULT);
    ret = bt_mesh_schedule_message_send(msg_id, msg);
    if (ret) {
        BT_ERR("%s: message send err %d", __func__, ret);
        k_free(msg);
    }

    return ret;
}

int provisioner_send_invite_handle(u8_t* uuid, u32_t size, s32_t attentionDuration)
{
    int ret;

    BT_DBG("%s", __func__);
    if (link.rx.prev_id != XACT_NVAL) {
        link.rx.prev_id = XACT_NVAL;
    }
    link.tx.id = link.tx.prev_id = 0;

    ret = bt_mesh_prover_start_prov(uuid);
    if (ret == 0) {
        /* start timer for provisioning */
        k_delayed_work_cancel(&link.provisioning_timer);
        k_delayed_work_submit(&link.provisioning_timer, PROVISIONING_TIMEOUT);
    }

    return ret;
}

int provisioner_send_start_handle(u32_t algorithms, s32_t public_key_oob, s32_t auth_method, s32_t auth_action, s32_t auth_size,
                           u8_t* netkey, s32_t netkey_index, s32_t iv_index, s32_t addr, s32_t flags, s32_t mode)
{
    struct net_buf_simple *buf = PROV_BUF(6);

    BT_DBG("%s", __func__);
    prov_buf_init(buf, PROV_START);
    net_buf_simple_add_u8(buf, PROV_ALG_P256);
    net_buf_simple_add_u8(buf, public_key_oob);

    link.oob_method = auth_method;
    link.oob_action = auth_action;
    link.oob_size = auth_size;

    link.unicast_addr = addr;

    net_buf_simple_add_u8(buf, link.oob_method);
    net_buf_simple_add_u8(buf, link.oob_action);
    net_buf_simple_add_u8(buf, link.oob_size);

    memcpy(&link.conf_inputs[12], &buf->data[1], 5);
    if (prov_send(buf)) {
        BT_ERR("Failed to send Start");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return -1;
    }

    link.expect = PROV_PUB_KEY;
    /* denis: modified! not use timer to send public key. */
    link.expect_ack = 1;

    return 0;
}

int provisioner_send_confirm_handle(u8_t *confirm, u32_t size)
{
    struct net_buf_simple *buf = PROV_BUF(17);

    BT_DBG("%s", __func__);
    if (size != 16) {
        BT_ERR("%s: incorrect size %d\n", __func__, size);
        return -1;
    }

    prov_buf_init(buf, PROV_CONFIRM);
    net_buf_simple_add_mem(buf, confirm, size);

    if (prov_send(buf)) {
        BT_ERR("Failed to send Provisioning Confirm");
        close_link(PROV_ERR_RESOURCES, CLOSE_REASON_FAILED);
        return -1;
    }

    link.expect = PROV_CONFIRM;
    return 0;
}

int provisioner_set_auth_result_handle(u8_t *result, u32_t size)
{
    if (!result) {
        BT_INFO("%s:result pointer is NULL\n", __func__);
        return -1;
    }
    BT_INFO("%s:auth result:%d, %s\n", __func__, *result, *result == 1 ? "success" : "fail");
    if (*result == 1) {
        send_prov_data();
        link.expect = PROV_COMPLETE;
    } else {
        prov_send_fail_msg(PROV_ERR_CFM_FAILED);
        link.expect = PROV_FAILED;
    }
    return 0;
}

int bt_mesh_schedule_prov_handle(int msg_id, void *pMsg)
{
    switch (msg_id) {
    case MSG_ID_PROV_SEND_INVITE:
        {
            struct prov_send_invite_t *msg = (struct prov_send_invite_t *)pMsg;

            if (msg) {
                provisioner_send_invite_handle(msg->uuid, msg->size, msg->attentionDuration);
            } else {
                BT_ERR("MSG_ID_PROV_SEND_INVITE: Invalide Parameter");
            }
        }
        break;
    case MSG_ID_PROV_SEND_START:
        {
            struct prov_send_start_t *msg = (struct prov_send_start_t *)pMsg;

            if (msg) {
                provisioner_send_start_handle(msg->algorithms, msg->public_key_oob, msg->auth_method, msg->auth_action,
                                            msg->auth_size, msg->netkey, msg->netkey_index, msg->iv_index, msg->addr,
                                            msg->flags, msg->mode);
            } else {
                BT_ERR("MSG_ID_PROV_SEND_INVITE: Invalide Parameter");
            }
        }
        break;
    case MSG_ID_PROV_SEND_CONFIRM:
        {
            struct prov_send_confirm_t *msg = (struct prov_send_confirm_t *)pMsg;

            if (msg) {
                provisioner_send_confirm_handle(msg->confirm, msg->size);
            } else {
                BT_ERR("MSG_ID_PROV_SEND_INVITE: Invalide Parameter");
            }
        }
        break;
    case MSG_ID_PROV_AUTH_RESULT:
        {
            struct prov_auth_result_t *msg = (struct prov_auth_result_t *)pMsg;

            provisioner_set_auth_result_handle(msg->result, msg->size);
        }
        break;
    }

    return 0;
}

#endif
