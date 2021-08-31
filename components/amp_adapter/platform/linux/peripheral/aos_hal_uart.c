/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <errno.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "aos_hal_uart.h"

int32_t aos_hal_uart_callback(uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *args)
{
    return 0;
}

typedef unsigned char UINT8; /* Unsigned  8 bit quantity        */
typedef signed char INT8; /* Signed    8 bit quantity        */
typedef unsigned short UINT16; /* Unsigned 16 bit quantity        */
typedef signed short INT16; /* Signed   16 bit quantity        */
typedef uint32_t UINT32; /* Unsigned 32 bit quantity        */
typedef int32_t INT32; /* Signed   32 bit quantity        */

static struct termios term_orig;

#define MAX_UART_NUM 1

enum _uart_status_e {
    _UART_STATUS_CLOSED = 0,
    _UART_STATUS_OPENED,
};

typedef struct {
    uint8_t uart;
    uint8_t status;
    pthread_t threaid;
    pthread_mutex_t mutex;
} _uart_drv_t;

static _uart_drv_t _uart_drv[MAX_UART_NUM];

extern int32_t uart_read_byte(uint8_t *rx_byte);
extern void uart_write_byte(uint8_t byte);
extern uint8_t uart_is_tx_fifo_empty(void);
extern uint8_t uart_is_tx_fifo_full(void);
extern void uart_set_tx_stop_end_int(uint8_t set);

static void exit_cleanup(void)
{
    tcsetattr(0, TCSANOW, &term_orig);
}

int32_t aos_hal_uart_init(uart_dev_t *uart)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    int err_num;
    _uart_drv_t *pdrv = &_uart_drv[0];
    struct termios term_vi;

    if (pdrv->status == _UART_STATUS_CLOSED) {
        pdrv->status = _UART_STATUS_OPENED;
        tcgetattr(0, &term_orig);
        term_vi = term_orig;
        term_vi.c_lflag &= (~ICANON & ~ECHO); // leave ISIG ON- allow intr's
        term_vi.c_iflag &= (~IXON & ~ICRNL);
        tcsetattr(0, TCSANOW, &term_vi);
        atexit(exit_cleanup);

        err_num = pthread_mutex_init(&pdrv->mutex, NULL);
        if (0 != err_num) {
            perror("create mutex failed\n");
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

int32_t aos_hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    pthread_mutex_destroy(&pdrv->mutex);
    pdrv->status = _UART_STATUS_CLOSED;
    tcsetattr(0, TCSANOW, &term_orig);
    return 0;
}

int32_t aos_hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i = 0;
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    pthread_mutex_lock(&pdrv->mutex);

    for (i = 0; i < size; i++) {
        putchar(((uint8_t *)data)[i]);
    }

    pthread_mutex_unlock(&pdrv->mutex);

    return 0;
}

int aaa = 0;
int32_t aos_hal_uart_recv_poll(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size)
{
    fd_set fdset;
    int ret;
    char nbuf;
    char *read_buf = data;
    (void *)uart;

    if (data == NULL || recv_size == NULL) {
        return -1;
    }
    *recv_size = 0;

    /* Obtain the size of the packet and put it into the "len"
        variable. */
    int readlen = read(0, &nbuf, 1);
    if (readlen > 0) {
        *(char *)data = nbuf;
        *recv_size = 1;
        return 0;
    }

    return -1;
}

int32_t aos_hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    fd_set fdset;
    int ret;
    char nbuf;
    char *read_buf = data;
    struct timeval st;
    time_t t_begin, t_now;

    (void *)uart;

    t_begin = clock();

    if (data == NULL || recv_size == NULL) {
        return -1;
    }
    *recv_size = 0;

    while (1) {
        t_now = clock();
        if (difftime(t_now, t_begin) > timeout) {
            break;
        }
        usleep(10);
        FD_ZERO(&fdset);
        FD_SET(0, &fdset);

        /* Wait for a packet to arrive. */
        st.tv_sec = 0;
        st.tv_usec = 100;
        ret = select(1, &fdset, NULL, NULL, &st);
        if (ret == 1) {
            /* Obtain the size of the packet and put it into the "len"
             variable. */
            int readlen = read(0, &nbuf, 1);
            if (readlen < 0) {
                perror("cli read eror");
                continue;
            }

            /* Handle incoming packet. */
            read_buf[*recv_size] = nbuf;
            *recv_size += 1;
            if (*recv_size >= expect_size) {
                break;
            }
        }
    }
    return 0;
}
