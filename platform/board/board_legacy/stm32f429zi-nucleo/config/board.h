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
    PORT_UART_AT,
    PORT_UART_RS485,
    PORT_UART_SCANNER,
    PORT_UART_LORA,
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
    PORT_CAN_NORMAL,
    PORT_CAN_CANOPEN,
    PORT_CAN_SIZE,
    PORT_CAN_INVALID = 255,
}PORT_CAN_TYPE;

typedef enum{
    PORT_TIMER_CANOPEN,
    PORT_TIMER_3 = 3,
    PORT_TIMER_4 = 4,
    PORT_TIMER_5 = 5,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255,
}PORT_TIMER_TYPE;

typedef enum{
    PORT_PWM_1,
    PORT_PWM_2,
    PORT_PWM_3,
    PORT_PWM_4,
    PORT_PWM_SIZE,
    PORT_PWM_INVALID = 255,
}PORT_PWM_TYPE;

typedef enum{
    PORT_I2C_1 = 1,
    PORT_I2C_2,
    PORT_I2C_SIZE,
    PORT_I2C_INVALID = 255,
}PORT_I2C_TYPE;

#endif /*__BOARD__H_*/

