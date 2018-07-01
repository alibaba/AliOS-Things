
NAME := linkkitapp

GLOBAL_DEFINES      += MQTT_DIRECT ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD HAL_ASYNC_API COAP_USE_PLATFORM_LOG

$(NAME)_SOURCES     := linkkit-example.c linkkit_app.c linkkit_export.c

$(NAME)_COMPONENTS := protocol.linkkit.iotkit connectivity.mqtt fota netmgr framework.common  protocol.linkkit.cm protocol.linkkit.dm protocol.linkkit.alcs ywss4linkkit

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(HOST_MCU_FAMILY),esp8266)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifeq ($(HOST_MCU_FAMILY),mk3165)
GLOBAL_DEFINES += AWSS_DISABLE_REGISTRAR
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES += FOTA_RAM_LIMIT_MODE
GLOBAL_DEFINES += ESP8266_CHIPSET
endif
#GLOBAL_CFLAGS += -DON_PRE
#GLOBAL_CFLAGS += -DON_DAILY
