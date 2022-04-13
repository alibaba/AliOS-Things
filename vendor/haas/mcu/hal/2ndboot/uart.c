/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "libc.h"
#include "boot_ringbuffer.h"

static enum HAL_UART_ID_T trace_uart = HAL_UART_ID_0;

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

    hal_uart_dma_recv_mask(trace_uart, hal_trace_rx_buf, TRACE_RX_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}

static void console_uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    boot_ringbuff_fill(&uart_ringbuff, hal_trace_rx_buf, xfer_size);
	console_hal_trace_rx_start();
}

int hal_trace_rx_start(void)
{
	struct HAL_UART_CFG_T uart_cfg = {
	    .dma_rx = true,
		.dma_tx = false,
		.dma_rx_stop_on_err = false,
	};
	hal_uart_reopen(trace_uart, &uart_cfg);
    hal_uart_irq_set_dma_handler(trace_uart, console_uart_dma_rx_handler, NULL, NULL);
    console_hal_trace_rx_start();

    return 0;
}

static int uart_ringbuffer_init(void)
{
    int ret;
    uint8_t *buffer = NULL;

    memset(&uart_ringbuff, 0, sizeof(uart_ringbuff));
    buffer = uart_rx_ringbuf;
    if(buffer == NULL) {
        return -1;
    }

    ret = boot_ringbuff_init(&uart_ringbuff, buffer, TRACE_RX_BUFFER_SIZE*2);

    return ret;
}


void uart_init(void)
{
    uart_ringbuffer_init();
    hal_trace_rx_start();
    return;
}

void uart_send(unsigned char *buf, int len)
{
    for (uint32_t i = 0; i < len; i++) {
        hal_uart_blocked_putc(trace_uart, buf[i]);
    }
}

void uart_send_string(char *buf)
{
    int len = strlen(buf);
    uart_send(buf, len);
}

void uart_send_byte(unsigned char buf)
{
    hal_uart_blocked_putc(trace_uart, buf);
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

