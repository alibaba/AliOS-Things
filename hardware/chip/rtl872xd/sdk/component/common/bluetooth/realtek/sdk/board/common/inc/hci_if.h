/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _HCI_IF_H_
#define _HCI_IF_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HCI_IF_EVT_OPENED,     /* hci I/F open completed */
    HCI_IF_EVT_CLOSED,     /* hci I/F close completed */
    HCI_IF_EVT_DATA_IND,   /* hci I/F rx data indicated */
    HCI_IF_EVT_DATA_XMIT,  /* hci I/F tx data transmitted */
    HCI_IF_EVT_ERROR,      /* hci I/F error occurred */
} T_HCI_IF_EVT;

typedef bool (*P_HCI_IF_CALLBACK)(T_HCI_IF_EVT evt, bool status, uint8_t *p_buf, uint32_t len);

bool hci_if_open(P_HCI_IF_CALLBACK p_callback);

bool hci_if_close(void);

bool hci_if_write(uint8_t *p_buf, uint32_t len);

bool hci_if_confirm(uint8_t *p_buf);


#ifdef __cplusplus
}
#endif

#endif /* _HCI_IF_H_ */
