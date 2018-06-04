#ifndef CUSTOM_IO_CHK_H
#define CUSTOM_IO_CHK_H

#if ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT0))
#define _M_P00_MODE     (0x0 << GPIO_00)
#define _M_P00_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT1))
#define _M_P00_MODE     (0x1 << PWM_0)
#define _M_P00_FUNC_SEL    (0x1 << SEL_PWM_0)
#elif ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P00_MODE define error!
#elif ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P00_MODE define error!
#elif ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT4))
#define _M_P00_MODE     (0x1 << ADC_3)
#define _M_P00_FUNC_SEL    (0)
#elif ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT5))
#define _M_P00_MODE     (0x1 << GPIO_00)
#define _M_P00_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P00_MODE define error!
#endif

#if ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT0))
#define _M_P01_MODE     (0x0 << GPIO_01)
#define _M_P01_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT1))
#define _M_P01_MODE     (0x1 << PWM_1)
#define _M_P01_FUNC_SEL    (0x1 << SEL_PWM_1)
#elif ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P01_MODE define error!
#elif ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P01_MODE define error!
#elif ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P01_MODE define error!
#elif ((M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT5))
#define _M_P01_MODE     (0x1 << GPIO_01)
#define _M_P01_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P01_MODE define error!
#endif

#if ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P00_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P01_MODE) == (M_CUSTOM_ALT0))
#define _M_P02_MODE     (0x0 << GPIO_02)
#define _M_P02_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT1))
#define _M_P02_MODE     (0x1 << PWM_2)
#define _M_P02_FUNC_SEL    (0x1 << SEL_PWM_2)
#elif ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P02_MODE define error!
#elif ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P02_MODE define error!
#elif ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P02_MODE define error!
#elif ((M_CUSTOM_P02_MODE) == (M_CUSTOM_ALT5))
#define _M_P02_MODE     (0x1 << GPIO_02)
#define _M_P02_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P02_MODE define error!
#endif

#if ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT0) && \
    (M_CUSTOM_P21_MODE) != (M_CUSTOM_ALT1) && \
    (M_CUSTOM_P22_MODE) != (M_CUSTOM_ALT1))
#define _M_P03_MODE     (0x1 << UART0_RXD_II)
#define _M_P03_FUNC_SEL    (0x1 << SEL_UART0_II)
#endif
#elif ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P04_MODE) != (M_CUSTOM_ALT0))
#define _M_P03_MODE     (0x1 << PWM_3)
#define _M_P03_FUNC_SEL    (0x1 << SEL_PWM_3)
#endif
#elif ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P03_MODE define error!
#elif ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P03_MODE define error!
#elif ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P03_MODE define error!
#elif ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT5))
#if ((M_CUSTOM_P04_MODE) != (M_CUSTOM_ALT0))
#define _M_P03_MODE     (0x1 << GPIO_03)
#define _M_P03_FUNC_SEL    (0)
#endif
#else
    #error M_CUSTOM_P03_MODE define error!
#endif

#if ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P03_MODE) == (M_CUSTOM_ALT0) && \
    (M_CUSTOM_P21_MODE) != (M_CUSTOM_ALT1) && \
    (M_CUSTOM_P22_MODE) != (M_CUSTOM_ALT1))
#define _M_P04_MODE     (0x1 << UART0_TXD_II)
#define _M_P04_FUNC_SEL    (0x1 << SEL_UART0_II)
#endif
#elif ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P03_MODE) != (M_CUSTOM_ALT0))
#define _M_P04_MODE     (0x1 << PWM_4)
#define _M_P04_FUNC_SEL    (0x1 << SEL_PWM_4)
#endif
#elif ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P04_MODE define error!
#elif ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P04_MODE define error!
#elif ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P04_MODE define error!
#elif ((M_CUSTOM_P04_MODE) == (M_CUSTOM_ALT5))
#if ((M_CUSTOM_P03_MODE) != (M_CUSTOM_ALT0))
#define _M_P04_MODE     (0x1 << GPIO_04)
#define _M_P04_FUNC_SEL    (0)
#endif
#else
    #error M_CUSTOM_P04_MODE define error!
#endif

