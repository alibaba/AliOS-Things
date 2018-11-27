/**
 * \file
 *
 * \brief UART HAL related functionality implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
#include <atmel_start.h>
#include <hal/soc/soc.h>
#include <string.h>
#include <aos/errno.h>

/* Global Variable for STUDIO Output */
uart_dev_t uart_0 = {
   .port = STDIO_UART,    /* uart port */
   .config = {921600, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = &USART_2      /* priv data */
};

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = -1;
    enum usart_character_size size;
    enum usart_parity parity;
    enum usart_stop_bits stop;
    union usart_flow_control_state flow;

    if (uart == NULL) {
        return -1;
    }

    switch (uart->port) {
        case 0:
            uart->priv = &USART_0;
            break;
        case 2:
            uart->priv = &USART_2;
            break;
        /* if ohter uart exist add init code here */
        default:
        break;
    }

	return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }
    if (uart->priv == NULL) { /* USART_2 is used for STDIO */
        uart->priv = &USART_2;
    }

    if (io_write(&((struct usart_os_descriptor *)uart->priv)->io, data, size, timeout) != size) {
      return EIO;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    if (uart->priv == NULL) { /* USART_2 is used for STDIO */
        uart->priv = &USART_2;
    }

    *recv_size = io_read(&((struct usart_os_descriptor *)uart->priv)->io, (uint8_t *)data, expect_size, timeout);

    return (*recv_size == 0) ? -1 : 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart == NULL) {
        return -1;
    }

    return usart_os_deinit((struct usart_os_descriptor *)(uart->priv));
}
