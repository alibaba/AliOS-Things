#include "main.h"
#include "rec_uart.h"

void hal_uart_send_string( char *str);
void hal_uart_send(unsigned char *buff, int len);


void rec_uart_init()
{
    hal_uart_send_string("uart init success\r\n");
}

void rec_uart_deinit()
{
    ;
}


void rec_uart_send(unsigned char *buff, int len)
{
    hal_uart_send(buff, len);
}

void rec_uart_send_string(char *buff)
{
    hal_uart_send_string(buff);
}

unsigned char uart_recv_byte(unsigned char *c)
{
    return hal_get_uart_receive_data(c);
}