#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT0))
#define _M_P05_MODE     (0x0 << UART1_RTS)
#define _M_P05_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P08_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT2))
#define _M_P05_MODE     (0x1 << I2S_BCLK_I)
#define _M_P05_FUNC_SEL    (0x1 << SEL_I2STRX_I)
#endif
#elif ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P05_MODE define error!
#elif ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P05_MODE define error!
#elif ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P05_MODE define error!
#elif ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT5))
#define _M_P05_MODE     (0x1 << GPIO_05)
#define _M_P05_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P05_MODE define error!
#endif

#if ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT0))
#define _M_P06_MODE     (0x0 << UART1_RXD)
#define _M_P06_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P08_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT2))
#define _M_P06_MODE     (0x1 << I2S_DI_I)
#define _M_P06_FUNC_SEL    (0x1 << SEL_I2STRX_I)
#endif
#elif ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P06_MODE define error!
#elif ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P06_MODE define error!
#elif ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P06_MODE define error!
#elif ((M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT5))
#define _M_P06_MODE     (0x1 << GPIO_06)
#define _M_P06_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P06_MODE define error!
#endif

#if ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT0))
#define _M_P07_MODE     (0x0 << UART1_TXD)
#define _M_P07_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P08_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT2))
#define _M_P07_MODE     (0x1 << I2S_DO_I)
#define _M_P07_FUNC_SEL    (0x1 << SEL_I2STRX_I)
#endif
#elif ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P07_MODE define error!
#elif ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P07_MODE define error!
#elif ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P07_MODE define error!
#elif ((M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT5))
#define _M_P07_MODE     (0x1 << SIO_07)
#define _M_P07_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P07_MODE define error!
#endif

#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P08_MODE define error!
#elif ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT1))
#define _M_P08_MODE     (0x1 << SPI_M_CLK)
#define _M_P08_FUNC_SEL    (0x1 << SEL_SPI_MST)
#endif
#elif ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P05_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT1))
#define _M_P08_MODE     (0x1 << I2S_BCLK_II)
#define _M_P08_FUNC_SEL    (0x1 << SEL_I2STRX_II)
#endif
#elif ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P08_MODE define error!
#elif ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P08_MODE define error!
#elif ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT5))
#define _M_P08_MODE     (0x1 << GPIO_08)
#define _M_P08_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P08_MODE define error!
#endif

#if ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT0) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT0))
#define _M_P09_MODE     (0x0 << UART1_CTS)
#define _M_P09_FUNC_SEL    (0)
#endif
#elif ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P08_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT2))
#define _M_P09_MODE     (0x1 << I2S_LRCLK_I)
#define _M_P09_FUNC_SEL    (0x1 << SEL_I2STRX_I)
#endif
#elif ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P09_MODE define error!
#elif ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P09_MODE define error!
#elif ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT4))
    #error M_CUSTOM_P09_MODE define error!
#elif ((M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT5))
#define _M_P09_MODE     (0x1 << GPIO_09)
#define _M_P09_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P09_MODE define error!
#endif

#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P10_MODE define error!
#elif ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT1))
#define _M_P10_MODE     (0x1 << SPI_M_MISO)
#define _M_P10_FUNC_SEL    (0x1 << SEL_SPI_MST)
#endif
#elif ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P05_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT1))
#define _M_P10_MODE     (0x1 << I2S_DI_II)
#define _M_P10_FUNC_SEL    (0x1 << SEL_I2STRX_II)
#endif
#elif ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT3))
#if ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT3) && \
    (M_CUSTOM_P21_MODE) != (M_CUSTOM_ALT2) && \
    (M_CUSTOM_P22_MODE) != (M_CUSTOM_ALT2))
#define _M_P10_MODE     (0x1 << I2C_M_SDA_I)
#define _M_P10_FUNC_SEL    (0x1 << SEL_I2C_MST_I)
#endif
#elif ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT4))
#if ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT4))
#define _M_P10_MODE     (0x0 << SPI_S_MOSI)
#define _M_P10_FUNC_SEL    (0x1 << SPI_RAW_DATA)
#endif
#elif ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT5))
#define _M_P10_MODE     (0x1 << GPIO_10)
#define _M_P10_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P10_MODE define error!
#endif

#if ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P11_MODE define error!
#elif ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT1))
    #error M_CUSTOM_P11_MODE define error!
#elif ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT2))
    #error M_CUSTOM_P11_MODE define error!
#elif ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P11_MODE define error!
#elif ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT4))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT4))
#define _M_P11_MODE     (0x0 << SPI_S_CLK)
#define _M_P11_FUNC_SEL    (0x1 << SPI_RAW_DATA)
#endif
#elif ((M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT5))
#define _M_P11_MODE     (0x1 << GPIO_11)
#define _M_P11_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P11_MODE define error!
#endif

