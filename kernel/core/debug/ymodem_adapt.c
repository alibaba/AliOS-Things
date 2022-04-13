/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "hal_uart.h"
#include "aos/hal/uart.h"
#include "ymodem_ringbuffer.h"

static enum HAL_UART_ID_T uart_ins = HAL_UART_ID_2;

#define TRACE_RX_BUFFER_SIZE 1032
__SRAMBSS unsigned char hal_trace_rx_buf[TRACE_RX_BUFFER_SIZE];
__SRAMBSS unsigned char uart_rx_ringbuf[TRACE_RX_BUFFER_SIZE*2];
struct HAL_DMA_DESC_T dma_desc_rx;

static boot_ringbuff_t uart_ringbuff = {NULL, NULL, NULL, NULL, 0, 0};

static void console_hal_trace_rx_start(void)
{
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    hal_uart_dma_recv_mask(uart_ins, hal_trace_rx_buf, TRACE_RX_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}


static void console_uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    boot_ringbuff_fill(&uart_ringbuff, hal_trace_rx_buf, xfer_size);
	console_hal_trace_rx_start();
}


uart_dev_t uart_2;

int uart2_init()
{
	uart_2.port = 2;
	uart_2.config.baud_rate = 1500000;
	uart_2.config.data_width = DATA_WIDTH_8BIT;
	uart_2.config.flow_control = FLOW_CONTROL_DISABLED;
	uart_2.config.mode = MODE_TX_RX;
	uart_2.config.parity = NO_PARITY;
	uart_2.config.stop_bits = STOP_BITS_1;

	return hal_uart_init(&uart_2);
}

int hal_trace_rx_start(void)
{
    struct HAL_UART_CFG_T uart_cfg = {
        .dma_rx = true,
        .dma_tx = false,
        .dma_rx_stop_on_err = false,
    };
    uart2_init();// use uart2 to ymodem push
    hal_uart_reopen(uart_ins, &uart_cfg);
    hal_uart_irq_set_dma_handler(uart_ins, console_uart_dma_rx_handler, NULL, NULL);
    console_hal_trace_rx_start();

    return 0;
}

static int ymodem_uart_ringbuffer_init(void)
{
    int ret;
    uint8_t *buffer = NULL;

    memset(&uart_ringbuff, 0, sizeof(uart_ringbuff));

    buffer = uart_rx_ringbuf;

    ret = boot_ringbuff_init(&uart_ringbuff, buffer, TRACE_RX_BUFFER_SIZE*2);

    return ret;
}


void ymodem_uart_init(void)
{
    ymodem_uart_ringbuffer_init();
    hal_trace_rx_start();
    return;
}

void uart_send(unsigned char *buf, int len)
{
    for (uint32_t i = 0; i < len; i++) {
        hal_uart_blocked_putc(uart_ins, buf[i]);
    }
}

void uart_send_byte(unsigned char buf)
{
    hal_uart_blocked_putc(uart_ins, buf);
}

unsigned char uart_recv_byte(unsigned char *c)
{
    int ret = 0;

    ret = boot_ringbuff_read(&uart_ringbuff, c, 1);

    if(ret < 0) {
        return 0;
    }

    return 1;
}