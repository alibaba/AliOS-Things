/*
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 * 
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ali_transport.h"
#include "ali_error_src.h"
#include "aes.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
#include "ble_gatt.h"
#include <aos/aos.h>

#define MOD "ble_transport"

#define HEADER_SIZE                 4       /**< Size of packet header. */
#define MAX_NUM_OF_FRAMES           16      /**< Maximum number of frames. */
#define AES_BLK_SIZE                16      /**< AES block size. */

#define CHECK_ENC(data)             ((data[0] & 0x10) != 0)     /**< Check encryption bit. */
#define GET_MSG_ID(data)            (data[0] & 0xF)             /**< Get msg_id from data. */
#define GET_CMD(data)               (data[1])                   /**< Get cmd from data. */
#define GET_TOTAL_FRM(data)         ((data[2] >> 4) & 0x0F)     /**< Get total_frame from data. */
#define GET_FRM_SEQ(data)           (data[2] & 0x0F)            /**< Get frame_seq from data. */
#define GET_LEN(data)               (data[3])                   /**< Get length from data. */

#define rx_active(p)                rx_frames_left(p)           /**< Whether there are frames left behind. */

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/**@brief Reset Tx state machine. */
static void reset_tx (ali_transport_t * p_transport)
{
    uint32_t err_code;

    p_transport->tx.len         = 0;
    p_transport->tx.bytes_sent  = 0;
    p_transport->tx.msg_id      = 0;
    p_transport->tx.cmd         = 0;
    p_transport->tx.total_frame = 0;
    p_transport->tx.frame_seq   = 0;
    p_transport->tx.pkt_req     = 0;
    p_transport->tx.pkt_cfm     = 0;

    if (p_transport->timeout != 0)
    {
        err_code = aos_timer_stop(&p_transport->tx.timer); 
        VERIFY_SUCCESS_VOID(err_code);
    }
}


/**@brief Reset Rx state machine. */
static void reset_rx (ali_transport_t * p_transport)
{
    uint32_t err_code;

    p_transport->rx.cmd         = 0;
    p_transport->rx.total_frame = 0;
    p_transport->rx.frame_seq   = 0;
    p_transport->rx.bytes_received = 0;

    if (p_transport->timeout != 0)
    {
        err_code = aos_timer_stop(&p_transport->rx.timer); 
        VERIFY_SUCCESS_VOID(err_code);
    }
}


/**@brief Tx timeout handler.
 */
static void on_tx_timeout_helper(ali_transport_t * p_transport)
{
    ali_transport_event_t evt;

    /* send event to higher layer. */
    evt.type                 = ALI_TRANSPORT_EVT_TX_TIMEOUT;
    evt.data.rxtx.p_data     = p_transport->tx.data;
    evt.data.rxtx.length     = p_transport->tx.len;
    evt.data.rxtx.cmd        = p_transport->tx.cmd;
    evt.data.rxtx.num_frames = p_transport->tx.frame_seq + 1;
    p_transport->event_handler(p_transport->p_evt_context, &evt);

    /* clean up */
    reset_tx(p_transport);
}

static void on_tx_timeout(void *arg1, void *arg2)
{
    ali_transport_t *p_transport = (ali_transport_t *)arg1;

    on_tx_timeout_helper(p_transport);
}


/**@brief Rx timeout handler.
 */
static void on_rx_timeout_helper(ali_transport_t * p_transport)
{
    ali_transport_event_t evt;

    /* send event to higher layer. */
    evt.type                 = ALI_TRANSPORT_EVT_RX_TIMEOUT;
    evt.data.rxtx.p_data     = p_transport->rx.buff;
    evt.data.rxtx.length     = p_transport->rx.bytes_received;
    evt.data.rxtx.cmd        = p_transport->rx.cmd;
    evt.data.rxtx.num_frames = p_transport->rx.frame_seq + 1;
    p_transport->event_handler(p_transport->p_evt_context, &evt);

    /* clean up */
    reset_rx(p_transport);
}

static void on_rx_timeout(void *arg1, void *arg2)
{
    ali_transport_t *p_transport = (ali_transport_t *)arg1;

    on_rx_timeout_helper(p_transport);
}

#define notify_error(p_transport, src, err_code) \
    do {\
        printf("We are in %s %d\r\n", __FILE__, __LINE__);\
        notify_error2(p_transport, src, err_code);\
    } while (0)

/**@brief Report error. */
static void notify_error2(ali_transport_t * p_transport, uint32_t src, uint32_t err_code)
{
    ali_transport_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_TRANSPORT_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_transport->event_handler(p_transport->p_evt_context, &evt);
}