#if ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P12_MODE define error!
#elif ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT1))
#define _M_P12_MODE     (0x1 << SPI_M_MOSI)
#define _M_P12_FUNC_SEL    (0x1 << SEL_SPI_MST)
#endif
#elif ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P05_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT1))
#define _M_P12_MODE     (0x1 << I2S_DO_II)
#define _M_P12_FUNC_SEL    (0x1 << SEL_I2STRX_II)
#endif
#elif ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT3))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT3) && \
    (M_CUSTOM_P21_MODE) != (M_CUSTOM_ALT2) && \
    (M_CUSTOM_P22_MODE) != (M_CUSTOM_ALT2))
#define _M_P12_MODE     (0x1 << I2C_M_SCL_I)
#define _M_P12_FUNC_SEL    (0x1 << SEL_I2C_MST_I)
#endif
#elif ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT4))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT4))
#define _M_P12_MODE     (0x0 << SPI_S_MISO)
#define _M_P12_FUNC_SEL    (0x1 << SPI_RAW_DATA)
#endif
#elif ((M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT5))
#define _M_P12_MODE     (0x1 << GPIO_12)
#define _M_P12_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P12_MODE define error!
#endif

#if ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P13_MODE define error!
#elif ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT1))
#define _M_P13_MODE     (0x1 << SPI_M_CS)
#define _M_P13_FUNC_SEL    (0x1 << SEL_SPI_MST)
#endif
#elif ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P05_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT1))
#define _M_P13_MODE     (0x1 << I2S_LRCLK_II)
#define _M_P13_FUNC_SEL    (0x1 << SEL_I2STRX_II)
#endif
#elif ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT3))
#define _M_P13_MODE     (0x0 << GPIO_13)
#define _M_P13_FUNC_SEL    (0)
#elif ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT4))
#if ((M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P11_MODE) == (M_CUSTOM_ALT4) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT4))
#define _M_P13_MODE     (0x0 << SPI_S_CS)
#define _M_P13_FUNC_SEL    (0x1 << SPI_RAW_DATA)
#endif
#elif ((M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT5))
#define _M_P13_MODE     (0x1 << GPIO_13)
#define _M_P13_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P13_MODE define error!
#endif

#if 0
#if ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT0))
#define _M_P14_MODE     (SPI_flash_IO0_DI)
#elif ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P14_MODE define error!
#elif ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P14_MODE define error!
#elif ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P14_MODE define error!
#elif ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P14_MODE define error!
#elif ((M_CUSTOM_P14_MODE) == (M_CUSTOM_ALT5))
#define _M_P14_MODE     (SIO_14)
#else
    #error M_ M_CUSTOM_P14_MODE define error!
#endif

#if ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT0))
#define _M_P15_MODE     (SPI_flash_CLK)
#elif ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P15_MODE define error!
#elif ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P15_MODE define error!
#elif ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P15_MODE define error!
#elif ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P15_MODE define error!
#elif ((M_CUSTOM_P15_MODE) == (M_CUSTOM_ALT5))
#define _M_P15_MODE     (SIO_15)
#else
    #error M_ M_CUSTOM_P15_MODE define error!
#endif

#if ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT0))
#define _M_P16_MODE     (SPI_flash_IO3_HD)
#elif ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P16_MODE define error!
#elif ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P16_MODE define error!
#elif ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P16_MODE define error!
#elif ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P16_MODE define error!
#elif ((M_CUSTOM_P16_MODE) == (M_CUSTOM_ALT5))
#define _M_P16_MODE     (GPIO_16)
#else
    #error M_ M_CUSTOM_P16_MODE define error!
#endif

#if ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT0))
#define _M_P17_MODE     (SPI_flash_CSN)
#elif ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P17_MODE define error!
#elif ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P17_MODE define error!
#elif ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P17_MODE define error!
#elif ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P17_MODE define error!
#elif ((M_CUSTOM_P17_MODE) == (M_CUSTOM_ALT5))
#define _M_P17_MODE     (GPIO_17)
#else
    #error M_ M_CUSTOM_P17_MODE define error!
#endif

