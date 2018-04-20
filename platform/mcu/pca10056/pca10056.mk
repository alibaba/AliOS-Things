NAME := pca10056
HOST_OPENOCD := pca10056
$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal vfs digest_algorithm protocols.bluetooth

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_ARM

GLOBAL_INCLUDES += \
				   src/pca10056/base_pro \
				   	Drivers/toolchain/cmsis/include  \
					Drivers \
					Drivers/device \
					Drivers/boards \
					Drivers/drivers_nrf/clock \
					Drivers/drivers_nrf/common \
					Drivers/drivers_nrf/gpiote \
					Drivers/drivers_nrf/hal \
					Drivers/drivers_nrf/nrf_soc_nosd \
					Drivers/libraries/atomic \
					Drivers/libraries/balloc \
					Drivers/libraries/bsp \
					Drivers/libraries/button \
					Drivers/libraries/experimental_log \
					Drivers/libraries/experimental_log/src \
					Drivers/libraries/experimental_memobj \
					Drivers/libraries/experimental_section_vars \
					Drivers/libraries/strerror \
					Drivers/libraries/timer \
					Drivers/libraries/util \
					Drivers/toolchain \
					../../arch/arm/armv7m/gcc/m4/ \
					../../../board/pca10056/ \
					Drivers/drivers_nrf/delay/ \
					Drivers/drivers_nrf/systick \
					Drivers/arch \
					Drivers/arch/cortex_m \
					Drivers/drivers_nrf/uart \
					Drivers/libraries/uart \
					Drivers/libraries/fifo \
					Drivers/drivers_nrf \
					Drivers/libraries/bootloader/dfu \
					Drivers/libraries/fstorage \
					Drivers/softdevice/s140/headers/nrf52 \
					Drivers/softdevice/s140/headers \
					Drivers/libraries/crc32 \

$(NAME)_SOURCES := Drivers/boards/boards.c \
				   Drivers/toolchain/system_nrf52840.c \
				   Drivers/libraries/bsp/bsp.c \
				   Drivers/drivers_nrf/clock/nrf_drv_clock.c \
				   Drivers/drivers_nrf/common/nrf_drv_common.c \
				   Drivers/drivers_nrf/gpiote/nrf_drv_gpiote.c \
				   Drivers/drivers_nrf/nrf_soc_nosd/nrf_nvic.c \
				   Drivers/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
				   Drivers/drivers_nrf/systick/nrf_drv_systick.c \
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
				   Drivers/clock_control/nrf5_power_clock.c \
				   Drivers/libraries/uart/app_uart_fifo.c \
				   Drivers/drivers_nrf/uart/nrf_drv_uart.c \
				   src/pca10056/base_pro/soc_init.c \
				   Drivers/libraries/fifo/app_fifo.c \
				   Drivers/libraries/bootloader/dfu/nrf_dfu_settings.c \
				   Drivers/libraries/bootloader/dfu/nrf_dfu_flash.c \
				   Drivers/libraries/crc32/crc32.c \
				   Drivers/libraries/fstorage/nrf_fstorage_nvmc.c \
				   Drivers/libraries/fstorage/nrf_fstorage.c \
				   Drivers/drivers_nrf/hal/nrf_nvmc.c \

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c \
                   aos/aos.c

ifeq ($(ble),1)
$(NAME)_SOURCES  += hal/ais_ota_port.c
endif

$(NAME)_SOURCES  += hal/misc.c

GLOBAL_CFLAGS += -DNRF52840_XXAA -DBOARD_PCA10056
                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52840.s
     
else ifeq ($(COMPILER), iar)


else
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52840.S

endif
     

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
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
                   -march=armv7-m  \
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
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/pca10056/Drivers/toolchain/arm/
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/mcu/pca10056/nrf52_common.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T platform/mcu/pca10056/nrf52_common_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/mcu/pca10056/nrf52_common_kernel.ld
endif

GLOBAL_CFLAGS += -DNRF_DFU_SETTINGS_VERSION=0

GLOBAL_DEFINES += NRF52840_XXAA

endif