/**@brief Encryption. */
static void encrypt (ali_transport_t * p_transport, uint8_t * data, uint16_t len)
{
    uint16_t bytes_encrypted = 0;
    uint16_t bytes_to_pad, l_len;

    while (bytes_encrypted < len)
    {
        // Zero padding, also known as "No-padding"  in specification v1.0.4
        l_len = MIN(len - bytes_encrypted, AES_BLK_SIZE);
        if ((bytes_to_pad = AES_BLK_SIZE - l_len) != 0)
        {
            p_transport->tx.zeroes_padded = bytes_to_pad;
            memset(data + bytes_encrypted + AES_BLK_SIZE - bytes_to_pad, 0, bytes_to_pad);
        }

        /* ECB engine. */
        memcpy(p_transport->tx.ecb_context.data + bytes_encrypted, data + bytes_encrypted, AES_BLK_SIZE);
        AES_Encrypt(p_transport->tx.ecb_context.data + bytes_encrypted, p_transport->tx.ecb_context.key);
        memcpy(data + bytes_encrypted, p_transport->tx.ecb_context.data + bytes_encrypted, AES_BLK_SIZE);
        bytes_encrypted += l_len;
    }
}

/**@brief Decryption. */
static void decrypt (ali_transport_t * p_transport, uint8_t * data, uint16_t len)
{
    uint16_t bytes_decrypted = 0;

    while (bytes_decrypted < len)
    {
        AES_Decrypt(data + bytes_decrypted, p_transport->p_key);
        bytes_decrypted += AES_BLK_SIZE;
    }
}


/**@brief Build packet. */
static uint32_t build_packet (ali_transport_t * p_transport, uint8_t * data, uint16_t len)
{
    uint32_t ret = NRF_SUCCESS;

    if (p_transport->max_pkt_size < len + HEADER_SIZE)
    {
        return NRF_ERROR_NO_MEM;
    }

    p_transport->tx.zeroes_padded = 0;

    /* Header */
    p_transport->tx.buff[0] = ((ALI_TRANSPORT_VERSION     & 0x7) << 5)
                            | ((p_transport->tx.encrypted & 0x1) << 4)
                            |  (p_transport->tx.msg_id    & 0xF);

    p_transport->tx.buff[1] = p_transport->tx.cmd;

    p_transport->tx.buff[2] = ((p_transport->tx.total_frame & 0x0F) << 4)
                            | (p_transport->tx.frame_seq & 0x0F);

    p_transport->tx.buff[3] = len;

    /* Payload */
    if (len != 0)
    {
        memcpy(p_transport->tx.buff + HEADER_SIZE, data, len);

        // In-place encryption.
        if (p_transport->tx.encrypted != 0)
        {
            encrypt (p_transport, p_transport->tx.buff + HEADER_SIZE, len);
        }
    }
    return ret;
}


/**@brief Function to get number of Tx bytes left.
 */
static uint16_t tx_bytes_left (ali_transport_t * p_transport)
{
    return (p_transport->tx.len - p_transport->tx.bytes_sent);
}


/**@brief Function to check if Rx is on-going. */
static bool rx_frames_left (ali_transport_t * p_transport)
{
    return (p_transport->rx.total_frame != p_transport->rx.frame_seq);
}


/**@brief Function to try sending.
 */
static ret_code_t try_send (ali_transport_t * p_transport)
{
    ret_code_t ret = NRF_SUCCESS;
    uint16_t len, pkt_len, bytes_left;
    uint16_t pkt_payload_len = p_transport->max_pkt_size - HEADER_SIZE;
    uint16_t pkt_sent;

    LOGD(MOD, "try_send entry.");

    bytes_left = tx_bytes_left(p_transport);

    LOGD(MOD, "%s %d total %d bytes for sending.", __FILE__, __LINE__, bytes_left);

    if (p_transport->tx.encrypted != 0)
    {
        pkt_payload_len &= ~(AES_BLK_SIZE - 1);
    }

    do
    {
        len = MIN(bytes_left, pkt_payload_len);
        ret = build_packet (p_transport, p_transport->tx.data + p_transport->tx.bytes_sent, len);
        VERIFY_SUCCESS(ret);

        pkt_len = len + p_transport->tx.zeroes_padded + HEADER_SIZE;
        ret = p_transport->tx.active_func(p_transport->tx.p_context,
                                          p_transport->tx.buff,
                                          pkt_len);
        if (ret == NRF_SUCCESS)
        {
            p_transport->tx.pkt_req++;
            p_transport->tx.frame_seq++;
            p_transport->tx.bytes_sent += len;
            bytes_left = tx_bytes_left(p_transport);
        }
#if 0
        else if (ret == BLE_ERROR_NO_TX_PACKETS)
        {
            ret = NRF_SUCCESS;
            break;              // wait until timeout or tx-done
        }
#endif
        else if (ret == NRF_ERROR_BUSY && p_transport->tx.active_func == p_transport->tx.indicate_func)
        {
            ret = NRF_SUCCESS;
            break;              // wait until timeout or tx-done
        }
        else
        {
            VERIFY_SUCCESS(ret);
        }
    } while (bytes_left > 0) ;  // send until there are still bytes

    /* Start Tx timeout timer */
    if ((bytes_left != 0) && (p_transport->timeout != 0))
    {
        ret = aos_timer_start(&p_transport->tx.timer);
        VERIFY_SUCCESS(ret);
    }
    pkt_sent = p_transport->tx.len / pkt_payload_len;
    if ((pkt_sent * pkt_payload_len < p_transport->tx.len &&
        p_transport->tx.len != 0) || p_transport->tx.cmd == ALI_CMD_ERROR) {
        pkt_sent++;
    }
    aos_post_event(EV_BLE, CODE_BLE_TX_COMPLETED, pkt_sent);

    LOGD(MOD, "try_send exit.");

    return ret;
}


