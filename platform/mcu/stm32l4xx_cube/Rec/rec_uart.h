
#ifndef __REC_UART_H__
#define __REC_UART_H__

void rec_uart_init(void);
void rec_uart_deinit(void);
void rec_uart_send(unsigned char *buff, int len);
void rec_uart_send_string(char *buff);
unsigned char uart_recv_byte(unsigned char *c);

#endif // __REC_UART_H__

