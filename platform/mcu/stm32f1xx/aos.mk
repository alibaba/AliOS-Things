NAME := mcu_stm32f1xx

HOST_OPENOCD := stm32f1xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32f1xx

$(NAME)_COMPONENTS += arch_armv7m rhino newlib_stub

GLOBAL_DEFINES += USE_HAL_DRIVER

GLOBAL_INCLUDES += hal\
                   hal/include \
                   drivers/STM32F1xx_HAL_Driver/Inc \
                   drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
                   drivers/CMSIS/Include \
                   drivers/CMSIS/Device/ST/STM32F1xx/Include

$(NAME)_SOURCES += drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_crc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dac.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_fsmc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_sdmmc.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
                   drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c

$(NAME)_SOURCES += hal/hal_uart_stm32f1.c \
                   hal/hw.c \
                   hal/hal_flash_stm32f1.c \
                   hal/hal_i2c_stm32f1.c \
                   hal/hal_spi_stm32f1.c \
                   hal/hal_gpio_stm32f1.c

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
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3 \
                  -L --apcs=/softfp \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent

else
GLOBAL_LDFLAGS += -mcpu=cortex-m3 \
                  -mlittle-endian \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  -mfloat-abi=soft \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
