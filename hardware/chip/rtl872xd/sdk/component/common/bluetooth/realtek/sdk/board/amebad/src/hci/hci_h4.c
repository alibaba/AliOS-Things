/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_task.h"
#include "os_mem.h"
#include "os_msg.h"
#include "os_sched.h"
#include "trace.h"
//#include "bte_api.h"
#include "hci_if.h"
#include "hci_tp.h"
//#include "bt_defs.h"
#include "bt_types.h"
#include "hci_code.h"
#include "hci_proto.h"

/***** Just for compile, need fix *****/
#define HCI_ACL_HDR_LEN        5   /* packet type (1), handle (2), length (2) */
#define HCI_EVT_HDR_LEN        3   /* packet type (1), event type (1), length (1) */
#define HCI_RX_ACL_BUF_OFFSET  0
#define br_rx_frag_pool        0
#define le_rx_frag_pool        0
#define bt_sys_pool            0
/***** Just for compile, need fix *****/

typedef struct
{
    uint8_t     hdr_buf[HCI_ACL_HDR_LEN];
    uint8_t     hdr_offset;
    uint8_t     hdr_len;
    uint16_t    payload_len;
    uint8_t    *rx_buf;
    uint16_t    rx_len;
    uint16_t    rx_offset;
} T_HCI_H4;

T_HCI_H4 hci_h4;

void hci_h4_recv(void);

bool hci_h4_tp_open_cb(bool status)
{
    uint8_t msg;

    if (status == true)
    {
        msg = HCI_IF_MSG_READY;
    }
    else
    {
        msg = HCI_IF_MSG_FAIL;
    }

    return os_msg_send(hci_if.msg_q, &msg, 0);
}

bool hci_h4_tp_tx_cb(void)
{
    uint8_t msg = HCI_IF_MSG_TX_RSP;

    return os_msg_send(hci_if.msg_q, &msg, 0);
}

bool hci_h4_tp_rx_ind(void)
{
    uint8_t msg = HCI_IF_MSG_RX_IND;

    return os_msg_send(hci_if.msg_q, &msg, 0);
}

uint8_t *hci_h4_alloc_buf(void)
{
    uint16_t len;
    uint16_t offset;
    uint8_t  pool_id;

    switch (hci_h4.hdr_buf[0])
    {
    case HCI_ACL_PKT:
        offset  = HCI_RX_ACL_BUF_OFFSET;
#if F_BT_BREDR_SUPPORT
        pool_id = br_rx_frag_pool;
#else
        pool_id = le_rx_frag_pool;
#endif
        break;

    case HCI_EVT_PKT:
        offset  = 0;
        pool_id = bt_sys_pool;
        break;

    default:
        return NULL;
    }

    len = offset + hci_h4.hdr_len + hci_h4.payload_len;

    hci_h4.rx_buf = os_mem_zalloc(0, len);//bte_buffer_get(pool_id, len);
    if (hci_h4.rx_buf != NULL)
    {
        memcpy(hci_h4.rx_buf + offset, hci_h4.hdr_buf, hci_h4.hdr_len);
        hci_h4.rx_offset = offset + hci_h4.hdr_len;
        hci_h4.rx_len = len;

        /* put pkt type at the first byte for ACL data */
        hci_h4.rx_buf[0] = hci_h4.hdr_buf[0];
    }

    return hci_h4.rx_buf;
}

void hci_h4_open(void)
{
    memset(&hci_h4, 0, sizeof(T_HCI_H4));

    hci_tp_open(hci_h4_tp_open_cb, hci_h4_tp_rx_ind);
}

void hci_h4_close(void)
{
#if F_BT_CONTROLLER_POWER_CONTROL
    if (hci_h4.rx_buf != NULL)
    {
        os_mem_free(hci_h4.rx_buf);//bte_buffer_put(hci_h4.rx_buf);
        hci_h4.rx_buf = NULL;
        HCI_PRINT_ERROR0("hci_h4_close: rx_buf != NULL");
    }
#endif
    hci_tp_close();
}

bool hci_h4_send(uint8_t *p_buf, uint16_t len)
{
    return hci_tp_send(p_buf, len, hci_h4_tp_tx_cb);
}

