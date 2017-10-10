/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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
#include <cpu_event.h>
#include <pthread.h>
#include <signal.h>
#include "hal/soc/soc.h"

#ifdef VCALL_RHINO
#include <k_api.h>

typedef unsigned char
UINT8;          /* Unsigned  8 bit quantity        */
typedef signed   char
INT8;           /* Signed    8 bit quantity        */
typedef unsigned short
UINT16;         /* Unsigned 16 bit quantity        */
typedef signed   short
INT16;          /* Signed   16 bit quantity        */
typedef uint32_t
UINT32;         /* Unsigned 32 bit quantity        */
typedef int32_t
INT32;          /* Signed   32 bit quantity        */

static struct termios term_orig;
static sigset_t sigset;

#define MAX_UART_NUM         1
#define CLI_BUFQUE_NUM       2

enum _uart_status_e {
    _UART_STATUS_CLOSED = 0,
    _UART_STATUS_OPENED,
};


typedef struct {
    uint8_t             uart;
    uint8_t             status;
    kbuf_queue_t       *bufque;
    kmutex_t            tx_mutex;
    pthread_t           threaid;
} _uart_drv_t;

static _uart_drv_t _uart_drv[MAX_UART_NUM];

extern int32_t uart_read_byte( uint8_t *rx_byte );
extern void uart_write_byte( uint8_t byte );
extern uint8_t uart_is_tx_fifo_empty( void );
extern uint8_t uart_is_tx_fifo_full( void );
extern void uart_set_tx_stop_end_int( uint8_t set );

static void exit_cleanup(void)
{
    tcsetattr(0, TCSANOW, &term_orig);
}

static void cli_input_handler(char *nbuf)
{
    _uart_drv_t    *pdrv = &_uart_drv[0];

    krhino_buf_queue_send(pdrv->bufque, nbuf, 1);
    cpu_event_free(nbuf);
    return;
}

static void * clirev_thread(void * arg)
{
    fd_set fdset;
    int ret;


    sigaddset(&sigset, SIGALRM);
    ret = pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    if(ret != 0) {
        perror("sigmask failed");
        return NULL;
    }

    while(1) {
        FD_ZERO(&fdset);
        FD_SET(0, &fdset);

        /* Wait for a packet to arrive. */
        ret = select(1, &fdset, NULL, NULL, NULL);
        if(ret == 1) {
            char *nbuf = cpu_event_malloc(8);

            /* Obtain the size of the packet and put it into the "len"
             variable. */
            int readlen = read(0, nbuf, 1);
            if (readlen < 0) {
                perror("cli read eror");
                continue;
            }

            /* Handle incoming packet. */
            cpu_call_handler((cpu_event_handler)cli_input_handler, nbuf);
        } else if(ret == -1) {
            perror("cli_thread: select");
        }
    }

    return NULL;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    _uart_drv_t    *pdrv = &_uart_drv[0];
    struct termios  term_vi;
    kstat_t         stat;

    if (pdrv->status == _UART_STATUS_CLOSED) {
        krhino_mutex_create( &pdrv->tx_mutex, "TX_MUTEX" );
        pdrv->status = _UART_STATUS_OPENED;
        tcgetattr(0, &term_orig);
        term_vi = term_orig;
        term_vi.c_lflag &= (~ICANON & ~ECHO);   // leave ISIG ON- allow intr's
        term_vi.c_iflag &= (~IXON & ~ICRNL);
        tcsetattr(0, TCSANOW, &term_vi);
        atexit(exit_cleanup);
        stat = krhino_buf_queue_dyn_create(&pdrv->bufque, "cli", 256 , CLI_BUFQUE_NUM);
        if(stat != RHINO_SUCCESS) {
            return stat;
        }

        /* it has to be pthread, as it needs to access linux functions */
        pthread_create(&pdrv->threaid, NULL, clirev_thread, NULL);
    }
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    krhino_mutex_del(&pdrv->tx_mutex);
    pdrv->status = _UART_STATUS_CLOSED;
    tcsetattr(0, TCSANOW, &term_orig);
    krhino_buf_queue_dyn_del(pdrv->bufque);
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i = 0;
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    krhino_mutex_lock(&pdrv->tx_mutex, RHINO_WAIT_FOREVER);

    for ( i = 0; i < size; i++ ) {
        putchar( ((uint8_t *)data)[i] );
    }

    krhino_mutex_unlock( &pdrv->tx_mutex );

    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    uint32_t       readlen  = 0;
    uint32_t       totallen = 0;
    kstat_t        retval;
    _uart_drv_t   *pdrv = &_uart_drv[uart->port];

    while(1) {
        retval = krhino_buf_queue_recv(pdrv->bufque, RHINO_WAIT_FOREVER, data, &readlen);
        if(retval != RHINO_SUCCESS) {
            if (recv_size) {
                *recv_size = totallen;
            }
            return -1;
        }
        totallen += readlen;
        if(totallen >=  expect_size) {
            break;
        }
    }
    if (recv_size) {
        *recv_size = totallen;
    }
    return 0;
}
#else
#ifdef AOS_ATCMD
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define AT_UART_PORT 1
#define AT_UART_DEV "/dev/ttyUSB1"
static int at_uart_fd = -1;

int32_t hal_uart_init(uart_dev_t *uart)
{
    if (uart->port != AT_UART_PORT) return 0;

    // still have problem with below stty setting <TODO>
    //system("stty -F /dev/ttyUSB1 ispeed 115200 ospeed 115200 -echo cs8 -cstopb -parenb");
    if ((at_uart_fd = open(AT_UART_DEV, O_RDWR | O_NOCTTY)) == -1) {
        printf("open at uart failed\r\n");
        return -1;
    }

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart->port == AT_UART_PORT) close(at_uart_fd);
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    if (uart->port == AT_UART_PORT) {
        write(at_uart_fd, data, size);
    }
    else write(1, data, size);
    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int fd, n;
    if (uart->port == AT_UART_PORT) fd = at_uart_fd;
    else fd = 1;

    if ((n = read(fd, data, expect_size)) == -1) {
        printf("read failed\r\n");
        return -1;
    }

    if (uart->port != AT_UART_PORT && *(char *)data == '\n')
        *(char *)data = '\r';
    if (recv_size)
        *recv_size = n;

    return 0;
}
#else // #ifdef AOS_ATCMD
int32_t hal_uart_init(uart_dev_t *uart)
{
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    write(1, data, size);
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int n = read(1, data, expect_size);
    if (*(char *)data == '\n')
        *(char *)data = '\r';
    if (recv_size)
        *recv_size = n;
    return 0;
}
#endif
#endif

