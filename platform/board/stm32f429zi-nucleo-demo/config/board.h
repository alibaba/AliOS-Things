#ifndef __BOARD__H_
#define __BOARD__H_
#define HARDWARE_REVISION   "V1.0"
#define MODEL               "STM32F4"

#include "stm32f4xx_hal.h"
#include "hal_gpio_stm32f4.h"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

#define SOC_PACKAGE_PIN_MAX      144
#define TOTAL_GPIO_NUM            7

#define MAX_I2C_BUS_NUM          2

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

typedef enum {
    PORT_ADC_1,
    PORT_ADC_2,
    PORT_ADC_SIZE,
    PORT_ADC_INVALID =255,
} PORT_ADC_TYPE;

typedef enum {
    PORT_DAC_1,
    PORT_DAC_SIZE,
    PORT_DAC_INVALID =255,
} PORT_DAC_TYPE;

typedef enum{
    PORT_CAN_NORMAL,
    PORT_CAN_CANOPEN,
    PORT_CAN_SIZE,
    PORT_CAN_INVALID = 255,
}PORT_CAN_TYPE;

typedef enum{
    PORT_TIMER_CANOPEN = 0,
    PORT_TIMER_0 = 0,
    PORT_TIMER_1 = 1,
    PORT_TIMER_2 = 2,
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

typedef enum{
    PORT_WDG_0 = 1,
    PORT_WDG_SIZE,
    PORT_WDG_INVALID = 255,
}PORT_WDG_TYPE;

/*
    hal support list
*/
#define AOS_HAL_GPIO_ENABLED        1
#define AOS_HAL_UART_ENABLED        1
#define AOS_HAL_SPI_ENABLED         1
#define AOS_HAL_I2C_ENABLED         1
#define AOS_HAL_FLASH_ENABLED       0
#define AOS_HAL_TIMER_ENABLED       1
#define AOS_HAL_PWM_ENABLED         1
#define AOS_HAL_WDG_ENABLED         1
#define AOS_HAL_RTC_ENABLED         0
#define AOS_HAL_ADC_ENABLED         1
#define AOS_HAL_DAC_ENABLED         1
#define AOS_HAL_RNG_ENABLED         0
#define AOS_HAL_I2S_ENABLED         0
#define AOS_HAL_QSPI_ENABLED        0
#define AOS_HAL_CAN_ENABLED         0
#define AOS_HAL_NAND_ENABLED        0
#define AOS_HAL_NOR_ENABLED         0

/* Pin and port redefinition */

/* LED */

/** The map of LED Pin of stm32f429zi-nucleo board
 *  LD1  <--> LED0
 *  LD2  <--> LDE1
 *  LD3  <--> LED2
 */
#define LED0     HAL_GPIO_16 // LD1 <--> PB0
#define LED1     HAL_GPIO_23 // LD2 <--> PB7
#define LED2     HAL_GPIO_30 // LD3 <--> PB14

/* Button */
#define KEY0     HAL_GPIO_45 // USER <--> PC13

#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC

/* NIC */
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG

/* STLINK */
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD

/* USB */
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA

/* DEBUG */
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA

#if (AOS_HAL_GPIO_ENABLED > 0)

#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT LED0
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN KEY0
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT KEY0
#endif

#endif /* AOS_HAL_GPIO_ENABLED > 0 */

#if (AOS_HAL_TIMER_ENABLED > 0)

#ifndef HALAPP_LED_TEST
#define HALAPP_LED_TEST      LED0
#endif

#ifndef HALAPP_TIMER_TEST
#define HALAPP_TIMER_TEST     PORT_TIMER_5
#endif
#endif /* AOS_HAL_TIMER_ENABLED > 0 */

#if (AOS_HAL_PWM_ENABLED > 0)

#ifndef HALAPP_PWM_TEST
#define HALAPP_PWM_TEST          PORT_PWM_3
#endif

#endif /* AOS_HAL_PWM_ENABLED > 0 */

#if (AOS_HAL_ADC_ENABLED > 0)
#define HALAPP_ADC PORT_ADC_1
#endif /* AOS_HAL_ADC_ENABLED */

#if (AOS_HAL_DAC_ENABLED > 0)
#include "hal_dac_stm32f4.h"
#define HALAPP_DAC PORT_DAC_1
#define HALAPP_DAC_CHANNEL HAL_DAC_CHANNEL_1
#endif /* AOS_HAL_DAC_ENABLED */

#endif /*__BOARD__H_*/

