/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_task.h"
#include "os_mem.h"
#include "os_msg.h"
#include "trace.h"
//#include "bt_defs.h"
//#include "bte_api.h"
#include "hci_code.h"
#include "hci_if.h"
#include "hci_proto.h"
#include "bt_flags.h"
//#include "otp.h"

/***** Just for compile, need fix *****/
#define HCI_TASK_SIZE        2048
#define HCI_TASK_PRIORITY    5		//11-(-5)=16
#define HCI_TX_ACL_RSVD_SIZE 8
/***** Just for compile, need fix *****/

T_HCI_IF hci_if;

void hci_if_tx_req(void)
{
    T_HCI_XMIT_DATA  tx_data;
    uint8_t         *p_buf;
    uint16_t         len;

    if (hci_if.tx_buf == NULL)
    {
        if (os_msg_recv(hci_if.xmit_q, &tx_data, 0) == true)
        {
            hci_if.tx_buf = tx_data.p_buf;
            hci_if.tx_len = tx_data.len;
            p_buf         = tx_data.p_buf;
            len           = tx_data.len;

            if (*p_buf == HCI_ACL_PKT)
            {
                p_buf += HCI_TX_ACL_RSVD_SIZE;
                len   -= HCI_TX_ACL_RSVD_SIZE;
            }

            hci_if.proto->send(p_buf, len);
        }
    }
}

void hci_if_tx_rsp(void)
{
    hci_if.callback(HCI_IF_EVT_DATA_XMIT, true, hci_if.tx_buf, hci_if.tx_len);
    hci_if.tx_buf = NULL;
    hci_if_tx_req();
}

void hci_if_task(void *p_param)
{
    uint8_t msg;

    (void)p_param;

    while (true)
    {
        if (os_msg_recv(hci_if.msg_q, &msg, 0xFFFFFFFF) == true)
        {
            switch (msg)
            {
            case HCI_IF_MSG_OPEN:
                hci_if.state = HCI_IF_STATE_OPEN;
                hci_if.proto->open();
                break;

            case HCI_IF_MSG_READY:
                hci_if.state = HCI_IF_STATE_READY;
                hci_if.callback(HCI_IF_EVT_OPENED, true, NULL, 0);
                break;

            case HCI_IF_MSG_FAIL:
                hci_if.state = HCI_IF_STATE_IDLE;
                hci_if.callback(HCI_IF_EVT_OPENED, false, NULL, 0);
                break;

            case HCI_IF_MSG_TX_REQ:
                hci_if_tx_req();
                break;

            case HCI_IF_MSG_TX_RSP:
                hci_if_tx_rsp();
                break;

            case HCI_IF_MSG_RX_IND:
                hci_if.proto->recv();
                break;

            case HCI_IF_MSG_RX_CFM:
                hci_if.proto->cfm();
                break;

            case HCI_IF_MSG_CLOSE:
                hci_if.state = HCI_IF_STATE_IDLE;
                hci_if.proto->close();
                hci_if.callback(HCI_IF_EVT_CLOSED, true, NULL, 0);
                break;

            default:
                HCI_PRINT_ERROR1("hci_if_task: unknown msg 0x%02x", msg);
                break;
            }
        }
    }
}

bool hci_if_open(P_HCI_IF_CALLBACK p_callback)
{
    uint8_t msg = HCI_IF_MSG_OPEN;

    if (hci_if.task_handle == NULL)
    {
        hci_if.state    = HCI_IF_STATE_IDLE;
        hci_if.callback = p_callback;
        hci_if.proto    = &hci_h4_proto;

        os_msg_queue_create(&(hci_if.msg_q), 32, sizeof(uint8_t));
        os_msg_queue_create(&(hci_if.xmit_q), 16, sizeof(T_HCI_XMIT_DATA));
        os_msg_queue_create(&(hci_if.cfm_q), 16, sizeof(uint8_t *));

        os_task_create(&hci_if.task_handle, "HCI I/F", hci_if_task, NULL,
                       HCI_TASK_SIZE, HCI_TASK_PRIORITY);
    }
    else
    {
        HCI_PRINT_INFO0("hci_if_open: reopen");
    }

    return os_msg_send(hci_if.msg_q, &msg, 0);
}

bool hci_if_close(void)
{
    uint8_t msg = HCI_IF_MSG_CLOSE;
    HCI_PRINT_INFO0("hci_if_close");

    return os_msg_send(hci_if.msg_q, &msg, 0);
}

bool hci_if_write(uint8_t *p_buf, uint32_t len)
{
    T_HCI_XMIT_DATA tx_data;
    uint8_t         msg;

    /* HCI_PRINT_TRACE2("hci_if_write: buf %p, len %d", p_buf, len); */

    tx_data.p_buf = p_buf;
    tx_data.len   = len;
    msg           = HCI_IF_MSG_TX_REQ;

    if (os_msg_send(hci_if.xmit_q, &tx_data, 0) == true)
    {
        return os_msg_send(hci_if.msg_q, &msg, 0);
    }
    else
    {
        return false;
    }
}

bool hci_if_confirm(uint8_t *p_buf)
{
    uint8_t msg = HCI_IF_MSG_RX_CFM;

    /* HCI_PRINT_TRACE1("hci_if_confirm: buf %p", p_buf); */

    if (os_msg_send(hci_if.cfm_q, &p_buf, 0) == true)
    {
        return os_msg_send(hci_if.msg_q, &msg, 0);
    }
    else
    {
        return false;
    }
}

#if F_BT_DEINIT
extern void hci_h4_pre_deinit(void);
extern void hci_h4_deinit(void);
void hci_if_del_task(void)
{
    //hci_h4_pre_deinit();
    if (hci_if.task_handle != NULL)
    {
        os_task_delete(hci_if.task_handle);
        hci_if.task_handle = NULL;
    }
    if (hci_if.msg_q)
    {
        os_msg_queue_delete(hci_if.msg_q);
        hci_if.msg_q = NULL;
    }
    if (hci_if.xmit_q)
    {
        os_msg_queue_delete(hci_if.xmit_q);
        hci_if.xmit_q = NULL;
    }
    if (hci_if.cfm_q)
    {
        os_msg_queue_delete(hci_if.cfm_q);
        hci_if.cfm_q = NULL;
    }
}

void hci_if_deinit(void)
{
    hci_h4_deinit();
    memset(&hci_if, 0, offsetof(T_HCI_IF, proto));
}
#endif
