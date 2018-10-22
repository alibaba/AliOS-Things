/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "transport.h"
#include "core.h"
#include "common.h"
#include "utils.h"
#include "ble_service.h"

#include "breeze_hal_sec.h"
#include "breeze_hal_os.h"

// TODO: rm ota related code
#include "breeze_export.h"

#define HEADER_SIZE       4  /**< Size of packet header. */
#define AES_BLK_SIZE      16 /**< AES block size. */

#define CHECK_ENC(data)  ((data[0] & 0x10) != 0) /**< Check encryption bit. */
#define GET_MSG_ID(data) (data[0] & 0xF)         /**< Get msg_id from data. */
#define GET_CMD(data)    (data[1])               /**< Get cmd from data. */
#define GET_TOTAL_FRM(data) \
    ((data[2] >> 4) & 0x0F)                /**< Get total_frame from data. */
#define GET_FRM_SEQ(data) (data[2] & 0x0F) /**< Get frame_seq from data. */
#define GET_LEN(data)     (data[3])        /**< Get length from data. */

/*global transport event*/
extern bool g_dn_complete;

/**@brief Reset Tx state machine. */
static void reset_tx(transport_t *p_transport)
{
    p_transport->tx.len = 0;
    p_transport->tx.bytes_sent = 0;
    p_transport->tx.msg_id = 0;
    p_transport->tx.cmd = 0;
    p_transport->tx.total_frame = 0;
    p_transport->tx.frame_seq = 0;
    p_transport->tx.pkt_req = 0;
    p_transport->tx.pkt_cfm = 0;

    if (p_transport->timeout != 0) {
        os_timer_stop(&p_transport->tx.timer);
    }
}

/**@brief Reset Rx state machine. */
static void reset_rx(transport_t *p_transport)
{
    p_transport->rx.cmd = 0;
    p_transport->rx.total_frame = 0;
    p_transport->rx.frame_seq = 0;
    p_transport->rx.bytes_received = 0;

    if (p_transport->timeout != 0) {
        os_timer_stop(&p_transport->rx.timer);
    }
}

static void on_tx_timeout(void *arg1, void *arg2)
{
    transport_t *p_transport = (transport_t *)arg2;

    BREEZE_LOG_ERR("tx timeout\r\n");
    reset_tx(p_transport);
}

static void on_rx_timeout(void *arg1, void *arg2)
{
    transport_t *p_transport = (transport_t *)arg2;

    BREEZE_LOG_ERR("rx timeout\r\n");
    reset_rx(p_transport);
}

static bool is_valid_rx_command(uint8_t cmd) {
    if (cmd == BZ_CMD_CTRL ||
        cmd == BZ_CMD_QUERY ||
        cmd == BZ_CMD_EXT_DOWN ||
        cmd == BZ_CMD_AUTH_REQ ||
        cmd == BZ_CMD_AUTH_CFM ||
        cmd == BZ_CMD_OTA_VER_REQ ||
        cmd == BZ_CMD_OTA_REQ ||
        cmd == BZ_CMD_OTA_SIZE ||
        cmd == BZ_CMD_OTA_DONE ||
        cmd == BZ_CMD_OTA_DATA) {
        return true;
    }
    return false;
}

static bool is_valid_tx_command(uint8_t cmd) {
    if (cmd == BZ_CMD_STATUS ||
        cmd == BZ_CMD_REPLY ||
        cmd == BZ_CMD_EXT_UP ||
        cmd == BZ_CMD_AUTH_RAND ||
        cmd == BZ_CMD_AUTH_RSP ||
        cmd == BZ_CMD_AUTH_KEY ||
        cmd == BZ_CMD_OTA_VER_RSP ||
        cmd == BZ_CMD_OTA_RSP ||
        cmd == BZ_CMD_OTA_PUB_SIZE ||
        cmd == BZ_CMD_OTA_CHECK_RESULT ||
        cmd == BZ_CMD_OTA_UPDATE_PROCESS ||
        cmd == BZ_CMD_ERR) {
        return true;
    }
    return false;
}

