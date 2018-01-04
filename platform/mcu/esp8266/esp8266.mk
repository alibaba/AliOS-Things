HOST_OPENOCD := esp8266

NAME := esp8266

$(NAME)_TYPE := kernel 

$(NAME)_COMPONENTS := framework.common modules.fs.kv cli libc
$(NAME)_COMPONENTS += protocols.net alicrypto

ESP_INC_PATH     := bsp/include
GLOBAL_INCLUDES  += $(ESP_INC_PATH)
GLOBAL_INCLUDES  += $(ESP_INC_PATH)/xtensa

GLOBAL_CFLAGS    += -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls
GLOBAL_LDFLAGS   += -nostdlib -Lplatform/mcu/esp8266/bsp/lib -lcirom
GLOBAL_LDFLAGS   += -lgcc -lstdc++ -lgcov -lm

GLOBAL_LDS_FILES += platform/mcu/esp8266/bsp/ld/eagle.app.v6.new.1024.app1.ld
GLOBAL_LDFLAGS   += -Lplatform/mcu/esp8266/bsp/ld

GLOBAL_DEFINES   += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES   += CONFIG_AOS_CLI_BOARD

GLOBAL_CFLAGS    += -DXT_USE_THREAD_SAFE_CLIB=0
$(NAME)_SOURCES  := bsp/entry.c
$(NAME)_SOURCES  += bsp/syscall.c
$(NAME)_SOURCES  += hal/uart.c
$(NAME)_CFLAGS   := -std=gnu99

ifneq ($(wifi),0)
$(NAME)_CFLAGS   += -DENABLE_WIFI
endif
$(NAME)_CFLAGS	 += -Iplatform/mcu/esp8266/bsp

ifeq (0,1)
libs := $(wildcard platform/mcu/esp8266/lib/*.a)
libs := $(foreach lib,$(libs),lib/$(notdir $(lib)))
$(NAME)_PREBUILT_LIBRARY := $(libs)
endif

ifeq ($(vcall),freertos)
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/espos
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/freertos
$(NAME)_PREBUILT_LIBRARY += lib/libespos.a
$(NAME)_PREBUILT_LIBRARY += lib/libfreertos.a
else
$(NAME)_COMPONENTS       += rhino platform/arch/xtensa/lx106
$(NAME)_SOURCES          += aos/hook_impl.c
$(NAME)_SOURCES          += aos/soc_impl.c
endif

ifneq ($(mesh),0)
endif
