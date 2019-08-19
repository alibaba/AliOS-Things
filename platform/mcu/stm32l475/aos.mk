
NAME := mcu_stm32l475

HOST_OPENOCD := stm32l475

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32l475

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino netmgr

GLOBAL_INCLUDES += src/common/csp/lwip/include             \
                   src/common/csp/wifi/inc                 \
                   src/B-L475E-IOT01/include               \
                   src/B-L475E-IOT01/runapp                \
                   src/B-L475E-IOT01/hal                   \
                   Drivers/STM32L4xx_HAL_Driver/Inc        \
                   Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
                   Drivers/BSP/B-L475E-IOT01               \
                   Drivers/BSP/Components/es_wifi          \
                   Drivers/BSP/Components/hts221           \
                   Drivers/BSP/Components/lis3mdl          \
                   Drivers/BSP/Components/lps22hb          \
                   Drivers/BSP/Components/lsm6dsl          \
                   Drivers/BSP/Components/vl53l0x          \
                   Drivers/BSP/Components/mx25r6435f       \
                   Drivers/BSP/Components/Common           \
                   Drivers/CMSIS/Include                   \
                   Middlewares/USB_Device/Core/Inc         \
                   src

GLOBAL_CFLAGS += -DSTM32L475xx

GLOBAL_DEFINES += TARGET_STM32L4

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4           \
                   -mlittle-endian           \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M                         \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  -nostartfiles             \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/stm32l475/B-L475E-IOT01.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l475/STM32L475.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/stm32l475/STM32L475VGTx_FLASH.ld
endif

$(NAME)_SOURCES := src/B-L475E-IOT01/runapp/stm32l4xx_hal_msp.c                   \
                   src/B-L475E-IOT01/runapp/stm32l4xx_it.c                        \
                   src/B-L475E-IOT01/runapp/soc_init.c                            \
                   src/B-L475E-IOT01/runapp/system_stm32l4xx.c                    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c               \
                   src/B-L475E-IOT01/hal/flash_l4.c                               \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c       \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c        \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.c          \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.c              \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.c           \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.c           \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.c           \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.c           \
                   Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.c                   \
                   Drivers/BSP/Components/es_wifi/es_wifi.c                       \
                   Drivers/BSP/Components/hts221/hts221.c                         \
                   Drivers/BSP/Components/lis3mdl/lis3mdl.c                       \
                   Drivers/BSP/Components/lps22hb/lps22hb.c                       \
                   Drivers/BSP/Components/lsm6dsl/lsm6dsl.c                       \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api.c                   \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_calibration.c       \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_core.c              \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_ranging.c           \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_api_strings.c           \
                   Drivers/BSP/Components/vl53l0x/vl53l0x_platform_log.c          \
                   aos/soc_impl.c                                                 \
                   src/B-L475E-IOT01/runapp/aos.c                                 \
                   src/common/csp/wifi/src/es_wifi_io.c                           \
                   src/common/csp/wifi/src/wifi.c                                 \
                   src/B-L475E-IOT01/hal/hw.c                                     \
                   src/B-L475E-IOT01/hal/flash_port.c                             \
                   src/B-L475E-IOT01/hal/hal_i2c_stm32l4.c                        \
                   src/B-L475E-IOT01/sensor/vl53l0x_platform.c                    \
                   src/B-L475E-IOT01/sensor/vl53l0x_proximity.c                   \
                   src/B-L475E-IOT01/sensor/sensors_data.c                        \
                   src/B-L475E-IOT01/sensor/sensors.c                             \
                   src/B-L475E-IOT01/hal/wifi_port.c

ifneq ($(IDE),keil)
$(NAME)_SOURCES += src/B-L475E-IOT01/sensor/qspi.c               \
                   Middlewares/USB_Device/Core/Src/usbd_core.c   \
                   Middlewares/USB_Device/Core/Src/usbd_ctlreq.c \
                   Middlewares/USB_Device/Core/Src/usbd_ioreq.c
endif

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_armcc.s
$(NAME)_LINK_FILES := src/B-L475E-IOT01/runapp/startup_stm32l475xx_armcc.o
$(NAME)_LINK_FILES += src/B-L475E-IOT01/runapp/stm32l4xx_it.o
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_icc.s
else
$(NAME)_SOURCES += src/B-L475E-IOT01/runapp/startup_stm32l475xx_gcc.s
endif