static void do_encrypt(transport_t *p_transport, uint8_t *data, uint16_t len)
{
    uint16_t bytes_to_pad, blk_num = len >> 4;
    uint8_t *decrypt_buf;
    uint8_t encrypt_data[ENCRYPT_DATA_SIZE];

    bytes_to_pad = (AES_BLK_SIZE - len % AES_BLK_SIZE) % AES_BLK_SIZE;
    if (bytes_to_pad) {
        memset(data + len, 0, bytes_to_pad);
        p_transport->tx.zeroes_padded = bytes_to_pad;
        blk_num++;
        p_transport->tx.buff[3] += bytes_to_pad;
    }
    ais_aes128_cbc_encrypt(p_transport->p_aes_ctx, data, blk_num, encrypt_data);
    memcpy(data, encrypt_data, blk_num << 4);
}

static void do_decrypt(transport_t *p_transport, uint8_t *data, uint16_t len)
{
    uint16_t blk_num = len >> 4;
    uint8_t *buffer;
    uint8_t decrypt_data[ENCRYPT_DATA_SIZE];

    ais_aes128_cbc_decrypt(p_transport->p_aes_ctx, data, blk_num, decrypt_data);
    memcpy(data, decrypt_data, len);
}

/**@brief Build packet. */
static uint32_t build_packet(transport_t *p_transport, uint8_t *data,
                             uint16_t len)
{
    uint32_t ret = BZ_SUCCESS;

    p_transport->tx.zeroes_padded = 0;
    p_transport->tx.buff[0] = ((BZ_TRANSPORT_VER & 0x7) << 5) |
                              ((p_transport->tx.encrypted & 0x1) << 4) |
                              (p_transport->tx.msg_id & 0xF);
    p_transport->tx.buff[1] = p_transport->tx.cmd;
    p_transport->tx.buff[2] = ((p_transport->tx.total_frame & 0x0F) << 4) |
                              (p_transport->tx.frame_seq & 0x0F);
    p_transport->tx.buff[3] = len;

    /* Payload */
    if (len != 0) {
        memcpy(p_transport->tx.buff + HEADER_SIZE, data, len);
        if (p_transport->tx.encrypted != 0) {
            do_encrypt(p_transport, p_transport->tx.buff + HEADER_SIZE, len);
        }
    }

    if(g_dn_complete == false){
        p_transport->tx.buff[0] &= (~(0x01 <<4));
    }

    if (g_dn_complete == true){
        p_transport->tx.buff[3] = len;
    }

    return ret;
}

/**@brief Function to get number of Tx bytes left.
 */
static uint16_t tx_bytes_left(transport_t *p_transport)
{
    return (p_transport->tx.len - p_transport->tx.bytes_sent);
}

/**@brief Function to check if Rx is on-going. */
static bool rx_frames_left(transport_t *p_transport)
{
    return (p_transport->rx.total_frame != p_transport->rx.frame_seq);
}

static ret_code_t send_fragment(transport_t *p_transport)
{
    ret_code_t ret = BZ_SUCCESS;
    uint16_t len, pkt_len, bytes_left;
    uint16_t pkt_payload_len = p_transport->max_pkt_size - HEADER_SIZE;
    uint16_t pkt_sent = 0;

    bytes_left = tx_bytes_left(p_transport);

    if (p_transport->tx.encrypted != 0) {
        pkt_payload_len &= ~(AES_BLK_SIZE - 1);
    }

    do {
        len = MIN(bytes_left, pkt_payload_len);
        build_packet(p_transport, p_transport->tx.data + p_transport->tx.bytes_sent, len);
        pkt_len = len + p_transport->tx.zeroes_padded + HEADER_SIZE;
        ret = p_transport->tx.active_func(p_transport->tx.buff, pkt_len);
        if (ret == BZ_SUCCESS) {
            p_transport->tx.pkt_req++;
            p_transport->tx.frame_seq++;
            p_transport->tx.bytes_sent += len;
            bytes_left = tx_bytes_left(p_transport);
            pkt_sent++;
        }
        if (ret != BZ_SUCCESS ||
            p_transport->tx.active_func == ble_ais_send_indication) {
            break;
        }
    }  while (bytes_left > 0);

    /* Start Tx timeout timer */
    if ((bytes_left != 0) && (p_transport->timeout != 0)) {
        os_timer_start(&p_transport->tx.timer);
    }
    os_post_event(OS_EV_BLE, OS_EV_CODE_BLE_TX_COMPLETED, pkt_sent);
    return ret;
}

