/**
******************************************************************************
* @file    platform_mcu_peripheral.h 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide all the headers of functions for stm32f2xx platform
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

#pragma once

//#include "rtl8195a.h"
#include "PinNames.h"
#include "objects.h"

#include "mico_rtos.h"
#include "RingBufferUtils.h"

#include "serial_api.h"
#include "i2c_api.h"
#include "pwmout_api.h"
#include "spi_api.h"
#include "aos/aos.h"
//#include "platform_peripheral.h"

#ifdef __cplusplus
extern "C"
{
#endif


 #if 1

#define NUMBER_OF_GPIO_PORTS      (8)

#define NUMBER_OF_GPIO_IRQ_LINES  (16)

#define NUMBER_OF_UART_PORTS      (6)

#define INVALID_UART_PORT_NUMBER  (0xff)

#define NUMBER_OF_SPI_PORTS       (3)

typedef struct
{
    uint32_t            channel;
    uint32_t            complete_flags;
    uint32_t            error_flags;
} platform_dma_config_t;

typedef struct
{
    PinName pin;	
    gpio_t gpio_obj;
    gpio_irq_t gpio_irq_obj;
} platform_gpio_t;

typedef struct analogin_s analogin_t;

typedef struct
{
    analogin_t adc_obj;
    PinName pin;	
} platform_adc_t;

typedef struct
{
    pwmout_t pwm_obj; 
    PinName pin;	
} platform_pwm_t;


typedef struct
{
    spi_t spi_obj;
    PinName mosi;	
    PinName miso;	
    PinName sclk;	
    PinName ssel;	
} platform_spi_t;

typedef struct
{
    platform_spi_t*           peripheral;
    mico_mutex_t              spi_mutex;	
} platform_spi_driver_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;

typedef struct
{
    i2c_t i2c_obj;
    PinName sda;	
    PinName scl;	
} platform_i2c_t;


typedef void (* wakeup_irq_handler_t)(void *arg);

typedef struct
{
    serial_t serial_obj;
    PinName tx;
    PinName rx;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             rx_buffer;
#ifndef NO_MICO_RTOS
    //mico_semaphore_t           rx_complete;
    //mico_semaphore_t           tx_complete;
    //mico_mutex_t               tx_mutex;
    //mico_semaphore_t           sem_wakeup;
    aos_sem_t           rx_complete;
    aos_sem_t           tx_complete;
    aos_mutex_t               tx_mutex;
    aos_sem_t           sem_wakeup;    
#else
    volatile bool              rx_complete;
    volatile bool              tx_complete;
#endif
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
} platform_uart_driver_t;

#if 1
OSStatus platform_gpio_enable_clock          ( const platform_gpio_t* gpio );

OSStatus platform_mcu_powersave_init         ( void );

void     platform_uart_irq                   ( uint32_t id, SerialIrq event );
void     platform_loguart_irq                ( void* id);
OSStatus platform_rtc_enter_powersave        ( void );
OSStatus platform_rtc_abort_powersave        ( void );
OSStatus platform_rtc_exit_powersave         ( uint32_t requested_sleep_time, uint32_t *cpu_sleep_time );


OSStatus platform_gpio_irq_manager_init      ( void );

OSStatus platform_rtc_init                   ( void );

void     platform_uart_tx_dma_irq            ( uint32_t id );
void     platform_uart_rx_dma_irq            ( uint32_t id );


typedef struct flash_s flash_t;

typedef struct
{
    //platform_flash_type_t      flash_type;
    flash_t                     flash_obj;	
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
    uint32_t                   flash_protect_opt;
    uint32_t                   flash_readonly_start; // can't erase and write
    uint32_t                   flash_readonly_len;
} platform_flash_t;

typedef struct
{
    //const platform_flash_t*    peripheral;
    mico_mutex_t               flash_mutex;
    volatile bool              initialized;
} platform_flash_driver_t;
#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif





