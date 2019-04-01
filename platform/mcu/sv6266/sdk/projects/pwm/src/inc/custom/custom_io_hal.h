#ifndef CUSTOM_IO_HAL_H
#define CUSTOM_IO_HAL_H

#define M_CUSTOM_ALT0 (0)
#define M_CUSTOM_ALT1 (1)
#define M_CUSTOM_ALT2 (2)
#define M_CUSTOM_ALT3 (3)
#define M_CUSTOM_ALT4 (4)
#define M_CUSTOM_ALT5 (5)

/**
* +-----+---------+---------+---------+---------+---------+---------+
* | PIN |   ALT0  |   ALT1  |   ALT2  |   ALT3  |   ALT4  |   ALT5  |
* +-----+---------+---------+---------+---------+---------+---------+
* |   0 |    AICE |    PWM0 |         |         |    ADC3 |  GPIO00 |
* |   1 |    AICE |    PWM1 |         |         |         |  GPIO01 |
* |   2 |    AICE |    PWM2 |         |         |         |  GPIO02 |
* |   3 |   UART0 |    PWM3 |         |         |         |  GPIO03 |
* |   4 |   UART0 |    PWM4 |         |         |         |  GPIO04 |
* |   5 |   UART1 |    I2S0 |         |         |         |  GPIO05 |
* |   6 |   UART1 |    I2S0 |         |         |         |  GPIO06 |
* |   7 |   UART1 |    I2S0 |         |         |         |  SIO07  |
* |   8 |         |    SPIM |    I2S0 |         |         |  GPIO08 |
* |   9 |   UART1 |    I2S0 |         |         |         |  GPIO09 |
* |  10 |         |    SPIM |    I2S0 |   I2CM0 |    SPIS |  GPIO10 |
* |  11 |         |         |         |         |    SPIS |  GPIO11 |
* |  12 |         |    SPIM |    I2S0 |   I2CM0 |    SPIS |  GPIO12 |
* |  13 |         |    SPIM |    I2S0 |   PSRAM |    SPIS |  GPIO13 |
* +-----+---------+---------+---------+---------+---------+---------+
* |  20 |         |    I2S0 |    I2S0 |         |    ADC2 |  SIO20  |
* |  21 |         |   UART0 |   I2CM0 |         |    ADC1 |  GPIO21 |
* |  22 |         |   UART0 |   I2CM0 |         |    ADC0 |  GPIO22 |
* +-----+---------+---------+---------+---------+---------+---------+
*/

//ALT0 : AICE    /   ALT1 : PWM0    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : ADC3    /ALT5 : GPIO00
#define M_CUSTOM_P00_MODE       M_CUSTOM_ALT1

//ALT0 : AICE    /   ALT1 : PWM1    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO01
#define M_CUSTOM_P01_MODE       M_CUSTOM_ALT1

//ALT0 : AICE    /   ALT1 : PWM2    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO02
#define M_CUSTOM_P02_MODE       M_CUSTOM_ALT1

//ALT0 : UART0_RX    /   ALT1 : PWM3    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO03
#define M_CUSTOM_P03_MODE       M_CUSTOM_ALT1

//ALT0 : UART0_TX    /   ALT1 : PWM4    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO04
#define M_CUSTOM_P04_MODE       M_CUSTOM_ALT1

//ALT0 : UART1_RTS    /   ALT1 : I2S0_BCLK    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO05
#define M_CUSTOM_P05_MODE       M_CUSTOM_ALT5

//ALT0 : UART1_RXD    /   ALT1 : I2S0_DI    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO06
#define M_CUSTOM_P06_MODE       M_CUSTOM_ALT5

//ALT0 : UART1_TXD    /   ALT1 : I2S0_DO    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : SIO07
#define M_CUSTOM_P07_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : SPI_M_CLK    /ALT2 : I2S0_BCLK    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO08
#define M_CUSTOM_P08_MODE       M_CUSTOM_ALT5

//ALT0 : UART1_CTS    /   ALT1 : I2S0_LRCLK    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : NONE    /ALT5 : GPIO09
#define M_CUSTOM_P09_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : SPI_M_MISO    /ALT2 : I2S0_DI    /ALT3 : I2C0_M_SDA    /ALT4 : SPI_S_MOSI    /ALT5 : GPIO10
#define M_CUSTOM_P10_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : NONE    /ALT2 : NONE    /ALT3 : NONE    /ALT4 : SPI_S_CLK    /ALT5 : GPIO11
#define M_CUSTOM_P11_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : SPI_M_MOSI    /ALT2 : I2S0_DO    /ALT3 : I2C0_M_SCL    /ALT4 : SPI_S_MISO    /ALT5 : GPIO12
#define M_CUSTOM_P12_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : SPI_M_CS    /ALT2 : I2S0_LRCLK    /ALT3 : PSRAM_CS    /ALT4 : SPI_S_CS    /ALT5 : GPIO13
#define M_CUSTOM_P13_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : I2S0_MCLK    /ALT2 : I2S0_MCLK    /ALT3 : NONE    /ALT4 : ADC2    /ALT5 : SIO20
#define M_CUSTOM_P20_MODE       M_CUSTOM_ALT5

//ALT0 : NONE    /   ALT1 : UART0_TXD    /ALT2 : I2C0_M_SCL    /ALT3 : NONE    /ALT4 : ADC1    /ALT5 : GPIO21
#define M_CUSTOM_P21_MODE       M_CUSTOM_ALT1

//ALT0 : NONE    /   ALT1 : UART0_RXD    /ALT2 : I2C0_M_SDA    /ALT3 : NONE    /ALT4 : ADC0    /ALT5 : GPIO22
#define M_CUSTOM_P22_MODE       M_CUSTOM_ALT1

#endif /* end of include guard: CUSTOM_IO_HAL_H */

