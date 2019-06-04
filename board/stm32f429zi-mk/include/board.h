#ifndef __BOARD__H_
#define __BOARD__H_
#define HARDWARE_REVISION   "V1.0"
#define MODEL               "STM32F4"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

#define SOC_PACKAGE_PIN_MAX      144
#define TOTAL_GPIO_NUM            3

#define MAX_I2C_BUS_NUM          3


typedef enum{
    PORT_UART_STD,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
}PORT_UART_TYPE;

typedef enum{
    PORT_SPI_1,
    PORT_SPI_2,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID =255,
}PORT_SPI_TYPE;

#endif /*__BOARD__H_*/

