/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <poll.h>
#include <vfsdev/uart_dev.h>
#include "aos/kernel.h"
#include "amp_hal_uart.h"

#ifdef CONFIG_UART_NUM
#define PLATFORM_UART_NUM CONFIG_UART_NUM
#else
#define PLATFORM_UART_NUM 4
#endif

static int uart_fd_table[PLATFORM_UART_NUM];

int32_t amp_hal_uart_init(uart_dev_t *uart)
{
    const char *fmt = "/dev/ttyUART%u";
    char dev_name[32];

    if (!uart || uart->port >= PLATFORM_UART_NUM)
        return -1;

    snprintf(dev_name, sizeof(dev_name) - 1, fmt, (unsigned)uart->port);
    dev_name[sizeof(dev_name) - 1] = '\0';
    uart_fd_table[uart->port] = open(dev_name, 0);

    return uart_fd_table[uart->port] < 0 ? -1 : 0;
}

int32_t amp_hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    ssize_t r;

    if (!uart || uart->port >= PLATFORM_UART_NUM)
        return -1;

    if (!data || size == 0)
        return -1;

    r = write(uart_fd_table[uart->port], data, size);

    return r == size ? 0 : -1;
}

int32_t amp_hal_uart_recv(uart_dev_t *uart, void *data,
                          uint32_t expect_size, uint32_t timeout)
{
    /* deprecated */
    return -1;
}

int32_t amp_hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                             uint32_t *recv_size, uint32_t timeout)
{
    int32_t ret = 0;
    int fd;

    if (!uart || uart->port >= PLATFORM_UART_NUM)
        return -1;

    if (!data || expect_size == 0 || !recv_size)
        return -1;

    fd = uart_fd_table[uart->port];

    while (1) {
        struct pollfd poll_fds[1];
        int timeo;
        ssize_t r;

        r = read(fd, data, expect_size);

        if (r < 0) {
            *recv_size = 0;
            ret = -1;
            break;
        }

        if (r > 0) {
            *recv_size = (uint32_t)r;
            ret = 0;
            break;
        }

        memset(poll_fds, 0, sizeof(poll_fds));
        poll_fds[0].fd = fd;
        poll_fds[0].events = POLLIN;

        if (timeout == AOS_WAIT_FOREVER)
            timeo = -1;
        else if (timeout > INT_MAX)
            timeo = INT_MAX;
        else
            timeo = (int)timeout;

        if (poll(poll_fds, 1, timeo) <= 0) {
            *recv_size = 0;
            ret = -1;
            break;
        }

        if ((poll_fds[0].revents & POLLERR) ||
            !(poll_fds[0].revents & POLLIN)) {
            *recv_size = 0;
            ret = -1;
            break;
        }
    }

    return ret;
}

typedef struct {
    void (*callback)(int, void *, uint16_t, void *);
    void *userdata;
    uart_dev_t *uart;
    int task_running;
    int stop;
    aos_mutex_t lock;
    aos_sem_t sem;
    aos_task_t task;
} uart_recv_notify_t;

static uart_recv_notify_t *uart_recv_notifiers[PLATFORM_UART_NUM];

static void uart_recv_handler(void *args)
{
    uart_recv_notify_t *notify = (uart_recv_notify_t *)args;
    uart_dev_t *uart;
    char recv_buffer[256];
    int recv_size;
    int ret;

    if (!notify)
        return;

    uart = notify->uart;
    while (1) {
        aos_mutex_lock(&notify->lock, AOS_WAIT_FOREVER);
        if (notify->stop) {
            aos_mutex_unlock(&notify->lock);
            break;
        }
        aos_mutex_unlock(&notify->lock);

        ret = amp_hal_uart_recv_II(uart, recv_buffer, sizeof(recv_buffer),
                                   &recv_size, 100);
        if (ret || recv_size <= 0)
            continue;

        notify->callback(uart->port, recv_buffer, recv_size, notify->userdata);
    }

    aos_sem_signal(&notify->sem);
}

#if 0
extern int32_t hal_uart_receive_register(int port,  void(*cb)(int, void *), void *args);
static int uart_port_registered[7];
#endif

int32_t amp_hal_uart_callback(uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *args)
{
#if 0
    if (uart_port_registered[uart->port])
        return 0;
    hal_uart_receive_register(uart, cb, args);
    uart_port_registered[uart->port] = 1;
#else
    uart_recv_notify_t *notify = uart_recv_notifiers[uart->port];

    if (!notify) {
        notify = aos_malloc(sizeof(uart_recv_notify_t));
        if (!notify)
            return -1;
        memset(notify, 0, sizeof(uart_recv_notify_t));
        aos_mutex_new(&notify->lock);
        aos_sem_new(&notify->sem, 0);
        uart_recv_notifiers[uart->port] = notify;
    }

    notify->callback = cb;
    notify->userdata = args;
    notify->uart = uart;

    if (!notify->task_running) {
        if (aos_task_new_ext(&notify->task, "amp_uart_recv",
            uart_recv_handler, notify, 2048, 32))
            return -1;
        notify->task_running = 1;
    }
#endif
    return 0;
}

int32_t amp_hal_uart_finalize(uart_dev_t *uart)
{
    uart_recv_notify_t *notify = uart_recv_notifiers[uart->port];
    if (notify) {
        if (notify->task_running) {
            aos_mutex_lock(&notify->lock, AOS_WAIT_FOREVER);
            notify->stop = 1;
            aos_mutex_unlock(&notify->lock);
            aos_sem_wait(&notify->sem, 3000);
        }
        aos_mutex_free(&notify->lock);
        aos_sem_free(&notify->sem);
        aos_free(notify);
        uart_recv_notifiers[uart->port] = NULL;
    }

    close(uart_fd_table[uart->port]);
    uart_fd_table[uart->port] = -1;

    return 0;
}
