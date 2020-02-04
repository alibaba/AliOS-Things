NAME         := mcu_msp432p4xx
HOST_OPENOCD := msp432p4xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu msp432p4xx

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino

GLOBAL_INCLUDES += . \
                   CMSIS/Include

$(NAME)_SOURCES := ti/devices/msp432p4xx/driverlib/adc14.c     \
                   ti/devices/msp432p4xx/driverlib/aes256.c    \
                   ti/devices/msp432p4xx/driverlib/comp_e.c    \
                   ti/devices/msp432p4xx/driverlib/cpu.c       \
                   ti/devices/msp432p4xx/driverlib/crc32.c     \
                   ti/devices/msp432p4xx/driverlib/cs.c        \
                   ti/devices/msp432p4xx/driverlib/dma.c       \
                   ti/devices/msp432p4xx/driverlib/flash.c     \
                   ti/devices/msp432p4xx/driverlib/flash_a.c   \
                   ti/devices/msp432p4xx/driverlib/fpu.c       \
                   ti/devices/msp432p4xx/driverlib/gpio.c      \
                   ti/devices/msp432p4xx/driverlib/i2c.c       \
                   ti/devices/msp432p4xx/driverlib/interrupt.c \
                   ti/devices/msp432p4xx/driverlib/lcd_f.c     \
                   ti/devices/msp432p4xx/driverlib/mpu.c       \
                   ti/devices/msp432p4xx/driverlib/pcm.c       \
                   ti/devices/msp432p4xx/driverlib/pmap.c      \
                   ti/devices/msp432p4xx/driverlib/pss.c       \
                   ti/devices/msp432p4xx/driverlib/ref_a.c     \
                   ti/devices/msp432p4xx/driverlib/reset.c     \
                   ti/devices/msp432p4xx/driverlib/rtc_c.c     \
                   ti/devices/msp432p4xx/driverlib/spi.c       \
                   ti/devices/msp432p4xx/driverlib/sysctl.c    \
                   ti/devices/msp432p4xx/driverlib/sysctl_a.c  \
                   ti/devices/msp432p4xx/driverlib/systick.c   \
                   ti/devices/msp432p4xx/driverlib/timer_a.c   \
                   ti/devices/msp432p4xx/driverlib/timer32.c   \
                   ti/devices/msp432p4xx/driverlib/uart.c      \
                   ti/devices/msp432p4xx/driverlib/wdt_a.c     \
                   ti/drivers/adc/ADCMSP432.c                  \
                   ti/drivers/adcbuf/ADCBufMSP432.c            \
                   ti/drivers/capture/CaptureMSP432.c          \
                   ti/drivers/dma/UDMAMSP432.c                 \
                   ti/drivers/gpio/GPIOMSP432.c                \
                   ti/drivers/i2c/I2CMSP432.c                  \
                   ti/drivers/i2cslave/I2CSlaveMSP432.c        \
                   ti/drivers/nvs/NVSMSP432.c                  \
                   ti/drivers/nvs/NVSRAM.c                     \
                   ti/drivers/nvs/NVSSPI25X.c                  \
                   ti/drivers/power/PowerMSP432.c              \
                   ti/drivers/pwm/PWMTimerMSP432.c             \
                   ti/drivers/sd/SDSPI.c                       \
                   ti/drivers/spi/SPIMSP432DMA.c               \
                   ti/drivers/timer/TimerMSP432.c              \
                   ti/drivers/uart/UARTMSP432.c                \
                   ti/drivers/utils/List.c                     \
                   ti/drivers/utils/RingBuf.c                  \
                   ti/drivers/watchdog/WatchdogMSP432.c        \
                   ti/drivers/ADC.c                            \
                   ti/drivers/ADCBuf.c                         \
                   ti/drivers/Capture.c                        \
                   ti/drivers/I2C.c                            \
                   ti/drivers/I2CSlave.c                       \
                   ti/drivers/NVS.c                            \
                   ti/drivers/PWM.c                            \
                   ti/drivers/SD.c                             \
                   ti/drivers/SDSPILegacy.c                    \
                   ti/drivers/SPI.c                            \
                   ti/drivers/Timer.c                          \
                   ti/drivers/UART.c                           \
                   ti/drivers/Watchdog.c                       \
                   ti/kernel/ClockP_alios.c                    \
                   ti/kernel/HwiPMSP432_alios.c                \
                   ti/kernel/SemaphoreP_alios.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/hook_impl.c \
                   aos/aos.c \
                   hal/hal_uart_msp432p4.c \
                   hal/hw.c  \
                   hal/hal_gpio_msp432p4.c \
                   hal/hal_i2c_msp432p4.c \
                   hal/hal_flash_msp432p4.c # \
                   hal/hal_sd_msp432p4.c \
                   hal/hal_adc_msp432p4.c \
                   hal/hal_rtc_msp432p4.c \
                   hal/hal_spi_msp432p4.c \
                   hal/hal_qspi_msp432p4.c \
                   hal/hal_nand_msp432p4.c \
                   hal/hal_nor_msp432p4.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard          \
                 -mfpu=fpv4-sp-d16         \
                 -w
GLOBAL_CFLAGS += -D__VFP_FP__
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4   \
                   -mlittle-endian   \
                   -mthumb           \
                   -mfloat-abi=hard  \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4                    \
                  -L --fpu=vfpv4_sp_d16                 \
                  -L --apcs=/hardfp                     \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs       \
                  -mfloat-abi=hard          \
                  -mfpu=fpv4-sp-d16         \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
