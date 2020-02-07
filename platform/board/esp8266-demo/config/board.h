#ifndef __BOARD_H
#define __BOARD_H

typedef enum {
    PORT_TIMER_0,
    PORT_TIMER_1,
    PORT_TIMER_2,
    PORT_TIMER_3,
    PORT_TIMER_4,
    PORT_TIMER_5,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255
};

typedef enum{
    PORT_PWM_1,
    PORT_PWM_2,
    PORT_PWM_3,
    PORT_PWM_4,
    PORT_PWM_SIZE,
    PORT_PWM_INVALID = 255,
}PORT_PWM_TYPE;

#if defined(SUPPORT_SINGAPORE_DOMAIN)
#define AOS_MAIN_TASK_STACK_SIZE (7.5*1024)
#elif defined(ESP8266_CHIPSET)
#define AOS_MAIN_TASK_STACK_SIZE (2*1024)
#else
#define AOS_MAIN_TASK_STACK_SIZE (6*1024)
#endif

/*
hal support list
*/
#define AOS_HAL_GPIO_ENABLED        1
#define AOS_HAL_UART_ENABLED        1
#define AOS_HAL_SPI_ENABLED         0
#define AOS_HAL_I2C_ENABLED         1
#define AOS_HAL_FLASH_ENABLED       1
#define AOS_HAL_PWM_ENABLED         0
#define AOS_HAL_TIMER_ENABLED       1
#define AOS_HAL_WDG_ENABLED         1
#define AOS_HAL_RTC_ENABLED         0
#define AOS_HAL_ADC_ENABLED         1
#define AOS_HAL_DAC_ENABLED         0
#define AOS_HAL_RNG_ENABLED         0
#define AOS_HAL_I2S_ENABLED         0
#define AOS_HAL_QSPI_ENABLED        0
#define AOS_HAL_CAN_ENABLED         0
#define AOS_HAL_NAND_ENABLED        0
#define AOS_HAL_NOR_ENABLED         0

//LED
#define LED2    0xff //

//KEY
#define KEY1    14  //GPIOA-PIN14

#endif /* __BOARD_H */
