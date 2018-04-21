#ifndef RDA5981X_PINCONFIG_H
#define RDA5981X_PINCONFIG_H

#include "pinnames.h"

/* UART PIN defines */
#define RDA_UART0_TXD      GPIO_PIN27
#define RDA_UART0_RXD      GPIO_PIN26
#define RDA_UART1_TXD      GPIO_PIN25
#define RDA_UART1_RXD      GPIO_PIN24
#define RDA_UART1_RTS      GPIO_PIN23   
#define RDA_UART1_CTS      GPIO_PIN22
#define RDA_UART1_TXD_2    GPIO_PIN2
#define RDA_UART1_RXD_2    GPIO_PIN1

/* SPI PIN defines */
#define RDA_SPI0_CLK       GPIO_PIN22
#define RDA_SPI0_CS        GPIO_PIN23
#define RDA_SPI0_MOSI      GPIO_PIN24
#define RDA_SPI0_MISO      GPIO_PIN25
#define RDA_SPI0_CLK_2     GPIO_PIN4
#define RDA_SPI0_CS_2      GPIO_PIN26
#define RDA_SPI0_MOSI_2    GPIO_PIN6
#define RDA_SPI0_MISO_2    GPIO_PIN7
#define RDA_SPI0_CS_3      GPIO_PIN27
#define RDA_SPI0_MOSI_3    GPIO_PIN12
#define RDA_SPI0_MISO_3    GPIO_PIN13
#define RDA_SPI0_CS_4      GPIO_PIN5


/* I2C PIN defines */
#define RDA_I2C0_SCL       GPIO_PIN3
#define RDA_I2C0_SDA       GPIO_PIN2
#define RDA_I2C0_SCL_2     GPIO_PIN23
#define RDA_I2C0_SDA_2     GPIO_PIN22

/* ADC PIN defines */
#define RDA_ADC0
#define RDA_ADC1

#endif