void hci_h4_recv(void)
{
    uint16_t rx_len;

    while (hci_h4.hdr_offset == 0 || hci_h4.hdr_offset < hci_h4.hdr_len)
    {
        rx_len = hci_tp_recv(&hci_h4.hdr_buf[hci_h4.hdr_offset], 1);

        /* No available rx data for HCI header */
        if (rx_len == 0)
        {
            return;
        }

        hci_h4.hdr_offset++;

        if (hci_h4.hdr_offset == 1)
        {
            switch (hci_h4.hdr_buf[0])
            {
            case HCI_ACL_PKT:
                hci_h4.hdr_len = HCI_ACL_HDR_LEN;
                break;

            case HCI_EVT_PKT:
                hci_h4.hdr_len = HCI_EVT_HDR_LEN;
                break;

            default:
                HCI_PRINT_ERROR1("hci_h4_recv: invalid pkt type %u", hci_h4.hdr_buf[0]);
                hci_h4.hdr_offset = 0;
                hci_h4.payload_len = 0;
                hci_if.callback(HCI_IF_EVT_ERROR, false, NULL, 0);
                break;
            }
        }
        else if (hci_h4.hdr_offset == hci_h4.hdr_len)
        {
            switch (hci_h4.hdr_buf[0])
            {
            case HCI_ACL_PKT:
                LE_ARRAY_TO_UINT16(hci_h4.payload_len, &hci_h4.hdr_buf[3]);
                if (hci_h4.payload_len == 0)
                {
                    hci_h4.hdr_offset = 0;
                }
                break;

            case HCI_EVT_PKT:
                hci_h4.payload_len = (uint16_t)hci_h4.hdr_buf[2];
                break;

            default:
                break;
            }
        }
    }

    if (hci_h4.payload_len && hci_h4.rx_buf == NULL)
    {
        hci_h4.rx_buf = hci_h4_alloc_buf();
        if (hci_h4.rx_buf == NULL)
        {
            return;
        }
    }

    while (hci_h4.rx_buf && hci_h4.rx_offset < hci_h4.rx_len)
    {
        rx_len = hci_tp_recv(hci_h4.rx_buf + hci_h4.rx_offset,
                             hci_h4.rx_len - hci_h4.rx_offset);

        /* No available rx data for HCI payload */
        if (rx_len == 0)
        {
            return;
        }

        hci_h4.rx_offset += rx_len;

        if (hci_h4.rx_offset == hci_h4.rx_len)
        {
            hci_h4.hdr_offset = 0;

            if (hci_if.state == HCI_IF_STATE_READY)
            {
                hci_if.callback(HCI_IF_EVT_DATA_IND, true, hci_h4.rx_buf, hci_h4.rx_len);
				os_mem_free(hci_h4.rx_buf);
                hci_h4.rx_buf = NULL;
                hci_h4_recv();
            }
            else if (hci_if.state == HCI_IF_STATE_OPEN)
            {
                hci_tp_config(hci_h4.rx_buf, hci_h4.rx_len);
                os_mem_free(hci_h4.rx_buf);//bte_buffer_put(hci_h4.rx_buf);
                hci_h4.rx_buf = NULL;
            }
            else if (hci_if.state == HCI_IF_STATE_IDLE)
            {
                os_mem_free(hci_h4.rx_buf);//bte_buffer_put(hci_h4.rx_buf);
                hci_h4.rx_buf = NULL;
            }
        }
    }
}

void hci_h4_cfm(void)
{
    uint8_t *p_buf;

    if (os_msg_recv(hci_if.cfm_q, &p_buf, 0) == true)
    {
        os_mem_free(hci_h4.rx_buf);//bte_buffer_put(p_buf);
    }

    hci_h4_recv();
}

const T_HCI_PROTO hci_h4_proto =
{
    .open   = hci_h4_open,
    .close  = hci_h4_close,
    .send   = hci_h4_send,
    .recv   = hci_h4_recv,
    .cfm    = hci_h4_cfm,
};

#if F_BT_DEINIT
void hci_h4_pre_deinit(void)
{
    hci_tp_del();
}

void hci_h4_deinit(void)
{
    memset(&hci_h4, 0, sizeof(hci_h4));
}
#endif
