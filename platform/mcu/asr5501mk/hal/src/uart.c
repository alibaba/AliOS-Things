#include <stdint.h>
#include "lega_rhino.h"
#include "lega_uart.h"
#include "aos/hal/uart.h"
#include "board.h"

#define HAL_UART_BUF_QUEUE_BYTES 128
lega_queue_t hal_uart_buf_queue[LEGA_UART_NUM];
char * hal_uart_buf_queue_name[LEGA_UART_NUM] =
{
    "uart0_buffer_queue",
    "uart1_buffer_queue",
    "uart2_buffer_queue"
};

void hal_uart0_callback_handler(char ch)
{
    lega_rtos_push_to_queue(&hal_uart_buf_queue[LEGA_UART0_INDEX], &ch, LEGA_NEVER_TIMEOUT);
}

void hal_uart1_callback_handler(char ch)
{
    lega_rtos_push_to_queue(&hal_uart_buf_queue[LEGA_UART1_INDEX], &ch, LEGA_NEVER_TIMEOUT);
}

void hal_uart2_callback_handler(char ch)
{
    lega_rtos_push_to_queue(&hal_uart_buf_queue[LEGA_UART2_INDEX], &ch, LEGA_NEVER_TIMEOUT);
}

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = 0;
	
	if(NULL == uart){
		return -1;
	}
	/*logic port to physical port*/
	if(uart->port == PORT_UART_STD){
		uart->port = LEGA_UART1_INDEX;
		uart->priv = (void *)(hal_uart1_callback_handler);
	}
    else if(uart->port == PORT_UART_TEMP){
		uart->port = LEGA_UART0_INDEX;
	}

    ret = lega_uart_init((lega_uart_dev_t *)uart);
    if(!ret)
    {
        ret = lega_rtos_init_queue(&hal_uart_buf_queue[uart->port], hal_uart_buf_queue_name[uart->port], sizeof(char), HAL_UART_BUF_QUEUE_BYTES);
    }
    return ret;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	if(NULL == uart){
		return -1;
	}
	/*logic port to physical port*/
	if(uart->port == PORT_UART_STD){
		uart->port = LEGA_UART1_INDEX;
		uart->priv = (void *)(hal_uart1_callback_handler);
	}
    else if(uart->port == PORT_UART_TEMP){
		uart->port = LEGA_UART0_INDEX;
	}

    return lega_uart_send((lega_uart_dev_t *)uart, data, size, timeout);
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    int i = 0;
    int32_t ret;
    uint8_t *pdata = (uint8_t *)data;

    if((uart == NULL) || (data == NULL))
    {
        return -1;
    }
	
	/*logic port to physical port*/
	if(uart->port == PORT_UART_STD){
		uart->port = LEGA_UART1_INDEX;
		uart->priv = (void *)(hal_uart1_callback_handler);
	}
    else if(uart->port == PORT_UART_TEMP){
		uart->port = LEGA_UART0_INDEX;
	}

    for (i = 0; i < expect_size; i++)
    {
        ret = lega_rtos_pop_from_queue(&hal_uart_buf_queue[uart->port], &pdata[i], timeout);
        if(ret)
        {
            return ret;
        }
    }

    return 0;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret;

    if((uart == NULL) || (data == NULL))
    {
        return -1;
    }
	
	/*logic port to physical port*/
	if(uart->port == PORT_UART_STD){
		uart->port = LEGA_UART1_INDEX;
		uart->priv = (void *)(hal_uart1_callback_handler);
	}
    else if(uart->port == PORT_UART_TEMP){
		uart->port = LEGA_UART0_INDEX;
	}

    for (i = 0; i < expect_size; i++)
    {
        ret = lega_rtos_pop_from_queue(&hal_uart_buf_queue[uart->port], &pdata[i], timeout);

        if(!ret)
        {
            rx_count++;
        }
        else
        {
            *recv_size = rx_count;
            return ret;
        }
    }
    if(recv_size)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret;
	
	if(NULL == uart){
		return -1;
	}

	/*logic port to physical port*/
	if(uart->port == PORT_UART_STD){
		uart->port = LEGA_UART1_INDEX;
		uart->priv = (void *)(hal_uart1_callback_handler);
	}
    else if(uart->port == PORT_UART_TEMP){
		uart->port = LEGA_UART0_INDEX;
	}

    ret = lega_rtos_deinit_queue(&hal_uart_buf_queue[uart->port]);
    if(!ret)
    {
        return lega_uart_finalize((lega_uart_dev_t *)uart);
    }
    else
    {
        return ret;
    }
}