#if ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT0))
#define _M_P18_MODE     (SPI_flash_IO1_DO)
#elif ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P18_MODE define error!
#elif ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P18_MODE define error!
#elif ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P18_MODE define error!
#elif ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P00_MODE define error!
#elif ((M_CUSTOM_P18_MODE) == (M_CUSTOM_ALT5))
#define _M_P18_MODE     (GPIO_18)
#else
    #error M_ M_CUSTOM_P18_MODE define error!
#endif

#if ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT0))
#define _M_P19_MODE     (SPI_flash_IO2_WP)
#elif ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT1))
    #error M_ M_CUSTOM_P19_MODE define error!
#elif ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT2))
    #error M_ M_CUSTOM_P19_MODE define error!
#elif ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT3))
    #error M_ M_CUSTOM_P19_MODE define error!
#elif ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT4))
    #error M_ M_CUSTOM_P19_MODE define error!
#elif ((M_CUSTOM_P19_MODE) == (M_CUSTOM_ALT5))
#define _M_P19_MODE     (GPIO_19)
#else
    #error M_ M_CUSTOM_P19_MODE define error!
#endif
#endif

#if ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT0))
    #error M_CUSTOM_P20_MODE define error!
#elif ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P05_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) == (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P08_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) != (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT2))
#define _M_P20_MODE     (0x1 << I2S_MCLK_I)
#define _M_P20_FUNC_SEL    (0x1 << SEL_I2STRX_I)
#endif
#elif ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P08_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P10_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P12_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P13_MODE) == (M_CUSTOM_ALT2) && \
        (M_CUSTOM_P05_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P06_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P07_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P09_MODE) != (M_CUSTOM_ALT1) && \
        (M_CUSTOM_P20_MODE) != (M_CUSTOM_ALT1))
#define _M_P20_MODE     (0x1 << I2S_MCLK_II)
#define _M_P20_FUNC_SEL    (0x1 << SEL_I2STRX_II)
#endif
#elif ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P20_MODE define error!
#elif ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT4))
#define _M_P20_MODE     (0x1 << ADC_2)
#define _M_P20_FUNC_SEL    (0)
#elif ((M_CUSTOM_P20_MODE) == (M_CUSTOM_ALT5))
#define _M_P20_MODE     (0x1 << SIO_20)
#define _M_P20_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P20_MODE define error!
#endif

#if ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT0))
#define _M_P21_MODE     (0x1 << I2C_S_SCL)
#define _M_P21_FUNC_SEL    (0x1 << SEL_I2C_SLV)
#endif
#elif ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT1) && \
    (M_CUSTOM_P03_MODE) != (M_CUSTOM_ALT0) && \
    (M_CUSTOM_P04_MODE) != (M_CUSTOM_ALT0))
#define _M_P21_MODE     (0x1 << UART0_TXD_I)
#define _M_P21_FUNC_SEL    (0x1 << SEL_UART0_I)
#endif
#elif ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT2) && \
    (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT3) && \
    (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT3))
#define _M_P21_MODE     (0x1 << I2C_M_SCL_II)
#define _M_P21_FUNC_SEL    (0x1 << SEL_I2C_MST_II)
#endif
#elif ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P21_MODE define error!
#elif ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT4))
#define _M_P21_MODE     (0x1 << ADC_1)
#define _M_P21_FUNC_SEL    (0)
#elif ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT5))
#define _M_P21_MODE     (0x1 << GPIO_21)
#define _M_P21_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P21_MODE define error!
#endif

#if ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT0))
#if ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT0))
#define _M_P22_MODE     (0x1 << I2C_S_SDA)
#define _M_P22_FUNC_SEL    (0x1 << SEL_I2C_SLV)
#endif
#elif ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT1))
#if ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT1) && \
    (M_CUSTOM_P03_MODE) != (M_CUSTOM_ALT0) && \
    (M_CUSTOM_P04_MODE) != (M_CUSTOM_ALT0))
#define _M_P22_MODE     (0x1 << UART0_RXD_I)
#define _M_P22_FUNC_SEL    (0x1 << SEL_UART0_I)
#endif
#elif ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT2))
#if ((M_CUSTOM_P21_MODE) == (M_CUSTOM_ALT2) && \
    (M_CUSTOM_P10_MODE) != (M_CUSTOM_ALT3) && \
    (M_CUSTOM_P12_MODE) != (M_CUSTOM_ALT3))
#define _M_P22_MODE     (0x1 << I2C_M_SDA_II)
#define _M_P22_FUNC_SEL    (0x1 << SEL_I2C_MST_II)
#endif
#elif ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT3))
    #error M_CUSTOM_P22_MODE define error!
