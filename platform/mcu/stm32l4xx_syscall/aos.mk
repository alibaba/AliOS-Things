NAME         := mcu_stm32l4xx_syscall
HOST_OPENOCD := stm32l4xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32l4xx_syscall

$(NAME)_COMPONENTS += arch_armv7m-svc
$(NAME)_COMPONENTS += newlib_stub rhino vfs lwip

GLOBAL_DEFINES += USE_HAL_DRIVER

GLOBAL_INCLUDES += Drivers/STM32L4xx_HAL_Driver/Inc        \
                   Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
                   Drivers/CMSIS/Include                   \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Include

$(NAME)_SOURCES := Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c               \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_comp.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp_ex.c       \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dcmi.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm_ex.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma2d.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dsi.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_firewall.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gfxmmu.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash_ex.c       \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hcd.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_irda.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lcd.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc_ex.c       \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp_ex.c      \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ospi.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard.c     \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard_ex.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smbus.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sram.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_swpmi.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c        \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tsc.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c       \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_wwdg.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_adc.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_comp.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crc.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crs.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dac.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma2d.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c           \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_i2c.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lptim.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lpuart.c         \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_opamp.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_pwr.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rng.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rtc.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_spi.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c          \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c            \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c          \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c

$(NAME)_SOURCES += aos/soc_impl.c         \
                   aos/hook_impl.c        \
                   aos/rttest_impl.c      \
                   hal/hal_uart_stm32l4.c \
                   hal/hw.c

GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard          \
                 -mfpu=fpv4-sp-d16         \
                 -w

GLOBAL_CFLAGS  += -D__VFP_FP__

GLOBAL_CXXFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

GLOBAL_ASMFLAGS += -mcpu=cortex-m4   \
                   -mlittle-endian   \
                   -mthumb           \
                   -mfloat-abi=hard  \
                   -mfpu=fpv4-sp-d16 \
                   -w

GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs       \
                  -mfloat-abi=hard          \
                  -mfpu=fpv4-sp-d16         \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

