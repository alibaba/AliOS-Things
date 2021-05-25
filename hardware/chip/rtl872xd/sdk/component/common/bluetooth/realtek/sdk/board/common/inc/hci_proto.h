/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _HCI_PROTO_H_
#define _HCI_PROTO_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HCI_IF_TASK_PRIORITY            5
#define HCI_IF_STACK_SIZE               0x400

#define HCI_IF_MSG_OPEN                 0x01    /* hci I/F open request */
#define HCI_IF_MSG_CLOSE                0x02    /* hci I/F close request */
#define HCI_IF_MSG_READY                0x03    /* hci I/F open succeeded */
#define HCI_IF_MSG_FAIL                 0x04    /* hci I/F open failed */
#define HCI_IF_MSG_TX_REQ               0x05    /* hci I/F xmit request */
#define HCI_IF_MSG_TX_RSP               0x06    /* hci I/F xmit response */
#define HCI_IF_MSG_RX_IND               0x07    /* hci I/F rx indicate */
#define HCI_IF_MSG_RX_CFM               0x08    /* hci I/F rx confirm */

typedef enum
{
    HCI_IF_STATE_IDLE,
    HCI_IF_STATE_OPEN,
    HCI_IF_STATE_READY,
} T_HCI_IF_STATE;

typedef struct
{
    uint8_t    *p_buf;
    uint16_t    len;
} T_HCI_XMIT_DATA;

typedef void (*P_HCI_PROTO_OPEN)(void);

typedef void (*P_HCI_PROTO_CLOSE)(void);

typedef bool (*P_HCI_PROTO_SEND)(uint8_t *p_buf, uint16_t len);

typedef void (*P_HCI_PROTO_RECV)(void);

typedef void (*P_HCI_PROTO_CFM)(void);

typedef struct
{
    P_HCI_PROTO_OPEN    open;
    P_HCI_PROTO_CLOSE   close;
    P_HCI_PROTO_SEND    send;
    P_HCI_PROTO_RECV    recv;
    P_HCI_PROTO_CFM     cfm;
} T_HCI_PROTO;

typedef struct
{
    T_HCI_IF_STATE       state;         /* hci I/F state */
    void                *task_handle;   /* hci I/F task handle */
    void                *msg_q;         /* task msg queue */
    void                *xmit_q;        /* tx req queue */
    void                *cfm_q;         /* rx cfm queue */
    uint8_t             *tx_buf;        /* tx buffer pointer */
    uint16_t             tx_len;        /* tx buffer length */
    P_HCI_IF_CALLBACK    callback;      /* hci I/F event callback */
    const T_HCI_PROTO   *proto;         /* hci h4/h5 proto interfaces */
} T_HCI_IF;

extern T_HCI_IF hci_if;
extern const T_HCI_PROTO hci_h4_proto;

#ifdef __cplusplus
}
#endif

#endif /* _HCI_PROTO_H_ */
