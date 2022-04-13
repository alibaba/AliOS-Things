/******************************************************************************
 *
 *  Copyright (C) 2016 Realtek Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/******************************************************************************
*
*   Module Name:
*       hci_h5.c
*
*   Abstract:
*       Contain HCI transport send/receive functions for UART H5 Interface.
*
*   Major Change History:
*         When             Who       What
*       ---------------------------------------------------------------
*       2016-09-23      cc   modified
*
*   Notes:
*         This is designed for UART H5 HCI Interface in Android 6.0.
*
******************************************************************************/
#define LOG_TAG "bt_h5_int"
#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
//#include <aos/osal_debug.h> //remove for build :yulong

#include "bt_skbuff.h"
#include "bt_list.h"

//////////////////#include <devices/hci.h>

#include <ble_os.h>

#define BT_DBG_ENABLED 0

#include <common/log.h>

#include "h5.h"

#ifndef YULONG_HCI
#define YULONG_HCI
#endif

#define H5_TRACE_DATA_ENABLE 0

#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define DATA_RETRANS_COUNT  40  //40*100 = 4000ms(4s)
#define SYNC_RETRANS_COUNT  20  //20*250 = 5000ms(5s)
#define CONF_RETRANS_COUNT  20

#define DATA_RETRANS_TIMEOUT_VALUE             1000 //ms
#define SYNC_RETRANS_TIMEOUT_VALUE             250
#define CONF_RETRANS_TIMEOUT_VALUE             500
#define WAIT_CT_BAUDRATE_READY_TIMEOUT_VALUE   500
#define H5_HW_INIT_READY_TIMEOUT_VALUE         40000//4

#define HCI_VSC_H5_INIT                0xFCEE


/* Maximum numbers of allowed internal
** outstanding command packets at any time
*/
#define INT_CMD_PKT_MAX_COUNT       8
#define INT_CMD_PKT_IDX_MASK        0x07


//HCI Event codes
#define HCI_CONNECTION_COMP_EVT             0x03
#define HCI_DISCONNECTION_COMP_EVT          0x05
#define HCI_COMMAND_COMPLETE_EVT            0x0E
#define HCI_COMMAND_STATUS_EVT              0x0F
#define HCI_NUM_OF_CMP_PKTS_EVT             0x13
#define HCI_BLE_EVT                         0x3E


#define PATCH_DATA_FIELD_MAX_SIZE     252
#define READ_DATA_SIZE  16

// HCI data types //
#define H5_RELIABLE_PKT         0x01
#define H5_UNRELIABLE_PKT       0x00

#define H5_ACK_PKT              0x00
#define HCI_COMMAND_PKT         0x01
#define HCI_ACLDATA_PKT         0x02
#define HCI_SCODATA_PKT         0x03
#define HCI_EVENT_PKT           0x04
#define H5_VDRSPEC_PKT          0x0E
#define H5_LINK_CTL_PKT         0x0F

#define H5_HDR_SEQ(hdr)         ((hdr)[0] & 0x07)
#define H5_HDR_ACK(hdr)         (((hdr)[0] >> 3) & 0x07)
#define H5_HDR_CRC(hdr)         (((hdr)[0] >> 6) & 0x01)
#define H5_HDR_RELIABLE(hdr)    (((hdr)[0] >> 7) & 0x01)
#define H5_HDR_PKT_TYPE(hdr)    ((hdr)[1] & 0x0f)
#define H5_HDR_LEN(hdr)         ((((hdr)[1] >> 4) & 0xff) + ((hdr)[2] << 4))
#define H5_HDR_SIZE             4

#define H5_CFG_SLID_WIN(cfg)    ((cfg) & 0x07)
#define H5_CFG_OOF_CNTRL(cfg)   (((cfg) >> 3) & 0x01)
#define H5_CFG_DIC_TYPE(cfg)    (((cfg) >> 4) & 0x01)
#define H5_CFG_VER_NUM(cfg)     (((cfg) >> 5) & 0x07)
#define H5_CFG_SIZE             1


#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)

/* Callback function for the returned event of internal issued command */
typedef void (*tTIMER_HANDLE_CBACK)(void *timer, void *arg);

typedef enum H5_RX_STATE {
    H5_W4_PKT_DELIMITER,
    H5_W4_PKT_START,
    H5_W4_HDR,
    H5_W4_DATA,
    H5_W4_CRC
} tH5_RX_STATE;

typedef enum H5_RX_ESC_STATE {
    H5_ESCSTATE_NOESC,
    H5_ESCSTATE_ESC
} tH5_RX_ESC_STATE;

typedef enum H5_LINK_STATE {
    H5_UNINITIALIZED,
    H5_INITIALIZED,
    H5_ACTIVE
} tH5_LINK_STATE;

#define H5_EVENT_RX                    0x0001
#define H5_EVENT_EXIT                  0x0200

static volatile uint8_t h5_retransfer_running = 0;
static volatile uint16_t h5_ready_events = 0;
static volatile uint8_t h5_data_ready_running = 0;

/* Control block for HCISU_H5 */
typedef struct HCI_H5_CB {
    uint32_t    int_cmd_rsp_pending;        /* Num of internal cmds pending for ack */

    uint8_t     sliding_window_size;
    uint8_t     oof_flow_control;
    uint8_t     dic_type;


    RTB_QUEUE_HEAD *unack;      // Unack'ed packets queue
    RTB_QUEUE_HEAD *rel;        // Reliable packets queue

    RTB_QUEUE_HEAD *unrel;      // Unreliable packets queue
    RTB_QUEUE_HEAD *recv_data;      // Unreliable packets queue


    uint8_t     rxseq_txack;        // rxseq == txack. // expected rx SeqNumber
    uint8_t     rxack;             // Last packet sent by us that the peer ack'ed //

    uint8_t     use_crc;
    uint8_t     is_txack_req;      // txack required? Do we need to send ack's to the peer? //

    // Reliable packet sequence number - used to assign seq to each rel pkt. */
    uint8_t     msgq_txseq;         //next pkt seq

    uint16_t    message_crc;
    uint32_t    rx_count;       //expected pkts to recv

    tH5_RX_STATE        rx_state;
    tH5_RX_ESC_STATE    rx_esc_state;
    tH5_LINK_STATE      link_estab_state;

    sk_buff     *rx_skb;
    sk_buff     *data_skb;
    sk_buff     *internal_skb;

    aos_timer_t     *timer_data_retrans;
    aos_timer_t     *timer_sync_retrans;
    aos_timer_t     *timer_conf_retrans;
    aos_timer_t     *timer_wait_ct_baudrate_ready;
    aos_timer_t     *timer_h5_hw_init_ready;

    uint32_t    data_retrans_count;
    uint32_t    sync_retrans_count;
    uint32_t    conf_retrans_count;

    aos_mutex_t mutex;
    aos_sem_t   cond;
    aos_task_t       thread_data_retrans;

    aos_mutex_t data_mutex;
    aos_sem_t  data_cond;
    aos_task_t       thread_data_ready_cb;

    uint8_t     cleanuping;

#ifndef YULONG_HCI
    aos_dev_t   *hci_dev;
#endif

} tHCI_H5_CB;

static tHCI_H5_CB rtk_h5;
static aos_mutex_t h5_wakeup_mutex;

extern int g_uart_id;
extern int g_bt_dis_pin;

/******************************************************************************
**  Variables
******************************************************************************/

/* Num of allowed outstanding HCI CMD packets */
volatile int num_hci_cmd_pkts = 1;

/******************************************************************************
**  Static function
******************************************************************************/
static aos_timer_t *OsAllocateTimer(tTIMER_HANDLE_CBACK timer_callback);
static int OsStartTimer(aos_timer_t *timerid, int msec, int mode);
static int OsStopTimer(aos_timer_t *timerid);
static uint16_t h5_wake_up();

static packet_recv h5_int_hal_callbacks;


