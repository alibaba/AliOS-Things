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
#define GPIO_TABLE_SZ            3
#define TOTAL_GPIO_NUM            3
#define MAX_I2C_BUS_NUM          3

/*############         Logical UART Port Type used in this board  ##############
 * Please keep APP_UART_STD=0 to avoid unexpected error. Other Port value does not 
 * follow special rule.                                   
 #####################################################                  #         */
typedef enum{
    PORT_UART_STD,
    PORT_UART_AT,
    /*PORT_UART_RS485, *///Example.
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
}PORT_UART_TYPE;

typedef enum{
    PORT_SPI_1,
    PORT_SPI_2,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID =255,
}PORT_SPI_TYPE;

typedef enum{
    PORT_I2C_1 = 1,
    PORT_I2C_2,
    PORT_I2C_SIZE,
    PORT_I2C_INVALID = 255,
}PORT_I2C_TYPE;

#endif /*__BOARD__H_*/

