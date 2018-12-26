#ifndef __HAL_BOOT_UART_H_
#define __HAL_BOOT_UART_H_

void hal_uart_Init(void);
void hal_uart_send_string( char *str);
unsigned char hal_get_uart_receive_data(unsigned char *c);
#endif