/******************************************************************************
**  Externs
******************************************************************************/

//timer API for retransfer
static int h5_alloc_data_retrans_timer();
static int h5_free_data_retrans_timer();
static int h5_stop_data_retrans_timer();
static int h5_start_data_retrans_timer();

static int h5_alloc_sync_retrans_timer();
static int h5_free_sync_retrans_timer();
static int h5_stop_sync_retrans_timer();
static int h5_start_sync_retrans_timer();

static int h5_alloc_conf_retrans_timer();
static int h5_free_conf_retrans_timer();
static int h5_stop_conf_retrans_timer();
static int h5_start_conf_retrans_timer();

static int h5_alloc_wait_controller_baudrate_ready_timer();
static int h5_free_wait_controller_baudrate_ready_timer();
static int h5_start_wait_controller_baudrate_ready_timer();

static int h5_alloc_hw_init_ready_timer();
static int h5_free_hw_init_ready_timer();
static int h5_stop_hw_init_ready_timer();
static int h5_start_hw_init_ready_timer();

static uint32_t  hci_h5_receive_msg(uint8_t *byte, uint16_t length);

static int h5_enqueue(IN sk_buff *skb);

// bite reverse in bytes
// 00000001 -> 10000000
// 00000100 -> 00100000
static const uint8_t byte_rev_table[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};
// reverse bit
static __inline uint8_t bit_rev8(uint8_t byte)
{
    return byte_rev_table[byte];
}

// reverse bit
static __inline uint16_t bit_rev16(uint16_t x)
{
    return (bit_rev8(x & 0xff) << 8) | bit_rev8(x >> 8);
}

static const uint16_t crc_table[] = {
    0x0000, 0x1081, 0x2102, 0x3183,
    0x4204, 0x5285, 0x6306, 0x7387,
    0x8408, 0x9489, 0xa50a, 0xb58b,
    0xc60c, 0xd68d, 0xe70e, 0xf78f
};

// Initialise the crc calculator
#define H5_CRC_INIT(x) x = 0xffff

/**
* Add "d" into crc scope, caculate the new crc value
*
* @param crc crc data
* @param d one byte data
*/
static void h5_crc_update(uint16_t *crc, uint8_t d)
{
    uint16_t reg = *crc;

    reg = (reg >> 4) ^ crc_table[(reg ^ d) & 0x000f];
    reg = (reg >> 4) ^ crc_table[(reg ^ (d >> 4)) & 0x000f];

    *crc = reg;
}

/**
* Get crc data.
*
* @param h5 realtek h5 struct
* @return crc data
*/
static uint16_t h5_get_crc(tHCI_H5_CB *h5)
{
    uint16_t crc = 0;
    uint8_t *data = hci_skb_get_data(h5->rx_skb) + hci_skb_get_data_length(h5->rx_skb) - 2;
    crc = data[1] + (data[0] << 8);
    return crc;
}

/**
* Just add 0xc0 at the end of skb,
* we can also use this to add 0xc0 at start while there is no data in skb
*
* @param skb socket buffer
*/
static void h5_slip_msgdelim(sk_buff *skb)
{
    const char pkt_delim = 0xc0;
    memcpy(hci_skb_put(skb, 1), &pkt_delim, 1);
}

/**
* Slip ecode one byte in h5 proto, as follows:
* 0xc0 -> 0xdb, 0xdc
* 0xdb -> 0xdb, 0xdd
* 0x11 -> 0xdb, 0xde
* 0x13 -> 0xdb, 0xdf
* others will not change
*
* @param skb socket buffer
* @c pure data in the one byte
*/
static void h5_slip_one_byte(sk_buff *skb, uint8_t unencode_form)
{
    const signed char esc_c0[2] = { 0xdb, 0xdc };
    const signed char esc_db[2] = { 0xdb, 0xdd };
    const signed char esc_11[2] = { 0xdb, 0xde };
    const signed char esc_13[2] = { 0xdb, 0xdf };

    switch (unencode_form) {
        case 0xc0:
            memcpy(hci_skb_put(skb, 2), &esc_c0, 2);
            break;

        case 0xdb:
            memcpy(hci_skb_put(skb, 2), &esc_db, 2);
            break;

        case 0x11: {
            if (rtk_h5.oof_flow_control) {
                memcpy(hci_skb_put(skb, 2), &esc_11, 2);
            } else {
                memcpy(hci_skb_put(skb, 1), &unencode_form, 1);
            }
        }
        break;

        case 0x13: {
            if (rtk_h5.oof_flow_control) {
                memcpy(hci_skb_put(skb, 2), &esc_13, 2);
            } else {
                memcpy(hci_skb_put(skb, 1), &unencode_form, 1);
            }
        }
        break;

        default:
            memcpy(hci_skb_put(skb, 1), &unencode_form, 1);
    }
}

/**
* Decode one byte in h5 proto, as follows:
* 0xdb, 0xdc -> 0xc0
* 0xdb, 0xdd -> 0xdb
* 0xdb, 0xde -> 0x11
* 0xdb, 0xdf -> 0x13
* others will not change
*
* @param h5 realtek h5 struct
* @byte pure data in the one byte
*/
static void h5_unslip_one_byte(tHCI_H5_CB *h5, unsigned char byte)
{
    const uint8_t c0 = 0xc0, db = 0xdb;
    const uint8_t oof1 = 0x11, oof2 = 0x13;
    uint8_t *hdr = (uint8_t *)hci_skb_get_data(h5->rx_skb);

    if (H5_ESCSTATE_NOESC == h5->rx_esc_state) {
        if (0xdb == byte) {
            h5->rx_esc_state = H5_ESCSTATE_ESC;
            //h5->rx_count--;
        } else {
            memcpy(hci_skb_put(h5->rx_skb, 1), &byte, 1);

            //Check Pkt Header's CRC enable bit
            if (H5_HDR_CRC(hdr) && h5->rx_state != H5_W4_CRC) {
                h5_crc_update(&h5->message_crc, byte);
            }

            h5->rx_count--;
        }
    } else if (H5_ESCSTATE_ESC == h5->rx_esc_state) {
        switch (byte) {
            case 0xdc:
                memcpy(hci_skb_put(h5->rx_skb, 1), &c0, 1);

                if (H5_HDR_CRC(hdr) && h5->rx_state != H5_W4_CRC) {
                    h5_crc_update(&h5-> message_crc, 0xc0);
                }

                h5->rx_esc_state = H5_ESCSTATE_NOESC;
                h5->rx_count--;
                break;

            case 0xdd:
                memcpy(hci_skb_put(h5->rx_skb, 1), &db, 1);

                if (H5_HDR_CRC(hdr) && h5->rx_state != H5_W4_CRC) {
                    h5_crc_update(&h5-> message_crc, 0xdb);
                }

                h5->rx_esc_state = H5_ESCSTATE_NOESC;
                h5->rx_count--;
                break;

            case 0xde:
                memcpy(hci_skb_put(h5->rx_skb, 1), &oof1, 1);

                if (H5_HDR_CRC(hdr) && h5->rx_state != H5_W4_CRC) {
                    h5_crc_update(&h5-> message_crc, oof1);
                }

                h5->rx_esc_state = H5_ESCSTATE_NOESC;
                h5->rx_count--;
                break;

            case 0xdf:
                memcpy(hci_skb_put(h5->rx_skb, 1), &oof2, 1);

                if (H5_HDR_CRC(hdr) && h5->rx_state != H5_W4_CRC) {
                    h5_crc_update(&h5-> message_crc, oof2);
                }

                h5->rx_esc_state = H5_ESCSTATE_NOESC;
                h5->rx_count--;
                break;

            default:
                BT_ERR("Error: Invalid byte %02x after esc byte", byte);
                hci_skb_free(&h5->rx_skb);
                h5->rx_skb = NULL;
                h5->rx_state = H5_W4_PKT_DELIMITER;
                h5->rx_count = 0;
                break;
        }
    }
}

