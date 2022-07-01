/**
 * \file            lwesp_ll_posix.c
 * \brief           Low-level communication with ESP device for WIN32
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Author:          imi415 <imi415.public@gmail.com>
 * Version:         v1.1.2-dev
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

#include "system/lwesp_ll.h"
#include "lwesp/lwesp.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_input.h"

#if !__DOXYGEN__

static uint8_t initialized = 0;

static int uart_fd;

#define LOG_BUFFER_LEN 4096

static uint32_t data_buffer_ptr = 0;
static uint8_t data_buffer[LOG_BUFFER_LEN];

static lwesp_sys_thread_t uart_thread_handle;

static void uart_thread(void* param);

#define UART_FILE "/dev/ttyUSB0"

static size_t
send_data(const void* data, size_t len) {
    if (uart_fd <= 0) {
        return 0;
    }
    int ret = write(uart_fd, data, len);
    return ret;
}

static void
configure_uart(uint32_t baudrate) {
    if (!initialized) {
        uart_fd = open(UART_FILE, O_RDWR);
        if (uart_fd < 0) {
            fprintf(stderr, "Open serial device failed\n");
            return;
        }
    }

    /* Set UART parameters here. */
    struct termios tio;
    if (tcgetattr(uart_fd, &tio) != 0) {
        fprintf(stderr, "Get serial attr failed.\n");
        return;
    }

    /* Only common baud rates are added. */
    int tio_baudrate;
    switch (baudrate) {
        case 9600:
            tio_baudrate = B9600;
            break;
        case 38400:
            tio_baudrate = B38400;
            break;
        case 57600:
            tio_baudrate = B57600;
            break;
        case 115200:
            tio_baudrate = B115200;
            break;
        case 921600:
            tio_baudrate = B921600;
            break;
        default:
            tio_baudrate = B115200;
            break;
    }

    cfsetispeed(&tio, tio_baudrate);
    cfsetospeed(&tio, tio_baudrate);

    tio.c_cflag = (tio.c_cflag & ~CSIZE) | CS8;
    tio.c_iflag &= ~IGNBRK;
    tio.c_oflag &= ~OPOST;

    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 10;

    tio.c_iflag &= ~(IXON | IXOFF | IXANY);
    tio.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~(PARENB | PARODD);
    tio.c_cflag &= ~CSTOPB;

    tio.c_cflag &= ~CRTSCTS;                    /* Without hardware flow control */
    /* tio.c_cflag |= CRTSCTS; */               /* With hardware flow control */

    cfmakeraw(&tio);

    if (tcsetattr(uart_fd, TCSANOW, &tio) != 0) {
        fprintf(stderr, "Set serial attr failed.\n");
        return;
    }

    tcflush(uart_fd, TCIOFLUSH);

    /* On first function call, create a thread to read data from COM port */
    if (!initialized) {
        lwesp_sys_thread_create(&uart_thread_handle, "lwesp_ll_thread", uart_thread, NULL, 0, 0);
    }
}

static void
log_command(uint8_t new_byte) {
    /* Log buffer not full */
    if (data_buffer_ptr < LOG_BUFFER_LEN - 1) {
        data_buffer[data_buffer_ptr] = new_byte;
        data_buffer_ptr++;
    }

    if (data_buffer_ptr > 0 && data_buffer[data_buffer_ptr - 1] == '\n') {
        data_buffer[data_buffer_ptr] = '\0';

        fprintf(stderr, "[AT <]: \e[32m%s\e[0m", data_buffer);
        data_buffer_ptr = 0;
    }
}

static void
uart_thread(void* param) {
    uint8_t data_byte;
    for (;;) {
        if (read(uart_fd, &data_byte, 1) > 0) {
            log_command(data_byte);
            /* Send received data to input processing module */
#if LWESP_CFG_INPUT_USE_PROCESS
            lwesp_input_process(&data_byte, 1);
#else  /* LWESP_CFG_INPUT_USE_PROCESS */
            lwesp_input(data_byte, 1);
#endif /* LWESP_CFG_INPUT_USE_PROCESS */
        }
    }
}

static uint8_t
reset_device(uint8_t state) {
    return 0;
}

lwespr_t
lwesp_ll_init(lwesp_ll_t* ll) {
#if !LWESP_CFG_MEM_CUSTOM
    /* Step 1: Configure memory for dynamic allocations */
    static uint8_t memory[0x10000];             /* Create memory for dynamic allocations with specific size */

    /*
     * Create memory region(s) of memory.
     * If device has internal/external memory available,
     * multiple memories may be used
     */
    lwesp_mem_region_t mem_regions[] = {
        { memory, sizeof(memory) }
    };
    if (!initialized) {
        lwesp_mem_assignmemory(mem_regions, LWESP_ARRAYSIZE(mem_regions));  /* Assign memory for allocations to ESP library */
    }
#endif /* !LWESP_CFG_MEM_CUSTOM */

    /* Step 2: Set AT port send function to use when we have data to transmit */
    if (!initialized) {
        ll->send_fn = send_data;                /* Set callback function to send data */
        ll->reset_fn = reset_device;
    }

    /* Step 3: Configure AT port to be able to send/receive data to/from ESP device */
    configure_uart(ll->uart.baudrate);          /* Initialize UART for communication */
    initialized = 1;

    return lwespOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 */
lwespr_t
lwesp_ll_deinit(lwesp_ll_t* ll) {
    if (uart_thread_handle != NULL) {
        lwesp_sys_thread_terminate(&uart_thread_handle);
        uart_thread_handle = NULL;
    }
    initialized = 0;                            /* Clear initialized flag */
    return lwespOK;
}

#endif /* !__DOXYGEN__ */
