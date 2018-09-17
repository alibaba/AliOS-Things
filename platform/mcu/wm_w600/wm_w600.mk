
NAME := WM_W600

HOST_OPENOCD := wm_w600

#SPI_WIFI_ENABLED := true

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
# $(NAME)_COMPONENTS += libc rhino hal netmgr framework.common mbedtls cjson cli digest_algorithm protocols.net
$(NAME)_COMPONENTS += rhino libc hal protocols.net netmgr framework.common

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=8
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += GCC_COMPILE=1

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m3
GLOBAL_INCLUDES += include
GLOBAL_INCLUDES += include/platform
GLOBAL_INCLUDES += include/os
GLOBAL_INCLUDES += include/WiFi
GLOBAL_INCLUDES += include/driver
GLOBAL_INCLUDES += include/net/lwip
GLOBAL_INCLUDES += include/net
GLOBAL_INCLUDES += include/app
GLOBAL_INCLUDES += cmsis

GLOBAL_CFLAGS += -mcpu=cortex-m3 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -mcpu=cortex-m3        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing


$(NAME)_SOURCES := aos/aos.c aos/soc_impl.c  aos/trace_impl.c aos/wm_osal_alios.c
$(NAME)_SOURCES += hal/uart.c
$(NAME)_SOURCES	+= hal/wifi_port.c hal/flash.c hal/ota_port.c


$(NAME)_PREBUILT_LIBRARY := libs/libWifi.a
$(NAME)_PREBUILT_LIBRARY += libs/libCommon.a
$(NAME)_PREBUILT_LIBRARY += libs/libCrypto.a
$(NAME)_PREBUILT_LIBRARY += libs/libDrivers.a
$(NAME)_PREBUILT_LIBRARY += libs/libSys.a
