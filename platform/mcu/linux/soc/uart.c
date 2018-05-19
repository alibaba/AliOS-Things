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

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
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

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
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
#include <string.h>
#include <assert.h>
#include <hal/atcmd.h>

static int at_uart_fd = -1;

static int read_and_discard_all_data(const int fd)
{
    int was_msg_already_printed = 0;

    while (1) {
        char buffer[1024];
        const ssize_t read_count = read(fd, buffer, sizeof(buffer));

        if (read_count == 0) {
            /* "EOF" or "connection closed at the other end"*/
            return 0;
        }

        if (read_count > 0) {
            if (!was_msg_already_printed) {
                printf("Some stale data was discarded.\r\n");
                was_msg_already_printed = 1;
            }

            continue;
        }

        assert(read_count == -1);  /* According to the specification. */

        const int errno_code = errno;

        if (errno_code == EINTR)
            continue;

        if (errno_code == EAGAIN ||
            errno_code == EWOULDBLOCK) {
            /**
             * We know that the file descriptor has been opened with
             * O_NONBLOCK or O_NDELAY, and these codes mean that there
             * is no data to read at present.
             */
            return 0;
        }

        /* Some other error has occurred. */
        return -1;
    }
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    char out[128];
    int fd;
    struct termios t_opt;
    speed_t baud;

    if (uart->port != AT_UART_PORT) return 0;

    if ((at_uart_fd = open(AT_UART_LINUX_DEV,
      O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
        printf("open at uart failed\r\n");
        return -1;
    }

    switch(uart->config.baud_rate) {
        case 115200: baud = B115200; break;
        case 921600: baud = B921600; break;
        default: baud = B115200; break;
    }

    fd = at_uart_fd;
    /* set the serial port parameters */
    fcntl(fd, F_SETFL, 0);
    if (0 != tcgetattr(fd, &t_opt))
        return -1;

    if (0 != cfsetispeed(&t_opt, baud))
        return -1;

    if (0 != cfsetospeed(&t_opt, baud))
        return -1;

    // 8N1, flow control, etc.
    t_opt.c_cflag |= (CLOCAL | CREAD);
    if (uart->config.parity == NO_PARITY)
        t_opt.c_cflag &= ~PARENB;
    if (uart->config.stop_bits == STOP_BITS_1)
        t_opt.c_cflag &= ~CSTOPB;
    else
        t_opt.c_cflag |= CSTOPB;
    t_opt.c_cflag &= ~CSIZE;
    switch (uart->config.data_width) {
    case DATA_WIDTH_5BIT:
        t_opt.c_cflag |= CS5;
        break;
    case DATA_WIDTH_6BIT:
        t_opt.c_cflag |= CS6;
        break;
    case DATA_WIDTH_7BIT:
        t_opt.c_cflag |= CS7;
        break;
    case DATA_WIDTH_8BIT:
        t_opt.c_cflag |= CS8;
        break;
    default:
        t_opt.c_cflag |= CS8;
        break;
    }
    t_opt.c_lflag &= ~(ECHO | ECHOE | ISIG | ICANON);
    if (uart->config.flow_control == FLOW_CONTROL_DISABLED)
        t_opt.c_cflag &= ~CRTSCTS;

    /**
     * AT is going to use a binary protocol, so make sure to
     * turn off any CR/LF translation and the like.
     */
    t_opt.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);

    t_opt.c_oflag &= ~OPOST;
    t_opt.c_cc[VMIN] = 0;
    t_opt.c_cc[VTIME] = 5;

    if (0 != tcsetattr(fd, TCSANOW, &t_opt)) {
        return -1;
    }

    printf("open at uart succeed\r\n");

    // clear uart buffer
    read_and_discard_all_data(fd);

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if (uart->port == AT_UART_PORT) close(at_uart_fd);
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data,
                      uint32_t size, uint32_t timeout)
{
    uint32_t ret, rmd = size;

    if (uart->port == AT_UART_PORT) {
        while (rmd > 0) {
            ret = write(at_uart_fd, data + size - rmd, rmd);
            if (ret == -1) {
                printf("write uart fd failed on error: %d.\r\n", errno);
                return -1;
            }
            rmd -= ret;
        }
    }
    else write(1, data, size);
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    int fd, n;

    if (uart->port == AT_UART_PORT) fd = at_uart_fd;
    else fd = 1;

    if ((n = read(fd, data, expect_size)) == -1) {
        //printf("read failed\r\n");
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

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    write(1, data, size);
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
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

