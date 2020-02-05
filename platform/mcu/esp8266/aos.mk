HOST_OPENOCD := esp8266

NAME := mcu_esp8266

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu esp8266

$(NAME)_COMPONENTS := yloop newlib_stub
$(NAME)_COMPONENTS += lwip netmgr

use_private_lwip := 1

ESP_INC_PATH    := bsp/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)
GLOBAL_INCLUDES += $(ESP_INC_PATH)/xtensa $(ESP_INC_PATH)/espressif $(ESP_INC_PATH)/espressif/esp8266
GLOBAL_INCLUDES += $(ESP_INC_PATH)/lwip $(ESP_INC_PATH)/lwip/ipv4 $(ESP_INC_PATH)/lwip/ipv6

# $(NAME)_INCLUDES := $(ESP_INC_PATH)/driver
GLOBAL_INCLUDES  += $(ESP_INC_PATH)/driver

#GLOBAL_INCLUDES  += common

GLOBAL_CFLAGS    += -DOTA_DUBANK

GLOBAL_CFLAGS    += -u call_user_start    \
                    -fno-inline-functions \
                    -ffunction-sections   \
                    -fdata-sections       \
                    -mlongcalls           \
                    -DESPOS_FOR_ESP8266   \
                    -Wl,-static

GLOBAL_LDFLAGS   += -nostdlib                      \
                    -Lplatform/mcu/esp8266/bsp/lib \
                    -Wl,--no-check-sections        \
                    -Wl,--gc-sections              \
                    -mtext-section-literals        \
                    -fno-builtin-printf            \
                    -Wl,-static                    \
                    -u call_user_start             \
                    -Wl,-EL                        \
                    -mlongcalls

GLOBAL_DEFINES += CONFIG_ESP_LWIP COAP_WITH_YLOOP
GLOBAL_DEFINES += MBEDTLS_AES_ROM_TABLES

$(NAME)_PREBUILT_LIBRARY := bsp/lib/libhal.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libcrypto.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libmain.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libcirom.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libnet80211.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libpp.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libwpa.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libphy.a
$(NAME)_PREBUILT_LIBRARY += bsp/lib/libgcc.a
ifeq ($(loopback), 1)
$(NAME)_PREBUILT_LIBRARY += bsp/lib/liblwip_loopback.a
else
$(NAME)_PREBUILT_LIBRARY += bsp/lib/liblwip.a
endif

GLOBAL_CFLAGS   += -DXT_USE_THREAD_SAFE_CLIB=0
$(NAME)_SOURCES := bsp/entry.c
$(NAME)_SOURCES += bsp/heap_iram.c
$(NAME)_SOURCES += bsp/syscall.c
$(NAME)_SOURCES += bsp/key.c

$(NAME)_SOURCES += bsp/driver/interrupt.c
$(NAME)_SOURCES += bsp/driver/uart.c

$(NAME)_SOURCES += hal/uart.c
$(NAME)_SOURCES += hal/flash.c
$(NAME)_SOURCES += hal/wdg.c
$(NAME)_SOURCES += hal/misc.c
$(NAME)_SOURCES += hal/gpio.c
$(NAME)_SOURCES += hal/wifi_port.c
$(NAME)_SOURCES += hal/upgrade_lib.c
$(NAME)_SOURCES += hal/hal_adc_esp8266.c
$(NAME)_SOURCES += hal/hal_i2c_esp8266.c
$(NAME)_SOURCES += hal/hal_timer_esp8266.c
$(NAME)_SOURCES += bsp/driver/gpio.c
$(NAME)_SOURCES += bsp/driver/hw_timer.c
$(NAME)_SOURCES += bsp/driver/i2c_master.c
$(NAME)_SOURCES += bsp/driver/spi_interface.c

$(NAME)_CFLAGS := -std=gnu99

ifneq ($(wifi),0)
$(NAME)_CFLAGS += -DENABLE_WIFI
endif
$(NAME)_CFLAGS += -Iplatform/mcu/esp8266/bsp

ifeq (0,1)
libs                     := $(wildcard platform/mcu/esp8266/lib/*.a)
libs                     := $(foreach lib,$(libs),lib/$(notdir $(lib)))
$(NAME)_PREBUILT_LIBRARY := $(libs)
endif

ifeq ($(osal),freertos)
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/espos
GLOBAL_CFLAGS            += -I $(SDK8266_PATH)/include/freertos
$(NAME)_PREBUILT_LIBRARY += lib/libespos.a
$(NAME)_PREBUILT_LIBRARY += lib/libfreertos.a
else
$(NAME)_COMPONENTS       += rhino arch_xtensa_lx106
#$(NAME)_SOURCES          += aos/hook_impl.c
#$(NAME)_SOURCES          += aos/soc_impl.c
endif

include platform/mcu/esp8266/espos/espos.mk

EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk
