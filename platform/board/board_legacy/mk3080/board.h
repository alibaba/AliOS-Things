/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


#define HARDWARE_REVISION   "V1.0"
#define MODEL               "MK3080"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 921600
#else
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 921600
#endif



/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
   
/******************************************************
 *                   Enumerations
 ******************************************************/
  
#define MICO_UNUSED 0xFF

typedef enum
{
    MICO_SYS_LED,
    MICO_RF_LED,
    BOOT_SEL,
    MFG_SEL,
    EasyLink_BUTTON,
    MICO_GPIO_1 ,
    MICO_GPIO_2,
    //MICO_GPIO_3,
    MICO_GPIO_4,
    MICO_GPIO_5,
    MICO_GPIO_6,
    MICO_GPIO_7,
    MICO_GPIO_8,
    MICO_GPIO_9,
    MICO_GPIO_10,
    //MICO_GPIO_11,
    MICO_GPIO_12,
    MICO_GPIO_13,
    MICO_GPIO_14,
    //MICO_GPIO_15,
    //MICO_GPIO_16,
    //MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
    MICO_GPIO_20,
    MICO_GPIO_21,
    MICO_GPIO_22,
    MICO_GPIO_23,
    //MICO_GPIO_24,
    //MICO_GPIO_25,
    //MICO_GPIO_26,
    //MICO_GPIO_27,
    //MICO_GPIO_28,
    MICO_GPIO_29,
    //MICO_GPIO_30,
    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
    MICO_SPI_1,
    MICO_SPI_2,
    MICO_SPI_3,		
    MICO_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_1,
    MICO_I2C_2,		
    MICO_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    MICO_I2C_NONE,
} mico_i2c_t;

typedef enum
{
    MICO_PWM_1,
    MICO_PWM_2,
    MICO_PWM_3,
    MICO_PWM_4,    
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_3,
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_1,
    MICO_UART_2,
    MICO_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    MICO_UART_NONE,
} mico_uart_t;

typedef enum
{
  MICO_FLASH_EMBEDDED,
  MICO_FLASH_SPI,  	
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,  
} mico_flash_t;

typedef enum
{
  MICO_PARTITION_USER_MAX
} mico_user_partition_t;


#ifdef BOOTLOADER
#define STDIO_UART          MICO_UART_NONE
#define STDIO_UART_BAUDRATE (115200) 
#else
#define STDIO_UART          MICO_UART_NONE
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define UART_FOR_APP     MICO_UART_1
#define MFG_TEST         MICO_UART_1
#define CLI_UART         MICO_UART_1

/* Components connected to external I/Os*/
#define Standby_SEL      (MICO_GPIO_29)

/* I/O connection <-> Peripheral Connections */
#define MICO_I2C_CP      (MICO_I2C_1)

//#define USE_MICO_SPI_FLASH

#define MICO_FLASH_FOR_PARA         MICO_FLASH_SPI
#define PARA_START_ADDRESS          (uint32_t)0x00000020
#define PARA_END_ADDRESS            (uint32_t)0x00003FFF
#define PARA_FLASH_SIZE             (PARA_END_ADDRESS - PARA_START_ADDRESS + 1)   /* 4k bytes*/


#ifdef __cplusplus
} /*extern "C" */
#endif