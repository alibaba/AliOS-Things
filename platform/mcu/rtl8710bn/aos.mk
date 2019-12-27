#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := mcu_rtl8710bn

HOST_OPENOCD := rtl8710bn

#SPI_WIFI_ENABLED := true

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu rtl8710bn

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
$(NAME)_COMPONENTS += ota_2ndboot
$(NAME)_LIBSUFFIX  := _2ndboot
else
$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino lwip netmgr debug
endif

#GLOBAL_DEFINES += WITH_LWIP

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += arch
GLOBAL_INCLUDES += sdk
GLOBAL_INCLUDES += aos
GLOBAL_INCLUDES += peripherals
GLOBAL_INCLUDES += include

$(NAME)_INCLUDES += .
$(NAME)_INCLUDES += include
$(NAME)_INCLUDES += ../../../components/network/lwip/include
$(NAME)_INCLUDES += ../../../components/network/lwip/include/lwip
$(NAME)_INCLUDES += arch
$(NAME)_INCLUDES += aos
$(NAME)_INCLUDES += peripherals
$(NAME)_INCLUDES += sdk
$(NAME)_INCLUDES += sdk/project/realtek_amebaz_va0_example/inc
$(NAME)_INCLUDES += sdk/component/os/os_dep/include
$(NAME)_INCLUDES += sdk/component/common/api/network/include
$(NAME)_INCLUDES += sdk/component/common/api
$(NAME)_INCLUDES += sdk/component/common/api/at_cmd
$(NAME)_INCLUDES += sdk/component/common/api/platform
$(NAME)_INCLUDES += sdk/component/common/api/wifi
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wpa_supplicant/src
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wowlan
$(NAME)_INCLUDES += sdk/component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant
$(NAME)_INCLUDES += sdk/component/common/drivers/modules
$(NAME)_INCLUDES += sdk/component/common/drivers/sdio/realtek/sdio_host/inc
$(NAME)_INCLUDES += sdk/component/common/drivers/inic/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/drivers/usb_class/device
$(NAME)_INCLUDES += sdk/component/common/drivers/usb_class/device/class
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/include
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/osdep
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hci
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/src/hal/OUTSRC
$(NAME)_INCLUDES += sdk/component/common/drivers/wlan/realtek/wlan_ram_map/rom
$(NAME)_INCLUDES += sdk/component/common/network
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/app/monitor/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/cmsis/device
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/ram_lib/crypto
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/fwlib/rom_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/os_dep/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libc/rom/string
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/std_lib/libgcc/rtl8195a/include
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/swlib/rtl_lib
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc
$(NAME)_INCLUDES += sdk/component/soc/realtek/8711b/misc/os
$(NAME)_INCLUDES += sdk/component/common/mbed/api
$(NAME)_INCLUDES += sdk/component/common/mbed/hal
$(NAME)_INCLUDES += sdk/component/common/mbed/hal_ext
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/cmsis/rtl8711b
$(NAME)_INCLUDES += sdk/component/common/mbed/targets/hal/rtl8711b

#GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
                 -mfpu=fpv4-sp-d16 \
                 -mfloat-abi=hard \
                 -DCONFIG_PLATFORM_8711B \
                 -DM3

GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian           \
                 -DCONFIG_PLATFORM_8711B   \
                 -DM3                      \
				 -DHardFault_Handler=Ali_HardFault_Handler

GLOBAL_ASMFLAGS += -DHardFault_Handler=Ali_HardFault_Handler

GLOBAL_CFLAGS += -w

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
GLOBAL_LDFLAGS += -T $(SOURCE_ROOT)/platform/mcu/rtl8710bn/script/rlx8711B-symbol-v02-img2-2ndboot.ld
else
GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)/platform/mcu/rtl8710bn
GLOBAL_LDFLAGS += -T $(SOURCE_ROOT)/platform/mcu/rtl8710bn/script/rlx8711B-symbol-v02-img2_xip1.ld
GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)/platform/mcu/rtl8710bn/lib/ -l_platform -l_wlan -l_wps -l_p2p -l_rtlstd
endif

#GLOBAL_LDFLAGS += -mcpu=cortex-m4        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  -mfpu=fpv4-sp-d16 \
                  -mfloat-abi=hard \
                  -Wl,--no-enum-size-warning \
                  -Wl,--no-wchar-size-warning \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
GLOBAL_LDFLAGS += -mcpu=cortex-m4        \
                  -mthumb\
                  -Os \
                  -nostartfiles \
                  --specs=nosys.specs \
                  -Wl,--no-enum-size-warning \
                  -Wl,--no-wchar-size-warning \
                  -Wl,--gc-sections \
                  -Wl,--cref
else
GLOBAL_LDFLAGS += -mcpu=cortex-m4             \
                  -mthumb                     \
                  -Os                         \
                  -nostartfiles               \
                  -Wl,--no-enum-size-warning  \
                  -Wl,--no-wchar-size-warning \
                  -Wl,--gc-sections           \
                  -Wl,--cref                  \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
$(NAME)_SOURCES :=  hal/2ndboot/startup.c  \
                    hal/2ndboot/flash.c    \
                    hal/2ndboot/sys.c      \
                    hal/2ndboot/uart.c     \
                    hal/2ndboot/wdg.c
else
$(NAME)_SOURCES := aos/soc_impl.c   \
                   aos/aos.c        \
                   aos/aos_osdep.c  \
                   aos/ethernetif.c \
                   aos/qc.c         \
                   hal/uart.c       \
                   hal/flash.c      \
                   hal/hw.c         \
                   hal/wifi_port.c  \
                   hal/gpio.c       \
                   hal/wdg.c

$(NAME)_SOURCES += CheckSumUtils.c

$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_rtc.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr_systick.c
$(NAME)_SOURCES  += hal/pwrmgmt_hal/board_cpu_pwr.c

include ./platform/mcu/rtl8710bn/peripherals/peripherals.mk
endif

GLOBAL_INCLUDES += hal/2ndboot

include ./platform/mcu/rtl8710bn/sdk/sdk.mk

EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk
