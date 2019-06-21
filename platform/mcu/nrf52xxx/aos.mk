NAME := mcu_nrf52xxx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk &ble controller for platform/mcu nrf52810 nrf52832 or nrf52840

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino kv

#$(NAME)_LIBSUFFIX := $(PLATFORM_MCU_BOARD)

GLOBAL_INCLUDES +=  .\
                    Drivers \
                    Drivers/device \
                    Drivers/boards \
                    Drivers/drivers_nrf/clock \
                    Drivers/drivers_nrf/common \
                    Drivers/drivers_nrf/gpiote \
                    Drivers/drivers_nrf/hal \
                    Drivers/drivers_nrf/nrf_soc_nosd \
                    Drivers/drivers_nrf/rtc \
                    Drivers/drivers_nrf/power \
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
                    Drivers/toolchain/cmsis/include \
                    Drivers/drivers_nrf/delay \
                    Drivers/drivers_nrf/systick \
                    Drivers/arch \
                    Drivers/arch/cortex_m \
                    Drivers/drivers_nrf/uart \
                    Drivers/libraries/uart \
                    Drivers/libraries/fifo \
                    Drivers/drivers_nrf \
                    Drivers/libraries/bootloader/dfu \
                    Drivers/libraries/fstorage \
                    Drivers/libraries/crc32

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
$(NAME)_SOURCES  += hal/ble_port.c
endif

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

include $($(NAME)_LOCATION)/$(HOST_MCU_NAME).mk

ifneq ($(bt_mesh_standalone_deploy),1)
$(NAME)_COMPONENTS += bt_host
include $($(NAME)_LOCATION)/bt_controller/bt_controller.mk
endif

GLOBAL_DEFINES-y += MBEDTLS_AES_ROM_TABLES

EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

