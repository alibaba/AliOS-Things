/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _HCI_TP_H_
#define _HCI_TP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*P_HCI_TP_OPEN_CB)(bool status);

typedef bool (*P_HCI_TP_TX_CB)(void);

typedef bool (*P_HCI_TP_RX_IND)(void);

void hci_tp_open(P_HCI_TP_OPEN_CB open_cb, P_HCI_TP_RX_IND rx_ind);

void hci_tp_close(void);

void hci_tp_del(void);

void hci_tp_config(uint8_t *p_buf, uint16_t len);

bool hci_tp_send(uint8_t *p_buf, uint16_t len, P_HCI_TP_TX_CB tx_cb);

uint16_t hci_tp_recv(uint8_t *p_buf, uint16_t size);

void hci_tp_set_rx_ind(void (*ready_to_rx)(void));

#ifdef __cplusplus
}
#endif

#endif /* _HCI_TP_H_ */