/**
* Prepare h5 packet, packet format as follow:
*  | LSB 4 octets  | 0 ~4095| 2 MSB
*  |packet header | payload | data integrity check |
*
* pakcket header fromat is show below:
*  | LSB 3 bits         | 3 bits             | 1 bits                       | 1 bits          |
*  | 4 bits     | 12 bits        | 8 bits MSB
*  |sequence number | acknowledgement number | data integrity check present | reliable packet |
*  |packet type | payload length | header checksum
*
* @param h5 realtek h5 struct
* @param data pure data
* @param len the length of data
* @param pkt_type packet type
* @return socket buff after prepare in h5 proto
*/
static sk_buff *h5_prepare_pkt(tHCI_H5_CB *h5, uint8_t *data, signed long len, signed long pkt_type)
{
    sk_buff *nskb;
    uint8_t hdr[4];
    uint16_t H5_CRC_INIT(h5_txmsg_crc);
    int rel, i;
    //BT_DBG("HCI h5_prepare_pkt");

    switch (pkt_type) {
        case HCI_ACLDATA_PKT:
        case HCI_COMMAND_PKT:
        case HCI_EVENT_PKT:
            rel = H5_RELIABLE_PKT;  // reliable
            break;

        case H5_ACK_PKT:
        case H5_VDRSPEC_PKT:
        case H5_LINK_CTL_PKT:
            rel = H5_UNRELIABLE_PKT;// unreliable
            break;

        default:
            BT_ERR("Unknown packet type");
            return NULL;
    }

    // Max len of packet: (original len +4(h5 hdr) +2(crc))*2
    //   (because bytes 0xc0 and 0xdb are escaped, worst case is
    //   when the packet is all made of 0xc0 and 0xdb :) )
    //   + 2 (0xc0 delimiters at start and end).

    nskb = hci_skb_alloc((len + 6) * 2 + 2);

    if (!nskb) {
        BT_DBG("nskb is NULL");
        return NULL;
    }

    //Add SLIP start byte: 0xc0
    h5_slip_msgdelim(nskb);
    // set AckNumber in SlipHeader
    hdr[0] = h5->rxseq_txack << 3;
    h5->is_txack_req = 0;

    //BT_DBG("We request packet no(%u) to card", h5->rxseq_txack);
    //BT_DBG("Sending packet with seqno %u and wait %u", h5->msgq_txseq, h5->rxseq_txack);

    if (H5_RELIABLE_PKT == rel) {
        // set reliable pkt bit and SeqNumber
        hdr[0] |= 0x80 + h5->msgq_txseq;
        //BT_DBG("Sending packet with seqno(%u)", h5->msgq_txseq);
        ++(h5->msgq_txseq);
        h5->msgq_txseq = (h5->msgq_txseq) & 0x07;
    }

    // set DicPresent bit
    if (h5->use_crc) {
        hdr[0] |= 0x40;
    }

    // set packet type and payload length
    hdr[1] = ((len << 4) & 0xff) | pkt_type;
    hdr[2] = (uint8_t)(len >> 4);
    // set checksum
    hdr[3] = ~(hdr[0] + hdr[1] + hdr[2]);

    // Put h5 header */
    for (i = 0; i < 4; i++) {
        h5_slip_one_byte(nskb, hdr[i]);

        if (h5->use_crc) {
            h5_crc_update(&h5_txmsg_crc, hdr[i]);
        }
    }

    // Put payload */
    for (i = 0; i < len; i++) {
        h5_slip_one_byte(nskb, data[i]);

        if (h5->use_crc) {
            h5_crc_update(&h5_txmsg_crc, data[i]);
        }
    }

    // Put CRC */
    if (h5->use_crc) {
        h5_txmsg_crc = bit_rev16(h5_txmsg_crc);
        h5_slip_one_byte(nskb, (uint8_t)((h5_txmsg_crc >> 8) & 0x00ff));
        h5_slip_one_byte(nskb, (uint8_t)(h5_txmsg_crc & 0x00ff));
    }

    // Add SLIP end byte: 0xc0
    h5_slip_msgdelim(nskb);
    return nskb;
}

/**
* Removed controller acked packet from Host's unacked lists
*
* @param h5 realtek h5 struct
*/
static void h5_remove_acked_pkt(tHCI_H5_CB *h5)
{
    RT_LIST_HEAD *Head = NULL;
    RT_LIST_ENTRY *Iter = NULL, *Temp = NULL;
    RTK_BUFFER *skb = NULL;

    int pkts_to_be_removed = 0;
    int seqno = 0;
    int i = 0;

    aos_mutex_lock(&h5_wakeup_mutex, AOS_WAIT_FOREVER);

    seqno = h5->msgq_txseq;
    pkts_to_be_removed = RtbGetQueueLen(h5->unack);

    while (pkts_to_be_removed) {
        if (h5->rxack == seqno) {
            break;
        }

        pkts_to_be_removed--;
        seqno = (seqno - 1) & 0x07;
    }

    if (h5->rxack != seqno) {
        BT_DBG("Peer acked invalid packet");
    }


    // remove ack'ed packet from bcsp->unack queue
    i = 0;//  number of pkts has been removed from un_ack queue.
    Head = (RT_LIST_HEAD *)(h5->unack);
    LIST_FOR_EACH_SAFELY(Iter, Temp, Head) {
        skb = LIST_ENTRY(Iter, sk_buff, List);

        if (i >= pkts_to_be_removed) {
            break;
        }

        hci_skb_unlink(skb, h5->unack);
        hci_skb_free(&skb);
        i++;
    }

    if (0 == hci_skb_queue_get_length(h5->unack)) {
        h5_stop_data_retrans_timer();
        rtk_h5.data_retrans_count = 0;
    }

    if (i != pkts_to_be_removed) {
        BT_DBG("Removed only (%u) out of (%u) pkts", i, pkts_to_be_removed);
    }

    aos_mutex_unlock(&h5_wakeup_mutex);

}

static void hci_h5_send_sync_req()
{
    //uint16_t bytes_sent = 0;
    unsigned char    h5sync[2]     = {0x01, 0x7E};
    sk_buff *skb = NULL;

    skb = hci_skb_alloc_and_init(H5_LINK_CTL_PKT, h5sync, sizeof(h5sync));

    if (!skb) {
        BT_ERR("hci_skb_alloc_and_init fail!");
        return;
    }

    BT_DBG("H5: --->>>send sync req");

    h5_enqueue(skb);
    h5_wake_up();

    return;
}

static void hci_h5_send_sync_resp()
{
    //uint16_t bytes_sent = 0;
    unsigned char h5syncresp[2] = {0x02, 0x7D};
    sk_buff *skb = NULL;

    skb = hci_skb_alloc_and_init(H5_LINK_CTL_PKT, h5syncresp, sizeof(h5syncresp));

    if (!skb) {
        BT_ERR("hci_skb_alloc_and_init fail!");
        return;
    }

    BT_DBG("H5: --->>>send sync resp");
    h5_enqueue(skb);
    h5_wake_up();

    return;
}

static void hci_h5_send_conf_req()
{
    //uint16_t bytes_sent = 0;
    unsigned char h5conf[3] = {0x03, 0xFC, 0x14};
    //unsigned char h5conf[2] = {0x03, 0xFC};
    sk_buff *skb = NULL;

    skb = hci_skb_alloc_and_init(H5_LINK_CTL_PKT, h5conf, sizeof(h5conf));

    if (!skb) {
        BT_ERR("hci_skb_alloc_and_init fail!");
        return;
    }

    BT_DBG("H5: --->>>send conf req");
    h5_enqueue(skb);
    h5_wake_up();

    return;
}


