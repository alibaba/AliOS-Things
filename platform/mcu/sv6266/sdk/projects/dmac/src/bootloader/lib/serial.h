#ifndef _SERIAL_H
#define _SERIAL_H

#include "soc_defs.h"

void serial_init(void);
void serial_tx(int);
int serial_rx_ready(void);
int serial_rx(void);

void serial_uart1_init(int BaudRate);
void serial_uart1_tx(int);
int serial_uart1_rx_ready(void);
int serial_uart1_rx(void);
#endif
