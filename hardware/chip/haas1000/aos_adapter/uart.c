/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#define BES_HAL_DEBUG 0
#include <aos/kernel.h>
#include <aos/tty_core.h>
#include <ulog/ulog.h>
#include <hal_uart.h>
#include <hal_iomux.h>
#include <hal_trace.h>
#include <plat_types.h>

#define UART_DMA_RING_BUFFER_SIZE 256 /* mast be 2^n */

static __SRAMBSS unsigned char _hal_uart0_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart1_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart2_buf[UART_DMA_RING_BUFFER_SIZE];

typedef struct {
    aos_tty_t tty;
    unsigned char *rx_buf;
    unsigned char tx_buf[UART_DMA_RING_BUFFER_SIZE];
    void (*rx_handler)(uint32_t, int, union HAL_UART_IRQ_T);
    void (*tx_handler)(uint32_t, int);
    bool rx_busy;
    bool tx_busy;
} tty_uart_t;

static void hal_set_uart_iomux(uint32_t uart_id)
{
    if (uart_id == HAL_UART_ID_0)
        hal_iomux_set_uart0();
    else if (uart_id == HAL_UART_ID_1)
        hal_iomux_set_uart1();
    else
        hal_iomux_set_uart2();
}

static void tty_uart_unregister(aos_tty_t *tty)
{
}

static aos_status_t tty_uart_startup(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    uint32_t id = tty->dev.id;
    struct HAL_UART_CFG_T cfg;

    switch (tty->termios.c_cflag & CBAUD) {
    case B50:
        cfg.baud = 50;
        break;
    case B75:
        cfg.baud = 75;
        break;
    case B110:
        cfg.baud = 110;
        break;
    case B134:
        cfg.baud = 134;
        break;
    case B150:
        cfg.baud = 150;
        break;
    case B200:
        cfg.baud = 200;
        break;
    case B300:
        cfg.baud = 300;
        break;
    case B600:
        cfg.baud = 600;
        break;
    case B1200:
        cfg.baud = 1200;
        break;
    case B1800:
        cfg.baud = 1800;
        break;
    case B2400:
        cfg.baud = 2400;
        break;
    case B4800:
        cfg.baud = 4800;
        break;
    case B9600:
        cfg.baud = 9600;
        break;
    case B19200:
        cfg.baud = 19200;
        break;
    case B38400:
        cfg.baud = 38400;
        break;
    case B57600:
        cfg.baud = 57600;
        break;
    case B115200:
        cfg.baud = 115200;
        break;
    case B230400:
        cfg.baud = 230400;
        break;
    case B460800:
        cfg.baud = 460800;
        break;
    case B500000:
        cfg.baud = 500000;
        break;
    case B576000:
        cfg.baud = 576000;
        break;
    case B921600:
        cfg.baud = 921600;
        break;
    case B1000000:
        cfg.baud = 1000000;
        break;
    case B1152000:
        cfg.baud = 1152000;
        break;
    case B1500000:
        cfg.baud = 1500000;
        break;
    case B2000000:
        cfg.baud = 2000000;
        break;
    case B2500000:
        cfg.baud = 2500000;
        break;
    case B3000000:
        cfg.baud = 3000000;
        break;
    case B3500000:
        cfg.baud = 3500000;
        break;
    case B4000000:
        cfg.baud = 4000000;
        break;
    default:
        cfg.baud = 9600;
        break;
    }

    if (tty->termios.c_cflag & PARENB) {
        if (tty->termios.c_cflag & PARODD)
            cfg.parity = HAL_UART_PARITY_ODD;
        else
            cfg.parity = HAL_UART_PARITY_EVEN;
    } else {
        cfg.parity = HAL_UART_PARITY_NONE;
    }

    if (tty->termios.c_cflag & CSTOPB)
        cfg.stop = HAL_UART_STOP_BITS_2;
    else
        cfg.stop = HAL_UART_STOP_BITS_1;

    switch (tty->termios.c_cflag & CSIZE) {
    case CS5:
        cfg.data = HAL_UART_DATA_BITS_5;
        break;
    case CS6:
        cfg.data = HAL_UART_DATA_BITS_6;
        break;
    case CS7:
        cfg.data = HAL_UART_DATA_BITS_7;
        break;
    case CS8:
        cfg.data = HAL_UART_DATA_BITS_8;
        break;
    default:
        cfg.data = HAL_UART_DATA_BITS_8;
        break;
    }

    switch (tty->termios.c_cflag & CRTSCTS) {
    case RTSFLOW:
        cfg.flow = HAL_UART_FLOW_CONTROL_RTS;
        break;
    case CTSFLOW:
        cfg.flow = HAL_UART_FLOW_CONTROL_CTS;
        break;
    case CRTSCTS:
        cfg.flow = HAL_UART_FLOW_CONTROL_RTSCTS;
        break;
    default:
        cfg.flow = HAL_UART_FLOW_CONTROL_NONE;
        break;
    }

    cfg.tx_level = HAL_UART_FIFO_LEVEL_1_2;
    cfg.rx_level = HAL_UART_FIFO_LEVEL_1_2;
    cfg.dma_rx = true;
    cfg.dma_tx = true;
    cfg.dma_rx_stop_on_err = false;

    if (hal_uart_open(id, &cfg))
        return -EIO;

    hal_set_uart_iomux(id);
    hal_uart_irq_set_dma_handler(id, uart->rx_handler, uart->tx_handler, NULL);

    return 0;
}

