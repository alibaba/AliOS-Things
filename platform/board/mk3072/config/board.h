#ifndef __BOARD_H__
#define __BOARD_H__

#include "aos/hal/flash.h"

extern hal_logic_partition_t hal_partitions[];
extern void flash_partition_init(void);

typedef enum {
    PORT_UART_STD,
    PORT_UART_AT,
    PORT_UART_RS485,
    PORT_UART_SCANNER,
    PORT_UART_LORA,
    PORT_UART_TEMP,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255
} PORT_UART_TYPE;

typedef enum{
    PORT_I2C_1 = 0,
    PORT_I2C_SIZE,
    PORT_I2C_INVALID = 255,
    PORT_I2C_DEMO = PORT_I2C_1,
}PORT_I2C_TYPE;

typedef enum{
    PORT_SPI_1,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID =255,
    PORT_SPI_DEMO = PORT_SPI_1,
}PORT_SPI_TYPE;

typedef enum {
    PORT_ADC_1,
    PORT_ADC_2,
    PORT_ADC_SIZE,
    PORT_ADC_INVALID =255,
    PORT_ADC_DEMO = PORT_ADC_1,
} PORT_ADC_TYPE;

/*
    hal support list
*/
#define AOS_HAL_GPIO_ENABLED        1
#define AOS_HAL_UART_ENABLED        1
#define AOS_HAL_SPI_ENABLED         0
#define AOS_HAL_I2C_ENABLED         0
#define AOS_HAL_FLASH_ENABLED       0
#define AOS_HAL_PWM_ENABLED         0
#define AOS_HAL_TIMER_ENABLED       0
#define AOS_HAL_WDG_ENABLED         0
#define AOS_HAL_RTC_ENABLED         0
#define AOS_HAL_ADC_ENABLED         0
#define AOS_HAL_DAC_ENABLED         0
#define AOS_HAL_RNG_ENABLED         0
#define AOS_HAL_I2S_ENABLED         0
#define AOS_HAL_QSPI_ENABLED        0
#define AOS_HAL_CAN_ENABLED         0
#define AOS_HAL_NAND_ENABLED        0
#define AOS_HAL_NOR_ENABLED         0

#if (AOS_HAL_GPIO_ENABLED > 0)
#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT (0UL)
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN  (0UL)
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT (0UL)
#endif
#endif /* AOS_HAL_GPIO_ENABLED */

#endif /* __BOARD_H__ */
