#include "main.h"
#include "2ndboot.h"

void hal_uart_send_string( char *str);
void hal_uart_send(unsigned char *buff, int len);


void uart_init(void)
{
    hal_uart_send_string("uart init success\r\n");
}

void uart_send(unsigned char *buf, int len)
{
    hal_uart_send(buf, len);
}

void uart_send_string(char *buf)
{
    hal_uart_send_string(buf);
}
void uart_send_byte(unsigned char c) {
    hal_uart_send(&c, 1);
}

unsigned char uart_recv_byte(unsigned char *c)
{
    return hal_get_uart_receive_data(c);
}
