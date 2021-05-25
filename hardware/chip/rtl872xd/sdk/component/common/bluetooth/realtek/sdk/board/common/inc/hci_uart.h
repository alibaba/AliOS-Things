/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */
#ifndef _HCI_UART_H_
#define _HCI_UART_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
  

typedef bool (*P_UART_TX_CB)(void);
typedef bool (*P_UART_RX_CB)(void);


bool hci_uart_init(P_UART_RX_CB rx_ind);
bool hci_uart_deinit(void);

uint16_t hci_uart_recv(uint8_t *p_buf, uint16_t size);

bool hci_uart_tx(uint8_t *p_buf, uint16_t len, P_UART_TX_CB tx_cb);

void hci_uart_set_baudrate(uint32_t baudrate);


void set_hci_uart_out(bool flag);

bool hci_uart_tx_bridge(uint8_t rc);

bool hci_uart_rx_bridge(uint8_t rc);


#ifdef __cplusplus
}
#endif

#endif /* _HCI_UART_H_ */
