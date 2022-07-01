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
#include "uart.h"
#include "irq.h"
#include "pendsv.h"


#if !__DOXYGEN__

#if !LWESP_CFG_INPUT_USE_PROCESS
#error "LWESP_CFG_INPUT_USE_PROCESS must be enabled in `lwesp_config.h` to use this driver."
#endif /* LWESP_CFG_INPUT_USE_PROCESS */

static volatile uint8_t      lwesp_input_is_running, initialized;
#define CONFIG_UART_RECV_BUFF (4*1024)
#define CONIFG_UART_NUM (PYB_UART_1)
pyb_uart_obj_t uart_obj;
static uint8_t uart_recv_buf[CONFIG_UART_RECV_BUFF];
/**
 * \brief           USART data processing
 */
static void uart_ll_thread_entry(void *arg) {
    uint8_t data_byte;
    int ret;
    pyb_uart_obj_t *obj = &uart_obj;
    LWESP_UNUSED(arg);
    LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[uart_ll_thread_entry] started \r\n");


    while (lwesp_input_is_running) {
        while (!uart_rx_wait(obj, 0));
        data_byte = uart_rx_char(obj);
         
        #if LWESP_CFG_INPUT_USE_PROCESS
        lwesp_input_process(&data_byte, 1);
        #else
        lwesp_input(data_byte, 1);
        #endif
    
    }
}
static void configure_uart(uint32_t baudrate)
{  
    int ret;
    pyb_uart_obj_t *obj = &uart_obj;
    memset(obj, 0, sizeof(pyb_uart_obj_t));
    obj->base.type = &pyb_uart_type;
    obj->uart_id = CONIFG_UART_NUM;
    MP_STATE_PORT(pyb_uart_obj_all)[obj->uart_id - 1] = obj;
    
    obj->is_static = true;
    obj->timeout = 0;
    obj->timeout_char = 0;
    lwesp_sys_thread_t input_thread;
    ret = uart_init(obj, baudrate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, 0);
    if (ret == true) {
         LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[uart[%d] init success] \r\n", uart_obj.uart_id);

    } else {
         LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[uart init fail] \r\n");
    }
    uint32_t min_timeout_char = 13000 / baudrate + 2;
    if (obj->timeout_char < min_timeout_char) {
        obj->timeout_char = min_timeout_char;
        obj->timeout = min_timeout_char;
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "uart_recv_buf len:%d \r\n", sizeof(uart_recv_buf));
    uart_set_rxbuf(obj, sizeof(uart_recv_buf), uart_recv_buf);

    uint32_t actual_baudrate = uart_get_baudrate(obj);

    // check we could set the baudrate within 5%
    uint32_t baudrate_diff;
    if (actual_baudrate > baudrate) {
        baudrate_diff = actual_baudrate - baudrate;
    } else {
        baudrate_diff = baudrate - actual_baudrate;
    }
    if (20 * baudrate_diff > actual_baudrate) {
        LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
     "set baudrate %lu is not within 5 percent of desired value", actual_baudrate);
    }

    lwesp_input_is_running = 1;
    ret = lwesp_sys_thread_create(&input_thread, "lwesp_input", uart_ll_thread_entry, NULL, LWESP_SYS_THREAD_SS, 1); 
    if (ret != 1) {
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
send_data(const void* mem, size_t len) {
    pyb_uart_obj_t *obj = &uart_obj;
    int errcode = 0;
    uint8_t byte;
    uint8_t *data = (uint8_t *)mem;
    uint8_t num = 1;
    if (data == NULL || len <= 0)
        return 0;
    for (int i = 0; i < len; i++) {
    retry:
        byte = data[i];
        while (!uart_tx_wait(obj, 0));
        num = uart_tx_data(obj, &byte, 1, &errcode); 
        if (num != 1) {
            LWESP_DEBUGF(LWESP_CFG_DBG_THREAD | LWESP_DBG_TYPE_TRACE,
                 "[send_data] err errcode %d \r\n", errcode);
            goto retry;
        }
      
    }
    // write the data
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
        ll->reset_fn = NULL;
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
