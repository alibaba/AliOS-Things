NAME := mcu_nrf52xxx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := driver & sdk &ble controller for platform/mcu nrf52832 or nrf52840

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino kernel.fs.vfs digest_algorithm network.bluetooth.bt

#$(NAME)_LIBSUFFIX := $(PLATFORM_MCU_BOARD)
$(NAME)_LIBSUFFIX := -52832

GLOBAL_DEFINES += CONFIG_ARM


GLOBAL_INCLUDES += \
		   ../../arch/arm/armv7m/gcc/m4/ \
                   Drivers/arch \
                   Drivers/arch/cortex_m \
                   Drivers/boards \
                   Drivers/toolchain \
                   Drivers/toolchain/cmsis/include \
                   Drivers/libraries/experimental_section_vars \
                   Drivers/libraries/strerror \
                   Drivers/libraries/timer \
                   Drivers/libraries/util \
		   Drivers/libraries/button \
                   Drivers/device \
                   Drivers/drivers_nrf \
                   Drivers/drivers_nrf/common \
		   Drivers/drivers_nrf/clock \
		   Drivers/drivers_nrf/gpiote \
                   Drivers/drivers_nrf/uart \
                   Drivers/drivers_nrf/hal \
                   Drivers/drivers_nrf/delay \
                   Drivers/drivers_nrf/systick \
		   Drivers/libraries/atomic \
		   Drivers/libraries/balloc \
		   Drivers/libraries/bsp \
                   Drivers/libraries/uart \
                   Drivers/libraries/fifo \
                   Drivers/libraries/bootloader/dfu \
                   Drivers/libraries/fstorage \
                   Drivers/libraries/crc32 \
		   Drivers/libraries/experimental_log \
		   Drivers/libraries/experimental_log/src \
		   Drivers/libraries/experimental_memobj \

$(NAME)_SOURCES += Drivers/boards/boards.c \
                   Drivers/drivers_nrf/clock/nrf_drv_clock.c \
                   Drivers/drivers_nrf/common/nrf_drv_common.c \
                   Drivers/drivers_nrf/gpiote/nrf_drv_gpiote.c \
                   Drivers/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
                   Drivers/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
                   Drivers/drivers_nrf/systick/nrf_drv_systick.c \
                   Drivers/drivers_nrf/uart/nrf_drv_uart.c \
                   Drivers/drivers_nrf/hal/nrf_nvmc.c \
                   Drivers/libraries/bsp/bsp.c \
                   Drivers/libraries/util/app_error.c \
                   Drivers/libraries/util/app_error_weak.c \
                   Drivers/libraries/util/app_util_platform.c \
                   Drivers/libraries/util/nrf_assert.c \
                   Drivers/libraries/balloc/nrf_balloc.c \
                   Drivers/libraries/experimental_memobj/nrf_memobj.c \
                   Drivers/libraries/strerror/nrf_strerror.c \
                   Drivers/libraries/button/app_button.c \
                   Drivers/libraries/timer/app_timer.c \
                   Drivers/libraries/experimental_log/src/nrf_log_frontend.c \
                   Drivers/libraries/uart/app_uart.c \
                   Drivers/libraries/bootloader/dfu/nrf_dfu_settings.c \
                   Drivers/libraries/bootloader/dfu/nrf_dfu_flash.c \
                   Drivers/libraries/crc32/crc32.c \
                   Drivers/libraries/fstorage/nrf_fstorage_nvmc.c \
                   Drivers/libraries/fstorage/nrf_fstorage.c \
                   Drivers/clock_control/nrf5_power_clock.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/aos.c
ifeq ($(ble),1)
$(NAME)_SOURCES  += hal/ais_ota_port.c
endif

$(NAME)_SOURCES  += hal/misc.c
$(NAME)_SOURCES  += hal/gpio.c
GLOBAL_CFLAGS += -DNRF_DFU_SETTINGS_VERSION=0

ifeq ($(HOST_MCU_NAME), nrf52840)
HOST_OPENOCD := pca10056
GLOBAL_CFLAGS += -DNRF52840_XXAA -DBOARD_PCA10056
GLOBAL_DEFINES += NRF52840_XXAA

GLOBAL_INCLUDES += ../../../board/pca10056/ \
                   src/pca10056/base_pro \
		   Drivers/softdevice/s140/headers/nrf52 \
                   Drivers/softdevice/s140/headers \

$(NAME)_SOURCES += src/pca10056/base_pro/soc_init.c \
	           Drivers/toolchain/system_nrf52.c \

else ifeq ($(HOST_MCU_NAME), nrf52832)
$(NAME)_COMPONENTS += kernel.fs.kv
HOST_OPENOCD := pca10040
GLOBAL_DEFINES += NRF52832_XXAA
GLOBAL_CFLAGS += -DNRF52832_XXAA -DBOARD_PCA10040 -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52_PAN_74 -DNRF52
GLOBAL_ASMFLAGS += -DNRF52832_XXAA -DBOARD_PCA10040 -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD  -DNRF52 -DNRF52_PAN_74

GLOBAL_INCLUDES += ../../../board/pca10040/ \
                   src/pca10040/base_pro \
                   Drivers/softdevice/s132/headers/nrf52 \
                   Drivers/softdevice/s132/headers \
		   Drivers/drivers_nrf/power \
		   Drivers/drivers_nrf/rtc \

$(NAME)_SOURCES += src/pca10040/base_pro/soc_init.c \
	           Drivers/drivers_nrf/power/nrf_drv_power.c \
	           Drivers/toolchain/system_nrf52.c \
		   Drivers/drivers_nrf/rtc/nrf_drv_rtc.c \

$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_rtc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_systick.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr.c
$(NAME)_SOURCES  += hal/flash.c

endif

ifeq ($(COMPILER), armcc)
ifeq ($(HOST_MCU_NAME), nrf52840)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52840.s
else ifeq ($(HOST_MCU_NAME), nrf52832)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52.s
endif

else ifeq ($(COMPILER), iar)

else
ifeq ($(HOST_MCU_NAME), nrf52840)
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52840.S
else ifeq ($(HOST_MCU_NAME), nrf52832)
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52.S
endif
endif

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections -DNRF_DFU_SETTINGS_VERSION=0
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7e-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 \
                   -march=armv7e-m  \
                   -mlittle-endian \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M   \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
ifeq ($(HOST_MCU_NAME), nrf52840)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/pca10056/Drivers/toolchain/arm/
else ifeq ($(HOST_MCU_NAME), nrf52832)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/nrf52xxx/Drivers/toolchain/arm/
endif
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else

ifeq ($(MBINS),)
ifeq ($(HOST_MCU_NAME), nrf52840)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common.ld
else ifeq ($(HOST_MCU_NAME), nrf52832)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa.ld
endif
else ifeq ($(MBINS),app)
ifeq ($(HOST_MCU_NAME), nrf52840)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common_app.ld
else  ifeq ($(HOST_MCU_NAME), nrf52832)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_app.ld
endif
else ifeq ($(MBINS),kernel)
ifeq ($(HOST_MCU_NAME), nrf52840)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52_common_kernel.ld
else  ifeq ($(HOST_MCU_NAME), nrf52832)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_kernel.ld
endif
endif

endif


