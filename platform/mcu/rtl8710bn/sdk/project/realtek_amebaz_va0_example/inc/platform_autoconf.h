/*
 * Automatically generated by make menuconfig: don't edit
 */
#define AUTOCONF_INCLUDED

/*
 * < MENUCONFIG FOR CHIP CONFIG
 */

/*
 * < CONFIG CHIP
 */
#define CONFIG_RTL8711B 1
#undef  ARM_CORE_CM3
#define ARM_CORE_CM4 1
#define CONFIG_CHIP_A_CUT 1
#undef  CONFIG_FPGA

/*
 * < CONFIG CPU CLK 
 */
#define CONFIG_CPU_CLK 1
#define CONFIG_CPU_125MHZ 1
#undef  CONFIG_CPU_62_5MHZ
#undef  CONFIG_CPU_31_25MHZ
#undef  CONFIG_CPU_15_625MHZ
#undef  CONFIG_CPU_7_8125MHZ
#undef  CONFIG_CPU_4MHZ
#undef  CONFIG_FPGA_CLK
#define PLATFORM_CLOCK (125000000)
#define CPU_CLOCK_SEL_VALUE (0)

/*
 * < CONFIG OSC8M CLK 
 */
#define CONFIG_OSC8M_CLK 1
#define CONFIG_OSC8M_8388608HZ 1
#undef  CONFIG_OSC8M_8192000HZ
#undef  CONFIG_OSC8M_8000000HZ
#undef  CONFIG_OSC8M_16777216HZ
#define OSC8M_CLOCK (8388608)

/*
 * < CONFIG TEST MODE 
 */
#undef  CONFIG_MP
#undef  CONFIG_CP
#undef  CONFIG_FT
#undef  CONFIG_EQC
#undef  CONFIG_RTL_SIM
#undef  CONFIG_POST_SIM

/*
 * < CONFIG OS 
 */
#define CONFIG_KERNEL 1
#undef PLATFORM_FREERTOS 0
#undef  PLATFORM_UCOSII
#undef  PLATFORM_ECOS
#define PLATFORM_ALIOS 1
#undef  CONFIG_TASK_SCHEDUL_DIS
#define TASK_SCHEDULER_DISABLED (0)

/*
 * < CONFIG GTIMER 
 */
#define CONFIG_TIMER_EN 1
#define CONFIG_TIMER_MODULE 1

/*
 * < CONFIG WDG 
 */
#define CONFIG_WDG 1
#define CONFIG_WDG_MODULE 1

/*
 * < CONFIG GDMA 
 */
#define CONFIG_GDMA_EN 1
#define CONFIG_GDMA_MODULE 1

/*
 * < CONFIG GPIO 
 */
#define CONFIG_GPIO_EN 1
#define CONFIG_GPIO_MODULE 1

/*
 * < CONFIG SPI 
 */
#define CONFIG_SPI_COM_EN 1
#define CONFIG_SPI_COM_MODULE 1

/*
 * < CONFIG UART 
 */
#define CONFIG_UART_EN 1
#define CONFIG_UART_MODULE 1

/*
 * < CONFIG I2C 
 */
#define CONFIG_I2C_EN 1
#define CONFIG_I2C_MODULE 1

/*
 * < CONFIG I2S 
 */
#define CONFIG_I2S_EN 1
#define CONFIG_I2S_MODULE 1

/*
 * < CONFIG SOC PS 
 */
#define CONFIG_SOC_PS_EN 1
#define CONFIG_SOC_PS_MODULE 1

/*
 * < CONFIG CRYPTO 
 */
#define CONFIG_CRYPTO_EN 1
#define CONFIG_CRYPTO_MODULE 1

/*
 * < CONFIG PWM 
 */
#define CONFIG_PWM_EN 1

/*
 * < CONFIG EFUSE 
 */
#define CONFIG_EFUSE_EN 1
#define CONFIG_EFUSE_MODULE 1

/*
 * < CONFIG SPIC 
 */
#define CONFIG_SPIC_EN 1
#define CONFIG_SPIC_MODULE 1
#define CONFIG_SPIC_PHASE_CALIBATION 1
#undef  CONFIG_SPIC_4BYTES_ADDRESS

/*
 * < CONFIG ADC 
 */
#define CONFIG_ADC_EN 1
#define CONFIG_ADC_MODULE 1

/*
 * < CONFIG SDIO Device 
 */
#define CONFIG_SDIO_DEVICE_EN 1
#define CONFIG_SDIO_DEVICE_NORMAL 1
#define CONFIG_SDIO_DEVICE_MODULE 1

/*
 * < CONFIG USB 
 */
#define CONFIG_USB_EN 1
#define CONFIG_USB_MODULE 1

/*
 * < CONFIG RDP 
 */
#define CONFIG_RDP_ENABLE 1

/*
 * < CONFIG PINMUX 
 */
#undef  CONFIG_PINMAP_ENABLE

/*
 * < CONFIG PER TEST 
 */
#undef  CONFIG_PER_TEST

/*
 * < CONFIG WIFI 
 */
#define CONFIG_WIFI_EN 1
#define CONFIG_WIFI_NORMAL 1
#undef  CONFIG_WIFI_TEST
#define CONFIG_WIFI_MODULE 1

/*
 * < CONFIG NETWORK 
 */
#define CONFIG_NETWORK 1

/*
 * < CONFIG INIC 
 */
#undef  CONFIG_INIC_EN

/*
 * < CONFIG USB_NIC 
 */
#undef  CONFIG_USB_DONGLE_NIC_EN

/*
 * < RTK STD lib 
 */
#define CONFIG_RTLIB_EN 1
#define CONFIG_RTLIB_MODULE 1
#undef  CONFIG_RTLIB_VERIFY

/*
 * < Add MBED SDK 
 */
#undef  CONFIG_MBED_ENABLED

/*
 * < Build App Demo 
 */
#undef  CONFIG_APP_DEMO

/*
 * < Dhrystone 
 */
#undef  CONFIG_DHRYSTONE_TEST

/*
 * < SSL 
 */
#undef  CONFIG_SSL_ROM_TEST

/*
 * < System Debug Message Config 
 */
#define CONFIG_UART_LOG_HISTORY 1
#define CONFIG_DEBUG_LOG 1
#define CONFIG_DEBUG_ERR_MSG 1
#undef  CONFIG_DEBUG_WARN_MSG
#undef  CONFIG_DEBUG_INFO_MSG

/*
 * < Build Option 
 */
#define CONFIG_TOOLCHAIN_ASDK 1
#undef  CONFIG_TOOLCHAIN_ARM_GCC
#define CONFIG_LINK_ROM_LIB 1
#undef  CONFIG_LINK_ROM_SYMB