static void tty_uart_shutdown(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    uint32_t id = tty->dev.id;

    hal_uart_close(id);
}

static aos_status_t tty_uart_set_attr(aos_tty_t *tty)
{
    tty_uart_shutdown(tty);

    return tty_uart_startup(tty);
}

static void start_rx_dma(tty_uart_t *uart)
{
    uint32_t id = uart->tty.dev.id;
    struct HAL_DMA_DESC_T desc;
    uint32_t desc_count = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;
    hal_uart_dma_recv_mask(id, uart->rx_buf, UART_DMA_RING_BUFFER_SIZE, &desc, &desc_count, &mask);
}

static void tty_uart_enable_rx(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    uart->rx_busy = true;
    start_rx_dma(uart);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static void tty_uart_disable_rx(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    uint32_t id = tty->dev.id;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    uart->rx_busy = false;
    hal_uart_stop_dma_recv(id);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static void tty_uart_start_tx(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    uint32_t id = tty->dev.id;
    size_t count;

    if (uart->tx_busy)
        return;

    uart->tx_busy = true;
    count = aos_tty_tx_buffer_consume(tty, uart->tx_buf, sizeof(uart->tx_buf));
    hal_uart_dma_send_sync_cache(id, uart->tx_buf, count, NULL, NULL);
}

static void tty_uart_stop_tx(aos_tty_t *tty)
{
    tty_uart_t *uart = aos_container_of(tty, tty_uart_t, tty);
    uint32_t id = tty->dev.id;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);

    if (uart->tx_busy) {
        uart->tx_busy = false;
        hal_uart_stop_dma_send(id);
    }

    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static const aos_tty_ops_t tty_uart_ops = {
    .unregister = tty_uart_unregister,
    .startup    = tty_uart_startup,
    .shutdown   = tty_uart_shutdown,
    .set_attr   = tty_uart_set_attr,
    .enable_rx  = tty_uart_enable_rx,
    .disable_rx = tty_uart_disable_rx,
    .start_tx   = tty_uart_start_tx,
    .stop_tx    = tty_uart_stop_tx,
};

static tty_uart_t tty_uarts[3];

static void tty_uart_rx_handler(tty_uart_t *uart, size_t rx_count)
{
    aos_tty_t *tty = &uart->tty;
    uint32_t id = tty->dev.id;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);

    if (!uart->rx_busy) {
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return;
    }

    (void)aos_tty_rx_buffer_produce(tty, uart->rx_buf, rx_count);
    start_rx_dma(uart);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static void tty_uart1_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    tty_uart_rx_handler(&tty_uarts[1], xfer_size);
}

static void tty_uart2_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    tty_uart_rx_handler(&tty_uarts[2], xfer_size);
}

static void tty_uart_tx_handler(tty_uart_t *uart)
{
    aos_tty_t *tty = &uart->tty;
    uint32_t id = tty->dev.id;
    size_t count;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);

    if (!uart->tx_busy) {
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return;
    }

    count = aos_tty_tx_buffer_consume(tty, uart->tx_buf, sizeof(uart->tx_buf));
    if (count == 0) {
        uart->tx_busy = false;
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return;
    }

    hal_uart_dma_send_sync_cache(id, uart->tx_buf, count, NULL, NULL);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}

static void tty_uart1_tx_handler(uint32_t xfer_size, int dma_error)
{
    tty_uart_tx_handler(&tty_uarts[1]);
}

static void tty_uart2_tx_handler(uint32_t xfer_size, int dma_error)
{
    tty_uart_tx_handler(&tty_uarts[2]);
}

static int tty_uart_init(void)
{
    int ret;

    tty_uarts[1].tty.dev.id = 1;
    tty_uarts[1].tty.ops = &tty_uart_ops;
    tty_uarts[1].tty.flags = 0;
    tty_uarts[1].rx_buf = _hal_uart1_buf;
    tty_uarts[1].rx_handler = tty_uart1_rx_handler;
    tty_uarts[1].tx_handler = tty_uart1_tx_handler;
    tty_uarts[1].rx_busy = false;
    tty_uarts[1].tx_busy = false;
    ret = (int)aos_tty_register(&tty_uarts[1].tty);
    if (ret)
        return ret;

    tty_uarts[2].tty.dev.id = 2;
    tty_uarts[2].tty.ops = &tty_uart_ops;
    tty_uarts[2].tty.flags = 0;
    tty_uarts[2].rx_buf = _hal_uart2_buf;
    tty_uarts[2].rx_handler = tty_uart2_rx_handler;
    tty_uarts[2].tx_handler = tty_uart2_tx_handler;
    tty_uarts[2].rx_busy = false;
    tty_uarts[2].tx_busy = false;
    ret = (int)aos_tty_register(&tty_uarts[2].tty);
    if (ret) {
        (void)aos_tty_unregister(1);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(tty_uart_init)
