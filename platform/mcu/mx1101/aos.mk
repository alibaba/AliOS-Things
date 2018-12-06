
NAME := mcu_MX1101

HOST_OPENOCD := mx1101

SPI_WIFI_ENABLED := true

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu MX1101

$(NAME)_COMPONENTS += arch_armv7m
# $(NAME)_COMPONENTS += libc rhino hal netmgr mbedtls cjson cli digest_algorithm
$(NAME)_COMPONENTS += rhino

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=5
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_CFLAGS += -mcpu=cortex-m3           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -mcpu=cortex-m3           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  -nostartfiles             \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_SOURCES := aos/soc_impl.c
$(NAME)_SOURCES += hal/i2c.c \
                   hal/flash.c

include ./platform/mcu/mx1101/sdk/sdk.mk