ret_code_t transport_init(transport_t *p_transport, ali_init_t const *p_init)
{
    ret_code_t ret = BZ_SUCCESS;

    /* Initialize context */
    memset(p_transport, 0, sizeof(transport_t));
    p_transport->max_pkt_size = GATT_MTU_SIZE_DEFAULT - 3;
    p_transport->timeout = p_init->transport_timeout;

    if (p_transport->timeout != 0) {
        os_timer_t *tx_timer = &p_transport->tx.timer;
        os_timer_t *rx_timer = &p_transport->rx.timer;

        ret = os_timer_new(tx_timer, on_tx_timeout, p_transport,
                           p_transport->timeout);
        VERIFY_SUCCESS(ret);

        ret = os_timer_new(rx_timer, on_rx_timeout, p_transport,
                           p_transport->timeout);
        VERIFY_SUCCESS(ret);
    }

    return ret;
}

void transport_reset(transport_t *p_transport)
{
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);

    /* reset Tx/Rx state machine */
    reset_tx(p_transport);
    reset_rx(p_transport);

    ais_aes128_destroy(p_transport->p_aes_ctx);
    p_transport->p_aes_ctx = NULL;
    g_dn_complete = false;
}

ret_code_t transport_tx(transport_t *p_transport,
                        uint8_t tx_type, uint8_t cmd,
                        uint8_t const *const p_data, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_transport);

    uint16_t pkt_payload_len;

    if (p_data == NULL && length != 0) {
        return BZ_ENULL;
    }

    /* Check if packet encryption is required. */
    if (p_transport->p_key != NULL &&
        (cmd == BZ_CMD_STATUS || cmd == BZ_CMD_REPLY || cmd == BZ_CMD_EXT_UP ||
         ((cmd & BZ_CMD_TYPE_MASK) == BZ_CMD_AUTH && cmd != BZ_CMD_AUTH_RAND))) {
        p_transport->tx.encrypted = 1;
        pkt_payload_len = (p_transport->max_pkt_size - HEADER_SIZE) & ~(AES_BLK_SIZE - 1);
    } else {
        p_transport->tx.encrypted = 0;
        pkt_payload_len = p_transport->max_pkt_size - HEADER_SIZE;
    }

    if (tx_bytes_left(p_transport) != 0 ||
        p_transport->tx.pkt_req != p_transport->tx.pkt_cfm) {
        return BZ_EBUSY;
    }

    p_transport->tx.data = (uint8_t *)p_data;
    p_transport->tx.len = length;
    p_transport->tx.bytes_sent = 0;
    p_transport->tx.cmd = cmd;
    p_transport->tx.frame_seq = 0;
    p_transport->tx.pkt_req = 0;
    p_transport->tx.pkt_cfm = 0;

    if (cmd == BZ_CMD_REPLY || cmd == BZ_CMD_EXT_UP) {
        p_transport->tx.msg_id = p_transport->rx.msg_id;
    }

    if (cmd == BZ_CMD_STATUS) {
        p_transport->tx.msg_id = 0;
    }

    p_transport->tx.total_frame = length / pkt_payload_len;
    if (p_transport->tx.total_frame * pkt_payload_len == length && length != 0) {
        p_transport->tx.total_frame--;
    }

    /* Check if notification or indication. */
    if (tx_type == TX_NOTIFICATION) {
        p_transport->tx.active_func = ble_ais_send_notification;
    } else {
        p_transport->tx.active_func = ble_ais_send_indication;
    }

    send_fragment(p_transport);
    return BZ_SUCCESS;
}