ret_code_t ali_transport_init(ali_transport_t * p_transport, ali_transport_init_t const * p_init)
{
    ret_code_t ret = NRF_SUCCESS;

    LOGD(MOD, "ali_transport_init entry.");

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_transport);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->tx_buffer);
    VERIFY_PARAM_NOT_NULL(p_init->rx_buffer);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func_notify);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func_indicate);
    if (p_init->tx_buffer_len == 0 || p_init->rx_buffer_len == 0)
    {
        return NRF_ERROR_NULL;
    }

    /* Initialize context */
    memset(p_transport, 0, sizeof(ali_transport_t));
    p_transport->tx.buff           = p_init->tx_buffer;
    p_transport->tx.buff_size      = p_init->tx_buffer_len;
    p_transport->rx.buff           = p_init->rx_buffer;
    p_transport->rx.buff_size      = p_init->rx_buffer_len;
    p_transport->max_pkt_size      = GATT_MTU_SIZE_DEFAULT - 3;
    p_transport->timeout           = p_init->timeout;
    p_transport->event_handler     = p_init->event_handler;
    p_transport->p_evt_context     = p_init->p_evt_context;
    p_transport->p_key             = p_init->p_key;
    p_transport->tx.notify_func    = p_init->tx_func_notify;
    p_transport->tx.indicate_func  = p_init->tx_func_indicate;
    p_transport->tx.p_context      = p_init->p_tx_func_context;

    /* Initialize ECB context. */
    if (p_transport->p_key != NULL)
    {
        memcpy(p_transport->tx.ecb_context.key, p_transport->p_key, AES_BLK_SIZE);
    }

    /* Initialize Tx and Rx timeout timers. */
    if (p_transport->timeout != 0)
    {
        aos_timer_t *tx_timer = &p_transport->tx.timer;
        aos_timer_t *rx_timer = &p_transport->rx.timer;

        ret = aos_timer_new_ext(tx_timer, on_tx_timeout, p_transport, p_transport->timeout, 0, 0);
        VERIFY_SUCCESS(ret);

        ret = aos_timer_new_ext(rx_timer, on_rx_timeout, p_transport, p_transport->timeout, 0, 0);
        VERIFY_SUCCESS(ret);
    }

    LOGD(MOD, "ali_transport_init exit.");

    return ret;
}


void ali_transport_reset(ali_transport_t * p_transport)
{
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);

    /* reset Tx/Rx state machine */
    reset_tx(p_transport);
    reset_rx(p_transport);
}