static void hci_h5_send_conf_resp()
{
    //uint16_t bytes_sent = 0;
    unsigned char h5confresp[2] = {0x04, 0x7B};
    sk_buff *skb = NULL;

    skb = hci_skb_alloc_and_init(H5_LINK_CTL_PKT, h5confresp, sizeof(h5confresp));

    if (!skb) {
        BT_ERR("hci_skb_alloc_and_init fail!");
        return;
    }

    BT_DBG("H5: --->>>send conf resp");
    h5_enqueue(skb);
    h5_wake_up();

    return;
}

static void rtk_notify_hw_h5_init_result(uint8_t result)
{
    BT_DBG("rtk_notify_hw_h5_init_result");
    uint8_t sync_event[6] = {0x0e, 0x04, 0x03, 0xee, 0xfc, 0x00};
    // we need to make a sync event to bt
    sk_buff     *rx_skb;
    rx_skb = hci_skb_alloc_and_init(HCI_EVENT_PKT, sync_event, sizeof(sync_event));

    aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
    hci_skb_queue_tail(rtk_h5.recv_data, rx_skb);
    aos_sem_signal(&rtk_h5.data_cond);
    aos_mutex_unlock(&rtk_h5.data_mutex);
}


static sk_buff *h5_dequeue()
{
    sk_buff *skb = NULL;

    //   First of all, check for unreliable messages in the queue,
    //   since they have higher priority
    aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
    if ((skb = (sk_buff *)hci_skb_dequeue_head(rtk_h5.unrel)) != NULL) {
        sk_buff *nskb = h5_prepare_pkt(&rtk_h5,
                                       hci_skb_get_data(skb),
                                       hci_skb_get_data_length(skb),
                                       hci_skb_get_pkt_type(skb));

        if (nskb) {
            aos_mutex_unlock(&rtk_h5.data_mutex);
            hci_skb_free(&skb);
            return nskb;
        } else {
            hci_skb_queue_head(rtk_h5.unrel, skb);
        }
    }
    aos_mutex_unlock(&rtk_h5.data_mutex);

    //   Now, try to send a reliable pkt. We can only send a
    //   reliable packet if the number of packets sent but not yet ack'ed
    //   is < than the winsize

    aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
    if (RtbGetQueueLen(rtk_h5.unack) < rtk_h5.sliding_window_size &&
        (skb = (sk_buff *)hci_skb_dequeue_head(rtk_h5.rel)) != NULL) {
        sk_buff *nskb = h5_prepare_pkt(&rtk_h5,
                                       hci_skb_get_data(skb),
                                       hci_skb_get_data_length(skb),
                                       hci_skb_get_pkt_type(skb));

        if (nskb) {
            hci_skb_queue_tail(rtk_h5.unack, skb);
            aos_mutex_unlock(&rtk_h5.data_mutex);
            h5_start_data_retrans_timer();
            return nskb;
        } else {
            hci_skb_queue_head(rtk_h5.rel, skb);
        }
    }
    aos_mutex_unlock(&rtk_h5.data_mutex);

    //   We could not send a reliable packet, either because there are
    //   none or because there are too many unack'ed packets. Did we receive
    //   any packets we have not acknowledged yet
    if (rtk_h5.is_txack_req) {
        // if so, craft an empty ACK pkt and send it on BCSP unreliable
        // channel
        sk_buff *nskb = h5_prepare_pkt(&rtk_h5, NULL, 0, H5_ACK_PKT);
        return nskb;
    }

    // We have nothing to send
    return NULL;
}

static int h5_enqueue(IN sk_buff *skb)
{
    //Pkt length must be less than 4095 bytes
    if (hci_skb_get_data_length(skb) > 0xFFF) {
        BT_ERR("skb len > 0xFFF");
        hci_skb_free(&skb);
        return 0;
    }

    switch (hci_skb_get_pkt_type(skb)) {
        case HCI_ACLDATA_PKT:
        case HCI_COMMAND_PKT:
            aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
            hci_skb_queue_tail(rtk_h5.rel, skb);
            aos_mutex_unlock(&rtk_h5.data_mutex);
            break;


        case H5_LINK_CTL_PKT:
        case H5_ACK_PKT:
        case H5_VDRSPEC_PKT:
            aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
            hci_skb_queue_tail(rtk_h5.unrel, skb);/* 3-wire LinkEstablishment*/
            aos_mutex_unlock(&rtk_h5.data_mutex);
            break;

        default:
            hci_skb_free(&skb);
            break;
    }

    return 0;
}


static uint16_t h5_wake_up()
{
    uint16_t bytes_sent = 0;
    sk_buff *skb = NULL;
    uint8_t *data = NULL;
    uint32_t data_len = 0;

    aos_mutex_lock(&h5_wakeup_mutex, AOS_WAIT_FOREVER);

    //BT_DBG("h5_wake_up");
    while (NULL != (skb = h5_dequeue())) {
        data = hci_skb_get_data(skb);
        data_len = hci_skb_get_data_length(skb);

#if H5_TRACE_DATA_ENABLE
        {
            uint32_t iTemp;
            printf("H5 TX: length(%d)\n", data_len);

            for (iTemp = 0; iTemp < data_len; iTemp++) {
                printf("0x%x ", data[iTemp]);
            }
            printf("\n");
        }
#endif
#ifndef YULONG_HCI
        //we adopt the hci_drv interface to send data
        bytes_sent = hci_send(rtk_h5.hci_dev, data, data_len);
#endif
        hci_skb_free(&skb);
    }

    aos_mutex_unlock(&h5_wakeup_mutex);
    return bytes_sent;
}

static void h5_process_ctl_pkts(void)
{
    //process h5 link establish
    uint8_t cfg;

    sk_buff *skb = rtk_h5.rx_skb;

    unsigned char   h5sync[2]     = {0x01, 0x7E},
                    h5syncresp[2] = {0x02, 0x7D},
                    h5conf[3]     = {0x03, 0xFC, 0x14},
                    h5confresp[2] = {0x04, 0x7B};

    if (rtk_h5.link_estab_state == H5_UNINITIALIZED) { //sync
        if (!memcmp(hci_skb_get_data(skb), h5sync, 2)) {
            BT_DBG("H5: <<<---recv sync req");
            hci_h5_send_sync_resp();
        } else if (!memcmp(hci_skb_get_data(skb), h5syncresp, 2)) {
            BT_DBG("H5: <<<---recv sync resp");
            h5_stop_sync_retrans_timer();
            rtk_h5.sync_retrans_count  = 0;
            rtk_h5.link_estab_state = H5_INITIALIZED;

            //send config req
            hci_h5_send_conf_req();
            h5_start_conf_retrans_timer();
        }

    } else if (rtk_h5.link_estab_state == H5_INITIALIZED) { //config
        if (!memcmp(hci_skb_get_data(skb), h5sync, 0x2)) {

            BT_DBG("H5: <<<---recv sync req in H5_INITIALIZED");
            hci_h5_send_sync_resp();
        } else if (!memcmp(hci_skb_get_data(skb), h5conf, 0x2)) {
            BT_DBG("H5: <<<---recv conf req");
            hci_h5_send_conf_resp();
        } else if (!memcmp(hci_skb_get_data(skb), h5confresp,  0x2)) {
            BT_DBG("H5: <<<---recv conf resp");
            h5_stop_conf_retrans_timer();
            rtk_h5.conf_retrans_count  = 0;

            rtk_h5.link_estab_state = H5_ACTIVE;
            //notify hw to download patch
            memcpy(&cfg, hci_skb_get_data(skb) + 2, H5_CFG_SIZE);
            rtk_h5.sliding_window_size = H5_CFG_SLID_WIN(cfg);
            rtk_h5.oof_flow_control = H5_CFG_OOF_CNTRL(cfg);
            rtk_h5.dic_type = H5_CFG_DIC_TYPE(cfg);
            BT_DBG("rtk_h5.sliding_window_size(%d), oof_flow_control(%d), dic_type(%d)",
                            rtk_h5.sliding_window_size, rtk_h5.oof_flow_control, rtk_h5.dic_type);

            if (rtk_h5.dic_type) {
                rtk_h5.use_crc = 1;
            }

            rtk_notify_hw_h5_init_result(1);
        } else {
            BT_DBG("H5_INITIALIZED receive event, ingnore");
        }
    } else if (rtk_h5.link_estab_state == H5_ACTIVE) {
        if (!memcmp(hci_skb_get_data(skb), h5sync, 0x2)) {

            BT_DBG("H5: <<<---recv sync req in H5_ACTIVE");
            hci_h5_send_sync_resp();
            BT_DBG("H5 : H5_ACTIVE transit to H5_UNINITIALIZED");
            rtk_h5.link_estab_state = H5_UNINITIALIZED;
            hci_h5_send_sync_req();
            h5_start_sync_retrans_timer();
        } else if (!memcmp(hci_skb_get_data(skb), h5conf, 0x2)) {
            BT_DBG("H5: <<<---recv conf req in H5_ACTIVE");
            hci_h5_send_conf_resp();
        } else if (!memcmp(hci_skb_get_data(skb), h5confresp,  0x2)) {
            BT_DBG("H5: <<<---recv conf resp in H5_ACTIVE, discard");
        } else {
            BT_DBG("H5_ACTIVE receive unknown link control msg, ingnore");
        }

    }
}

