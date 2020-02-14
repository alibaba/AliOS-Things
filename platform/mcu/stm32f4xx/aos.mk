NAME := mcu_stm32f4xx

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4

# Host MCU alias for OpenOCD
HOST_OPENOCD := stm32f4x

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32f4xx

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino netmgr

# Global defines
GLOBAL_DEFINES  := USE_STDPERIPH_DRIVER \
                   _STM3x_              \
                   _STM32x_

# Convert the MCU variant into the required STM peripheral library constant
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F405 STM32F415 STM32F407 STM32F417))
GLOBAL_DEFINES += STM32F40_41xxx
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F427 STM32F437))
GLOBAL_DEFINES += STM32F427_437xx
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F429 STM32F439))
GLOBAL_DEFINES += STM32F429_439xx
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F401))
GLOBAL_DEFINES += STM32F401xx
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F411))
GLOBAL_DEFINES += STM32F411xE
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F412))
GLOBAL_DEFINES += STM32F412xG
endif
ifneq (,$(filter $(HOST_MCU_VARIANT), STM32F446))
GLOBAL_DEFINES += STM32F446xx
endif

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4
GLOBAL_INCLUDES += include          \
                   csp/lwip/include \
                   startup          \
                   driver           \
                   wifi/inc

GLOBAL_CFLAGS += -DSTM32F4xx

GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork -D__MICROLIB -DSTM32F4xx
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

GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  -nostartfiles             \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_SOURCES := platform_init.c              \
                   platform_vector_table.c      \
                   hardfault_handler.c          \
                   startup/startup_stm32f4xx.s  \
                   wifi/wifi_port.c             \
                   GCC/platform_unhandled_isr.c \
                   aos/soc_impl.c               \
                   aos/aos.c                    \
                   hal/i2c.c                    \
                   hal/hw.c                     \
                   hal/uart.c                   \
                   hal/flash.c                  \

GLOBAL_INCLUDES += ./mico/include

$(NAME)_SOURCES += mico/mico_rhino.c

###      wifi/src/wifi.c hal/hw.c hal/wifi_port.c  hal/flash_port.c

GLOBAL_LDFLAGS += -L ./platform/mcu/stm32f4xx/$(TOOLCHAIN_NAME)

# Components
#$(NAME)_COMPONENTS += $(TOOLCHAIN_NAME)
include ./platform/mcu/stm32f4xx/spi_flash/spi_flash.mk

include ./platform/mcu/stm32f4xx/peripherals/peripherals.mk
include ./platform/mcu/stm32f4xx/peripherals/libraries/libraries.mk

ifndef NO_WIFI
$(NAME)_SOURCES += wlan_bus_driver/wlan_platform_common.c \
                   wlan_bus_driver/wlan_platform.c        \
                   wlan_bus_driver/wlan_bus_$(BUS).c
else
ifdef SHARED_WIFI_SPI_BUS
$(NAME)_SOURCES += wlan_bus_driver/wlan_bus_SPI.c
endif #SHARED_WIFI_SPI_BUS
endif #NO_WIFI

# $(NAME)_CFLAGS = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

ifeq ($(APP),bootloader)
####################################################################################
# Building bootloader
####################################################################################

DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     +=

else
ifneq ($(filter spi_flash_write, $(APP)),)
####################################################################################
# Building spi_flash_write
####################################################################################

PRE_APP_BUILDS  += bootloader
GLOBAL_LDFLAGS  += -T platform/mcu/stm32f4xx/$(TOOLCHAIN_NAME)/app_ram$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES  += __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
GLOBAL_INCLUDES +=

else
####################################################################################
# Building standard application to run with bootloader
####################################################################################

PRE_APP_BUILDS  += bootloader
GLOBAL_LDFLAGS  += -T  platform/mcu/stm32f4xx/$(TOOLCHAIN_NAME)/app_with_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES +=

endif # APP=spi_flash_write
endif # APP=bootloader

GLOBAL_DEFINES += CONFIG_ARM
