NAME := mcu_wm_w600
HOST_OPENOCD := wm_w600

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu wm_w600

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += GCC_COMPILE=1

GLOBAL_INCLUDES += drivers/include \
                   drivers/include/platform \
                   drivers/include/os \
                   drivers/include/WiFi \
                   drivers/include/driver \
                   drivers/include/net/lwip \
                   drivers/include/net \
                   drivers/include/app \
                   drivers/cmsis


$(NAME)_SOURCES += hal/uart.c \
                   hal/wifi_port.c \
                   hal/flash.c
                   #hal/ota_port.c

$(NAME)_PREBUILT_LIBRARY := drivers/libs/libWifi.a \
                            drivers/libs/libCommon.a \
                            drivers/libs/libCrypto.a \
                            drivers/libs/libDrivers.a \
                            drivers/libs/libSys.a

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

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