/**
* Check if it's a hci frame, if it is, complete it with response or parse the cmd complete event
*
* @param skb socket buffer
*
*/
static uint8_t hci_recv_frame(sk_buff *skb, uint8_t pkt_type)
{
    uint8_t intercepted = 0;

    uint32_t data_len = hci_skb_get_data_length(skb);

    (void)data_len;
    BT_DBG("UART H5 RX: length = %d", data_len);

    //we only intercept evt packet here
    if (pkt_type == HCI_EVENT_PKT) {
        uint8_t     *p;
        uint8_t     event_code;
        uint16_t    opcode;
        p = (uint8_t *)hci_skb_get_data(skb);

        event_code = *p++;
        // BT_DBG("hci_recv_frame event_code(0x%x), len = %d", event_code, len);

        if (event_code == HCI_COMMAND_COMPLETE_EVT) {
            num_hci_cmd_pkts = *p++;
            STREAM_TO_UINT16(opcode, p);

            if (opcode == HCI_VSC_UPDATE_BAUDRATE) {
                intercepted = 1;
                rtk_h5.internal_skb = skb;
                BT_DBG("CommandCompleteEvent for command h5_start_wait_controller_baudrate_ready_timer (0x%04X)", opcode);
                h5_start_wait_controller_baudrate_ready_timer();
            }
        }

    }

    // BT_DBG("hci_recv_frame intercepted = %d", intercepted);
    return intercepted;
}

/**
* after rx data is parsed, and we got a rx frame saved in h5->rx_skb,
* this routinue is called.
* things todo in this function:
* 1. check if it's a hci frame, if it is, complete it with response or ack
* 2. see the ack number, free acked frame in queue
* 3. reset h5->rx_state, set rx_skb to null.
*
* @param h5 realtek h5 struct
*
*/
static uint8_t h5_complete_rx_pkt(tHCI_H5_CB *h5)
{
    int pass_up = 1;
    uint8_t *h5_hdr = NULL;
    uint8_t pkt_type = 0;
    uint8_t status = 0;

    h5_hdr = (uint8_t *)hci_skb_get_data(h5->rx_skb);

#if H5_TRACE_DATA_ENABLE
    {
        uint32_t iTemp;
        uint32_t data_len = hci_skb_get_data_length(h5->rx_skb);
        uint8_t *data = (uint8_t *)hci_skb_get_data(h5->rx_skb);
        printf("H5 RX: length(%d)\n", data_len);

        for (iTemp = 0; iTemp < data_len; iTemp++) {
            printf("0x%x ", data[iTemp]);
        }
        printf("\n");
    }
#endif

    if (H5_HDR_RELIABLE(h5_hdr)) {
        aos_mutex_lock(&h5_wakeup_mutex, AOS_WAIT_FOREVER);
        h5->rxseq_txack = H5_HDR_SEQ(h5_hdr) + 1;
        h5->rxseq_txack %= 8;
        h5->is_txack_req = 1;
        aos_mutex_unlock(&h5_wakeup_mutex);
        // send down an empty ack if needed.
        h5_wake_up();
    }

    h5->rxack = H5_HDR_ACK(h5_hdr);
    pkt_type = H5_HDR_PKT_TYPE(h5_hdr);
    //BT_DBG("h5_complete_rx_pkt, pkt_type = %d", pkt_type);

    switch (pkt_type) {
        case HCI_ACLDATA_PKT:
            pass_up = 1;
            break;

        case HCI_EVENT_PKT:
            pass_up = 1;
            break;

        case HCI_SCODATA_PKT:
            pass_up = 1;
            break;

        case HCI_COMMAND_PKT:
            pass_up = 1;
            break;

        case H5_LINK_CTL_PKT:
            pass_up = 0;
            break;

        case H5_ACK_PKT:
            pass_up = 0;
            break;

        default:
            BT_ERR("Unknown pkt type(%d)", H5_HDR_PKT_TYPE(h5_hdr));
            pass_up = 0;
            break;
    }

    // remove h5 header and send packet to hci
    h5_remove_acked_pkt(h5);

    if (H5_HDR_PKT_TYPE(h5_hdr) == H5_LINK_CTL_PKT) {

        hci_skb_pull(h5->rx_skb, H5_HDR_SIZE);
        h5_process_ctl_pkts();
    }

    // decide if we need to pass up.
    if (pass_up) {
        hci_skb_pull(h5->rx_skb, H5_HDR_SIZE);
        hci_skb_set_pkt_type(h5->rx_skb, pkt_type);

        //send command or acl data it to bluedroid stack
        sk_buff *skb_complete_pkt = h5->rx_skb;

        status = hci_recv_frame(skb_complete_pkt, pkt_type);

        if (!status) {
            aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
            hci_skb_queue_tail(rtk_h5.recv_data, h5->rx_skb);
            aos_sem_signal(&rtk_h5.data_cond);
            aos_mutex_unlock(&rtk_h5.data_mutex);
        }
    } else {
        // free ctl packet
        hci_skb_free(&h5->rx_skb);
    }

    h5->rx_state = H5_W4_PKT_DELIMITER;
    rtk_h5.rx_skb = NULL;
    return pkt_type;
}


