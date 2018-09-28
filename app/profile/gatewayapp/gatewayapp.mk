NAME := gatewayapp

GLOBAL_DEFINES += RHINO_CONFIG_MODBUS=1


ifeq ($(PT_SCANNER),1)
GLOBAL_DEFINES += PT_SCANNER=1
endif

ifeq ($(PT_SENSOR),1)
GLOBAL_DEFINES += PT_SENSOR=1
endif

ifeq ($(dtc),1)

GLOBAL_DEFINES      +=  MQTT_DIRECT  ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD COAP_WITH_YLOOP DEPRECATED_LINKKIT __DEMO__

$(NAME)_SOURCES     := gateway.c gateway_app.c gateway_tsl.c

$(NAME)_COMPONENTS := feature.linkkit-noawss sensor middleware/common utility/cjson network/netmgr middleware/udata rhino.bus.mbmaster

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES +=FOTA_RAM_LIMIT_MODE AWSS_NEED_REBOOT
endif

GLOBAL_DEFINES += DATA_TO_CLOUD

ifeq ($(GW_LINKEDGE_WS),1)
LINKEDGE_SERVER ?= "192.168.1.100"
GLOBAL_DEFINES += GW_LINKEDGE_WS=1 LINKEDGE_SERVER="\"${LINKEDGE_SERVER}\""
$(NAME)_SOURCES += linkedge_ws.c
$(NAME)_COMPONENTS += connectivity/websockets
else

endif

else

$(NAME)_SOURCES := gateway.c

$(NAME)_COMPONENTS :=  middleware/common network/netmgr middleware/udata sensor rhino.bus.mbmaster

GLOBAL_INCLUDES += .

endif