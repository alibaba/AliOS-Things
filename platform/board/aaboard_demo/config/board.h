/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __BOARD__H_
#define __BOARD__H_
#ifdef __cplusplus
extern "C" {
#endif
#define HARDWARE_REVISION "V1.0"
#define MODEL             "STM32F4"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

typedef enum{
    PORT_UART_STD  = 0,
    PORT_UART_AT   = 1,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
    PORT_UART_DEMO = PORT_UART_AT,
}PORT_UART_TYPE;

typedef enum {
    PORT_ADC_1,
    PORT_ADC_2,
    PORT_ADC_SIZE,
    PORT_ADC_INVALID =255,
    PORT_ADC_DEMO = PORT_ADC_1,
} PORT_ADC_TYPE;

typedef enum {
    PORT_TIMER_0,
    PORT_TIMER_1,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255,
    PORT_TIMER_DEMO = PORT_TIMER_1,
};

typedef enum {
    PORT_PWM_1,
    PORT_PWM_2,
    PORT_PWM_3,
    PORT_PWM_4,
    PORT_PWM_SIZE,
    PORT_PWM_INVALID = 255,
    PORT_PWM_DEMO = PORT_PWM_3,
} PORT_PWM_TYPE;

typedef enum {
    PORT_I2C_1 = 0,
    PORT_I2C_SIZE,
    PORT_I2C_INVALID = 255,
} PORT_I2C_TYPE;

typedef enum {
    PORT_SPI_1,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID =255,
    PORT_SPI_DEMO = PORT_SPI_1,
} PORT_SPI_TYPE;

typedef enum {
    PORT_WDG_0 = 0,
    PORT_WDG_SIZE,
    PORT_WDG_INVALID = 255,
} PORT_WDG_TYPE;

typedef enum {
    PORT_RTC_0 = 0,
    PORT_RTC_SIZE,
    PORT_RTC_INVALID =255,
} PORT_RTC_TYPE;

/*
    hal support list
*/
#define AOS_HAL_GPIO_ENABLED        1
#define AOS_HAL_UART_ENABLED        1
#define AOS_HAL_SPI_ENABLED         1
#define AOS_HAL_I2C_ENABLED         1
#define AOS_HAL_FLASH_ENABLED       1
#define AOS_HAL_TIMER_ENABLED       1
#define AOS_HAL_PWM_ENABLED         1
#define AOS_HAL_WDG_ENABLED         1
#define AOS_HAL_RTC_ENABLED         1
#define AOS_HAL_ADC_ENABLED         1
#define AOS_HAL_DAC_ENABLED         0
#define AOS_HAL_RNG_ENABLED         0
#define AOS_HAL_I2S_ENABLED         0
#define AOS_HAL_QSPI_ENABLED        0
#define AOS_HAL_CAN_ENABLED         0
#define AOS_HAL_NAND_ENABLED        0
#define AOS_HAL_NOR_ENABLED         0


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void board_gpio_init(void);
void board_dma_init(void);
void SystemClock_Config(void);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

//LED
//#define LED1    128 //GPIO21  //GPIOB-PIN5   FOR ST-LINK**********************************
#define LED2    5     //GPIOA-PIN5
#define LED3    ((uint8_t)0xff)
#define LED4    ((uint8_t)0xff)

//KEY
#define KEY1    45    //GPIOC-PIN13
//#define KEY2    67  //GPIOE-PIN3
//#define KEY3    66  //GPIOE-PIN2
#define KEY4    ((uint8_t)0xff)

#define KEY1_EXTI_IRQn EXTI15_10_IRQn

/* Private defines -----------------------------------------------------------*/
#define B1_EXTI_IRQn            EXTI15_10_IRQn
#define USART3_TX_Pin           GPIO_PIN_10
#define USART3_TX_GPIO_Port     GPIOC
#define USART3_RX_Pin           GPIO_PIN_11
#define USART3_RX_GPIO_Port     GPIOC
#define TMS_Pin                 GPIO_PIN_13
#define TMS_GPIO_Port           GPIOA
#define TCK_Pin                 GPIO_PIN_14
#define TCK_GPIO_Port           GPIOA
#define SWO_Pin                 GPIO_PIN_3
#define SWO_GPIO_Port           GPIOB

#if (AOS_HAL_GPIO_ENABLED > 0)

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT LED2
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN KEY1
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT KEY1
#endif
#endif /* AOS_HAL_GPIO_ENABLED > 0 */

#ifdef __cplusplus
}
#endif

#endif /*__BOARD__H_*/
