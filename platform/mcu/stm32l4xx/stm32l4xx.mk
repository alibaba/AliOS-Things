#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := STM32L4xx

HOST_OPENOCD := stm32f4x

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS := rhino hal netmgr framework.common mbedtls cjson cli digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4
GLOBAL_INCLUDES += include \
                   startup    \
                   driver  \
                   bsp/B-L475E-IOT01 \
                   bsp/Components/es_wifi \
                   bsp/Components/hts221 \
                   bsp/Components/lis3mdl \
                   bsp/Components/lps22hb \
                   bsp/Components/lsm6dsl \
                   bsp/Components/vl53l0x \
                   wifi/inc


GLOBAL_CFLAGS += -DSTM32L475xx 

GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -march=armv5te  \
                  -mcpu=cortex-m4        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_LDFLAGS += -T platform/mcu/stm32l4xx/STM32L475VGTx_FLASH.ld

$(NAME)_SOURCES := startup/startup_stm32l475xx.s \
                   startup/stm32l4xx_hal_msp.c      \
                   startup/stm32l4xx_it.c           \
                   startup/soc_init.c          \
                   cmsis/system_stm32l4xx.c      \
                   driver/stm32l4xx_hal.c        \
                   driver/flash_l4.c  \
                   driver/stm32l4xx_hal_flash.c  \
                   driver/stm32l4xx_hal_flash_ex.c \
                   driver/stm32l4xx_hal_flash_ramfunc.c \
                   driver/stm32l4xx_hal_i2c.c    \
                   driver/stm32l4xx_hal_i2c_ex.c \
                   driver/stm32l4xx_hal_pwr.c    \
                   driver/stm32l4xx_hal_qspi.c   \
                   driver/stm32l4xx_hal_rcc_ex.c \
                   driver/stm32l4xx_hal_rng.c    \
                   driver/stm32l4xx_hal_rtc.c    \
                   driver/stm32l4xx_hal_rtc_ex.c \
                   driver/stm32l4xx_hal_spi.c    \
                   driver/stm32l4xx_hal_spi_ex.c \
                   driver/stm32l4xx_hal_rcc.c    \
                   driver/stm32l4xx_hal_uart.c   \
                   driver/stm32l4xx_hal_uart_ex.c  \
                   driver/stm32l4xx_hal_gpio.c   \
                   driver/stm32l4xx_hal_dma.c    \
                   driver/stm32l4xx_hal_pwr_ex.c \
                   driver/stm32l4xx_hal_cortex.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_accelero.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_gyro.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_hsensor.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_magneto.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_psensor.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01_tsensor.c \
                   bsp/B-L475E-IOT01/stm32l475e_iot01.c \
                   bsp/Components/es_wifi/es_wifi.c \
                   bsp/Components/hts221/hts221.c \
                   bsp/Components/lis3mdl/lis3mdl.c \
                   bsp/Components/lps22hb/lps22hb.c \
                   bsp/Components/lsm6dsl/lsm6dsl.c \
                   bsp/Components/vl53l0x/vl53l0x_api.c \
                   bsp/Components/vl53l0x/vl53l0x_api_calibration.c \
                   bsp/Components/vl53l0x/vl53l0x_api_core.c \
                   bsp/Components/vl53l0x/vl53l0x_api_ranging.c \
                   bsp/Components/vl53l0x/vl53l0x_api_strings.c \
                   bsp/Components/vl53l0x/vl53l0x_platform_log.c \
                   aos/soc_impl.c                \
                   aos/aos.c                    \
                   wifi/src/es_wifi_io.c        \
                   wifi/src/wifi.c              \
                   hal/hw.c                     \
                   hal/wifi_port.c              \
                   hal/flash_port.c              \
                   hal/ota_port.c              \
                   ../../arch/arm/armv7m/gcc/m4/port_c.c \
                   ../../arch/arm/armv7m/gcc/m4/port_s.S