/**
* Parse the receive data in h5 proto.
*
* @param h5 realtek h5 struct
* @param data point to data received before parse
* @param count num of data
* @return reserved count
*/
static uint32_t h5_recv(tHCI_H5_CB *h5, uint8_t *data, int count)
{
    //   unsigned char *ptr;
    uint8_t *ptr;
    uint8_t *skb_data = NULL;
    uint8_t *hdr = NULL;
    bool     complete_packet = 0;

    ptr = (uint8_t *)data;

    // BT_DBG("count %d rx_state %d rx_count %ld", count, h5->rx_state, h5->rx_count);

    while (count) {
        if (h5->rx_count) {
            if (*ptr == 0xc0) {
                BT_ERR("short h5 packet %d %d", h5->rx_count, count);
                hci_skb_free(&h5->rx_skb);
                h5->rx_state = H5_W4_PKT_START;
                h5->rx_count = 0;
            } else {
                h5_unslip_one_byte(h5, *ptr);
            }

            ptr++;
            count--;
            continue;
        }

        //BT_DBG("h5_recv rx_state=%d", h5->rx_state);
        switch (h5->rx_state) {
            case H5_W4_HDR:
                // check header checksum. see Core Spec V4 "3-wire uart" page 67
                skb_data = hci_skb_get_data(h5->rx_skb);
                hdr = (uint8_t *)skb_data;

                if ((0xff & (uint8_t) ~(skb_data[0] + skb_data[1] +
                                        skb_data[2])) != skb_data[3]) {
                    BT_ERR("h5 hdr checksum error!!!");
                    hci_skb_free(&h5->rx_skb);
                    h5->rx_state = H5_W4_PKT_DELIMITER;
                    h5->rx_count = 0;
                    continue;
                }

                if (H5_HDR_RELIABLE(hdr)
                    && (H5_HDR_SEQ(hdr) != h5->rxseq_txack)) {
                    BT_ERR("Out-of-order %u %u",
                                    H5_HDR_SEQ(hdr), h5->rxseq_txack);
                    h5->is_txack_req = 1;
                    h5_wake_up();

                    hci_skb_free(&h5->rx_skb);
                    h5->rx_state = H5_W4_PKT_DELIMITER;
                    h5->rx_count = 0;

                    continue;
                }

                h5->rx_state = H5_W4_DATA;
                //payload length: May be 0
                h5->rx_count = H5_HDR_LEN(hdr);
                continue;

            case H5_W4_DATA:

                hdr = (uint8_t *)hci_skb_get_data(h5->rx_skb);

                if (H5_HDR_CRC(hdr)) {
                    // pkt with crc /
                    h5->rx_state = H5_W4_CRC;
                    h5->rx_count = 2;
                } else {
                    h5_complete_rx_pkt(h5); //Send ACK
                    complete_packet = 1;
                    //BT_DBG("--------> H5_W4_DATA ACK\n");
                }

                continue;

            case H5_W4_CRC:
                if (bit_rev16(h5->message_crc) != h5_get_crc(h5)) {
                    BT_ERR("Checksum failed, computed(%04x)received(%04x)",
                                    bit_rev16(h5->message_crc), h5_get_crc(h5));
                    hci_skb_free(&h5->rx_skb);
                    h5->rx_state = H5_W4_PKT_DELIMITER;
                    h5->rx_count = 0;
                    continue;
                }

                hci_skb_trim(h5->rx_skb, hci_skb_get_data_length(h5->rx_skb) - 2);
                h5_complete_rx_pkt(h5);
                complete_packet = 1;
                continue;

            case H5_W4_PKT_DELIMITER:
                switch (*ptr) {
                    case 0xc0:
                        h5->rx_state = H5_W4_PKT_START;
                        break;

                    default:
                        break;
                }

                ptr++;
                count--;
                break;

            case H5_W4_PKT_START:
                switch (*ptr) {
                    case 0xc0:
                        ptr++;
                        count--;
                        break;

                    default:
                        h5->rx_state = H5_W4_HDR;
                        h5->rx_count = 4;
                        h5->rx_esc_state = H5_ESCSTATE_NOESC;
                        H5_CRC_INIT(h5->message_crc);

                        // Do not increment ptr or decrement count
                        // Allocate packet. Max len of a H5 pkt=
                        // 0xFFF (payload) +4 (header) +2 (crc)
                        h5->rx_skb = hci_skb_alloc(0x1005);

                        if (!h5->rx_skb) {
                            h5->rx_state = H5_W4_PKT_DELIMITER;
                            h5->rx_count = 0;
                            return 0;
                        }

                        break;
                }

                break;
        }
    }

    return complete_packet;
}

/******************************************************************************
**  Static functions
******************************************************************************/
static void data_ready_cb_thread(void *arg)
{
    sk_buff *skb;

    BT_DBG("data_ready_cb_thread started");

    while (h5_data_ready_running) {

        aos_sem_wait(&rtk_h5.data_cond, AOS_WAIT_FOREVER);

        while ((hci_skb_queue_get_length(rtk_h5.recv_data) == 0)) {
            aos_sem_wait(&rtk_h5.data_cond, AOS_WAIT_FOREVER);
        }

        aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
        if ((skb = hci_skb_dequeue_head(rtk_h5.recv_data)) != NULL) {
            rtk_h5.data_skb = skb;
        }
        aos_mutex_unlock(&rtk_h5.data_mutex);

        int length = hci_skb_get_data_length(rtk_h5.data_skb);

        if (h5_int_hal_callbacks) {
            h5_int_hal_callbacks(hci_skb_get_pkt_type(rtk_h5.data_skb),
                                 hci_skb_get_data(rtk_h5.data_skb),
                                 length);
        }

        hci_skb_free(&rtk_h5.data_skb);
    }

}

static void data_retransfer_thread(void *arg)
{
    uint16_t events;

    BT_DBG("data_retransfer_thread started");

    while (h5_retransfer_running) {

        aos_sem_wait(&rtk_h5.cond, AOS_WAIT_FOREVER);

        events = h5_ready_events;
        h5_ready_events = 0;


        if (events & H5_EVENT_RX) {
            sk_buff *skb;
            BT_ERR("retransmitting (%u) pkts, retransfer count(%d)", hci_skb_queue_get_length(rtk_h5.unack), rtk_h5.data_retrans_count);

            if (rtk_h5.data_retrans_count < DATA_RETRANS_COUNT) {
                while ((skb = hci_skb_dequeue_tail(rtk_h5.unack)) != NULL) {

                    rtk_h5.msgq_txseq = (rtk_h5.msgq_txseq - 1) & 0x07;
                    hci_skb_queue_head(rtk_h5.rel, skb);

                }

                rtk_h5.data_retrans_count++;
                h5_wake_up();

            }

        } else if (events & H5_EVENT_EXIT) {
            break;
        }

    }

    BT_DBG("data_retransfer_thread exiting");
}

static void h5_retransfer_signal_event(uint16_t event)
{
    aos_mutex_lock(&rtk_h5.mutex, AOS_WAIT_FOREVER);
    h5_ready_events |= event;
    aos_sem_signal(&rtk_h5.cond);
    aos_mutex_unlock(&rtk_h5.mutex);
}

static int create_data_retransfer_thread()
{
    if (h5_retransfer_running) {
        BT_ERR("create_data_retransfer_thread has been called repeatedly without calling cleanup ?");
    }

    h5_retransfer_running = 1;
    h5_ready_events = 0;

    aos_mutex_new(&rtk_h5.mutex);
    aos_sem_new(&rtk_h5.cond, 0);

    if (aos_task_new_ext(&rtk_h5.thread_data_retrans, "data_retransfer",
                         (void *)data_retransfer_thread, NULL, 3072, AOS_DEFAULT_APP_PRI + 3) != 0) {
        BT_ERR("pthread_create thread_data_retrans failed!");
        h5_retransfer_running = 0;
        return -1 ;
    }

    return 0;

}

static int create_data_ready_cb_thread()
{
    if (h5_data_ready_running) {
        BT_ERR("create_data_ready_cb_thread has been called repeatedly without calling cleanup ?");
    }

    h5_data_ready_running = 1;

    int ret = aos_mutex_new(&rtk_h5.data_mutex);

    if (ret != 0) {
        return -1;
    }

    ret = aos_sem_new(&rtk_h5.data_cond, 0);

    if (ret != 0) {
        aos_mutex_free(&rtk_h5.data_mutex);
        return -1;
    }

    if (aos_task_new_ext(&rtk_h5.thread_data_ready_cb, "data_ready_cb",
                         (void *)data_ready_cb_thread, NULL, 1024 * 6, AOS_DEFAULT_APP_PRI + 3) != 0) {
        BT_ERR("pthread_create thread_data_ready_cb failed!");
        h5_data_ready_running = 0;
        aos_mutex_free(&rtk_h5.data_mutex);
        aos_sem_free(&rtk_h5.data_cond);
        return -1 ;
    }

    return 0;

}

#ifndef YULONG_HCI
static void hci_event(hci_event_t event, uint32_t size, void *priv)
{
    hci_h5_receive_msg(NULL , 0);
}
#endif