ret_code_t ali_transport_send(ali_transport_t * p_transport, ali_transport_tx_type_t tx_type,
                              uint8_t cmd, uint8_t const * const p_data, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_transport);

    uint16_t pkt_payload_len;

    /* Check parameters. */
    if (p_data == NULL && length != 0)
    {
        return NRF_ERROR_NULL;
    }

    /* Check if packet encryption is required. */
    if (p_transport->p_key != NULL
        && (cmd == ALI_CMD_STATUS || cmd == ALI_CMD_REPLY
            || ((cmd & ALI_CMD_TYPE_MASK) == ALI_CMD_TYPE_AUTH && cmd != ALI_CMD_AUTH_RAND)))
    {
        p_transport->tx.encrypted = 1;
        pkt_payload_len = (p_transport->max_pkt_size - HEADER_SIZE) & ~(AES_BLK_SIZE - 1);
    }
    else
    {
        p_transport->tx.encrypted = 0;
        pkt_payload_len = p_transport->max_pkt_size - HEADER_SIZE;
    }

    /* Continue checking parameters. */
    if (length > ALI_TRANSPORT_MAX_TX_DATA_LEN
        || length > MAX_NUM_OF_FRAMES * pkt_payload_len)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    /* Check if there is on-going Tx. */
    if (tx_bytes_left(p_transport) != 0
        || p_transport->tx.pkt_req != p_transport->tx.pkt_cfm)
    {
        return NRF_ERROR_BUSY;
    }

    p_transport->tx.data       = (uint8_t *)p_data;
    p_transport->tx.len        = length;
    p_transport->tx.bytes_sent = 0;
    p_transport->tx.cmd        = cmd;
    p_transport->tx.frame_seq  = 0;
    p_transport->tx.pkt_req    = 0;
    p_transport->tx.pkt_cfm    = 0;

    if (cmd == ALI_CMD_STATUS || cmd == ALI_CMD_REPLY)
    {
        p_transport->tx.msg_id = p_transport->rx.msg_id;
    }

    /* Total # of frames. */
    p_transport->tx.total_frame = length / pkt_payload_len;
    if (p_transport->tx.total_frame * pkt_payload_len == length && length != 0)
    {
        p_transport->tx.total_frame--;
    }

    /* Check if notification or indication. */
    if (tx_type == ALI_TRANSPORT_TX_TYPE_NOTIFY)
    {
        p_transport->tx.active_func = p_transport->tx.notify_func;
    }
    else
    {
        p_transport->tx.active_func = p_transport->tx.indicate_func;
    }

    /* try sending until no tx packet or any other error. */
    return try_send (p_transport);
}


void ali_transport_on_rx_data(ali_transport_t * p_transport, uint8_t * p_data, uint16_t length)
{
    uint16_t len, buff_left;
    uint32_t err_code;
    ali_transport_event_t evt;

    /* Check parameters. */
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0)
    {
        return;
    }
    else if ((length - HEADER_SIZE + p_transport->rx.bytes_received) > p_transport->rx.buff_size)
    {
        reset_rx (p_transport);
        notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE, NRF_ERROR_DATA_SIZE);
        return;
    }

    /* Check if the 1st fragment. */
    if (!rx_active (p_transport))
    {
        if (GET_FRM_SEQ(p_data) != 0)
        {
            notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_1ST_FRAME, NRF_ERROR_INVALID_DATA);
            return;
        }

        /* Backup information from the 1st packet. */
        p_transport->rx.msg_id         = GET_MSG_ID(p_data);
        p_transport->rx.cmd            = GET_CMD(p_data);
        p_transport->rx.total_frame    = GET_TOTAL_FRM(p_data);
        p_transport->rx.frame_seq      = 0;
        p_transport->rx.bytes_received = 0;
    }
    else
    {
        /* Check if header fields are correct. */
        if ((p_transport->rx.msg_id != GET_MSG_ID(p_data))
            || (p_transport->rx.cmd != GET_CMD(p_data))
            || (p_transport->rx.total_frame != GET_TOTAL_FRM(p_data))
            || (((p_transport->rx.frame_seq + 1) & 0xF) != GET_FRM_SEQ(p_data) && p_transport->rx.cmd != ALI_CMD_FW_DATA))
        {
            notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES, NRF_ERROR_INVALID_DATA);
            reset_rx(p_transport);
            return;
        }
        else if ((((p_transport->rx.frame_seq + 1) & 0xF) != GET_FRM_SEQ(p_data) && p_transport->rx.cmd == ALI_CMD_FW_DATA))
        {
            /* Special case for FW data (cmd=0x2F) */
            notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC, NRF_ERROR_INVALID_DATA);
            reset_rx(p_transport);
            return;
        }
        else
        {
            p_transport->rx.frame_seq = GET_FRM_SEQ(p_data);
        }
    }

    /* Check encrypted message. */
    if (CHECK_ENC(p_data) != 0)
    {
        /* Check if encrypted messate is aligned to 16 bytes. */
        if ((length - HEADER_SIZE) % 16 != 0)
        {
            notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_ENCRYPTED, NRF_ERROR_INVALID_DATA);
            reset_rx(p_transport);
            return;
        }

        /* Check if key is present. */
        if (p_transport->p_key == NULL)
        {
            notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_ENCRYPTED, NRF_ERROR_FORBIDDEN);
            reset_rx(p_transport);
            return;
        }
    }

    /* Check data length. */
    if ((length != HEADER_SIZE + GET_LEN(p_data) && CHECK_ENC(p_data) == 0)     // Must be exact when wihtout encryption.
        || (length < HEADER_SIZE + GET_LEN(p_data) && CHECK_ENC(p_data) != 0))
    {
        notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES, NRF_ERROR_DATA_SIZE);
        reset_rx(p_transport);
        return;
    }

    /* Copy payload. */
    buff_left = p_transport->rx.buff_size - p_transport->rx.bytes_received;
    if ((len = MIN(buff_left, GET_LEN(p_data))) > 0)
    {
        // In-place decryption.
        if (CHECK_ENC(p_data) != 0)
        {
            decrypt(p_transport, p_data + HEADER_SIZE, length - HEADER_SIZE);
        }
        memcpy(p_transport->rx.buff + p_transport->rx.bytes_received, p_data + HEADER_SIZE, len);
        p_transport->rx.bytes_received += len;
    }

    /* Check if all the frames have been received. */
    if (!rx_frames_left(p_transport))
    {
        /* send event to higher layer. */
        evt.type                 = ALI_TRANSPORT_EVT_RX_DONE;
        evt.data.rxtx.p_data     = p_transport->rx.buff;
        evt.data.rxtx.length     = p_transport->rx.bytes_received;
        evt.data.rxtx.cmd        = p_transport->rx.cmd;
        evt.data.rxtx.num_frames = p_transport->rx.frame_seq + 1;

        /* Reset Rx state machine. */
        reset_rx (p_transport);

        p_transport->event_handler(p_transport->p_evt_context, &evt);
    }
    else
    {
        if (p_transport->timeout != 0)
        {
            err_code = aos_timer_start(&p_transport->rx.timer);
            VERIFY_SUCCESS_VOID(err_code);
        }
    }
}


