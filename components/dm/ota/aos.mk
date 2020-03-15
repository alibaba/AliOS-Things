NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.1.0
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

$(NAME)_COMPONENTS += cjson
$(NAME)_COMPONENTS-$(!OTA_CONFIG_BLE) += http libiot_mqtt

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES += hal/ota_hal_plat.c \
                   hal/ota_hal_os.c \
                   hal/ota_hal_trans.c \
                   hal/ota_hal_digest.c

$(NAME)_INCLUDES += include hal

ifeq ($(USE_ITLS),y)
   $(NAME)_COMPONENTS += itls
   ifeq ($(COMPILER),armcc)
   GLOBAL_CFLAGS += -DOTA_CONFIG_ITLS -DCONFIG_HTTP_SECURE_ITLS=1 -DOTA_SIGNAL_CHANNEL=1 -DCONFIG_HTTP_SECURE=1
   else
   GLOBAL_DEFINES += OTA_CONFIG_ITLS CONFIG_HTTP_SECURE_ITLS=1 OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1
   endif
else
   $(NAME)_COMPONENTS += mbedtls
   ifeq ($(COMPILER),armcc)
       GLOBAL_CFLAGS += -DCONFIG_HTTP_SECURE=1
       ifneq (,$(filter mcu_esp8266,$(HOST_MCU_FAMILY)))
       GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=1
       else
       GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=1
       endif
   else
       GLOBAL_DEFINES += CONFIG_HTTP_SECURE=1
       ifneq (,$(filter mcu_esp8266,$(HOST_MCU_FAMILY)))
       GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
       else
       GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
       endif
   endif
endif
ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/keil/$(HOST_ARCH)/ota_agent.a
else 
ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := lib/iar/$(HOST_ARCH)/ota_agent.a
else
ifneq (,$(filter breeze,$(COMPONENTS)))
ifneq (,$(filter mcu_nrf52xxx mcu_esp32,$(HOST_MCU_FAMILY)))
$(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/ota_agent_ble.a
endif
else
ifneq (,$(filter itls,$(COMPONENTS)))
$(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/ota_agent_itls.a
else
ifneq (,$(filter mcu_esp32,$(HOST_MCU_FAMILY)))
$(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/ota_agent_esp32.a #esp32
else
ifneq (,$(filter mcu_stm32l4xx_cube mcu_stm32f4xx_cube mcu_mtk7682 mcu_m487jidae mcu_msp432p4xx mcu_xmc, $(HOST_MCU_FAMILY)))
$(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/ota_agent_softabi.a #mt7682s
else
$(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/ota_agent.a #mk3080
endif
endif
endif
endif
endif
endif

