NAME := mcu_stm32f1xx
HOST_OPENOCD := stm32f1xx
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32l4xx

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino vfs

GLOBAL_DEFINES += USE_HAL_DRIVER

GLOBAL_INCLUDES += hal\
                   hal/include \
                   Drivers/STM32F1xx_HAL_Driver/Inc \
                   Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
                   Drivers/CMSIS/Include \
                   Drivers/CMSIS/Device/ST/STM32F1xx/Include

$(NAME)_SOURCES := Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c  \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c  \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c  \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_crc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dac.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_fsmc.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.c    \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
                   Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \
                   Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c

$(NAME)_SOURCES += hal/hal_uart_stm32f1.c \
                   hal/hw.c \
                   hal/hal_flash_stm32f1.c \
                   hal/hal_i2c_stm32f1.c \
                   hal/hal_spi_stm32f1.c \
                   hal/hal_gpio_stm32f1.c #\
                   hal/hal_sd_stm32f1.c \
                   hal/hal_adc_stm32f1.c \
                   hal/hal_rtc_stm32f1.c \
                   hal/hal_qspi_stm32f1.c \
                   hal/hal_nand_stm32f1.c \
                   hal/hal_nor_stm32f1.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M3 --apcs=/softfp -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M3 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m3 \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=soft \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M3 -g --apcs=/softfp --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M3 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m3 \
                   -mlittle-endian \
                   -mthumb \
                   -mfloat-abi=soft \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3   \
                  -L --apcs=/softfp \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent

else
GLOBAL_LDFLAGS += -mcpu=cortex-m3  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  -mfloat-abi=soft \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
