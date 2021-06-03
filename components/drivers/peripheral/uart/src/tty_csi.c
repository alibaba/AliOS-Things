/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/tty_csi.h>

#define RX_BUF_SIZE             256
#define TX_BUF_SIZE             256
#define RX_TASK_STACK_SIZE      4096
#define TX_TASK_STACK_SIZE      4096
#define RX_TASK_PRIO            30
#define TX_TASK_PRIO            30

static aos_status_t set_format(aos_tty_csi_t *tty_csi, tcflag_t mode)
{
    csi_uart_data_bits_t data_bits;
    csi_uart_stop_bits_t stop_bits;
    csi_uart_parity_t parity;
    csi_error_t r;

    switch (mode & CSIZE) {
        case CS5:
            data_bits = UART_DATA_BITS_5;
            break;
        case CS6:
            data_bits = UART_DATA_BITS_6;
            break;
        case CS7:
            data_bits = UART_DATA_BITS_7;
            break;
        case CS8:
            data_bits = UART_DATA_BITS_8;
            break;
        default:
            data_bits = UART_DATA_BITS_8;
            break;
    }

    if (mode & CSTOPB)
        stop_bits = UART_STOP_BITS_2;
    else
        stop_bits = UART_STOP_BITS_1;

    if (mode & PARENB) {
        if (mode & PARODD)
            parity = UART_PARITY_ODD;
        else
            parity = UART_PARITY_EVEN;
    } else {
        parity = UART_PARITY_NONE;
    }

    r = csi_uart_format(&tty_csi->csi_uart, data_bits, parity, stop_bits);

    return (r == CSI_OK) ? 0 : -EIO;
}

static aos_status_t set_baudrate(aos_tty_csi_t *tty_csi, tcflag_t mode)
{
    uint32_t baudrate;
    csi_error_t r;

    switch (mode & CBAUD) {
        case B50:
            baudrate = 50;
            break;
        case B75:
            baudrate = 75;
            break;
        case B110:
            baudrate = 110;
            break;
        case B134:
            baudrate = 134;
            break;
        case B150:
            baudrate = 150;
            break;
        case B200:
            baudrate = 200;
            break;
        case B300:
            baudrate = 300;
            break;
        case B600:
            baudrate = 600;
            break;
        case B1200:
            baudrate = 1200;
            break;
        case B1800:
            baudrate = 1800;
            break;
        case B2400:
            baudrate = 2400;
            break;
        case B4800:
            baudrate = 4800;
            break;
        case B9600:
            baudrate = 9600;
            break;
        case B19200:
            baudrate = 19200;
            break;
        case B38400:
            baudrate = 38400;
            break;
        case B57600:
            baudrate = 57600;
            break;
        case B115200:
            baudrate = 115200;
            break;
        case B230400:
            baudrate = 230400;
            break;
        case B460800:
            baudrate = 460800;
            break;
        case B500000:
            baudrate = 500000;
            break;
        case B576000:
            baudrate = 576000;
            break;
        case B921600:
            baudrate = 921600;
            break;
        case B1000000:
            baudrate = 1000000;
            break;
        case B1152000:
            baudrate = 1152000;
            break;
        case B1500000:
            baudrate = 1500000;
            break;
        case B2000000:
            baudrate = 2000000;
            break;
        case B2500000:
            baudrate = 2500000;
            break;
        case B3000000:
            baudrate = 3000000;
            break;
        case B3500000:
            baudrate = 3500000;
            break;
        case B4000000:
            baudrate = 4000000;
            break;
        default:
            return -EINVAL;
    }

    r = csi_uart_baud(&tty_csi->csi_uart, baudrate);

    return (r == CSI_OK) ? 0 : -EIO;
}

static void rx_task_func(void *arg)
{
    aos_tty_csi_t *tty_csi = (aos_tty_csi_t *)arg;
    aos_tty_t *tty = &tty_csi->tty;
    csi_uart_t *csi_uart = &tty_csi->csi_uart;
    uint8_t buf[RX_BUF_SIZE];

    while (1) {
        ssize_t r;

        (void)aos_sem_wait(&tty_csi->rx_sem, AOS_WAIT_FOREVER);
        r = csi_uart_receive(csi_uart, buf, sizeof(buf), 0);
        if (r > 0) {
            aos_irqsave_t flags;

            flags = aos_spin_lock_irqsave(&tty->lock);
            (void)aos_tty_rx_buffer_produce(tty, buf, r);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        }

        aos_msleep(20);
        aos_sem_signal(&tty_csi->rx_sem);
    }
}

static void tx_task_func(void *arg)
{
    aos_tty_csi_t *tty_csi = (aos_tty_csi_t *)arg;
    aos_tty_t *tty = &tty_csi->tty;
    csi_uart_t *csi_uart = &tty_csi->csi_uart;
    uint8_t buf[TX_BUF_SIZE];
    size_t count = 0;
    size_t offset = 0;

    while (1) {
        (void)aos_sem_wait(&tty_csi->tx_sem, AOS_WAIT_FOREVER);

        if (count == 0) {
            aos_irqsave_t flags;

            flags = aos_spin_lock_irqsave(&tty->lock);
            count = aos_tty_tx_buffer_consume(tty, buf, sizeof(buf));
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        }

        if (count > 0) {
            ssize_t r;

            r = csi_uart_send(csi_uart, &buf[offset], count - offset, 0);
            if (r > 0) {
                offset += r;
                if (offset == count) {
                    count = 0;
                    offset = 0;
                }
            } else if (r < 0) {
                count = 0;
                offset = 0;
            }
        }

        aos_msleep(20);
        aos_sem_signal(&tty_csi->tx_sem);
    }
}

