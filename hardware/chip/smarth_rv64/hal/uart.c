#include "k_api.h"
#include "aos/hal/uart.h"
#include "drv_usart.h"
#include <csi_config.h>
#include <csi_core.h>
#include "pin.h"
#include "k_config.h"
#include "board.h"

#define MAX_BUF_UART_BYTES   128
kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[MAX_BUF_UART_BYTES];
//static uint8_t rx_buffer[1];
extern usart_handle_t console_handle;

/* can not get data here
void usart_hal_event_fun(int32_t idx, usart_event_e event);
{
    if(idx != CONSOLE_IDX || event != USART_EVENT_RECEIVE_COMPLETE){
        return;
    }

}*/


int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret;
    kstat_t err_code;
    if(NULL == uart || uart->port != CONSOLE_IDX){
        return -1;
    }

    err_code = krhino_buf_queue_create(&g_buf_queue_uart, "buf_queue_uart",
        g_buf_uart, MAX_BUF_UART_BYTES, 1);
    if(err_code){
        return -1;
    }

    console_handle = csi_usart_initialize(uart->port, NULL);
    /* config the UART */
    ret = csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
    
    return ret;
}


int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    size_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

    }
    
    if (recv_size)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i;
    uint8_t byte;
    uint32_t ret = 0;
    for(i=0;i<size;i++)
    {
        byte = *((uint8_t *)data + i);
        ret |= csi_usart_putchar(console_handle, byte);
    }
    return ret;

}

void hal_reboot(void)
{
}

char uart_input_read(void)
{
    uint8_t ch = 0;
    csi_usart_getchar(console_handle, &ch);
    return (char)ch;
}