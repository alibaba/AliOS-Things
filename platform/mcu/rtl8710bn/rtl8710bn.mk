#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := rtl8710bn

HOST_OPENOCD := rtl8710bn

SPI_WIFI_ENABLED := true

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
#$(NAME)_COMPONENTS += libc rhino hal netmgr framework.common mbedtls cjson cli digest_algorithm
$(NAME)_COMPONENTS += libc rhino hal netmgr framework.common cli digest_algorithm protocols.net
#$(NAME)_COMPONENTS += testcase testcase.kernel.modules.kv_test
#$(NAME)_COMPONENTS += testcase.kernel.yloop_test
#$(NAME)_COMPONENTS += testcase.kernel.vcall_test
#$(NAME)_COMPONENTS += testcase.kernel.vfs_test
#$(NAME)_COMPONENTS += testcase.kernel.rhino_test
#$(NAME)_COMPONENTS += testcase.aosapi.api_test
#$(NAME)_COMPONENTS += testcase.framework.wifi_hal_test
#$(NAME)_COMPONENTS += testcase.framework.netmgr_test
#$(NAME)_COMPONENTS += testcase.framework.coap_test
#$(NAME)_COMPONENTS += testcase.basic_test
#$(NAME)_COMPONENTS += testcase.utility.hashtable_test
#$(NAME)_COMPONENTS += testcase.utility.cjson_test
#$(NAME)_COMPONENTS += testcase.utility.digest_algorithm_test
#$(NAME)_COMPONENTS += testcase testcase.kernel.modules.kv_test testcase.kernel.deviceIO_test
#$(NAME)_COMPONENTS += rhino hal protocols.net framework.common cli
#$(NAME)_COMPONENTS += rhino cli hal log protocols.net
#$(NAME)_COMPONENTS += libc rhino cli protocols.net hal vfs
#libc rhino hal netmgr framework.common mbedtls cjson cli digest_algorithm
$(NAME)_COMPONENTS += platform/mcu/rtl8710bn/sdk
$(NAME)_COMPONENTS += platform/mcu/rtl8710bn/peripherals

#GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/platform/mcu/rtl8710bn
#GLOBAL_LDFLAGS  += -I $(SOURCE_ROOT)/platform/mcu/rtl8710bn
#GLOBAL_LDFLAGS  += -I $(SOURCE_ROOT)\kernel\rhino\core\include

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
#GLOBAL_DEFINES += WITH_LWIP

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4
GLOBAL_INCLUDES += ../../../board/amebaz_dev
GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += arch
GLOBAL_INCLUDES += sdk
GLOBAL_INCLUDES += aos
GLOBAL_INCLUDES += peripherals
GLOBAL_INCLUDES += ../include
GLOBAL_INCLUDES += ../../../include/hal/soc
GLOBAL_INCLUDES += ../../../kernel/protocols/net/include
GLOBAL_INCLUDES += ../../../kernel/protocols/net/include/lwip

$(NAME)_INCLUDES += .
$(NAME)_INCLUDES += ../include
$(NAME)_INCLUDES += ../../../include/hal/soc
$(NAME)_INCLUDES += ../../../kernel/protocols/net/include
$(NAME)_INCLUDES += ../../../kernel/protocols/net/include/lwip
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
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
                 -mfpu=fpv4-sp-d16 \
                 -mfloat-abi=hard \
                 -DCONFIG_PLATFORM_8711B \
                 -DM3\
                 -fno-short-enums

GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
                 -DCONFIG_PLATFORM_8711B \
                 -DM3\
                 -fno-short-enums                 

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -L $(SOURCE_ROOT)/platform/mcu/rtl8710bn
#GLOBAL_LDFLAGS += -I $(SOURCE_ROOT)/platform/mcu/rtl8710bn
GLOBAL_LDFLAGS += -T $(SOURCE_ROOT)/platform/mcu/rtl8710bn/script/rlx8711B-symbol-v02-img2_xip1.ld                 
#GLOBAL_LDFLAGS += $(SOURCE_ROOT)/platform/mcu/rtl8710bn/bin/boot_all.o
GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)/platform/mcu/rtl8710bn/lib/ -l_platform -l_wlan -l_wps -l_p2p -l_rtlstd

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

GLOBAL_LDFLAGS += -mcpu=cortex-m4        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  -Wl,--no-enum-size-warning \
                  -Wl,--no-wchar-size-warning \
                  $(CLIB_LDFLAGS_NANO_FLOAT)                  

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

#$(NAME)_SOURCES += aos/soc_impl.c
#$(NAME)_SOURCES += aos/trace_impl.c
#$(NAME)_SOURCES += aos/aos.c
#$(NAME)_SOURCES += soc_impl.c
#$(NAME)_INCLUDES += ../../../kernel/vcall/mico/include
#$(NAME)_INCLUDES += ../../../framework/protocol/alink/os/platform
#$(NAME)_INCLUDES += ../include
#$(NAME)_INCLUDES += ../../../include/hal/soc
#$(NAME)_INCLUDES += ../../../include/aos
#$(NAME)_INCLUDES += arch
#$(NAME)_INCLUDES += peripherals

$(NAME)_SOURCES := aos/soc_impl.c          \
                   aos/trace_impl.c \
                   aos/aos.c \
                   aos/aos_osdep.c \
                   aos/ethernetif.c \
                   hal/uart.c \
                   hal/flash.c  \
                   hal/hw.c  \
                   hal/wifi_port.c \
                   hal/ota_port.c \
#$(NAME)_SOURCES  += hal/uart.c
#$(NAME)_SOURCES  += hal/flash.c
#$(NAME)_SOURCES  += hal/hw.c
#$(NAME)_SOURCES  += hal/wifi_port.c
#$(NAME)_SOURCES  += hal/ota_port.c
#$(NAME)_SOURCES  += hal/misc.c



#$(NAME)_COMPONENTS += platform/mcu/rtl8710bn/peripherals