#elif ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT4))
#define _M_P22_MODE     (0x1 << ADC_0)
#define _M_P22_FUNC_SEL    (0)
#elif ((M_CUSTOM_P22_MODE) == (M_CUSTOM_ALT5))
#define _M_P22_MODE     (0x1 << GPIO_22)
#define _M_P22_FUNC_SEL    (0)
#else
    #error M_CUSTOM_P22_MODE define error!
#endif

#ifndef _M_P00_MODE
    #error _M_P00_MODE define error!
#endif
#ifndef _M_P01_MODE
    #error _M_P01_MODE define error!
#endif
#ifndef _M_P02_MODE
    #error _M_P02_MODE define error!
#endif
#ifndef _M_P03_MODE
    #error _M_P03_MODE define error!
#endif
#ifndef _M_P04_MODE
    #error _M_P04_MODE define error!
#endif
#ifndef _M_P05_MODE
    #error _M_P05_MODE define error!
#endif
#ifndef _M_P06_MODE
    #error _M_P06_MODE define error!
#endif
#ifndef _M_P07_MODE
    #error _M_P07_MODE define error!
#endif
#ifndef _M_P08_MODE
    #error _M_P08_MODE define error!
#endif
#ifndef _M_P09_MODE
    #error _M_P09_MODE define error!
#endif
#ifndef _M_P10_MODE
    #error _M_P10_MODE define error!
#endif
#ifndef _M_P11_MODE
    #error _M_P11_MODE define error!
#endif
#ifndef _M_P12_MODE
    #error _M_P12_MODE define error!
#endif
#ifndef _M_P13_MODE
    #error _M_P13_MODE define error!
#endif

#if 0
#ifndef _M_P14_MODE
#endif
#ifndef _M_P15_MODE
#endif
#ifndef _M_P16_MODE
#endif
#ifndef _M_P17_MODE
#endif
#ifndef _M_P18_MODE
#endif
#ifndef _M_P19_MODE
#endif
#endif

#ifndef _M_P20_MODE
    #error _M_P20_MODE define error!
#endif
#ifndef _M_P21_MODE
    #error _M_P21_MODE define error!
#endif
#ifndef _M_P22_MODE
    #error _M_P22_MODE define error!
#endif

#if defined(_M_P00_MODE) && defined(_M_P01_MODE) && defined(_M_P02_MODE) && defined(_M_P03_MODE) && \
    defined(_M_P04_MODE) && defined(_M_P05_MODE) && defined(_M_P06_MODE) && defined(_M_P07_MODE) && \
    defined(_M_P08_MODE) && defined(_M_P09_MODE) && defined(_M_P10_MODE) && defined(_M_P11_MODE) && \
    defined(_M_P12_MODE) && defined(_M_P13_MODE) && defined(_M_P20_MODE) && defined(_M_P21_MODE) && \
    defined(_M_P22_MODE)
    #warning PIN CONFIG ALT SUCCESS!
#define _M_PIN_MODE (_M_P00_MODE | _M_P01_MODE | _M_P02_MODE | _M_P03_MODE | _M_P04_MODE | _M_P05_MODE | \
                    _M_P06_MODE | _M_P07_MODE | _M_P08_MODE | _M_P09_MODE | \
                    _M_P10_MODE | _M_P11_MODE | _M_P12_MODE | _M_P13_MODE | _M_P20_MODE | \
                    _M_P21_MODE | _M_P22_MODE)
#define _M_FUNC_SEL (_M_P00_FUNC_SEL | _M_P01_FUNC_SEL | _M_P02_FUNC_SEL | _M_P03_FUNC_SEL | _M_P04_FUNC_SEL | _M_P05_FUNC_SEL | \
                        _M_P06_FUNC_SEL | _M_P07_FUNC_SEL | _M_P08_FUNC_SEL | _M_P09_FUNC_SEL | \
                        _M_P10_FUNC_SEL | _M_P11_FUNC_SEL | _M_P12_FUNC_SEL | _M_P13_FUNC_SEL | _M_P20_FUNC_SEL | \
                        _M_P21_FUNC_SEL | _M_P22_FUNC_SEL) 
#else
    #error PIN CONFIG ALT ERROR!
#endif
#endif /* end of include guard: CUSTOM_IO_CHK_H */

