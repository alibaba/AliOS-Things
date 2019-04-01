#ifndef _DBGCON_H
#define _DBGCON_H

#define UART_SPR_BAUD_115200 (115200)
#define UART_SPR_BAUD_921600 (921600)

void dbgcon_init(int baudrate);
void dbgcon_tx(int);
int dbgcon_rx_ready(void);
int dbgcon_rx(void);

#endif
