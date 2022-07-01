/**
 * \file            lwesp_ll_win32.c
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
 * Version:         v1.1.2-dev
 */
#include "system/lwesp_ll.h"
#include "lwesp/lwesp.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_input.h"

#if !__DOXYGEN__

volatile uint8_t lwesp_ll_win32_driver_ignore_data;
static uint8_t initialized = 0;
static HANDLE thread_handle;
static volatile HANDLE com_port;                /*!< COM port handle */
static uint8_t data_buffer[0x1000];             /*!< Received data array */

static void uart_thread(void* param);

/**
 * \brief           Send data to ESP device, function called from ESP stack when we have data to send
 */
static size_t
send_data(const void* data, size_t len) {
    DWORD written;
    if (com_port != NULL) {
#if !LWESP_CFG_AT_ECHO
        const uint8_t* d = data;
        HANDLE hConsole;

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        for (DWORD i = 0; i < len; ++i) {
            printf("%c", d[i]);
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif /* !LWESP_CFG_AT_ECHO */

        WriteFile(com_port, data, len, &written, NULL);
        FlushFileBuffers(com_port);
        return written;
    }
    return 0;
}

/**
 * \brief           Configure UART (USB to UART)
 * \return          `1` if initialized, `0` otherwise
 */
static uint8_t
configure_uart(uint32_t baudrate) {
    size_t i;
    DCB dcb = { .DCBlength = sizeof(dcb) };

    /*
     * List of COM ports to probe for ESP devices
     * This may be different on your computer
     */
    static const char* com_port_names[] = {
        "\\\\.\\COM16",
        "\\\\.\\COM4",
        "\\\\.\\COM9",
        "\\\\.\\COM10"
    };

    /* Try to open one of listed COM ports */
    if (!initialized) {
        printf("Initializing COM port first time\r\n");
        for (i = 0; i < LWESP_ARRAYSIZE(com_port_names); ++i) {
            printf("Trying to open COM port %s\r\n", com_port_names[i]);
            com_port = CreateFileA(com_port_names[i], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
            if (GetCommState(com_port, &dcb)) {
                printf("Successfully received info for COM port %s. Using this one..\r\n", com_port_names[i]);
                break;
            } else {
                printf("Could not get info for COM port %s\r\n", com_port_names[i]);
            }
        }
        if (i == LWESP_ARRAYSIZE(com_port_names)) {
            printf("Could not get info for any COM port. Entering while loop\r\n");
            while (1) {
                Sleep(1000);
            }
        }
    }

    /* Configure COM port parameters */
    if (GetCommState(com_port, &dcb)) {
        COMMTIMEOUTS timeouts;

        /* Set port config */
        dcb.BaudRate = baudrate;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        if (SetCommState(com_port, &dcb)) {
            /* Set timeouts config */
            if (GetCommTimeouts(com_port, &timeouts)) {
                /* Set timeout to return immediately from ReadFile function */
                timeouts.ReadIntervalTimeout = MAXDWORD;
                timeouts.ReadTotalTimeoutConstant = 0;
                timeouts.ReadTotalTimeoutMultiplier = 0;
                if (SetCommTimeouts(com_port, &timeouts)) {
                    GetCommTimeouts(com_port, &timeouts);
                } else {
                    printf("[LWESP LL] Could not set port timeout config\r\n");
                }
            } else {
                printf("[LWESP LL] Could not get port timeout config\r\n");
            }
        } else {
            printf("[LWESP LL] Could not set port config\r\n");
        }
    } else {
        printf("[LWESP LL] Could not get port info\r\n");
    }

    /* On first function call, create a thread to read data from COM port */
    if (!initialized) {
        lwesp_sys_thread_create(&thread_handle, "lwesp_ll_thread", uart_thread, NULL, 0, 0);
    }
    return 1;
}

/**
 * \brief           UART thread
 */
static void
uart_thread(void* param) {
    DWORD bytes_read;
    lwesp_sys_sem_t sem;
    FILE* file = NULL;

    lwesp_sys_sem_create(&sem, 0);              /* Create semaphore for delay functions */
    while (com_port == NULL) {
        lwesp_sys_sem_wait(&sem, 1);            /* Add some delay with yield */
    }

    fopen_s(&file, "log_file.txt", "w+");       /* Open debug file in write mode */
    while (1) {
        while (com_port == NULL) {
            lwesp_sys_sem_wait(&sem, 1);
        }

        /*
         * Try to read data from COM port
         * and send it to upper layer for processing
         */
        do {
            ReadFile(com_port, data_buffer, sizeof(data_buffer), &bytes_read, NULL);
            if (bytes_read > 0) {
                HANDLE hConsole;
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
                for (DWORD i = 0; i < bytes_read; ++i) {
                    printf("%c", data_buffer[i]);
                }
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                if (lwesp_ll_win32_driver_ignore_data) {
                    printf("IGNORING..\r\n");
                    continue;
                }

                /* Send received data to input processing module */
#if LWESP_CFG_INPUT_USE_PROCESS
                lwesp_input_process(data_buffer, (size_t)bytes_read);
#else /* LWESP_CFG_INPUT_USE_PROCESS */
                lwesp_input(data_buffer, (size_t)bytes_read);
#endif /* !LWESP_CFG_INPUT_USE_PROCESS */

                /* Write received data to output debug file */
                if (file != NULL) {
                    fwrite(data_buffer, 1, bytes_read, file);
                    fflush(file);
                }
            }
        } while (bytes_read == (DWORD)sizeof(data_buffer));

        /* Implement delay to allow other tasks processing */
        lwesp_sys_sem_wait(&sem, 1);
    }
}

/**
 * \brief           Reset device GPIO management
 */
static uint8_t
reset_device(uint8_t state) {
    return 0;                                   /* Hardware reset was not successful */
}

/**
 * \brief           Callback function called from initialization process
 */
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
    if (!configure_uart(ll->uart.baudrate)) {   /* Initialize UART for communication */
        return lwespERR;
    }
    initialized = 1;
    return lwespOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 */
lwespr_t
lwesp_ll_deinit(lwesp_ll_t* ll) {
    if (thread_handle != NULL) {
        lwesp_sys_thread_terminate(&thread_handle);
        thread_handle = NULL;
    }
    initialized = 0;                            /* Clear initialized flag */
    return lwespOK;
}

#endif /* !__DOXYGEN__ */