static int hci_driver_send_cmd_cb(uint16_t opcode, uint8_t* send_data, uint32_t send_len, uint8_t* resp_data, uint32_t *resp_len)
{
    struct net_buf *buf;
    struct net_buf *rsp;
    int err;

    buf = bt_hci_cmd_create(opcode, send_len);
    if (!buf) {
        return - 1;
    }

    if (send_len > 0)
        net_buf_add_mem(buf, send_data, send_len);

    err = bt_hci_cmd_send_sync(opcode, buf, &rsp);

    if (err) {
        goto end;
    }

    if (resp_data == NULL) {
        err = 0;
        goto end;
    }

    if (*resp_len < rsp->len) {
        err = -1;
        goto end;
    }

    memcpy(resp_data, rsp->data, rsp->len);

    *resp_len = rsp->len;

end:
    net_buf_unref(buf);

    return err;
}

/*******************************************************************************
**
** Function        hci_h5_init
**
** Description     Initialize H5 module
**
** Returns         None
**
*******************************************************************************/
static void hci_h5_int_init(packet_recv h5_callbacks)
{
    BT_DBG("hci_h5_int_init");

    h5_int_hal_callbacks = h5_callbacks;
    memset(&rtk_h5, 0, sizeof(tHCI_H5_CB));

    /* Per HCI spec., always starts with 1 */
    num_hci_cmd_pkts = 1;

    h5_alloc_data_retrans_timer();
    h5_alloc_sync_retrans_timer();
    h5_alloc_conf_retrans_timer();
    h5_alloc_wait_controller_baudrate_ready_timer();
    h5_alloc_hw_init_ready_timer();

    aos_mutex_new(&h5_wakeup_mutex);

    rtk_h5.recv_data = RtbQueueInit();

    if (create_data_ready_cb_thread() != 0) {
        BT_ERR("H5 create_data_ready_cb_thread failed");
    }

    if (create_data_retransfer_thread() != 0) {
        BT_ERR("H5 create_data_retransfer_thread failed");
    }

    rtk_h5.unack = RtbQueueInit();
    rtk_h5.rel   = RtbQueueInit();
    rtk_h5.unrel = RtbQueueInit();

    rtk_h5.rx_state = H5_W4_PKT_DELIMITER;
    rtk_h5.rx_esc_state = H5_ESCSTATE_NOESC;

#ifndef YULONG_HCI
    rtk_h5.hci_dev = hci_open("hci");

    ASSERT(rtk_h5.hci_dev, "hci_open failed");

    hci_set_event(rtk_h5.hci_dev, hci_event, NULL);

    hci_start(rtk_h5.hci_dev, hci_driver_send_cmd_cb);
#endif
}

/*******************************************************************************
**
** Function        hci_h5_cleanup
**
** Description     Clean H5 module
**
** Returns         None
**
*******************************************************************************/
static void hci_h5_cleanup(void)
{
    BT_DBG("hci_h5_cleanup");

    rtk_h5.cleanuping = 1;

    h5_free_data_retrans_timer();
    h5_free_sync_retrans_timer();
    h5_free_conf_retrans_timer();
    h5_free_wait_controller_baudrate_ready_timer();
    h5_free_hw_init_ready_timer();

    if (h5_retransfer_running) {
        h5_retransfer_running = 0;
        h5_retransfer_signal_event(H5_EVENT_EXIT);

    }

    aos_mutex_free(&rtk_h5.mutex);
    aos_sem_free(&rtk_h5.cond);

    RtbQueueFree(rtk_h5.unack);
    RtbQueueFree(rtk_h5.rel);
    RtbQueueFree(rtk_h5.unrel);

    h5_int_hal_callbacks = NULL;
    rtk_h5.internal_skb = NULL;
}

