/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __BOARD__H_
#define __BOARD__H_
#define HARDWARE_REVISION "V1.0"
#define MODEL             "STM32F4"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

typedef enum {
    PORT_UART_STD,
    PORT_UART_AT,
    PORT_UART_RS485,
    PORT_UART_SCANNER,
    PORT_UART_LORA,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255
}PORT_UART_TYPE;

typedef enum {
    PORT_ADC1,
    PORT_ADC2,
    PORT_ADC3,
    PORT_ADC4,
    PORT_ADC5,
    PORT_ADC6,
    PORT_ADC_SIZE,
    PORT_ADC_INVALID = 255
}PORT_ADC_TYPE;

typedef enum {
    PORT_QSPI1,
    PORT_QSPI2,
    PORT_QSPI3,
    PORT_QSPI4,
    PORT_QSPI_SIZE,
    PORT_QSPI_INVALID = 255
};

typedef enum {
    PORT_SPI1,
    PORT_SPI2,
    PORT_SPI3,
    PORT_SPI4,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID = 255
};

typedef enum {
    PORT_TIMER1,
    PORT_TIMER2,
    PORT_TIMER3,
    PORT_TIMER4,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255
};

#endif /*__BOARD__H_*/