static void data_notify(uint8_t type, uint8_t *data, uint16_t len)
{
    ali_event_t evt;

    evt.type = type;
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    g_core->event_handler(g_core->p_evt_context, &evt);
}

static void notify_query_data(core_t *p_ali, uint8_t *data, uint16_t len)
{
    ali_event_t evt;

    /* send event to higher layer. */
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    p_ali->event_handler(p_ali->p_evt_context, &evt);
}


void transport_rx(transport_t *p_transport, uint8_t *p_data, uint16_t length)
{
    uint16_t              len, buff_left;
    uint32_t              err_code;

    /* Check parameters. */
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0) {
        return;
    } else if ((length - HEADER_SIZE + p_transport->rx.bytes_received) > RX_BUFF_LEN) {
        core_handle_err(ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE, BZ_EDATASIZE);
        reset_rx(p_transport);
        return;
    }

    /* Check if the 1st fragment. */
    if (!rx_frames_left(p_transport)) {
        if (GET_FRM_SEQ(p_data) != 0) {
            core_handle_err(ALI_ERROR_SRC_TRANSPORT_1ST_FRAME, BZ_EINVALIDDATA);
            reset_rx(p_transport);
            return;
        }

        /* Backup information from the 1st packet. */
        p_transport->rx.msg_id         = GET_MSG_ID(p_data);
        p_transport->rx.cmd            = GET_CMD(p_data);
        p_transport->rx.total_frame    = GET_TOTAL_FRM(p_data);
        p_transport->rx.frame_seq      = 0;
        p_transport->rx.bytes_received = 0;
    } else {
        /* Check if header fields are correct. */
        if ((p_transport->rx.msg_id != GET_MSG_ID(p_data)) ||
            (p_transport->rx.cmd != GET_CMD(p_data)) ||
            (p_transport->rx.total_frame != GET_TOTAL_FRM(p_data)) ||
            (((p_transport->rx.frame_seq + 1) & 0xF) != GET_FRM_SEQ(p_data) &&
             p_transport->rx.cmd != BZ_CMD_OTA_DATA)) {
            core_handle_err(ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES, BZ_EINVALIDDATA);
            reset_rx(p_transport);
            return;
        } else if ((((p_transport->rx.frame_seq + 1) & 0xF) !=
                      GET_FRM_SEQ(p_data) &&
                    p_transport->rx.cmd == BZ_CMD_OTA_DATA)) {
            core_handle_err(ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC, BZ_EINVALIDDATA);
            reset_rx(p_transport);
            return;
        } else {
            p_transport->rx.frame_seq = GET_FRM_SEQ(p_data);
        }
    }

    /* Check encrypted message. */
    if (CHECK_ENC(p_data) != 0) {
        /* Check if encrypted messate is aligned to 16 bytes. */
        if ((length - HEADER_SIZE) % 16 != 0) {
            core_handle_err(ALI_ERROR_SRC_TRANSPORT_ENCRYPTED, BZ_EINVALIDDATA);
            reset_rx(p_transport);
            return;
        }

        /* Check if key is present. */
        if (p_transport->p_key == NULL) {
            core_handle_err(ALI_ERROR_SRC_TRANSPORT_ENCRYPTED, BZ_EFORBIDDEN);
            reset_rx(p_transport);
            return;
        }
    }

    /* Check data length. */
    if ((length != HEADER_SIZE + GET_LEN(p_data) &&
         CHECK_ENC(p_data) == 0) // Must be exact when wihtout encryption.
        || (length < HEADER_SIZE + GET_LEN(p_data) && CHECK_ENC(p_data) != 0)) {
        core_handle_err(ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES, BZ_EDATASIZE);
        reset_rx(p_transport);
        return;
    }

    /* Copy payload. */
    buff_left = RX_BUFF_LEN - p_transport->rx.bytes_received;
    if ((len = MIN(buff_left, GET_LEN(p_data))) > 0) {
        // In-place decryption.
        if (CHECK_ENC(p_data) != 0) {
            do_decrypt(p_transport, p_data + HEADER_SIZE, length - HEADER_SIZE);
        }

        memcpy(p_transport->rx.buff + p_transport->rx.bytes_received,
               p_data + HEADER_SIZE, len);
        p_transport->rx.bytes_received += len;
    }

    /* Check if all the frames have been received. */
    if (!rx_frames_left(p_transport)) {
        if (!is_valid_rx_command(p_transport->rx.cmd)) {
            return;
        }
        if (p_transport->rx.bytes_received != 0) {
            if (p_transport->rx.cmd == BZ_CMD_QUERY) {
                data_notify(BZ_EVENT_RX_QUERY, p_transport->rx.buff, p_transport->rx.bytes_received);
            } else if (p_transport->rx.cmd == BZ_CMD_CTRL) {
                data_notify(BZ_EVENT_RX_CTRL, p_transport->rx.buff, p_transport->rx.bytes_received);
            }
        }

        auth_rx_command(&g_core->auth, p_transport->rx.cmd,
                        p_transport->rx.buff, p_transport->rx.bytes_received);
        extern void notify_ota_command(uint8_t cmd, uint8_t num_frame, uint8_t *data, uint16_t len);
        notify_ota_command(p_transport->rx.cmd, p_transport->rx.frame_seq + 1,
                           p_transport->rx.buff, p_transport->rx.bytes_received);
        extcmd_rx_command(&g_core->ext, p_transport->rx.cmd,
                          p_transport->rx.buff, p_transport->rx.bytes_received);
        reset_rx(p_transport);
    } else {
        if (p_transport->timeout != 0) {
            err_code = os_timer_start(&p_transport->rx.timer);
            VERIFY_SUCCESS_VOID(err_code);
        }
    }
}

