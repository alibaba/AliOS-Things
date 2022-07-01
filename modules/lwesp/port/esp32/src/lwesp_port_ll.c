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
 * Author:          Neo Xiong <xiongyu0523@gmail.com>
 * Version:         v1.1.2-dev
 */

#include "lwesp/lwesp.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_input.h"
#include "system/lwesp_ll.h"
#include "freertos/FreeRTOS.h"
#include <driver/uart_select.h>
#include "esp_log.h"
#include <soc/uart_caps.h>

#define LWESP_UART_NUM (2)

#if !__DOXYGEN__

#if !LWESP_CFG_INPUT_USE_PROCESS
#error "LWESP_CFG_INPUT_USE_PROCESS must be enabled in `lwesp_config.h` to use this driver."
#endif /* LWESP_CFG_INPUT_USE_PROCESS */


static volatile uint8_t      lwesp_input_is_running, initialized;

/**
 * \brief           USART data processing
 */
static void uart_ll_thread_entry(void *arg) {
    uint8_t data_byte;
    int ret;
    LWESP_UNUSED(arg);
    const int uart_num = LWESP_UART_NUM; 
    TaskHandle_t xHandle;
    while (lwesp_input_is_running) {
        ret = uart_read_bytes(uart_num, &data_byte, 1, portMAX_DELAY);
        if (ret < 0) {
            continue;
        }
        #if LWESP_CFG_INPUT_USE_PROCESS
        lwesp_input_process(&data_byte, 1);
        #else
        lwesp_input(data_byte, 1);
        #endif
    
    }
    vTaskDelete(NULL);
}


static void configure_uart(uint32_t baudrate)
{
    const int uart_num = LWESP_UART_NUM;
    int tx_io_num;
    int rx_io_num;
    TaskHandle_t xHandle;
    int ret; 
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 120,
        .source_clk = UART_SCLK_APB,
    };
    if (baudrate > 0) {
        uart_config.baud_rate = baudrate;
    }
    switch (uart_num) {
    case 0:
        tx_io_num = 1;
        rx_io_num = 3;
        break;
    case 1:
#if defined CONFIG_IDF_TARGET_ESP32
        tx_io_num = 10;
        rx_io_num = 9;
#elif defined CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
        tx_io_num = GPIO_UART_TXD_OUT;
        rx_io_num = GPIO_UART_RXD_IN;
#endif
        break;
#if SOC_UART_NUM > 2
    case 2:
#ifdef BOARD_M5STACKCORE2
        tx_io_num = 14;
        rx_io_num = 13;
#else
        tx_io_num = 17;
        rx_io_num = 16;
#endif
        break;
#endif
    default:
        return;
    }

    uart_set_pin(uart_num, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); 
    uart_param_config(uart_num, &uart_config);
    uart_driver_install(uart_num, 256, 256, 0, NULL, 0);
    
    lwesp_input_is_running = 1;
    ret = xTaskCreate(uart_ll_thread_entry, "lwesp_input", 4*1024, NULL, 5, &xHandle);
    if (ret != pdPASS) {
        return;
    }
}
#if defined(LWESP_RESET_PIN)
/**
 * \brief           Hardware reset callback
 */
static uint8_t
reset_device(uint8_t state) {
    return 1;
}
#endif /* defined(LWESP_RESET_PIN) */

/**
 * \brief           Send data to ESP device
 * \param[in]       data: Pointer to data to send
 * \param[in]       len: Number of bytes to send
 * \return          Number of bytes sent
 */
static size_t
send_data(const void* data, size_t len) {
    const uint8_t* d = data;
    const int uart_num = LWESP_UART_NUM;
    if (data == NULL || len <= 0)
        return 0;
    uart_write_bytes(uart_num, (const char*)data, len);
    uart_wait_tx_done(uart_num, (TickType_t)portMAX_DELAY);

    return len;
}

/**
 * \brief           Callback function called from initialization process
 */
lwespr_t
lwesp_ll_init(lwesp_ll_t* ll) {

#if !LWESP_CFG_MEM_CUSTOM && !__DOXYGEN__
    static uint8_t memory[0x8000];             /* Create memory for dynamic allocations with specific size */

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
 #endif
    if (!initialized) {
        ll->send_fn = send_data;                /* Set callback function to send data */
#if defined(LWESP_RESET_PIN)
        ll->reset_fn = reset_device;            /* Set callback for hardware reset */
#endif /* defined(LWESP_RESET_PIN) */
        configure_uart(ll->uart.baudrate);          /* Initialize UART for communication */
    }
    initialized = 1;
    return lwespOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 */
lwespr_t
lwesp_ll_deinit(lwesp_ll_t* ll) {
    lwesp_input_is_running = 0;

    initialized = 0;
    LWESP_UNUSED(ll);
    return lwespOK;
}

#endif /* !__DOXYGEN__ */
