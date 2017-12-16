#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := stm32l475

HOST_OPENOCD := stm32l475

STM32_NONSTD_SOCKET := true

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal netmgr framework.common mbedtls cjson cli digest_algorithm

GLOBAL_DEFINES += STM32_USE_SPI_WIFI

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4

GLOBAL_INCLUDES += \
                   src/common/csp/wifi/inc     \
		   src/B-L475E-IOT01/include   \
                   src/B-L475E-IOT01/runapp    \
                   Drivers/STM32L4xx_HAL_Driver/Inc \
		   Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
                   Drivers/BSP/B-L475E-IOT01 \
                   Drivers/BSP/Components/es_wifi \
                   Drivers/BSP/Components/hts221 \
                   Drivers/BSP/Components/lis3mdl \
                   Drivers/BSP/Components/lps22hb \
                   Drivers/BSP/Components/lsm6dsl \
                   Drivers/BSP/Components/vl53l0x \
                   Drivers/CMSIS/Include \
		   ../../../include/hal


GLOBAL_CFLAGS += -DSTM32L475xx

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS += --c99 -c --cpu Cortex-M4.fp -D__MICROLIB -g --apcs=interwork --split_sections
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += --cpu Cortex-M4.fp   \
                  --littleend  \
		  --branchpatch=sdcomp-29491-629360 \
		   *.o \
                  --library_type=microlib \
		  --strict \
		  --scatter "platform\mcu\stm32l4xx\B-L475E-IOT01.sct" \
                  --summary_stderr \
		  --info summarysizes --xref --callgraph --symbols \
                  --info sizes --info totals --info unused --info veneers
else
GLOBAL_LDFLAGS += -mcpu=cortex-m4  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  -nostartfiles    \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif


GLOBAL_LDFLAGS += -T platform/mcu/stm32l475/STM32L475VGTx_FLASH.ld

$(NAME)_SOURCES := src/B-L475E-IOT01/runapp/stm32l4xx_hal_msp.c      \
                   src/B-L475E-IOT01/runapp/stm32l4xx_it.c           \
                   src/B-L475E-IOT01/runapp/soc_init.c          \
                   src/B-L475E-IOT01/runapp/system_stm32l4xx.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c        \
                   src/B-L475E-IOT01/hal/flash_l4.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.c \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.c \
                   Drivers/BSP/Components/es_wifi/es_wifi.c \
                   Drivers/BSP/Components/hts221/hts221.c \
                   Drivers/BSP/Components/lis3mdl/lis3mdl.c \
                   Drivers/BSP/Components/lps22hb/lps22hb.c \
                   Drivers/BSP/Components/lsm6dsl/lsm6dsl.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_calibration.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_core.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_ranging.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_strings.c \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_platform_log.c \
                   aos/soc_impl.c                \
                   aos/trace_impl.c             \
                   src/B-L475E-IOT01/runapp/aos.c                    \
                   src/common/csp/wifi/src/es_wifi_io.c        \
                   src/common/csp/wifi/src/wifi.c              \
                   src/B-L475E-IOT01/hal/hw.c                     \
                   src/B-L475E-IOT01/hal/wifi_port.c              \
                   src/B-L475E-IOT01/hal/flash_port.c              \
                   src/B-L475E-IOT01/hal/ota_port.c              \
                   src/B-L475E-IOT01/hal/hal_i2c_stm32l4.c       \
                   src/B-L475E-IOT01/sensor/vl53l0x_platform.c \
                   src/B-L475E-IOT01/sensor/vl53l0x_proximity.c \
                   src/B-L475E-IOT01/sensor/sensors_data.c \
                   src/B-L475E-IOT01/sensor/sensors.c \
                   src/B-L475E-IOT01/sensor/qspi.c

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_armcc.s
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_icc.s
else
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_gcc.s
endif