static void tty_csi_unregister(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    aos_sem_free(&tty_csi->tx_sem);
    aos_sem_free(&tty_csi->rx_sem);
}

static aos_status_t tty_csi_startup(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);
    tcflag_t mode;
    aos_status_t ret;

    if (csi_uart_init(&tty_csi->csi_uart, tty->dev.id) != CSI_OK)
        return -EIO;

    mode = CBAUD | CSIZE | CSTOPB | PARENB | PARODD;
    mode &= tty->termios.c_cflag;

    ret = set_format(tty_csi, mode);
    if (ret) {
        csi_uart_uninit(&tty_csi->csi_uart);
        return ret;
    }

    ret = set_baudrate(tty_csi, mode);
    if (ret) {
        csi_uart_uninit(&tty_csi->csi_uart);
        return ret;
    }

    tty_csi->mode = mode;

    ret = aos_task_new_ext(&tty_csi->rx_task, "tty_csi_rx",
                           rx_task_func, tty_csi,
                           RX_TASK_STACK_SIZE, RX_TASK_PRIO);
    if (ret) {
        csi_uart_uninit(&tty_csi->csi_uart);
        return ret;
    }

    ret = aos_task_new_ext(&tty_csi->tx_task, "tty_csi_tx",
                           tx_task_func, tty_csi,
                           TX_TASK_STACK_SIZE, TX_TASK_PRIO);
    if (ret) {
        (void)aos_task_delete(&tty_csi->rx_task);
        csi_uart_uninit(&tty_csi->csi_uart);
        return ret;
    }

    return 0;
}

static void tty_csi_shutdown(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    (void)aos_task_delete(&tty_csi->tx_task);
    (void)aos_task_delete(&tty_csi->rx_task);
    csi_uart_uninit(&tty_csi->csi_uart);
}

static aos_status_t tty_csi_set_attr(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);
    tcflag_t mode;
    aos_status_t ret;

    mode = CBAUD | CSIZE | CSTOPB | PARENB | PARODD;
    mode &= tty->termios.c_cflag;

    ret = set_format(tty_csi, mode);
    if (ret) {
        (void)set_format(tty_csi, tty_csi->mode);
        (void)set_baudrate(tty_csi, tty_csi->mode);
        return ret;
    }

    ret = set_baudrate(tty_csi, mode);
    if (ret) {
        (void)set_format(tty_csi, tty_csi->mode);
        (void)set_baudrate(tty_csi, tty_csi->mode);
        return ret;
    }

    tty_csi->mode = mode;

    return 0;
}

static void tty_csi_enable_rx(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    aos_sem_signal(&tty_csi->rx_sem);
}

static void tty_csi_disable_rx(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    (void)aos_sem_wait(&tty_csi->rx_sem, AOS_WAIT_FOREVER);
}

static void tty_csi_enable_tx(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    aos_sem_signal(&tty_csi->tx_sem);
}

static void tty_csi_disable_tx(aos_tty_t *tty)
{
    aos_tty_csi_t *tty_csi = aos_container_of(tty, aos_tty_csi_t, tty);

    (void)aos_sem_wait(&tty_csi->tx_sem, AOS_WAIT_FOREVER);
}

static void tty_csi_start_tx(aos_tty_t *tty)
{
}

static const aos_tty_ops_t tty_csi_ops = {
    .unregister = tty_csi_unregister,
    .startup    = tty_csi_startup,
    .shutdown   = tty_csi_shutdown,
    .set_attr   = tty_csi_set_attr,
    .enable_rx  = tty_csi_enable_rx,
    .disable_rx = tty_csi_disable_rx,
    .enable_tx  = tty_csi_enable_tx,
    .disable_tx = tty_csi_disable_tx,
    .start_tx   = tty_csi_start_tx,
};

aos_status_t aos_tty_csi_register(aos_tty_csi_t *tty_csi)
{
    aos_status_t ret;

    if (!tty_csi)
        return -EINVAL;

    tty_csi->tty.ops = &tty_csi_ops;
    ret = aos_sem_new(&tty_csi->rx_sem, 0);
    if (ret)
        return ret;

    ret = aos_sem_new(&tty_csi->tx_sem, 0);
    if (ret) {
        aos_sem_free(&tty_csi->rx_sem);
        return ret;
    }

    ret = aos_tty_register(&tty_csi->tty);
    if (ret) {
        aos_sem_free(&tty_csi->tx_sem);
        aos_sem_free(&tty_csi->rx_sem);
        return ret;
    }

    return 0;
}

aos_status_t aos_tty_csi_unregister(uint32_t id)
{
    return aos_tty_unregister(id);
}
