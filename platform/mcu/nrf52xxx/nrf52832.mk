NAME := pca10040
HOST_OPENOCD := pca10040
$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal rhino.vfs digest_algorithm network.bluetooth.bt rhino.fs.kv

GLOBAL_DEFINES += CONFIG_ARM

GLOBAL_INCLUDES += \
				   src/pca10040/base_pro \
				   	Drivers/toolchain/cmsis/include  \
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
					../../arch/arm/armv7m/gcc/m4/ \
					../../../board/pca10040/ \
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
                                        Drivers/softdevice/s132/headers/ \
				        Drivers/softdevice/s132/headers/nrf52 \
					Drivers/libraries/crc32 \

$(NAME)_SOURCES := Drivers/boards/boards.c \
				   Drivers/toolchain/system_nrf52.c \
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
				   Drivers/drivers_nrf/power/nrf_drv_power.c \
				   Drivers/libraries/uart/app_uart.c \
				   Drivers/drivers_nrf/uart/nrf_drv_uart.c \
				   src/pca10040/base_pro/soc_init.c \
				   Drivers/libraries/bootloader/dfu/nrf_dfu_settings.c \
				   Drivers/libraries/bootloader/dfu/nrf_dfu_flash.c \
				   Drivers/libraries/crc32/crc32.c \
				   Drivers/libraries/fstorage/nrf_fstorage_nvmc.c \
				   Drivers/libraries/fstorage/nrf_fstorage.c \
				   Drivers/drivers_nrf/hal/nrf_nvmc.c \
				   Drivers/drivers_nrf/rtc/nrf_drv_rtc.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c \
                   aos/aos.c

ifeq ($(ble),1)
$(NAME)_SOURCES  += hal/ais_ota_port.c
endif

$(NAME)_SOURCES  += hal/misc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_rtc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_systick.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr.c
$(NAME)_SOURCES  += hal/flash.c

#GLOBAL_CFLAGS += -O3 -g3
GLOBAL_CFLAGS += -DNRF52832_XXAA
GLOBAL_CFLAGS += -DBOARD_PCA10040
#GLOBAL_CFLAGS += -DBSP_DEFINES_ONLY
GLOBAL_CFLAGS += -DCONFIG_GPIO_AS_PINRESET
GLOBAL_CFLAGS += -DFLOAT_ABI_HARD
GLOBAL_CFLAGS += -DNRF52
GLOBAL_CFLAGS += -DNRF52_PAN_74
#GLOBAL_CFLAGS += -mcpu=cortex-m4
#GLOBAL_CFLAGS += -mthumb -mabi=aapcs
#GLOBAL_CFLAGS +=  -Wall -Werror
#GLOBAL_CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
#GLOBAL_CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
#GLOBAL_CFLAGS += -fno-builtin -fshort-enums

GLOBAL_ASMFLAGS += -DNRF52832_XXAA
GLOBAL_ASMFLAGS += -DBOARD_PCA10040
GLOBAL_ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
GLOBAL_ASMFLAGS += -DFLOAT_ABI_HARD
GLOBAL_ASMFLAGS += -DNRF52
GLOBAL_ASMFLAGS += -DNRF52_PAN_74
#GLOBAL_ASMFLAGS += -g3
#GLOBAL_ASMFLAGS += -mcpu=cortex-m4
#GLOBAL_ASMFLAGS += -mthumb -mabi=aapcs
#GLOBAL_ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16                   

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += Drivers/toolchain/arm/arm_startup_nrf52.s
     
else ifeq ($(COMPILER), iar)


else
$(NAME)_SOURCES += Drivers/toolchain/gcc/gcc_startup_nrf52.S

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
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/nrf52xxx/Drivers/toolchain/arm/
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/mcu/nrf52xxx/nrf52832_xxaa_kernel.ld
endif

GLOBAL_CFLAGS += -DNRF_DFU_SETTINGS_VERSION=0

GLOBAL_DEFINES += NRF52832_XXAA

GLOBAL_DEFINES += nrf52

endif