void ali_transport_on_tx_complete(ali_transport_t * p_transport, uint16_t pkt_sent)
{
    uint32_t err_code = NRF_SUCCESS;
    uint16_t bytes_left;
    ali_transport_event_t evt;

    /* Check parameters. */
    VERIFY_PARAM_NOT_NULL_VOID(p_transport);

    p_transport->tx.pkt_cfm += pkt_sent;

    /* Check whether there are still data to be sent. */
    bytes_left = tx_bytes_left(p_transport);
    if (bytes_left != 0)
    {
        LOGD(MOD, "bytes_left %d, please continue to sending.", bytes_left);
        /* try sending until no tx packet or any other error. */
        err_code = try_send (p_transport);
        VERIFY_SUCCESS_VOID(err_code);
    }
    else if (p_transport->tx.pkt_req == p_transport->tx.pkt_cfm
             && p_transport->tx.pkt_req != 0)
    {
        LOGD(MOD, "All bytes sent, let's notify higher level.");
        /* send event to higher layer. */
        evt.type                 = ALI_TRANSPORT_EVT_TX_DONE;
        evt.data.rxtx.p_data     = p_transport->tx.data;
        evt.data.rxtx.length     = p_transport->tx.len;
        evt.data.rxtx.cmd        = p_transport->tx.cmd;
        evt.data.rxtx.num_frames = p_transport->tx.frame_seq + 1;

        /* clean up */
        reset_tx (p_transport);

        p_transport->event_handler(p_transport->p_evt_context, &evt);
    }
    else if (p_transport->tx.pkt_req < p_transport->tx.pkt_cfm)
    {
        LOGD(MOD, "Something goes to wrong, let's reset tx.");
        reset_tx (p_transport);
        notify_error(p_transport, ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT, NRF_ERROR_INTERNAL);
    }

    LOGD(MOD, "%s exit.", __func__);
}


uint32_t ali_transport_set_mtu(ali_transport_t * p_transport, uint16_t mtu)
{
    VERIFY_PARAM_NOT_NULL(p_transport);

    if (mtu == 0
        || mtu - 3 > p_transport->tx.buff_size
        || mtu - 3 > p_transport->rx.buff_size)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    /* Assign MTU, which will take effect  */
    p_transport->max_pkt_size = mtu - 3;
    return NRF_SUCCESS;
}


uint32_t ali_transport_set_key(ali_transport_t * p_transport, uint8_t * p_key)
{
    VERIFY_PARAM_NOT_NULL(p_transport);
    VERIFY_PARAM_NOT_NULL(p_key);

    /* Copy key, which will take effect when encoding the next fragment. */
    p_transport->p_key = p_key;
    memcpy(p_transport->tx.ecb_context.key, p_transport->p_key, AES_BLK_SIZE);
    return NRF_SUCCESS;
}