/*******************************************************************************
**
** Function        hci_h5_receive_msg
**
** Description     Construct HCI EVENT/ACL packets and send them to stack once
**                 complete packet has been received.
**
** Returns         Number of read bytes
**
*******************************************************************************/
static uint8_t data_buffer[4096] = {0};
static uint32_t hci_h5_receive_msg(uint8_t *byte, uint16_t length)
{
#ifndef YULONG_HCI
    uint32_t read_len = hci_recv(rtk_h5.hci_dev, data_buffer, sizeof(data_buffer));

    if (read_len > 0) {

        return h5_recv(&rtk_h5, data_buffer, read_len);
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

/*******************************************************************************
**
** Function        hci_h5_send_cmd
**
** Description     get cmd data from hal and send cmd
**
**
** Returns          bytes send
**
*******************************************************************************/
static uint16_t hci_h5_send_cmd(hci_data_type_t type, uint8_t *data, uint16_t length)
{
    sk_buff *skb = NULL;
    uint16_t opcode;

    skb = hci_skb_alloc_and_init(type, data, length);

    if (!skb) {
        BT_ERR("send cmd hci_skb_alloc_and_init fail!");
        return -1;
    }

    h5_enqueue(skb);

    num_hci_cmd_pkts--;

    /* If this is an internal Cmd packet, the layer_specific field would
         * have stored with the opcode of HCI command.
         * Retrieve the opcode from the Cmd packet.
         */
    STREAM_TO_UINT16(opcode, data);
    BT_DBG("HCI Command opcode(0x%04X)", opcode);

    if (opcode == 0x0c03) {
        BT_DBG("RX HCI RESET Command, stop hw init timer");
        h5_stop_hw_init_ready_timer();
    }

    h5_wake_up();
    return length;
}

/*******************************************************************************
**
** Function        hci_h5_send_acl_data
**
** Description     get cmd data from hal and send cmd
**
**
** Returns          bytes send
**
*******************************************************************************/
static uint16_t hci_h5_send_acl_data(hci_data_type_t type, uint8_t *data, uint16_t length)
{
    sk_buff *skb = NULL;

    skb = hci_skb_alloc_and_init(type, data, length);

    if (!skb) {
        BT_ERR("hci_h5_send_acl_data, alloc skb buffer fail!");
        return -1;
    }

    h5_enqueue(skb);

    h5_wake_up();
    return length;
}

/*******************************************************************************
**
** Function        hci_h5_send_int_cmd
**
** Description     Place the internal commands (issued internally by vendor lib)
**                 in the tx_q.
**
** Returns         1/0
**
*******************************************************************************/
static uint8_t hci_h5_send_sync_cmd(uint16_t opcode, uint8_t *p_buf, uint16_t length)
{
    if (rtk_h5.link_estab_state == H5_UNINITIALIZED) {
        if (opcode == HCI_VSC_H5_INIT) {
            h5_start_hw_init_ready_timer();
            hci_h5_send_sync_req();
            h5_start_sync_retrans_timer();
        }
    } else if (rtk_h5.link_estab_state == H5_ACTIVE) {
        BT_DBG("hci_h5_send_sync_cmd(0x%x), link_estab_state = %d", opcode, rtk_h5.link_estab_state);
        return 0;
    }

    return 1;
}

/***
    Timer related functions
*/
static aos_timer_t *OsAllocateTimer(tTIMER_HANDLE_CBACK timer_callback)
{
    aos_timer_t *timer;
    int ret;

    timer = malloc(sizeof(aos_timer_t));

    if (timer == NULL) {
        return NULL;
    }

    ret = aos_timer_new_ext(timer, timer_callback, NULL, 0xFFFFFFUL, 0, 0);

    if (ret == 0) {
        return timer;
    } else {
        BT_ERR(" ret: %d", ret);
        return NULL;
    }
}

static int OsFreeTimer(aos_timer_t *timerid)
{
    aos_timer_free(timerid);

    free(timerid);

    return 0;
}

static int OsStartTimer(aos_timer_t *timerid, int msec, int mode)
{
    int ret;

    //Set the Timer when to expire through timer_settime
    if (!aos_timer_is_valid(timerid)) {
        BT_ERR("%s null\n", __func__);
        goto end;
    }

    ret = aos_timer_stop(timerid);

    if (ret != 0) {
       BT_ERR("%s aos_timer_stop %d\n", __func__, ret);
    //    return ret;
    }

    if (mode) {
        ret = aos_timer_change(timerid, msec);
    } else {
        ret = aos_timer_change_once(timerid, msec);
    }

    if (ret != 0) {
       BT_ERR("%s aos_timer_change %d\n", __func__, ret);
    //    return ret;
    }

    ret = aos_timer_start(timerid);

    if (ret != 0) {
       BT_ERR("%s aos_timer_start %d\n", __func__, ret);
    //    return ret;
    }

end:
    return 0;
}

static int OsStopTimer(aos_timer_t *timerid)
{
    return  aos_timer_stop(timerid);
}

static void h5_retransfer_timeout_handler(void *timer, void *arg)
{
    BT_DBG("h5_retransfer_timeout_handler");

    if (rtk_h5.cleanuping) {
        BT_ERR("h5_retransfer_timeout_handler H5 is cleanuping, EXIT here!");
        return;
    }

    h5_retransfer_signal_event(H5_EVENT_RX);
}

static void h5_sync_retrans_timeout_handler(void *timer, void *arg)
{
    BT_DBG("h5_sync_retrans_timeout_handler");

    if (rtk_h5.cleanuping) {
        BT_ERR("h5_sync_retrans_timeout_handler H5 is cleanuping, EXIT here!");
        return;
    }

    if (rtk_h5.sync_retrans_count < SYNC_RETRANS_COUNT) {
        hci_h5_send_sync_req();
        rtk_h5.sync_retrans_count ++;
    } else {
        h5_stop_sync_retrans_timer();
    }
}

static void h5_conf_retrans_timeout_handler(void *timer, void *arg)
{
    BT_DBG("h5_conf_retrans_timeout_handler");

    if (rtk_h5.cleanuping) {
        BT_ERR("h5_conf_retrans_timeout_handler H5 is cleanuping, EXIT here!");
        return;
    }

    BT_DBG("Wait H5 Conf Resp timeout, %d times", rtk_h5.conf_retrans_count);

    if (rtk_h5.conf_retrans_count < CONF_RETRANS_COUNT) {
        h5_start_conf_retrans_timer();
        hci_h5_send_conf_req();
        rtk_h5.conf_retrans_count++;
    } else {
        h5_stop_conf_retrans_timer();
    }

}

static void h5_wait_controller_baudrate_ready_timeout_handler(void *timer, void *arg)
{
    BT_DBG("h5_wait_ct_baundrate_ready_timeout_handler");

    if (rtk_h5.cleanuping) {
        BT_ERR("h5_wait_controller_baudrate_ready_timeout_handler H5 is cleanuping, EXIT here!");

        if (rtk_h5.internal_skb) {
            hci_skb_free(&rtk_h5.internal_skb);
        }

        return;
    }

    BT_DBG("No Controller retransfer, baudrate of controller ready");

    if (rtk_h5.internal_skb == NULL) {
        return;
    }
    aos_mutex_lock(&rtk_h5.data_mutex, AOS_WAIT_FOREVER);
    hci_skb_queue_tail(rtk_h5.recv_data, rtk_h5.internal_skb);
    aos_sem_signal(&rtk_h5.data_cond);
    aos_mutex_unlock(&rtk_h5.data_mutex);

    rtk_h5.internal_skb = NULL;
}

static void h5_hw_init_ready_timeout_handler(void *timer, void *arg)
{
    BT_DBG("h5_hw_init_ready_timeout_handler");

    if (rtk_h5.cleanuping) {
        BT_ERR("H5 is cleanuping, EXIT here!");
        return;
    }

    BT_DBG("TIMER_H5_HW_INIT_READY timeout, kill restart BT");
}

/*
** h5 data retrans timer functions
*/
static int h5_alloc_data_retrans_timer()
{
    // Create and set the timer when to expire
    rtk_h5.timer_data_retrans = OsAllocateTimer(h5_retransfer_timeout_handler);

    return 0;
}

static int h5_free_data_retrans_timer()
{
    return OsFreeTimer(rtk_h5.timer_data_retrans);
}

static int h5_start_data_retrans_timer()
{
    return OsStartTimer(rtk_h5.timer_data_retrans, DATA_RETRANS_TIMEOUT_VALUE, 0);
}

static int h5_stop_data_retrans_timer()
{
    return OsStopTimer(rtk_h5.timer_data_retrans);
}

/*
** h5 sync retrans timer functions
*/
static int h5_alloc_sync_retrans_timer()
{
    // Create and set the timer when to expire
    rtk_h5.timer_sync_retrans = OsAllocateTimer(h5_sync_retrans_timeout_handler);

    return 0;

}

static int h5_free_sync_retrans_timer()
{
    return OsFreeTimer(rtk_h5.timer_sync_retrans);
}

static int h5_start_sync_retrans_timer()
{
    return OsStartTimer(rtk_h5.timer_sync_retrans, SYNC_RETRANS_TIMEOUT_VALUE, 1);
}

static int h5_stop_sync_retrans_timer()
{
    return OsStopTimer(rtk_h5.timer_sync_retrans);
}

/*
** h5 config retrans timer functions
*/
static int h5_alloc_conf_retrans_timer()
{
    // Create and set the timer when to expire
    rtk_h5.timer_conf_retrans = OsAllocateTimer(h5_conf_retrans_timeout_handler);

    return 0;
}

static int h5_free_conf_retrans_timer()
{
    return OsFreeTimer(rtk_h5.timer_conf_retrans);
}

static int h5_start_conf_retrans_timer()
{
    return OsStartTimer(rtk_h5.timer_conf_retrans, CONF_RETRANS_TIMEOUT_VALUE, 1);
}

static int h5_stop_conf_retrans_timer()
{
    return OsStopTimer(rtk_h5.timer_conf_retrans);
}

/*
** h5 wait controller baudrate ready timer functions
*/
static int h5_alloc_wait_controller_baudrate_ready_timer()
{
    // Create and set the timer when to expire
    rtk_h5.timer_wait_ct_baudrate_ready = OsAllocateTimer(h5_wait_controller_baudrate_ready_timeout_handler);

    return 0;
}

static int h5_free_wait_controller_baudrate_ready_timer()
{
    return OsFreeTimer(rtk_h5.timer_wait_ct_baudrate_ready);
}

static int h5_start_wait_controller_baudrate_ready_timer()
{
    return OsStartTimer(rtk_h5.timer_wait_ct_baudrate_ready, WAIT_CT_BAUDRATE_READY_TIMEOUT_VALUE, 0);
}

/*
** h5 hw init ready timer functions
*/
static int h5_alloc_hw_init_ready_timer()
{
    // Create and set the timer when to expire
    rtk_h5.timer_h5_hw_init_ready = OsAllocateTimer(h5_hw_init_ready_timeout_handler);

    return 0;
}

static int h5_free_hw_init_ready_timer()
{
    return OsFreeTimer(rtk_h5.timer_h5_hw_init_ready);
}

static int h5_start_hw_init_ready_timer()
{
    return OsStartTimer(rtk_h5.timer_h5_hw_init_ready, H5_HW_INIT_READY_TIMEOUT_VALUE, 0);
}

static int h5_stop_hw_init_ready_timer()
{
    return OsStopTimer(rtk_h5.timer_h5_hw_init_ready);
}

const static h5_t hci_h5_int_func = {
    .h5_int_init        = hci_h5_int_init,
    .h5_int_cleanup     = hci_h5_cleanup,
    .h5_send_cmd        = hci_h5_send_cmd,
    .h5_send_acl_data   = hci_h5_send_acl_data,
    .h5_send_sync_cmd   = hci_h5_send_sync_cmd,
};

const h5_t *get_h5_interface()
{
    return &hci_h5_int_func;
}