void transport_txdone(transport_t *p_transport, uint16_t pkt_sent)
{
    uint32_t err_code = BZ_SUCCESS;
    uint16_t bytes_left;

    /* Check parameters. */
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);

    p_transport->tx.pkt_cfm += pkt_sent;

    /* Check whether there are still data to be sent. */
    bytes_left = tx_bytes_left(p_transport);
    if (bytes_left != 0) {
        /* try sending until no tx packet or any other error. */
        send_fragment(p_transport);
    } else if (p_transport->tx.pkt_req == p_transport->tx.pkt_cfm &&
               p_transport->tx.pkt_req != 0) {
        if (!is_valid_tx_command(p_transport->tx.cmd)) {
            return;
        }
        event_notify(BZ_EVENT_TX_DONE);
        // TODO: move ota to upper layer
        notify_ota_event(ALI_OTA_ON_TX_DONE, p_transport->tx.cmd);
        reset_tx(p_transport);
    } else if (p_transport->tx.pkt_req < p_transport->tx.pkt_cfm) {
        reset_tx(p_transport);
        core_handle_err(ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT, BZ_EINTERNAL);
    }
}

uint32_t transport_update_key(uint8_t *key)
{
    transport_t *transport = &g_core->transport;
    char *iv = "0123456789ABCDEF";

    transport->p_key = key;
    if (transport->p_aes_ctx) {
        ais_aes128_destroy(transport->p_aes_ctx);
        transport->p_aes_ctx = NULL;
    }

    transport->p_aes_ctx = ais_aes128_init(transport->p_key, iv);
    return BZ_SUCCESS;
}
