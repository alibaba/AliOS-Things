NAME := board_m400

JTAG := jlink_swd

$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M0
HOST_MCU_FAMILY      := stm32l0xx.stm32l071kb
SUPPORT_BINS         := no

$(NAME)_SOURCES := hal/board.c                 \
				   hal/hw_gpio.c           \
				   hal/hw_spi.c            \
				   hal/hw_rtc.c            \
				   hal/m400_hw.c           \
				   hal/m400_it.c           \
				   hal/vcom.c              \
				   port/dbg_port.c         \
				   component/debug/debug.c

#$(NAME)_COMPONENTS += rhino.fs.kv

linkwan ?= 0
ifeq ($(linkwan), 0)
$(NAME)_SOURCES += app/lora.c
GLOBAL_DEFINES += REGION_US915_HYBRID
else ifeq ($(linkwan), 1)
$(NAME)_SOURCES += port/lora_port.c        \
				   port/lorawan_lpm_port.c      \
				   port/lorawan_radio_port.c    \
				   port/lorawan_timer_port.c

$(NAME)_COMPONENTS += network.lorawan.lorachip network.lorawan
LORACHIP := sx1276

GLOBAL_DEFINES += M400_LORAWAN
endif

#$(NAME)_COMPONENTS += network.lorawan.lorachip network.lorawan
#LORACHIP := sx1276

#$(NAME)_LINK_FILES := src/eml3047_it.o

GLOBAL_INCLUDES +=  .            \
				    hal     \
					port    \
					component/at_cli   \
					component/debug    \
					app

GLOBAL_INCLUDES += \
../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc \
../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include

GLOBAL_DEFINES += \
USE_HAL_DRIVER \
STM32L071xx

GLOBAL_INCLUDES +=  ../../network/lorawan/lorachip/sx1276   \
					../../network/lorawan/lora/system  \
					../../network/lorawan/lora/mac  \
					../../network/lorawan/lora/radio  \
					../../network/lorawan/linkwan\include

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP
GLOBAL_DEFINES += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0 RHINO_CONFIG_NORMAL_PRT=0
GLOBAL_DEFINES += USE_FULL_LL_DRIVER USE_B_M400 RADIO_DIO_4 RADIO_DIO_5

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_M400
CONFIG_SYSINFO_DEVICE_NAME := M400

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/m400

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
#EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
