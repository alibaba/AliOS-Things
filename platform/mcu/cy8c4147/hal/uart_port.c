#include "aos/hal/uart.h"
#include "board.h"
#include "uart_port.h"
#include <project.h>

#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdbool.h>
uart_dev_t uart_0;
#ifndef CERTIFICATION
uart_os_t uart_os[UARTn];
#endif
static aos_queue_t rx_queue;
static char rxQueueBuf[UART_RX_QUEUE_SIZE];

extern uint32_t HW_Get_MFT_Baud(void);
int default_UART_Init(void)
{
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = HW_Get_MFT_Baud();
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    return hal_uart_init(&uart_0);
}

extern void wakeup_from_uart(char ch);
extern void linkwan_serial_input(uint8_t cmd);
void UART_API(_customISR)(void)
{
    while(0u != (UART_API(_GetRxInterruptSourceMasked()) & UART_FLAG(_INTR_RX_NOT_EMPTY)))
    {
        char ch=0;
        ch=(char)UART_API(_UartGetChar());
        UART_API(_ClearRxInterruptSource)(UART_FLAG(_INTR_RX_NOT_EMPTY));

#ifndef LOW_POWER_DISABLE
        wakeup_from_uart(ch);
#endif

        if(isprint(ch) || ch == '\r' || ch == '\n') {
#ifdef CONFIG_LINKWAN_AT
        linkwan_serial_input(ch);
#endif
#ifndef CONFIG_PRINT_ECHO_DISABLE
            UART_API(_UartPutChar(ch));
#endif
        }
        #ifndef CERTIFICATION
        if(ch){
            aos_queue_send(&rx_queue,&ch,sizeof(ch));
            aos_sem_signal(&uart_os[STDIO_UART].uart_rx_sem);
        }
        #endif
    }
    /* UART done */
    if(0u != (UART_API(_GetTxInterruptSourceMasked)() & UART_FLAG(_INTR_TX_UART_DONE)))
    {
        UART_API(_ClearTxInterruptSource)(UART_FLAG(_INTR_TX_UART_DONE));
        #ifndef CERTIFICATION

        aos_sem_signal(&uart_os[STDIO_UART].uart_tx_sem);
        #endif
    }
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    switch(uart->port)
    {
        case UART0:
        #ifndef CERTIFICATION

        aos_sem_new(&uart_os[uart->port].uart_tx_sem, 0);
        aos_sem_new(&uart_os[uart->port].uart_rx_sem, 0);
        aos_mutex_new(&uart_os[uart->port].uart_tx_mutex);
        aos_mutex_new(&uart_os[uart->port].uart_rx_mutex);
        aos_queue_new(&rx_queue, rxQueueBuf, UART_RX_QUEUE_SIZE, 1);
        #endif
        UART_API(_SetCustomInterruptHandler)(&UART_API(_customISR));
        UART_API(_Start)();
        break;

        default:
        break;
    }
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint8 *pdata = (uint8 *)data;

    (void) timeout;

    if((uart == NULL) || (data == NULL))
    {
        return -1;
    }

    switch(uart->port)
    {
        case UART0:
        #ifndef CERTIFICATION
        aos_mutex_lock(&uart_os[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);
        #endif
        UART_API(_SpiUartPutArray)(pdata, size);
        #ifndef CERTIFICATION
        aos_sem_wait(&uart_os[uart->port].uart_tx_sem, AOS_WAIT_FOREVER);
        aos_mutex_unlock(&uart_os[uart->port].uart_tx_mutex);
        #endif
        break;

        default:
        break;
    }    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint32 *pdata = (uint32 *)data;
    uint32 i = 0;

    (void)timeout;

    switch(uart->port)
    {
        case UART0:
        #ifndef CERTIFICATION
        aos_mutex_lock(&uart_os[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);
        #endif
        for(i = 0; i < expect_size; i++)
        {
            *pdata = UART_API(_UartGetChar());
        }
        #ifndef CERTIFICATION
        aos_mutex_unlock(&uart_os[uart->port].uart_rx_mutex);
        #endif
        break;

        default:
        break;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    char *pdata = (char *)data;
    uint32 i = 0;
    uint32 rx_count = 0;
    char ch=0;
    unsigned int size=0;
    (void)timeout;

    switch(uart->port)
    {
        case UART0:
        #ifndef CERTIFICATION
        aos_sem_wait(&uart_os[uart->port].uart_rx_sem, AOS_WAIT_FOREVER);
        aos_mutex_lock(&uart_os[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);
        #endif
        for(i = 0; i < expect_size; i++)
        {
            aos_queue_recv(&rx_queue,timeout,&ch,&size);
            *pdata = ch;
            if(*pdata != 0x0)
            {
                rx_count++;
            }
        }
        if(recv_size != NULL)
        {
            *recv_size = rx_count;
        }
        #ifndef CERTIFICATION
        aos_mutex_unlock(&uart_os[uart->port].uart_rx_mutex);
        #endif
        break;

        default:
        break;
    }

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    (void)uart;

    switch(uart->port)
    {
        case UART0:
        UART_API(_Stop());
        break;

        default:
        break;
    }

    return 0;
}

void uart_put_char(int c)
{
    hal_uart_send(&uart_0,&c,1,3000);
}

int32_t aos_uart_recv(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    return hal_uart_recv_II(&uart_0,data,expect_size,recv_size,timeout);
}
int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout)
{
    return hal_uart_send(&uart_0,data,size,timeout);
}
#ifndef CERTIFICATION
void log_cli_init(void)
{

}
#endif
